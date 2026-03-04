#include <ntifs.h>
#include <ntstrsafe.h>

#include "Process.h"
#include "Comm.h"
#include "DriverCommon.h"


//Step 1 name and link 
UNICODE_STRING DriverName = RTL_CONSTANT_STRING(L"\\Device\\DumbEDR");
UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\DumbEDR");
WCHAR g_LastMsg[512] = L"No events yet\r\n";
FAST_MUTEX g_Lock;
EDR_EVENT g_LatestEvent = { 0 };

//Prototype functions
VOID DriverUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS EdrCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS EdrDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);


extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	//Mutex  init 
	ExInitializeFastMutex(&g_Lock);

	//Create device
	PDEVICE_OBJECT DeviceObject = NULL;
	NTSTATUS status = IoCreateDevice(
		DriverObject, //Driver owning this device
		0,            //no extra storage 
		&DriverName, //Name 
		FILE_DEVICE_UNKNOWN,//Device type  
		0, //no special characteristics
		FALSE, //not exclusive
		&DeviceObject);//OUT: Device Object ptr 

	if (!NT_SUCCESS(status))
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Failed to create device (0x%X)\n", status);
		return status;
	}

	//Sym Link 
	status = IoCreateSymbolicLink(&symLink, &DriverName);
	if (!NT_SUCCESS(status))
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Failed to create symbolic link (0x%X)\n", status);
		IoDeleteDevice(DeviceObject);
		return status;
	}

	//Major func 
	DriverObject->MajorFunction[IRP_MJ_CREATE] = EdrCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = EdrCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = EdrDeviceControl;


	return STATUS_SUCCESS;
}


NTSTATUS EdrCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS EdrDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG icode = stack->Parameters.DeviceIoControl.IoControlCode;
	NTSTATUS status = STATUS_SUCCESS;

	switch (icode)
	{
		case IOCTL_BLUESTREET_SEND_DATA:
			{
				//Latest Event 
				ULONG oLen = stack->Parameters.DeviceIoControl.OutputBufferLength;

				if (oLen < sizeof(EDR_EVENT))
				{
					status = STATUS_BUFFER_TOO_SMALL;
					break;
				}

				//Copy data to usermode
				ExAcquireFastMutex(&g_Lock);
				RtlCopyMemory(Irp->AssociatedIrp.SystemBuffer, &g_LatestEvent, sizeof(EDR_EVENT));


				ExReleaseFastMutex(&g_Lock);

				Irp->IoStatus.Information = sizeof(EDR_EVENT);
				break;
			}
		default:
			status = STATUS_INVALID_DEVICE_REQUEST;
			break;
	}

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;

}

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
	//Process calbacks
	PsSetCreateProcessNotifyRoutine(ProcessCreateRoutine, TRUE);
	//Symlink and device 
	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(DriverObject->DeviceObject);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Driver unloaded\n");
}