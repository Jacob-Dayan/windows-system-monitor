@echo off
setlocal
pushd "%~dp0..\.."

echo Building monitor.exe (static, zig)...

zig c++ -std=c++20 -O3 -ffunction-sections -fdata-sections -fno-rtti -fomit-frame-pointer -DNDEBUG -Xclang -Wno-nullability-completeness -target x86_64-windows-gnu src\main.cpp src\logic.cpp src\interaction.cpp src\style.cpp -o monitor.exe -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 -Wl,--gc-sections -Wl,--strip-all %*

popd
