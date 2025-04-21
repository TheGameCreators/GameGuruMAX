#pragma once

#include <windows.h>

LONG WINAPI CrashHandler(EXCEPTION_POINTERS* pExceptionInfo);

void InitCrashHandler(); // Optional convenience wrapper
