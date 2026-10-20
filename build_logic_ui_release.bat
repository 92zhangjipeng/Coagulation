@echo off
chcp 65001 >nul
REM 逻辑 + 界面 / 工程结构：改了 Coagulation.pro、新增窗体或源文件、或不确定时
REM 完整 qmake + nmake（首次搭建 C:\coag_msvc_out 也用本脚本）
cd /d "%~dp0"
call "scripts\_coag_release_env.bat"
if errorlevel 1 exit /b 1

if not exist "%COAG_OUT%" mkdir "%COAG_OUT%"
cd /d "%COAG_OUT%"

echo ===== [逻辑+UI] qmake RELEASE =====
qmake "%COAG_SRC%\Coagulation.pro" %COAG_QMAKE_ARGS%
if errorlevel 1 exit /b 2

where jom >nul 2>&1
if errorlevel 1 (
  echo ===== [逻辑+UI] nmake release =====
  nmake release
) else (
  echo ===== [逻辑+UI] jom release ^(-j %NUMBER_OF_PROCESSORS%^) =====
  jom -j %NUMBER_OF_PROCESSORS% release
)
if errorlevel 1 exit /b 3

call "%~dp0scripts\_coag_release_post.bat"
exit /b %errorlevel%
