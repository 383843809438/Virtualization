#pragma once
#include <ntddk.h>
#ifdef __cplusplus
extern "C" {
#endif

	USHORT RegGetCS();
	USHORT RegGetDS();
	USHORT RegGetES();
	USHORT RegGetSS();
	USHORT RegGetFS();
	USHORT RegGetGS();
	USHORT RegGetDr0();
	USHORT RegGetDr1();
	USHORT RegGetDr2();
	USHORT RegGetDr3();
	ULONG64 GetGdtBase();
	USHORT GetGdtLimit();
	ULONG64 GetIdtBase();
	USHORT GetIdtLimit();
	USHORT GetLdtr();
	USHORT GetTrSelector();
#ifdef __cplusplus
}
#endif