@echo off
REM Shared MSVC2015 x64 + Qt 5.10 paths (junction build: coag_build_src -> repo)
call C:\VS\VC\vcvarsall.bat amd64
if errorlevel 1 exit /b 1
set "PATH=C:\Qt\Qt5.10.0\5.10.0\msvc2015_64\bin;%PATH%"
set "COAG_SRC=C:\coag_build_src"
set "COAG_OUT=C:\coag_msvc_out"
set "COAG_QMAKE_ARGS=-spec win32-msvc CONFIG+=release CONFIG-=debug CONFIG-=qml_debug"
exit /b 0
