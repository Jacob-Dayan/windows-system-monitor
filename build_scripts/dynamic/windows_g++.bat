@echo off
setlocal enabledelayedexpansion

set "SRCS="
for %%f in (src\*.cpp) do set "SRCS=!SRCS! %%f"

echo Building monitor.exe (dynamic, g++)...

g++ -std=c++20 -O3 -flto=auto -fno-fat-lto-objects -pipe ^
    -funroll-loops -fstrict-aliasing -ffast-math -finline-functions ^
    -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions ^
    -fomit-frame-pointer -fmerge-all-constants -fno-ident -DNDEBUG ^
    !SRCS! -o monitor.exe ^
    -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 ^
    -Wl,--gc-sections -Wl,--strip-all -s -Wl,--as-needed %*
if errorlevel 1 exit /b %errorlevel%

echo Done.
