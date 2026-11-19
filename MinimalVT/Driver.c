#include <ntddk.h>
#include"Cpu.h"
#include"Vmx.h"
BOOLEAN CommonCheckBios()
{
	ULONG64 bios = __readmsr(MSR_IA32_FEATURE_CONTROL);
	ULONG64 result = bios & 0x5ULL;
	if (result == 5ULL) {
		return TRUE;
	}
	return FALSE;
}
BOOLEAN CommonCheckCpuid()
{
	int cpuInfo[4] = { 0 };
	__cpuidex(cpuInfo, 1, 0);
	return (BOOLEAN)((cpuInfo[2] >> 5) & 1);
}
BOOLEAN CommonCheckCr4()
{
	ULONG64 cr4 = __readcr4();
	cr4 = cr4 >> 13;
	if ((cr4 & 1) == 0) {
		return TRUE;
	}
	return FALSE;
}
BOOLEAN VmxCheckSupport() {
	BOOLEAN bbios = CommonCheckBios();
	BOOLEAN bcpuid = CommonCheckCpuid();
	BOOLEAN bcr4 = CommonCheckCr4();
	DbgPrintEx(DPFLTR_IHVDRIVER_ID,0,
		"[+] bbios=%d, bcpuid=%d, bcr4=%d\n",	(int)bbios,
		(int)bcpuid,
		(int)bcr4);
	return bbios && bcpuid && bcr4;
}
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT  DriverObject, _In_ PUNICODE_STRING RegistryPath
) {
    UNREFERENCED_PARAMETER(RegistryPath);
    UNREFERENCED_PARAMETER(DriverObject);
	KAFFINITY oldAffinity;
	oldAffinity = KeSetSystemAffinityThreadEx(1ULL);//CPU0

	VmxCheckSupport();
	VmxTestOnOff();

	KeRevertToUserAffinityThreadEx(oldAffinity);//exit CPU0
    return STATUS_SUCCESS;
}