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
COMPILER=""

if command -v x86_64-w64-mingw32-g++ &>/dev/null; then
    COMPILER="x86_64-w64-mingw32-g++"
elif command -v x86_64-w64-mingw32-c++ &>/dev/null; then
    COMPILER="x86_64-w64-mingw32-c++"
elif [[ "$(uname -s 2>/dev/null)" =~ ^(MINGW|MSYS|CYGWIN) ]] && command -v g++ &>/dev/null; then
    COMPILER="g++"
else
    printf "${BOLD}${RED}Error:${RESET} MinGW cross-compiler was not found in PATH.\n" >&2
    printf "Install on Debian/Ubuntu with:\n  ${CYAN}sudo apt update && sudo apt install -y g++-mingw-w64-x86-64${RESET}\n" >&2
    exit 1
fi

printf "${BOLD}${CYAN}==> Building %s with MinGW GCC (%s)...${RESET}\n" "${TARGET}" "${COMPILER}"

"${COMPILER}" -std=c++20 -O3 -Wall -Wextra -Wpedantic -DNDEBUG \
    src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp \
    -o "${TARGET}" \
    -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 \
    -static -static-libgcc -static-libstdc++ "$@"

rm -f *.obj src/*.gch 2>/dev/null || true

printf "${BOLD}${GREEN}✔ Build succeeded:${RESET} %s\n" "${TARGET}"
