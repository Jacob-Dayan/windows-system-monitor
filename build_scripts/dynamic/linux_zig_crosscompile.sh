#!/usr/bin/env bash
set -euo pipefail

ZIG_TARGET="${ZIG_TARGET:-x86_64-windows-gnu}"

echo "Building monitor.exe (dynamic, zig)..."

zig c++ -Wno-nullability-completeness -std=c++20 -O3 -pipe \
    -funroll-loops -fstrict-aliasing -ffast-math \
    -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions \
    -fomit-frame-pointer -fmerge-all-constants -DNDEBUG \
    -target "${ZIG_TARGET}" \
    src/*.cpp \
    -o monitor.exe \
    -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 \
    -Wl,--gc-sections -Wl,--strip-all -s -Wl,--as-needed "$@"

echo "Done."
