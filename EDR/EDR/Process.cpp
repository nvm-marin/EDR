#include <ntifs.h>
#include "DriverCommon.h"
#include "Process.h"
#include "COMM.h"
#include <ntstrsafe.h>



VOID ProcessCreateRoutine(HANDLE PPID, HANDLE PID, BOOLEAN Create)
{
	PEPROCESS Process = NULL; 
	PUNICODE_STRING procName = NULL;
	
	NTSTATUS status = PsLookupProcessByProcessId(PID, &Process);
	if(NT_SUCCESS(status))
		status = SeLocateProcessImageName(Process, &procName);

	//Base 
	EDR_EVENT event = { 0 };
	event.Type = Create ? EdrEventProcessCreate : EdrEventProcessExit;
	event.Pid = (ULONG)(ULONG_PTR)PID;
	event.Ppid = (ULONG)(ULONG_PTR)PPID;

	// Image Path 
	if (Create && NT_SUCCESS(status) && procName && procName->Buffer)
	{
		RtlStringCchCopyNW(event.ImagePath, RTL_NUMBER_OF(event.ImagePath), procName->Buffer, procName->Length / sizeof(WCHAR));
	}


	EdrPublish(&event);

	//end
	if(Process)
		ObDereferenceObject(Process);


	if (procName)
		ExFreePool(procName);
	
}