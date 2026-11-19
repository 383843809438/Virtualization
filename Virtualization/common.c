#include "common.h"

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
