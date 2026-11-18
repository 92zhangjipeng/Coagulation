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

namespace {

const DWORD kDumpWaitMs = 20000;

struct DumpThreadParam {
    EXCEPTION_RECORD exceptionRecord;
    CONTEXT context;
    EXCEPTION_POINTERS pointers;
    DWORD crashedThreadId;
    bool hasException;
    HANDLE doneEvent;
};

bool tryMakeDir(const wchar_t* dir)
{
    if (CreateDirectoryW(dir, NULL)) {
        return true;
    }
    return GetLastError() == ERROR_ALREADY_EXISTS;
}

} // namespace

CCreateDump* CCreateDump::__instance = NULL;
wchar_t CCreateDump::s_dumpDir[MAX_PATH] = { 0 };
volatile long CCreateDump::s_dumping = 0;
void* CCreateDump::s_vectoredException = NULL;
void* CCreateDump::s_vectoredContinue = NULL;

CCreateDump::CCreateDump()
{
}

CCreateDump::~CCreateDump()
{
}

bool CCreateDump::DirectoryWritable(const wchar_t* dir)
{
    if (dir == NULL || dir[0] == L'\0') {
        return false;
    }
    wchar_t probe[MAX_PATH] = { 0 };
    if (swprintf_s(probe, MAX_PATH, L"%s\\_dump_ready.txt", dir) < 0) {
        return false;
    }
    HANDLE hFile = CreateFileW(probe, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    const char ready[] = "CrashDumps ready\r\n";
    DWORD written = 0;
    WriteFile(hFile, ready, static_cast<DWORD>(sizeof(ready) - 1), &written, NULL);
    FlushFileBuffers(hFile);
    CloseHandle(hFile);
    return true;
}

bool CCreateDump::InitDumpDirectory()
{
    wchar_t candidates[4][MAX_PATH];
    int candidateCount = 0;

    wchar_t modulePath[MAX_PATH] = { 0 };
    if (GetModuleFileNameW(NULL, modulePath, MAX_PATH) != 0) {
        wchar_t* slash = wcsrchr(modulePath, L'\\');
        if (slash != NULL) {
            *slash = L'\0';
            if (swprintf_s(candidates[candidateCount], MAX_PATH,
                           L"%s\\CrashDumps", modulePath) >= 0) {
                ++candidateCount;
            }
        }
    }

    wchar_t tempPath[MAX_PATH] = { 0 };
    if (GetTempPathW(MAX_PATH, tempPath) != 0) {
        if (swprintf_s(candidates[candidateCount], MAX_PATH,
                       L"%sCoagulationCrashDumps", tempPath) >= 0) {
            ++candidateCount;
        }
    }

    wchar_t profile[MAX_PATH] = { 0 };
    if (GetEnvironmentVariableW(L"USERPROFILE", profile, MAX_PATH) > 0) {
        if (swprintf_s(candidates[candidateCount], MAX_PATH,
                       L"%s\\Desktop\\CoagulationCrashDumps", profile) >= 0) {
            ++candidateCount;
        }
        if (swprintf_s(candidates[candidateCount], MAX_PATH,
                       L"%s\\CoagulationCrashDumps", profile) >= 0) {
            ++candidateCount;
        }
    }

    for (int i = 0; i < candidateCount; ++i) {
        if (!tryMakeDir(candidates[i])) {
            continue;
        }
        if (!DirectoryWritable(candidates[i])) {
            continue;
        }
        wcsncpy_s(s_dumpDir, MAX_PATH, candidates[i], _TRUNCATE);
        return true;
    }

    s_dumpDir[0] = L'\0';
    OutputDebugStringA("InitDumpDirectory: no writable CrashDumps path\n");
    return false;
}

bool CCreateDump::IsFatalExceptionCode(unsigned long code)
{
    switch (code) {
    case EXCEPTION_ACCESS_VIOLATION:
    case EXCEPTION_STACK_OVERFLOW:
    case EXCEPTION_ILLEGAL_INSTRUCTION:
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    case EXCEPTION_IN_PAGE_ERROR:
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
    case EXCEPTION_PRIV_INSTRUCTION:
    case EXCEPTION_DATATYPE_MISALIGNMENT:
    case 0xC0000374: // STATUS_HEAP_CORRUPTION
    case 0xC0000409: // STATUS_STACK_BUFFER_OVERRUN / fastfail
    case 0xC0000940: // STATUS_HEAP_CORRUPTION variant
    case 0x40000015: // STATUS_FATAL_APP_EXIT
        return true;
    default:
        return false;
    }
}

void CCreateDump::WriteSidecarNote(const wchar_t* dumpPath, const char* note)
{
    if (dumpPath == NULL || dumpPath[0] == L'\0') {
        return;
    }
    wchar_t txtPath[MAX_PATH] = { 0 };
    wcsncpy_s(txtPath, MAX_PATH, dumpPath, _TRUNCATE);
    wchar_t* dot = wcsrchr(txtPath, L'.');
    if (dot != NULL) {
        wcscpy_s(dot, MAX_PATH - (dot - txtPath), L".txt");
    } else {
        wcsncat_s(txtPath, MAX_PATH, L".txt", _TRUNCATE);
    }
    HANDLE hFile = CreateFileW(txtPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return;
    }
    if (note != NULL) {
        DWORD written = 0;
        WriteFile(hFile, note, static_cast<DWORD>(strlen(note)), &written, NULL);
    }
    FlushFileBuffers(hFile);
    CloseHandle(hFile);
}

void CCreateDump::WriteMiniDumpOnThread(_EXCEPTION_POINTERS* ExceptionInfo,
                                        DWORD crashedThreadId)
{
    if (s_dumpDir[0] == L'\0') {
        InitDumpDirectory();
    }
    if (s_dumpDir[0] == L'\0') {
        OutputDebugStringA("WriteMiniDump: dump dir empty\n");
        return;
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
        char errBuf[128];
        sprintf_s(errBuf, "CreateFileW dump failed gle=%lu\n", GetLastError());
        OutputDebugStringA(errBuf);
        WriteSidecarNote(dumpPath, errBuf);
        return;
    }

    MINIDUMP_EXCEPTION_INFORMATION exInfo;
    MINIDUMP_EXCEPTION_INFORMATION* pExInfo = NULL;
    if (ExceptionInfo != NULL) {
        exInfo.ThreadId = crashedThreadId ? crashedThreadId : GetCurrentThreadId();
        exInfo.ExceptionPointers = ExceptionInfo;
        exInfo.ClientPointers = FALSE;
        pExInfo = &exInfo;
    }

    const MINIDUMP_TYPE dumpType = static_cast<MINIDUMP_TYPE>(
                MiniDumpNormal
                | MiniDumpWithThreadInfo
                | MiniDumpWithUnloadedModules
                | MiniDumpWithIndirectlyReferencedMemory
                | MiniDumpIgnoreInaccessibleMemory);

    const BOOL ok = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
                                      dumpType, pExInfo, NULL, NULL);
    const DWORD gle = ok ? 0 : GetLastError();
    FlushFileBuffers(hFile);
    CloseHandle(hFile);

    char note[256];
    if (ok) {
        sprintf_s(note,
                  "MiniDumpWriteDump OK\r\npath written beside this txt\r\n");
        OutputDebugStringA("MiniDumpWriteDump OK\n");
    } else {
        sprintf_s(note, "MiniDumpWriteDump FAILED gle=%lu\r\n", gle);
        OutputDebugStringA(note);
        DeleteFileW(dumpPath);
    }
    WriteSidecarNote(dumpPath, note);
}

unsigned long __stdcall CCreateDump::DumpWriterThread(void* param)
{
    DumpThreadParam* req = static_cast<DumpThreadParam*>(param);
    if (req == NULL) {
        return 1;
    }
    if (req->hasException) {
        WriteMiniDumpOnThread(&req->pointers, req->crashedThreadId);
    } else {
        WriteMiniDumpOnThread(NULL, req->crashedThreadId);
    }
    if (req->doneEvent) {
        SetEvent(req->doneEvent);
    }
    return 0;
}

void CCreateDump::WriteMiniDump(_EXCEPTION_POINTERS* ExceptionInfo)
{
    if (InterlockedCompareExchange(&s_dumping, 1, 0) != 0) {
        return;
    }

    // 静态参数：栈溢出时崩溃线程栈不可靠，避免再在坏栈上放大结构体。
    static DumpThreadParam s_req;
    ZeroMemory(&s_req, sizeof(s_req));
    s_req.hasException = false;
    s_req.crashedThreadId = GetCurrentThreadId();
    s_req.doneEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

    if (ExceptionInfo != NULL && ExceptionInfo->ExceptionRecord != NULL
            && ExceptionInfo->ContextRecord != NULL) {
        s_req.exceptionRecord = *ExceptionInfo->ExceptionRecord;
        s_req.exceptionRecord.ExceptionRecord = NULL;
        s_req.context = *ExceptionInfo->ContextRecord;
        s_req.pointers.ExceptionRecord = &s_req.exceptionRecord;
        s_req.pointers.ContextRecord = &s_req.context;
        s_req.hasException = true;
    }

    HANDLE thread = CreateThread(NULL, 256 * 1024, DumpWriterThread, &s_req, 0, NULL);
    if (thread == NULL) {
        if (s_req.hasException) {
            WriteMiniDumpOnThread(&s_req.pointers, s_req.crashedThreadId);
        } else {
            WriteMiniDumpOnThread(NULL, s_req.crashedThreadId);
        }
    } else {
        const DWORD wait = WaitForSingleObject(thread, kDumpWaitMs);
        if (wait == WAIT_TIMEOUT) {
            OutputDebugStringA("DumpWriterThread timeout\n");
        }
        CloseHandle(thread);
    }

    if (s_req.doneEvent) {
        CloseHandle(s_req.doneEvent);
        s_req.doneEvent = NULL;
    }
    // 允许后续再次落盘（首轮 VEH 若碰上可恢复异常，不能永久占坑）
    InterlockedExchange(&s_dumping, 0);
}

void CCreateDump::ForceWriteMiniDump(_EXCEPTION_POINTERS* ExceptionInfo)
{
    WriteMiniDump(ExceptionInfo);
}

long CCreateDump::UnhandleExceptionFilter(_EXCEPTION_POINTERS* ExceptionInfo)
{
    // Qt/CRT 可能再次覆盖过滤器，崩溃时顺手挂回
    SetUnhandledExceptionFilter(UnhandleExceptionFilter);
    WriteMiniDump(ExceptionInfo);
    return EXCEPTION_EXECUTE_HANDLER;
}

long CCreateDump::VectoredExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo)
{
    if (ExceptionInfo == NULL || ExceptionInfo->ExceptionRecord == NULL) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    if (IsDebuggerPresent()) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    const DWORD code = ExceptionInfo->ExceptionRecord->ExceptionCode;
    if (IsFatalExceptionCode(code)) {
        // 首轮就落盘，避免后续过滤器被清掉后没 dump
        SetUnhandledExceptionFilter(UnhandleExceptionFilter);
        WriteMiniDump(ExceptionInfo);
    }
    return EXCEPTION_CONTINUE_SEARCH;
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
    if (IsFatalExceptionCode(code) || code == 0xE06D7363) {
        SetUnhandledExceptionFilter(UnhandleExceptionFilter);
        WriteMiniDump(ExceptionInfo);
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
            OutputDebugStringA("Failed to init CrashDumps directory\n");
            return false;
        }

        ULONG stackSize = 128 * 1024;
        SetThreadStackGuarantee(&stackSize);

        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
        ReinstallFilter();

        if (s_vectoredException == NULL) {
            s_vectoredException = AddVectoredExceptionHandler(1, VectoredExceptionHandler);
        }
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
        OutputDebugStringA("Unknown exception in DeclarDumpFile\n");
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
