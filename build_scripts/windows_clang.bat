@echo off
setlocal
pushd "%~dp0\.."

echo Building system_monitor.exe with LLVM Clang++...

where clang++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] clang++ was not found in your PATH.
    echo Please install LLVM Clang or add it to PATH.
    pause
    popd
    exit /b 1
)

clang++ -O3 -std=c++20 -Wall -Wextra src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp -o system_monitor.exe -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [OK] Build succeeded: system_monitor.exe
    popd
    exit /b 0
) else (
    echo.
    echo [ERROR] Build failed with error code %ERRORLEVEL%.
    pause
    popd
    exit /b %ERRORLEVEL%
)
