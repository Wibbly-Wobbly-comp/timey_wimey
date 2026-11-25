@echo off
setlocal
cd /d "%~dp0"

REM Caminhos padrao da instalacao do MSYS2. Ajuste se instalou em outra pasta.
if exist "C:\msys64\ucrt64\bin" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if exist "C:\msys64\usr\bin" set "PATH=C:\msys64\usr\bin;%PATH%"

echo ================================================
echo       TIMEY WIMEY - COMPILACAO WINDOWS
echo ================================================

where gcc >nul 2>nul
if errorlevel 1 (
    echo ERRO: GCC nao encontrado.
    echo Instale o MSYS2 e o pacote mingw-w64-ucrt-x86_64-gcc.
    pause
    exit /b 1
)

where make >nul 2>nul
if errorlevel 1 (
    echo ERRO: Make nao encontrado.
    echo Abra o terminal MSYS2 UCRT64 e instale o pacote make.
    pause
    exit /b 1
)

make clean
if errorlevel 1 goto erro
make
if errorlevel 1 goto erro

echo.
echo Compilacao concluida. Executaveis criados:
echo   timey_wimey.exe
echo   gerenciador.exe
echo.
pause
exit /b 0

:erro
echo.
echo A compilacao falhou. Confira as mensagens acima.
pause
exit /b 1
