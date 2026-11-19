#pragma once
#include <ntddk.h>

#include <initguid.h>
#include"WinApiDef.h"
#include"common.h"
#include"VMX.h"
void DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
    KeGenericCallDpc(VtShutdown, NULL);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
		"[+] DriverUnload called\n");
}
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT  DriverObject, _In_ PUNICODE_STRING RegistryPath
) {
    UNREFERENCED_PARAMETER(RegistryPath);
    UNREFERENCED_PARAMETER(DriverObject);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
        "[+] DriverEntry called\n");
    KeGenericCallDpc(CommonVtStart, NULL);
    return STATUS_SUCCESS;
}