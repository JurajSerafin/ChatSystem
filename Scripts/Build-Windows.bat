@echo off
setlocal enabledelayedexpansion

goto :main

:parse_args
    set PRESET=%1
    if "%PRESET%"=="" set PRESET=windows-debug

    set CONFIG=%2
    if "%CONFIG%"=="" set CONFIG=Debug

    set TARGET=%3
    if "%TARGET%"=="" set TARGET=all

    set NO_PAUSE=%4
goto :eof


:ensure_build_dir
    if not exist "Build\%PRESET%" (
        echo [WARNING] Build directory doesn't exist. Running configuration...
        cmake --preset=%PRESET%
        if %ERRORLEVEL% NEQ 0 (
            echo [ERROR] Configuration failed
            if not "%NO_PAUSE%"=="nopause" pause
            exit /b 1
        )
    )
goto :eof


:detect_cores
    set CORES=%NUMBER_OF_PROCESSORS%
goto :eof


:do_build
    echo.
    echo Building %TARGET% [%CONFIG%] with %CORES% cores ^(preset: %PRESET%^)...
    echo.

    if "%TARGET%"=="all" (
        cmake --build Build\%PRESET% --config %CONFIG% --parallel %CORES%
    ) else (
        cmake --build Build\%PRESET% --target %TARGET% --config %CONFIG% --parallel %CORES%
    )

    if %ERRORLEVEL% NEQ 0 (
        echo.
        echo [ERROR] Build failed!
        if not "%NO_PAUSE%"=="nopause" pause
        exit /b 1
    )
goto :eof


:print_summary
    echo.
    echo [OK] Build completed successfully!
    echo.
    echo Run application: Build\%PRESET%\App\ChatSystem.exe
    echo Run tests:       ctest --test-dir Build\%PRESET%
    echo Run server:      Build\%PRESET%\Server\ChatSystemServer.exe
    echo.
goto :eof


:main 
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..
cd /d "%PROJECT_ROOT%"

call :parse_args %1 %2 %3 %4
call :ensure_build_dir
call :detect_cores
call :do_build
call :print_summary

if not "%NO_PAUSE%"=="nopause" pause
exit /b 0
