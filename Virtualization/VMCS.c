#include "VMCS.h"
#pragma once

void initGuestArea(ULONG64 guestRSP, ULONG64 guestResumeRip)
{
	//控制寄存器CR
	__vmx_vmwrite(GUEST_CR0, __readcr0());
	__vmx_vmwrite(GUEST_CR3, __readcr3());
	__vmx_vmwrite(GUEST_CR4, __readcr4());
	//Debug Register
	__vmx_vmwrite(GUEST_DR7, __readdr(7));
	//RSP, RIP, and RFLAGS
	__vmx_vmwrite(GUEST_RSP, guestRSP);
	__vmx_vmwrite(GUEST_RIP, guestResumeRip);
	__vmx_vmwrite(GUEST_RFLAGS, __readeflags());
	//CS / SS / DS / ES / FS / GS / LDTR / TR
	__vmx_vmwrite(GUEST_CS_SELECTOR, RegGetCS());
	__vmx_vmwrite(GUEST_SS_SELECTOR, RegGetSS());
	__vmx_vmwrite(GUEST_DS_SELECTOR, RegGetDS());
	__vmx_vmwrite(GUEST_ES_SELECTOR, RegGetES());
	__vmx_vmwrite(GUEST_FS_SELECTOR, RegGetFS());
	__vmx_vmwrite(GUEST_GS_SELECTOR, RegGetGS());
	__vmx_vmwrite(GUEST_LDTR_SELECTOR, GetLdtr());
	__vmx_vmwrite(GUEST_TR_SELECTOR, GetTrSelector());
}
