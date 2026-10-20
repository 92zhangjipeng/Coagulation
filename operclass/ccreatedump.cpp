#include "ccreatedump.h"

#include <Windows.h>
#include <DbgHelp.h>
#include <csignal>
#include <exception>
#include <new>
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#pragma comment(lib, "dbghelp.lib")
#endif

CCreateDump* CCreateDump::__instance = NULL;
wchar_t CCreateDump::s_dumpDir[MAX_PATH] = { 0 };
volatile long CCreateDump::s_dumping = 0;
void* CCreateDump::s_vectoredContinue = NULL;

CCreateDump::CCreateDump()
{
}

CCreateDump::~CCreateDump()
{
}

bool CCreateDump::InitDumpDirectory()
{
    wchar_t modulePath[MAX_PATH] = { 0 };
    if (GetModuleFileNameW(NULL, modulePath, MAX_PATH) == 0) {
        return false;
    }

    wchar_t* slash = wcsrchr(modulePath, L'\\');
    if (slash == NULL) {
        return false;
    }
    *slash = L'\0';

    if (swprintf_s(s_dumpDir, MAX_PATH, L"%s\\CrashDumps", modulePath) < 0) {
        return false;
    }

    if (!CreateDirectoryW(s_dumpDir, NULL)) {
        const DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS) {
            wchar_t tempPath[MAX_PATH] = { 0 };
            if (GetTempPathW(MAX_PATH, tempPath) == 0) {
                OutputDebugStringA("CreateDirectoryW CrashDumps failed");
                return false;
            }
            if (swprintf_s(s_dumpDir, MAX_PATH, L"%sCoagulationCrashDumps", tempPath) < 0) {
                return false;
            }
            if (!CreateDirectoryW(s_dumpDir, NULL)) {
                const DWORD tempErr = GetLastError();
                if (tempErr != ERROR_ALREADY_EXISTS) {
                    OutputDebugStringA("CreateDirectoryW temp CrashDumps failed");
                    return false;
                }
            }
        }
    }
    return true;
}

void CCreateDump::WriteMiniDump(_EXCEPTION_POINTERS* ExceptionInfo)
{
    if (InterlockedCompareExchange(&s_dumping, 1, 0) != 0) {
        return;
    }

    if (s_dumpDir[0] == L'\0') {
        InitDumpDirectory();
    }

    wchar_t dumpPath[MAX_PATH] = { 0 };
    SYSTEMTIME syt;
    GetLocalTime(&syt);
    if (swprintf_s(dumpPath, MAX_PATH,
                   L"%s\\Coagulation_%04d%02d%02d_%02d%02d%02d.dmp",
                   s_dumpDir,
                   syt.wYear, syt.wMonth, syt.wDay,
                   syt.wHour, syt.wMinute, syt.wSecond) < 0) {
        return;
    }

    HANDLE hFile = CreateFileW(dumpPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        OutputDebugStringA("CreateFileW dump failed");
        return;
    }

    MINIDUMP_EXCEPTION_INFORMATION exInfo;
    MINIDUMP_EXCEPTION_INFORMATION* pExInfo = NULL;
    if (ExceptionInfo != NULL) {
        exInfo.ThreadId = GetCurrentThreadId();
        exInfo.ExceptionPointers = ExceptionInfo;
        exInfo.ClientPointers = FALSE;
        pExInfo = &exInfo;
    }

    const MINIDUMP_TYPE dumpType = static_cast<MINIDUMP_TYPE>(
                MiniDumpNormal
                | MiniDumpWithThreadInfo
                | MiniDumpWithUnloadedModules
                | MiniDumpIgnoreInaccessibleMemory);

    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
                      dumpType, pExInfo, NULL, NULL);
    FlushFileBuffers(hFile);
    CloseHandle(hFile);
}

long CCreateDump::UnhandleExceptionFilter(_EXCEPTION_POINTERS* ExceptionInfo)
{
    WriteMiniDump(ExceptionInfo);
    return EXCEPTION_EXECUTE_HANDLER;
}

long CCreateDump::VectoredContinueHandler(_EXCEPTION_POINTERS* ExceptionInfo)
{
    if (ExceptionInfo == NULL || ExceptionInfo->ExceptionRecord == NULL) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    if (IsDebuggerPresent()) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    const DWORD code = ExceptionInfo->ExceptionRecord->ExceptionCode;
    switch (code) {
    case EXCEPTION_ACCESS_VIOLATION:
    case EXCEPTION_STACK_OVERFLOW:
    case EXCEPTION_ILLEGAL_INSTRUCTION:
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    case EXCEPTION_IN_PAGE_ERROR:
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
    case EXCEPTION_PRIV_INSTRUCTION:
    case 0xC0000374: // STATUS_HEAP_CORRUPTION
    case 0xC0000409: // STATUS_STACK_BUFFER_OVERRUN / fastfail
        WriteMiniDump(ExceptionInfo);
        break;
    default:
        break;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

void CCreateDump::SignalAbortHandler(int)
{
    WriteMiniDump(NULL);
    _exit(3);
}

void CCreateDump::InvalidParameterHandler(const wchar_t*, const wchar_t*,
                                          const wchar_t*, unsigned int, uintptr_t)
{
    WriteMiniDump(NULL);
    _exit(3);
}

void CCreateDump::PureCallHandler()
{
    WriteMiniDump(NULL);
    _exit(3);
}

void CCreateDump::TerminateHandler()
{
    WriteMiniDump(NULL);
    _exit(3);
}

bool CCreateDump::ReinstallFilter()
{
    SetUnhandledExceptionFilter(UnhandleExceptionFilter);
    return true;
}

bool CCreateDump::DeclarDumpFile(const std::string&)
{
    try {
        if (!InitDumpDirectory()) {
            OutputDebugStringA("Failed to init CrashDumps directory");
            return false;
        }

        ULONG stackSize = 64 * 1024;
        SetThreadStackGuarantee(&stackSize);

        SetErrorMode(SEM_FAILCRITICALERRORS);
        ReinstallFilter();

        if (s_vectoredContinue == NULL) {
            s_vectoredContinue = AddVectoredContinueHandler(1, VectoredContinueHandler);
        }

        signal(SIGABRT, SignalAbortHandler);
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
        _set_invalid_parameter_handler(InvalidParameterHandler);
        _set_purecall_handler(PureCallHandler);
        std::set_terminate(TerminateHandler);
        std::set_new_handler([]() {
            WriteMiniDump(NULL);
            _exit(3);
        });

        return true;
    } catch (...) {
        OutputDebugStringA("Unknown exception in DeclarDumpFile");
        return false;
    }
}

const wchar_t* CCreateDump::dumpDirectory()
{
    return s_dumpDir;
}

CCreateDump* CCreateDump::Instance()
{
    if (__instance == NULL) {
        __instance = new CCreateDump;
    }
    return __instance;
}

void CCreateDump::Del_Instance()
{
    if (__instance) {
        delete __instance;
        __instance = nullptr;
    }
}
