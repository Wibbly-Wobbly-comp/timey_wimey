@echo off
setlocal
cd /d "%~dp0"

REM Permite localizar as DLLs da Raylib instalada pelo MSYS2 no caminho padrao.
if exist "C:\msys64\ucrt64\bin" set "PATH=C:\msys64\ucrt64\bin;%PATH%"

if not exist "timey_wimey.exe" (
    echo O jogo ainda nao foi compilado.
    echo Execute compilar_windows.bat primeiro.
    pause
    exit /b 1
)

start "Timey Wimey" timey_wimey.exe
