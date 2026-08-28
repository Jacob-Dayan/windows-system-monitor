#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "${ROOT_DIR}"

BOLD=$'\033[1m'
RESET=$'\033[0m'
CYAN=$'\033[36m'
GREEN=$'\033[32m'
RED=$'\033[31m'

TARGET="system_monitor.exe"
COMPILER="x86_64-w64-mingw32-g++"

if ! command -v "${COMPILER}" &>/dev/null; then
    printf "${BOLD}${RED}Error:${RESET} MinGW cross-compiler '${COMPILER}' was not found.\n" >&2
    printf "Install on Debian/Ubuntu with:\n  ${CYAN}sudo apt update && sudo apt install -y g++-mingw-w64-x86-64${RESET}\n" >&2
    exit 1
fi

printf "${BOLD}${CYAN}==> Building %s with MinGW GCC...${RESET}\n" "${TARGET}"

"${COMPILER}" -std=c++20 -O3 -Wall -Wextra \
    src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp \
    -o "${TARGET}" \
    -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 \
    -static -static-libgcc -static-libstdc++

rm -f *.obj src/*.gch 2>/dev/null || true

printf "${BOLD}${GREEN}✔ Build succeeded:${RESET} %s\n" "${TARGET}"
