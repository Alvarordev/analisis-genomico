@echo off
cd /d "%~dp0"
echo Compilando genomica.exe ...
g++ -std=c++17 -O2 -Wall src\main.cpp src\parser\LectorFasta.cpp src\linearProbing\TablaHash.cpp src\doubleHashing\TablaKmers.cpp src\maxHeap\MaxHeap.cpp src\grafo\GrafoADN.cpp src\modoGUI\ModoGUI.cpp -static -o genomica.exe
if errorlevel 1 (
    echo.
    echo No se pudo compilar. Revisa que g++ este instalado y en el PATH (ver README^).
    pause
    exit /b 1
)
echo Listo: genomica.exe
