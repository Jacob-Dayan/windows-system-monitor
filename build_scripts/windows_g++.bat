@echo off
setlocal
pushd "%~dp0.."

echo Building monitor.exe with G++ (MinGW-w64)...

where g++ >NUL 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] g++ was not found in your PATH.
    echo Please install MinGW-w64 or add it to PATH.
    popd
    exit /b 1
)

g++ -O3 -std=c++20 -Wall -Wextra -Wpedantic -DNDEBUG src\main.cpp src\logic.cpp src\interaction.cpp src\style.cpp -o monitor.exe -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 -static -static-libgcc -static-libstdc++ %*

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [OK] Build succeeded: monitor.exe
    popd
    exit /b 0
) else (
    echo.
    echo [ERROR] Build failed with error code %ERRORLEVEL%.
    popd
    exit /b %ERRORLEVEL%
)
