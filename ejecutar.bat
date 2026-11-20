@echo off
cd /d "%~dp0"
if not exist genomica.exe call compilar.bat
if not exist genomica.exe exit /b 1
genomica.exe data\HIV2_ALL_2021_env_DNA.2000.fasta
pause
