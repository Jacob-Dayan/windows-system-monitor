#!/usr/bin/env bash
set -euo pipefail

CLANG_TARGET="${CLANG_TARGET:-x86_64-w64-windows-gnu}"

echo "Building monitor.exe (static, clang++)..."

clang++ -std=c++20 -O3 -flto=thin -pipe \
    -funroll-loops -fstrict-aliasing -ffast-math -finline-functions \
    -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions \
    -fomit-frame-pointer -fmerge-all-constants -fno-ident -DNDEBUG \
    --target="${CLANG_TARGET}" \
    src/*.cpp \
    -o monitor.exe \
    -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 \
    -static -static-libgcc -static-libstdc++ \
    -Wl,--gc-sections -Wl,--strip-all -s -Wl,--exclude-libs,ALL "$@"

echo "Done."
