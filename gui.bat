@echo off
cd /d "%~dp0"
if not exist genomica.exe call compilar.bat
if not exist genomica.exe exit /b 1
where py >nul 2>nul
if %errorlevel%==0 (py gui\app.py) else (python gui\app.py)
if errorlevel 1 pause
