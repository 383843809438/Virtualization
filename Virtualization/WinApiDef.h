#pragma once

#include <ntddk.h>

// Minimal declarations used by the sample code
VOID KeGenericCallDpc(__in PKDEFERRED_ROUTINE Routine, __in_opt PVOID Context);
void KeSignalCallDpcDone(__in PVOID SystemArgument1);
LOGICAL KeSignalCallDpcSynchronize(__in PVOID SystemArgument2);
NTSYSAPI VOID __cdecl RtlRestoreContext(__in PCONTEXT ContextRecord, __in_opt PEXCEPTION_RECORD ExceptionRecord);