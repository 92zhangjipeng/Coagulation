#pragma once
#include <cstdint>
#include <string>

struct _EXCEPTION_POINTERS;

class CCreateDump
{
public:
    static CCreateDump* Instance();
    bool DeclarDumpFile(const std::string& dmpFileName);
    bool ReinstallFilter();
    void Del_Instance();
    const wchar_t* dumpDirectory();

    /** 主动写一份转储（QtFatal / 主动捕获等），ExceptionInfo 可为 nullptr */
    static void ForceWriteMiniDump(_EXCEPTION_POINTERS* ExceptionInfo);

private:
    CCreateDump();
    ~CCreateDump();

    static long __stdcall UnhandleExceptionFilter(_EXCEPTION_POINTERS* ExceptionInfo);
    static long __stdcall VectoredExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo);
    static long __stdcall VectoredContinueHandler(_EXCEPTION_POINTERS* ExceptionInfo);
    static void __cdecl SignalAbortHandler(int sig);
    static void __cdecl InvalidParameterHandler(const wchar_t* expression,
                                                const wchar_t* function,
                                                const wchar_t* file,
                                                unsigned int line,
                                                uintptr_t pReserved);
    static void __cdecl PureCallHandler();
    static void TerminateHandler();

    static bool IsFatalExceptionCode(unsigned long code);
    static bool InitDumpDirectory();
    static bool DirectoryWritable(const wchar_t* dir);
    static void WriteMiniDump(_EXCEPTION_POINTERS* ExceptionInfo);
    static void WriteMiniDumpOnThread(_EXCEPTION_POINTERS* ExceptionInfo,
                                      unsigned long crashedThreadId);
    static unsigned long __stdcall DumpWriterThread(void* param);
    static void WriteSidecarNote(const wchar_t* dumpPath, const char* note);

    static CCreateDump* __instance;
    static wchar_t s_dumpDir[260];
    static volatile long s_dumping;
    static void* s_vectoredException;
    static void* s_vectoredContinue;
};
