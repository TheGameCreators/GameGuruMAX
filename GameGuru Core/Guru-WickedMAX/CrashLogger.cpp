#include <windows.h>
#include <dbghelp.h>
#include <tchar.h>
#include <iostream>
#include <sstream>
#include <fstream>

#pragma comment(lib, "dbghelp.lib")

// What time is it
std::string GetTimestamp()
{
    time_t now = time(nullptr);
    struct tm localTime;
    localtime_s(&localTime, &now);

    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
    return std::string(buffer);
}

// Crash handler
LONG WINAPI CrashHandler(EXCEPTION_POINTERS* pExceptionInfo)
{
    MessageBoxA(
        NULL,
        "A crash has been detected! A crash report has been created in file 'Guru-Crash.log'",
        "GameGuru MAX Crash",
        MB_OK | MB_ICONERROR
    );

    // Get path to the EXE folder
    char exeFile[MAX_PATH];
    GetModuleFileNameA(NULL, exeFile, MAX_PATH);
    char exePath[MAX_PATH];
    strcpy_s(exePath, MAX_PATH, exeFile);
    char* lastSlash = strrchr(exePath, '\\');
    if (lastSlash) *(lastSlash + 1) = '\0';

    // Build paths
    char logPath[MAX_PATH];
    strcpy_s(logPath, exePath);
    strcat_s(logPath, "Guru-Crash.log");

    // Initialize symbol handler
    HANDLE process = GetCurrentProcess();
    if (!SymInitialize(process, NULL, FALSE)) {
        std::cerr << "Failed to initialize symbols." << std::endl;
        return 1;
    }

    // Load the module (EXE)
    DWORD64 baseAddress;
    baseAddress = SymLoadModuleEx(
        process,
        NULL,
        exeFile,
        NULL,
        0,
        0,
        NULL,
        0
    );

    // the address we need is not the runtime address the exception provides!
    DWORD64 moduleBase = (DWORD64)GetModuleHandle(NULL);
    DWORD64 crashAddress = (DWORD64)pExceptionInfo->ExceptionRecord->ExceptionAddress;
    DWORD64 offset = crashAddress - moduleBase;
    DWORD64 lookupAddress = baseAddress + offset;

    // Get source line info
    std::string lineInfo;
    IMAGEHLP_LINE64 lineData = { 0 };
    DWORD displacement = 0;
    lineData.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
    if (SymGetLineFromAddr64(process, lookupAddress, &displacement, &lineData))
    {
        std::ostringstream l;
        l << lineData.FileName << ":" << lineData.LineNumber;
        lineInfo = l.str();
    }
    SymCleanup(process);

    std::ostringstream log;
    log << "\r\n==== GAMEGURU MAX CRASH DETECTED ====\r\n";
    log << "Time:            " << GetTimestamp() << "\r\n";
    log << "Exception code:  0x" << std::hex << pExceptionInfo->ExceptionRecord->ExceptionCode << "\r\n";
    log << "Module address:   0x" << std::hex << moduleBase << "\r\n";
    log << "Crash address:   0x" << std::hex << crashAddress << "\r\n";
    log << "Base address:   0x" << std::hex << baseAddress << "\r\n";
    log << "Offset value:   0x" << std::hex << offset << "\r\n";
    log << "Lookup address:   0x" << std::hex << lookupAddress << "\r\n";
    if (!lineInfo.empty())
    {
        log << "Source Code:     " << lineInfo << "\r\n";
    }
    log << "=====================================\r\n";

    // Write to log
    HANDLE hFile = CreateFileA(logPath, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD bytesWritten;
        WriteFile(hFile, log.str().c_str(), (DWORD)log.str().size(), &bytesWritten, NULL);
        FlushFileBuffers(hFile);
        CloseHandle(hFile);
    }

    Sleep(100);
    return EXCEPTION_EXECUTE_HANDLER;
}

void InitCrashHandler()
{
    SetUnhandledExceptionFilter(CrashHandler);
}

