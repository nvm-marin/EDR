#include "COMM.h"
#include <ntifs.h>

VOID EdrPublish(_In_ const EDR_EVENT* event)
{
    switch (event->Type) {
    case EdrEventProcessCreate:
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
            "EDR: CREATE Pid=%lu Ppid=%lu '%ws'\n",
            event->Pid, event->Ppid, event->ImagePath);
        break;
    case EdrEventProcessExit:
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
            "EDR: EXIT Pid=%lu\n", event->Pid);
        break;
    default:
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "EDR: Unknown event type %d\n", (int)event->Type);
    }
}
