@echo off
setlocal
pushd "%~dp0..\.."

echo Building monitor.exe (dynamic, g++)...

g++ -std=c++20 -O3 -flto=auto -fno-fat-lto-objects -ffunction-sections -fdata-sections -fno-rtti -fomit-frame-pointer -fmerge-all-constants -DNDEBUG src\main.cpp src\logic.cpp src\interaction.cpp src\style.cpp -o monitor.exe -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 -Wl,--gc-sections -Wl,--strip-all -s -Wl,--as-needed %*

popd
