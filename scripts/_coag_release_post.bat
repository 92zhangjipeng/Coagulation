@echo off
if not exist "release\Coagulation.exe" (
  echo EXE_MISSING
  exit /b 4
)
copy /y "%COAG_SRC%\deps\UsbCodeDispose\bin\release\UsbCodeDispose.dll" "release\" >nul
echo BUILD_OK
dir "release\Coagulation.exe"
exit /b 0
