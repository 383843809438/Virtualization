#pragma once
#include <ntddk.h>
#include <intrin.h>
#include"WinApiDef.h"
#include"CPU.h"







typedef struct _GUEST_REGS
{
	ULONG64 Rax;        // +00
	ULONG64 Rcx;        // +08
	ULONG64 Rdx;        // +10
	ULONG64 Rbx;        // +18
	ULONG64 Rsp;        // +20
	ULONG64 Rbp;  // +28
	ULONG64 Rsi;        // +30
	ULONG64 Rdi;        // +38
	ULONG64 R8;         // +40
	ULONG64 R9;         // +48
	ULONG64 R10;        // +50
	ULONG64 R11;        // +58
	ULONG64 R12;        // +60
	ULONG64 R13;        // +68
	ULONG64 R14;        // +70
	ULONG64 R15;        // +78
} GUEST_REGS, * PGUEST_REGS;
BOOLEAN CommonCheckBios();
BOOLEAN CommonCheckCpuid();
BOOLEAN CommonCheckCr4();
//asm
BOOLEAN __fastcall AsmVmxLaunch(PVOID rcx);