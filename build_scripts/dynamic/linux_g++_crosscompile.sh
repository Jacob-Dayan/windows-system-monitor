#!/usr/bin/env bash
set -euo pipefail

CXX="${CXX:-x86_64-w64-mingw32-g++}"

echo "Building monitor.exe (dynamic, g++)..."

"${CXX}" -std=c++20 -O3 -flto=auto -fno-fat-lto-objects -pipe \
    -funroll-loops -fstrict-aliasing -ffast-math -finline-functions \
    -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions \
    -fomit-frame-pointer -fmerge-all-constants -fno-ident -DNDEBUG \
    src/*.cpp \
    -o monitor.exe \
    -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 \
    -Wl,--gc-sections -Wl,--strip-all -s -Wl,--as-needed "$@"

echo "Done."
