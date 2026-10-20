@echo off
REM Rebuild UsbCodeDispose (MSVC2015 x64) and sync into Coagulation\deps\UsbCodeDispose
call "C:\VS\VC\vcvarsall.bat" x64 || exit /b 1
set PATH=C:\Qt\Qt5.10.0\5.10.0\msvc2015_64\bin;%PATH%
set SRC=C:\USBdispose\UsbCodeDispose
set DEST=%~dp0deps\UsbCodeDispose

cd /d "%SRC%"
qmake UsbCodeDispose.pro -spec win32-msvc "CONFIG+=debug" "CONFIG-=release" || exit /b 2
nmake || exit /b 3
qmake UsbCodeDispose.pro -spec win32-msvc "CONFIG+=release" "CONFIG-=debug" || exit /b 4
nmake || exit /b 5

mkdir "%DEST%\include" 2>nul
mkdir "%DEST%\lib\debug" 2>nul
mkdir "%DEST%\lib\release" 2>nul
mkdir "%DEST%\bin\debug" 2>nul
mkdir "%DEST%\bin\release" 2>nul
copy /y "%SRC%\usbcodedispose.h" "%DEST%\include\"
copy /y "%SRC%\usbcodedispose_global.h" "%DEST%\include\"
copy /y "%SRC%\debug\UsbCodeDispose.dll" "%DEST%\bin\debug\"
copy /y "%SRC%\debug\UsbCodeDispose.lib" "%DEST%\lib\debug\"
copy /y "%SRC%\debug\UsbCodeDispose.lib" "%DEST%\bin\debug\"
copy /y "%SRC%\release\UsbCodeDispose.dll" "%DEST%\bin\release\"
copy /y "%SRC%\release\UsbCodeDispose.lib" "%DEST%\lib\release\"
echo SYNC_OK
