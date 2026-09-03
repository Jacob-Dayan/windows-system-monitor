@echo off
setlocal enabledelayedexpansion

if not defined ZIG_TARGET set "ZIG_TARGET=x86_64-windows-gnu"

set "SRCS="
for %%f in (src\*.cpp) do set "SRCS=!SRCS! %%f"

echo Building monitor.exe (dynamic, zig)...

zig c++ -Wno-nullability-completeness -std=c++20 -O3 -pipe ^
    -funroll-loops -fstrict-aliasing -ffast-math ^
    -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions ^
    -fomit-frame-pointer -fmerge-all-constants -DNDEBUG ^
    -target %ZIG_TARGET% ^
    !SRCS! -o monitor.exe ^
    -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 ^
    -Wl,--gc-sections -Wl,--strip-all -s -Wl,--as-needed %*
if errorlevel 1 exit /b %errorlevel%

echo Done.
