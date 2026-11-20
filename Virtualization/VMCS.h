#pragma once
#include <ntddk.h>
#include <intrin.h>
#include"CPU.h"
#include"reg.h"
void initGuestArea(ULONG64 guestRSP, ULONG64 guestResumeRip);