#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/../.."

CXX="${CXX:-x86_64-w64-mingw32-g++}"

echo "Building monitor.exe (static, g++)..."

${CXX} -std=c++20 -O3 -flto=auto -fno-fat-lto-objects \
    -ffunction-sections -fdata-sections -fno-rtti -fomit-frame-pointer -fmerge-all-constants -DNDEBUG \
    src/*.cpp \
    -o monitor.exe \
    -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 \
    -static -static-libgcc -static-libstdc++ \
    -Wl,--gc-sections -Wl,--strip-all -s -Wl,--exclude-libs,ALL "$@"

echo "Done."
