#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/../.."

TARGET="${ZIG_TARGET:-x86_64-windows-gnu}"

echo "Building monitor.exe (dynamic, zig)..."

zig c++ -std=c++20 -O3 \
    -ffunction-sections -fdata-sections -fno-rtti -fomit-frame-pointer -DNDEBUG \
    -Xclang -Wno-nullability-completeness \
    -target "${TARGET}" \
    src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp \
    -o monitor.exe \
    -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 \
    -Wl,--gc-sections -Wl,--strip-all "$@"

echo "Done."
