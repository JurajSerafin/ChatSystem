@echo off
setlocal enabledelayedexpansion

set PRESET=%1
if "%PRESET%"=="" set PRESET=debug

set TARGET=%2
if "%TARGET%"=="" set TARGET=all

REM Get project root
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..
cd /d "%PROJECT_ROOT%"

REM Check if build directory exists
if not exist "Build\%PRESET%" (
    echo [WARNING] Build directory doesn't exist. Running configuration...
    cmake --preset=%PRESET%
    if %ERRORLEVEL% NEQ 0 (
        echo [ERROR] Configuration failed
        pause
        exit /b 1
    )
)

REM Detect number of processors
set CORES=%NUMBER_OF_PROCESSORS%

echo.
echo Building %TARGET% with %CORES% cores (preset: %PRESET%)...
echo.

REM Build
if "%TARGET%"=="all" (
    cmake --build Build\%PRESET% --parallel %CORES%
) else (
    cmake --build Build\%PRESET% --target %TARGET% --parallel %CORES%
)

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [OK] Build completed successfully!
    echo.
    echo Run application: Build\%PRESET%\App\ChatSystem.exe
    echo Run tests:       ctest --test-dir Build\%PRESET%
    echo Run server:      Build\%PRESET%\Server\ChatSystemServer.exe
) else (
    echo.
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

if "%3"=="nopause" goto :eof
pause