#include "Vmx.h"
#pragma once

BOOLEAN VmxAllocateRegion(PVOID* Region, PHYSICAL_ADDRESS* PhysicalAddress)
{
    PHYSICAL_ADDRESS lowest = { 0 };
    PHYSICAL_ADDRESS highest;
    PHYSICAL_ADDRESS boundary = { 0 };
    highest.QuadPart = MAXLONGLONG;

    PVOID buffer = MmAllocateContiguousMemorySpecifyCache(
        PAGE_SIZE,
        lowest,
        highest,
        boundary,
        MmCached
    );
    if (buffer == NULL)
        return FALSE;

    RtlZeroMemory(buffer, PAGE_SIZE);

    PHYSICAL_ADDRESS pa = MmGetPhysicalAddress(buffer);
    if ((pa.QuadPart & (PAGE_SIZE - 1)) != 0)
    {
        MmFreeContiguousMemorySpecifyCache(
            buffer,
            PAGE_SIZE,
            MmCached
        );
        return FALSE;
    }

    *Region = buffer;
    *PhysicalAddress = pa;

    return TRUE;
}
BOOLEAN VmxInitializeRegion(PVOID Region)
{
    if (Region == NULL)
        return FALSE;

    ULONG64 vmxBasic = __readmsr(IA32_VMX_BASIC);

    ULONG revisionId = (ULONG)(vmxBasic & 0x7FFFFFFF);//low 31

    *(volatile ULONG*)Region = revisionId;

    return TRUE;
}
BOOLEAN VmxTestOnOff()
{
    DbgPrintEx(
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_ERROR_LEVEL,
        "[MinimalVT] VmxTestOnOff CALL: \n"
    );
    PVOID region = NULL;
    PHYSICAL_ADDRESS physicalAddress = { 0 };


    if (!VmxAllocateRegion(&region, &physicalAddress)) {
        DbgPrintEx(
            DPFLTR_IHVDRIVER_ID,
            DPFLTR_ERROR_LEVEL,
            "[MinimalVT] VmxAllocateRegion failed: \n"
        );
        return FALSE;
    }


    VmxInitializeRegion(region);


    ULONG64 originalCr4 = __readcr4();


    ULONG64 vmxeCr4 = originalCr4 | (1ULL << 13);


    __writecr4(vmxeCr4);


    ULONG64 vmxonPhysicalAddress =
        (ULONG64)physicalAddress.QuadPart;

    unsigned char status =
        __vmx_on(&vmxonPhysicalAddress);

    if (status != 0)
    {
        DbgPrintEx(
            DPFLTR_IHVDRIVER_ID,
            DPFLTR_ERROR_LEVEL,
            "[MinimalVT] VMXON failed: %u\n",
            status
        );

        __writecr4(originalCr4);

        MmFreeContiguousMemorySpecifyCache(
            region,
            PAGE_SIZE,
            MmCached
        );

        return FALSE;
    }

    DbgPrintEx(
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_ERROR_LEVEL,
        "[MinimalVT] VMXON succeeded!\n"
    );


    __vmx_off();


    __writecr4(originalCr4);


    MmFreeContiguousMemorySpecifyCache(
        region,
        PAGE_SIZE,
        MmCached
    );

    DbgPrintEx(
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_ERROR_LEVEL,
        "[MinimalVT] VMXOFF succeeded!\n"
    );

    return TRUE;
}

BOOLEAN VmxTestVmcs()
{
    PVOID vmxonRegion = NULL;
    PVOID vmcsRegion = NULL;

    PHYSICAL_ADDRESS vmxonPa = { 0 };
    PHYSICAL_ADDRESS vmcsPa = { 0 };
    ULONG64 originalCr4 = __readcr4();
    BOOLEAN vmxonActive = FALSE;
    BOOLEAN result = FALSE;
    return result;
}
