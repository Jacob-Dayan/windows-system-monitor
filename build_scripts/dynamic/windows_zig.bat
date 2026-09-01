@echo off
setlocal enabledelayedexpansion
pushd "%~dp0..\.."

set "SRCS="
for %%f in (src\*.cpp) do set "SRCS=!SRCS! %%f"

echo Building monitor.exe (dynamic, zig)...

zig c++ -std=c++20 -O3 -ffunction-sections -fdata-sections -fno-rtti -fomit-frame-pointer -DNDEBUG -Xclang -Wno-nullability-completeness -target x86_64-windows-gnu !SRCS! -o monitor.exe -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 -Wl,--gc-sections -Wl,--strip-all %*

popd
