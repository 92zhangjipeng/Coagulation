@echo off
REM 兼容旧习惯：等同于「逻辑+UI」全量配置编译
cd /d "%~dp0"
call build_logic_ui_release.bat
exit /b %errorlevel%
