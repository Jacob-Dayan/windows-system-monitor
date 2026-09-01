@echo off
setlocal enabledelayedexpansion
pushd "%~dp0..\.."

set "SRCS="
for %%f in (src\*.cpp) do set "SRCS=!SRCS! %%f"

echo Building monitor.exe (static, clang++)...

clang++ -std=c++20 -O3 -flto=thin -ffunction-sections -fdata-sections -fno-rtti -fomit-frame-pointer -fmerge-all-constants -DNDEBUG !SRCS! -o monitor.exe -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 -static -static-libgcc -static-libstdc++ -Wl,--gc-sections -Wl,--strip-all -s -Wl,--exclude-libs,ALL %*

popd
