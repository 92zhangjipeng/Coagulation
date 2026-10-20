@echo off
chcp 65001 >nul
REM 仅改界面：.ui / .qss / .qrc / Picture 资源（一般不必改 .pro）
REM 先 qmake 刷新 uic/rcc 规则，再优先编 UI/资源目标，最后链接
cd /d "%~dp0"
call "scripts\_coag_release_env.bat"
if errorlevel 1 exit /b 1

if not exist "%COAG_OUT%\Makefile" (
  echo [提示] 构建目录未初始化，请先运行 build_logic_ui_release.bat
  exit /b 10
)
cd /d "%COAG_OUT%"

echo ===== [UI] qmake RELEASE =====
qmake "%COAG_SRC%\Coagulation.pro" %COAG_QMAKE_ARGS%
if errorlevel 1 exit /b 2

echo ===== [UI] uic / rcc =====
nmake compiler_uic_make_all compiler_rcc_make_all
if errorlevel 1 (
  echo [提示] uic/rcc 专用目标不可用，改为完整 nmake release
  nmake release
  if errorlevel 1 exit /b 3
  goto :post
)

echo ===== [UI] nmake release =====
nmake release
if errorlevel 1 exit /b 3

:post
call "%~dp0scripts\_coag_release_post.bat"
exit /b %errorlevel%
