@echo off
setlocal
cd /d "%~dp0"

REM Permite localizar as DLLs instaladas pelo MSYS2 no caminho padrao.
if exist "C:\msys64\ucrt64\bin" set "PATH=C:\msys64\ucrt64\bin;%PATH%"

if not exist "gerenciador.exe" (
    echo O gerenciador ainda nao foi compilado.
    echo Execute compilar_windows.bat primeiro.
    pause
    exit /b 1
)

gerenciador.exe
pause
