#pragma once

#if defined(_KERNEL_MODE)
#include <ntdef.h>
#include <devioctl.h>
#else
#include <Windows.h>
#include <winioctl.h>
#endif

#define BLUESTREET_DEVICE 0x8000
#define IOCTL_BLUESTREET_SEND_DATA CTL_CODE(BLUESTREET_DEVICE, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)


#define DUMBEDR_PORT_NAME   L"\\DumbEDRPort"

typedef enum _EDR_EVENT_TYPE {
    EdrEventProcessCreate = 0,
    EdrEventProcessExit = 1,
    EdrEventFileCreate = 2,
    EdrEventFileWrite = 3,
} EDR_EVENT_TYPE;

typedef struct _EDR_EVENT {
    EDR_EVENT_TYPE Type;
    ULONG          Pid;
    ULONG          Ppid;
    WCHAR          ImagePath[260];
    WCHAR          FilePath[260];
} EDR_EVENT;
