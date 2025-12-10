@echo off
setlocal enabledelayedexpansion

echo ==========================================
echo ChatSystem - Windows Setup
echo ==========================================
echo.

REM Check tools
where ninja >nul 2>&1 || (
    echo [WARNING] Ninja not found (optional)
    set USE_NINJA=0
) || set USE_NINJA=1

REM Install vcpkg
echo.
echo Setting up vcpkg...
if not exist "vcpkg" (
    git clone https://github.com/microsoft/vcpkg.git
    call vcpkg\bootstrap-vcpkg.bat
) else (
    cd vcpkg && git pull && call bootstrap-vcpkg.bat && cd ..
)

set VCPKG_ROOT=%cd%\vcpkg
echo VCPKG_ROOT=%VCPKG_ROOT%

REM Configure
echo.
echo Configuring project (first build: ~25-35 minutes)...
cmake --preset=debug

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ==========================================
    echo Setup complete!
    echo ==========================================
    echo.
    echo Build: cmake --build Build\debug
    echo Run:   Build\debug\App\ChatSystem.exe
) else (
    echo [ERROR] Configuration failed
    pause & exit /b 1
)

pause