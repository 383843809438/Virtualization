#pragma once
#include <ntddk.h>
#include <intrin.h>
#include"CPU.h"
#include"common.h"
#include"reg.h"
typedef struct _VMX_VMCS {
	ULONG RevisionId;
	ULONG AbortIndicator;
	UCHAR Data[PAGE_SIZE - 2 * sizeof(ULONG)];
}VMX_VMCS, * PVMX_VMCS;
typedef struct _VCPU {
	PVMX_VMCS VMXON;
	PVMX_VMCS VMCS;
	PVOID VMMStack;
}VCPU, * PVCPU;
typedef struct
{
	USHORT sel;
	USHORT attributes;
	ULONG32 limit;
	ULONG64 base;
}SEGEMENT_SELECTOR;

#pragma pack(push, 1)
typedef struct {
	USHORT LimitLow;
	USHORT BaseLow;
	UCHAR  BaseMid;
	UCHAR AttributesLow;
	struct
	{
		UCHAR LimitHigh : 4;
		UCHAR AttributesHigh : 4;
	}Bits;
	UCHAR BaseHigh;
}SEGEMENT_DESCRIPTOR, * PSEGEMENT_DESCRIPTOR;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _TSS_DESCRIPTOR64
{
	USHORT LimitLow;
	USHORT BaseLow;
	UCHAR  BaseMid;
	UCHAR  Attributes;
	UCHAR  LimitHigh;
	UCHAR  BaseHigh;

	ULONG  BaseUpper;
	ULONG  Reserved;
} TSS_DESCRIPTOR64, * PTSS_DESCRIPTOR64;
#pragma pack(pop)

int VmxInitCpu();
int VMXSetupVmcs(PVOID guestRSP);
void VmxFillSelectorData(USHORT selector, USHORT index);
ULONG VmxMsrAdjust(ULONG msrNum, ULONG value);
DECLSPEC_NORETURN void VmxExitHandler(PGUEST_REGS guestRegs);
void CommonVtStart(_In_ struct _KDPC* Dpc, _In_opt_ PVOID DeferredContext, _In_opt_ PVOID SystemArgument1,
	_In_opt_ PVOID SystemArgument2);
void VtShutdown(KDPC* Dpc, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2);
void VmxFreeMemory();
//asm
void VmxJumpGuest(ULONG64 tarRsp, ULONG64 tarRip);