@echo off
chcp 65001 >nul
REM 仅改逻辑：.cpp / .h（未改 .ui、.qss、.qrc、Coagulation.pro）
REM 最快：只增量 nmake，不跑 qmake
cd /d "%~dp0"
call "scripts\_coag_release_env.bat"
if errorlevel 1 exit /b 1

if not exist "%COAG_OUT%\Makefile" (
  echo [提示] 构建目录未初始化，请先运行 build_logic_ui_release.bat
  exit /b 10
)
cd /d "%COAG_OUT%"

where jom >nul 2>&1
if errorlevel 1 (
  echo ===== [逻辑] nmake release ^(单线程，较慢；可安装 jom 到 PATH^) =====
  nmake release
) else (
  echo ===== [逻辑] jom release ^(-j %NUMBER_OF_PROCESSORS%^) =====
  jom -j %NUMBER_OF_PROCESSORS% release
)
if errorlevel 1 exit /b 3

call "%~dp0scripts\_coag_release_post.bat"
exit /b %errorlevel%
