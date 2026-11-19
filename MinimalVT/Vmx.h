#pragma once
#include <ntddk.h>
#include <intrin.h>
#include"Cpu.h"
#define VMXON_REGION_SIZE 0x1000

typedef struct _VMXON_REGION
{
    ULONG RevisionIdentifier;
    UCHAR Reserved[VMXON_REGION_SIZE - sizeof(ULONG)];

} VMXON_REGION, * PVMXON_REGION;
BOOLEAN VmxAllocateRegion(
    PVOID* Region,
    PHYSICAL_ADDRESS* PhysicalAddress
);
BOOLEAN VmxTestOnOff();
BOOLEAN VmxTestVmcs();