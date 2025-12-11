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
    echo Setting up vcpkg...
    if not exist "vcpkg" (
        git clone https://github.com/microsoft/vcpkg.git
        if ERRORLEVEL 1 (
            echo [ERROR] Failed to clone vcpkg
            pause
            exit /b 1
        )
        call vcpkg\bootstrap-vcpkg.bat
    ) else (
        pushd vcpkg
        git pull
        if ERRORLEVEL 1 echo [WARNING] Failed to pull latest vcpkg
        call bootstrap-vcpkg.bat
        popd
    )
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
    echo   1. Open ChatSystem.sln in Visual Studio, OR
    echo   2. Build from command line:
    echo.
    echo Build:           cmake --build Build\debug --config Debug
    echo Run application: Build\debug\App\Debug\ChatSystem.exe
    echo Run tests:       ctest --test-dir Build\debug -C Debug
    echo Run server:      Build\debug\Server\Debug\ChatSystemServer.exe
    echo.
    echo Or use Visual Studio:
    echo   - Open Build\debug\ChatSystem.sln
    echo   - Press F5 to build and run
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
