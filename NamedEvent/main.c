#include <ntddk.h>
PKEVENT g_ExitEvent = NULL;
HANDLE  g_ExitEventHandle = NULL;
NTSTATUS CreateExitEvent()
{
    UNICODE_STRING eventName =
        RTL_CONSTANT_STRING(L"\\BaseNamedObjects\\TinyVT_Exit");

    g_ExitEvent = IoCreateNotificationEvent(
        &eventName,
        &g_ExitEventHandle
    );

    if (g_ExitEvent == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }


    KeClearEvent(g_ExitEvent);

    return STATUS_SUCCESS;
}
HANDLE g_ControlThreadHandle = NULL;

VOID ControlThread(PVOID Context)
{
    UNREFERENCED_PARAMETER(Context);


    KeWaitForSingleObject(
        g_ExitEvent,
        Executive,
        KernelMode,
        FALSE,
        NULL
    );


    DbgPrintEx(0, 0,"[+] call");

    PsTerminateSystemThread(STATUS_SUCCESS);
}
NTSTATUS CreateControlThread()
{
    OBJECT_ATTRIBUTES attributes;
    InitializeObjectAttributes(
        &attributes,
        NULL,
        OBJ_KERNEL_HANDLE,
        NULL,
        NULL
    );

    return PsCreateSystemThread(
        &g_ControlThreadHandle,
        SYNCHRONIZE,
        &attributes,
        NULL,
        NULL,
        ControlThread,
        NULL
    );
}
EXTERN_C VOID DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    


    if (!NT_SUCCESS(CreateExitEvent()))
        return;

    if (!NT_SUCCESS(CreateControlThread()))
    {
        ZwClose(g_ExitEventHandle);
        g_ExitEventHandle = NULL;
        g_ExitEvent = NULL;
        return;
    }
}