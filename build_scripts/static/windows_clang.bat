@echo off
setlocal enabledelayedexpansion

set "SRCS="
for %%f in (src\*.cpp) do set "SRCS=!SRCS! %%f"

echo Building monitor.exe (static, clang++)...

clang++ -std=c++20 -O3 -flto=thin -pipe ^
    -funroll-loops -fstrict-aliasing -ffast-math -finline-functions ^
    -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions ^
    -fomit-frame-pointer -fmerge-all-constants -fno-ident -DNDEBUG ^
    !SRCS! -o monitor.exe ^
    -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 ^
    -static -static-libgcc -static-libstdc++ ^
    -Wl,--gc-sections -Wl,--strip-all -s -Wl,--exclude-libs,ALL %*
if errorlevel 1 exit /b %errorlevel%

echo Done.
