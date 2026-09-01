#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/../.."

TARGET="${CLANG_TARGET:-x86_64-w64-windows-gnu}"

echo "Building monitor.exe (static, clang++)..."

clang++ -std=c++20 -O3 -flto=thin \
    -ffunction-sections -fdata-sections -fno-rtti -fomit-frame-pointer -fmerge-all-constants -DNDEBUG \
    --target="${TARGET}" \
    src/*.cpp \
    -o monitor.exe \
    -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 \
    -static -static-libgcc -static-libstdc++ \
    -Wl,--gc-sections -Wl,--strip-all -s -Wl,--exclude-libs,ALL "$@"

echo "Done."
