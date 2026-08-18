@echo off
echo Building System Monitor from src/ with GCC...
g++ -O3 -std=c++20 src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp -o system_monitor.exe -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid
if %ERRORLEVEL% EQU 0 (
    echo.
    echo Successfully compiled system_monitor.exe!
    echo Run system_monitor.exe to launch.
    echo.
) else (
    echo.
    echo Compilation failed! Please inspect error messages above.
    pause
)
