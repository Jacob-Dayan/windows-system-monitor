@echo off
setlocal
cd /d "%~dp0\.."

echo Building system_monitor.exe with Zig C++...
zig c++ -O3 -std=c++20 src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp -o system_monitor.exe -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid

if %ERRORLEVEL% EQU 0 (
    echo Build succeeded: system_monitor.exe
) else (
    echo Build failed with error code %ERRORLEVEL%.
    pause
)
