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

private:
    CCreateDump();
    ~CCreateDump();

    static long __stdcall UnhandleExceptionFilter(_EXCEPTION_POINTERS* ExceptionInfo);
    static long __stdcall VectoredContinueHandler(_EXCEPTION_POINTERS* ExceptionInfo);
    static void __cdecl SignalAbortHandler(int sig);
    static void __cdecl InvalidParameterHandler(const wchar_t* expression,
                                                const wchar_t* function,
                                                const wchar_t* file,
                                                unsigned int line,
                                                uintptr_t pReserved);
    static void __cdecl PureCallHandler();
    static void TerminateHandler();
    static void WriteMiniDump(_EXCEPTION_POINTERS* ExceptionInfo);
    static bool InitDumpDirectory();

    static CCreateDump* __instance;
    static wchar_t s_dumpDir[260];
    static volatile long s_dumping;
    static void* s_vectoredContinue;
};
