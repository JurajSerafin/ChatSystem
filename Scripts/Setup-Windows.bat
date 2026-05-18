@echo off
setlocal enabledelayedexpansion


:check_tools
    where ninja >nul 2>&1
    if ERRORLEVEL 1 (
        echo [WARNING] Ninja not found (optional)
        set USE_NINJA=0
    ) else (
        set USE_NINJA=1
    )
goto :eof


:setup_vcpkg
    echo.
    echo Initializing vcpkg submodule...
    git submodule update --init
    call vcpkg\bootstrap-vcpkg.bat
    set VCPKG_ROOT=%cd%\vcpkg
    echo VCPKG_ROOT=%VCPKG_ROOT%
goto :eof


:configure_project
    echo.
    echo Configuring project (first build: ~25-35 minutes)...
    cmake --preset=windows-debug

    if %ERRORLEVEL% NEQ 0 (
        echo [ERROR] Configuration failed
        if not "%NO_PAUSE%"=="nopause" pause
        exit /b 1
    )
goto :eof


:print_summary
    echo.
    echo ==========================================
    echo Setup complete!
    echo ==========================================
    echo.
    echo Next steps:
    echo.
    echo This script must be run from:
    echo     "x64 Native Tools Command Prompt for VS 2022"
    echo.
    echo Build:           cmake --build Build\windows-debug --config Debug
    echo Run application: Build\windows-debug\App\ChatSystem.exe
    echo Run tests:       ctest --test-dir Build\windows-debug -C Debug
    echo Run server:      Build\windows-debug\Server\ChatSystemServer.exe
    echo.
goto :eof


echo ==========================================
echo ChatSystem - Windows Setup
echo ==========================================
echo.

call :check_tools
call :setup_vcpkg
call :configure_project
call :print_summary

pause
exit /b 0
