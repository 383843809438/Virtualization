#include "VMX.h"

VCPU g_vcpu[128];
int VmxInitCpu()
{
    ULONG cpuNumber = KeGetCurrentProcessorNumber();
    //VCPU currentCpu = g_vcpu[cpuNumber];
    PHYSICAL_ADDRESS pyhs = { 0 };
    pyhs.QuadPart = MAXULONG64;
    PVMX_VMCS pvmxon = MmAllocateContiguousMemory(sizeof(VMX_VMCS), pyhs);
    PVMX_VMCS pvmcs = MmAllocateContiguousMemory(sizeof(VMX_VMCS), pyhs);

    PVOID vmmStack = MmAllocateContiguousMemory(PAGE_SIZE * 6, pyhs);
    if (pvmxon == NULL || pvmcs == NULL|| vmmStack == NULL)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[VmxInitCpu] MmAllocateContiguousMemory failed!\n");
        return 1;
    }

    
    
    
    RtlZeroMemory(pvmxon, sizeof(VMX_VMCS));
    RtlZeroMemory(pvmcs, sizeof(VMX_VMCS));
    RtlZeroMemory(vmmStack, PAGE_SIZE * 6);

    pvmxon->RevisionId = (ULONG)__readmsr(MSR_IA32_VMX_BASIC);
    pvmcs->RevisionId = (ULONG)__readmsr(MSR_IA32_VMX_BASIC);
    g_vcpu[cpuNumber].VMXON = pvmxon;
    g_vcpu[cpuNumber].VMCS = pvmcs;
    g_vcpu[cpuNumber].VMMStack = vmmStack;

    ULONG64 my_cr4 = __readmsr(MSR_IA32_VMX_CR4_FIXED0);
    my_cr4 |= __readmsr(MSR_IA32_VMX_CR4_FIXED0);
    my_cr4 &= __readmsr(MSR_IA32_VMX_CR4_FIXED1);
    ULONG64 my_cr0 = __readmsr(MSR_IA32_VMX_CR0_FIXED0);
    my_cr0 |= __readmsr(MSR_IA32_VMX_CR0_FIXED0);
    my_cr0 &= __readmsr(MSR_IA32_VMX_CR0_FIXED1);
	__writecr4(my_cr4);
	__writecr0(my_cr0);

    PHYSICAL_ADDRESS physvmon = MmGetPhysicalAddress(pvmxon);
    PHYSICAL_ADDRESS physvmcs = MmGetPhysicalAddress(pvmcs);
    ULONG64 vmxonPhys = physvmon.QuadPart;
	UCHAR vmonResult = __vmx_on(&vmxonPhys);//开启虚拟化,进入VMX root模式
    if (vmonResult)
    {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[VmxInitCpu] __vmx_on failed with error code: %d\n", vmonResult);
        return vmonResult;
    }

    ULONG64 vmcsPhys = physvmcs.QuadPart;
	__vmx_vmclear(&vmcsPhys);
	__vmx_vmptrld(&vmcsPhys);
	//initGuestArea填充VMCS
    BOOLEAN b= AsmVmxLaunch((PVOID)initGuestArea);
    if (b)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[+] AsmVmxLaunch success!\n");
    }
    return 0;
}
void fillTRSelectorData() {
	USHORT selector = GetTrSelector();
	selector &= 0xFFF8;
    ULONG trLimit = __segmentlimit(selector);
    ULONG64 gdtBase = GetGdtBase();
	LARGE_INTEGER trSegement={0};
	PULONG trContext = (PULONG)(gdtBase + selector);
    trSegement.LowPart = ((trContext[0] >> 16) & 0xFFFF) | ((trContext[1] & 0xFF) << 16) | (((trContext[1] >> 24) & 0xFF) << 24);
    trSegement.HighPart = trContext[2];
    ULONG trAttr = (trContext[1] & 0xF0FF00) >> 8;
    __vmx_vmwrite(GUEST_TR_BASE, trSegement.QuadPart);
    __vmx_vmwrite(GUEST_TR_LIMIT, trLimit);
    __vmx_vmwrite(GUEST_TR_AR_BYTES, trAttr);
    __vmx_vmwrite(GUEST_TR_SELECTOR, selector);
    __vmx_vmwrite(HOST_TR_BASE, trSegement.QuadPart);
    __vmx_vmwrite(HOST_TR_SELECTOR, selector);

	
	
	
	

    
}
int VMXSetupVmcs(PVOID guestRSP)
{
    VmxFillSelectorData(RegGetES(), 0);
    VmxFillSelectorData(RegGetCS(), 1);
    VmxFillSelectorData(RegGetSS(), 2);
    VmxFillSelectorData(RegGetDS(), 3);
    VmxFillSelectorData(RegGetFS(), 4);
    VmxFillSelectorData(RegGetGS(), 5);
    VmxFillSelectorData(GetLdtr(), 6);
	__vmx_vmwrite(HOST_ES_SELECTOR, RegGetES()&0xFFF8);
	__vmx_vmwrite(HOST_CS_SELECTOR, RegGetCS()&0xFFF8);
	__vmx_vmwrite(HOST_SS_SELECTOR, RegGetSS()&0xFFF8);
	__vmx_vmwrite(HOST_DS_SELECTOR, RegGetDS()&0xFFF8);
	__vmx_vmwrite(HOST_FS_SELECTOR, RegGetFS()&0xFFF8);
	__vmx_vmwrite(HOST_GS_SELECTOR, RegGetGS()&0xFFF8);

	fillTRSelectorData();
	__vmx_vmwrite(GUEST_CR0, __readcr0());
	__vmx_vmwrite(GUEST_CR3, __readcr3());
	__vmx_vmwrite(GUEST_CR4, __readcr4());
	__vmx_vmwrite(GUEST_DR7, __readdr(7));
    __vmx_vmwrite(HOST_CR0, __readcr0());
    __vmx_vmwrite(HOST_CR3, __readcr3());
    __vmx_vmwrite(HOST_CR4, __readcr4());
    
    __vmx_vmwrite(VMCS_LINK_POINTER, SIZE_MAX);
    __vmx_vmwrite(VMCS_LINK_POINTER_HIGH, SIZE_MAX);
    __vmx_vmwrite(GUEST_IA32_DEBUGCTL, __readmsr(MSR_IA32_DEBUGCTL));
    __vmx_vmwrite(GUEST_IA32_PAT, __readmsr(MSR_IA32_PAT));
	__vmx_vmwrite(GUEST_IA32_EFER, __readmsr(MSR_IA32_EFER));

    __vmx_vmwrite(GUEST_IA32_SYSENTER_CS, __readmsr(MSR_IA32_SYSENTER_CS));
    __vmx_vmwrite(GUEST_IA32_SYSENTER_ESP, __readmsr(MSR_IA32_SYSENTER_ESP));
    __vmx_vmwrite(GUEST_IA32_SYSENTER_EIP, __readmsr(MSR_IA32_SYSENTER_EIP));
    __vmx_vmwrite(HOST_IA32_SYSENTER_CS, __readmsr(MSR_IA32_SYSENTER_CS));
    __vmx_vmwrite(HOST_IA32_SYSENTER_ESP, __readmsr(MSR_IA32_SYSENTER_ESP));
    __vmx_vmwrite(HOST_IA32_SYSENTER_EIP, __readmsr(MSR_IA32_SYSENTER_EIP));
	//设置GDT
	__vmx_vmwrite(GUEST_GDTR_BASE, GetGdtBase());
	__vmx_vmwrite(GUEST_GDTR_LIMIT, GetGdtLimit());
    __vmx_vmwrite(HOST_GDTR_BASE, GetGdtBase());
	//设置IDT
	__vmx_vmwrite(GUEST_IDTR_BASE, GetIdtBase());
	__vmx_vmwrite(GUEST_IDTR_LIMIT, GetIdtLimit());
    __vmx_vmwrite(HOST_IDTR_BASE, GetIdtBase());
    //guest rsp和rip
	__vmx_vmwrite(GUEST_RSP, (ULONG64)guestRSP);
	
    __vmx_vmwrite(GUEST_RFLAGS, __readeflags());

    ULONG cpuNumber = KeGetCurrentProcessorNumber();
    __vmx_vmwrite(HOST_RSP, (ULONG64)g_vcpu[cpuNumber].VMMStack + PAGE_SIZE * 5);

	ULONG64 basicMsr = __readmsr(MSR_IA32_VMX_BASIC);
    ULONG64 result = (basicMsr >> 55) & 1;
	ULONG entryMsrNum = MSR_IA32_VMX_ENTRY_CTLS;
	ULONG exitMsrNum = MSR_IA32_VMX_EXIT_CTLS;
    if (result)//判断 CPU 是否支持 VMX True Controls
    {
		entryMsrNum = MSR_IA32_VMX_TRUE_ENTRY_CTLS;
		exitMsrNum = MSR_IA32_VMX_TRUE_EXIT_CTLS;
    }
    __vmx_vmwrite(VM_ENTRY_CONTROLS,VmxMsrAdjust(entryMsrNum, 0x200));
	__vmx_vmwrite(VM_EXIT_CONTROLS,VmxMsrAdjust(exitMsrNum, 0x8200));
	result=__vmx_vmlaunch();//进入非root模式(回到guest模式),rip写的什么就会从什么地方开始执行，这里后面的代码不会执行，除非guest模式下的代码执行了vmcall指令
    if (result)//失败退出VT
    {
        DbgPrintEx(0, 0, "[-] __vmx_vmlaunch failed");
    }
    

    return 0;
}

void VmxFillSelectorData(USHORT selector, USHORT index)
{
    SEGEMENT_SELECTOR segmentSelector = { 0 };
    segmentSelector.sel = selector;
    segmentSelector.limit = __segmentlimit(segmentSelector.sel);
    ULONG64 gdtBase = GetGdtBase();
    PSEGEMENT_DESCRIPTOR segmentDes = (PSEGEMENT_DESCRIPTOR)(gdtBase + (ULONG64)(segmentSelector.sel & 0xFFF8));
    segmentSelector.base = segmentDes->BaseLow | segmentDes->BaseMid << 16 | segmentDes->BaseHigh << 24;
    segmentSelector.attributes = segmentDes->AttributesLow | (segmentDes->BaseHigh << 8);
    if (segmentSelector.attributes & 0x800)
    {
        segmentSelector.limit = (segmentSelector.limit << 12) + 0xFFF;
    }
    __vmx_vmwrite(GUEST_ES_SELECTOR + index * 2, segmentSelector.sel);
    __vmx_vmwrite(GUEST_ES_LIMIT + index * 2, segmentSelector.limit);
    __vmx_vmwrite(GUEST_ES_BASE + index * 2, segmentSelector.base);
    __vmx_vmwrite(GUEST_ES_AR_BYTES + index * 2, segmentSelector.attributes);
}

ULONG VmxMsrAdjust(ULONG msrNum, ULONG value)
{
    LARGE_INTEGER msrValue;
    msrValue.QuadPart  = __readmsr(msrNum);
    value = (msrValue.LowPart | value) & msrValue.HighPart;

    return value;
}
void VmxCpuidHandler(PGUEST_REGS guestRegs)
{
	if (guestRegs->Rax == 1)
	{
		guestRegs->Rax = 0x0000000;
		guestRegs->Rbx = 0;
		guestRegs->Rcx = 0;
		guestRegs->Rdx = 0;
		return;
    }
    else {
		int cpuInfo[4] = { 0 };
        __cpuidex(cpuInfo, (int)guestRegs->Rax, (int)guestRegs->Rcx);
        guestRegs->Rax = cpuInfo[0];
        guestRegs->Rbx = cpuInfo[1];
        guestRegs->Rcx = cpuInfo[2];
        guestRegs->Rdx = cpuInfo[3];    
    }

	
}
void VmxExitHandler(PGUEST_REGS guestRegs)
{
    ULONG64 exitReason = 0;
    __vmx_vmread(VMCS_EXIT_REASON, &exitReason);
    exitReason &= 0xFFFF;

	ULONG64 exitCodeLen = 0;
    __vmx_vmread(VMCS_EXIT_INSTRUCTION_LEN, &exitCodeLen);

	ULONG64 guestRip = 0;
	__vmx_vmread(GUEST_RIP, &guestRip);

    switch (exitReason)
    {
    case EXIT_REASON_CPUID: {
		VmxCpuidHandler(guestRegs);
    }
    case EXIT_REASON_VMCALL: {
        if (guestRegs->Rcx==1)//表示要关闭VT
        {
            __vmx_off();
			VmxJumpGuest(guestRegs->Rsp, guestRip+exitCodeLen);
        }
    }
    default:
        break;
    }
    
	__vmx_vmwrite(GUEST_RIP, guestRip + exitCodeLen);
}
void CommonVtStart(struct _KDPC* Dpc, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
{
    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(DeferredContext);

    BOOLEAN bbios = CommonCheckBios();
    BOOLEAN bcpuid = CommonCheckCpuid();
    BOOLEAN bcr4 = CommonCheckCr4();
    ULONG cpuNumber = KeGetCurrentProcessorNumber();
    DbgPrintEx(DPFLTR_IHVDRIVER_ID,
        0,
        "current cpuNumber: %u, bbios=%d, bcpuid=%d, bcr4=%d\n",
        (unsigned)cpuNumber,
        (int)bbios,
        (int)bcpuid,
        (int)bcr4);
    if (bbios && bcpuid && bcr4)
    {
        VmxInitCpu();
    }
    else {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID,
            0,
            "[CPU:%d]VT  not supported \n",
            (unsigned)cpuNumber
        );
    }

    KeSignalCallDpcSynchronize(SystemArgument2);
    KeSignalCallDpcDone(SystemArgument1);
}

void VtShutdown(KDPC* Dpc, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
{
    
	//释放内存
    VmxFreeMemory();
	ULONG64 myCr4 = __readcr4();
	myCr4 &= ~0x2000;
	__writecr4(myCr4);
	UNREFERENCED_PARAMETER(Dpc);
	UNREFERENCED_PARAMETER(DeferredContext);
	UNREFERENCED_PARAMETER(SystemArgument1);
	UNREFERENCED_PARAMETER(SystemArgument2);
	
	
	
	
}

void VmxFreeMemory()
{
    ULONG cpuNumber = KeGetCurrentProcessorNumber();
	VCPU currentCpu = g_vcpu[cpuNumber];
    if (currentCpu.VMCS)
    {
        MmFreeContiguousMemory(currentCpu.VMCS);
        currentCpu.VMCS = NULL;
    }if (currentCpu.VMMStack)
    {
        MmFreeContiguousMemory(currentCpu.VMMStack);
		currentCpu.VMMStack = NULL;
    }if (currentCpu.VMXON)
    {
        MmFreeContiguousMemory(currentCpu.VMXON);
        currentCpu.VMXON = NULL;
    }
}
