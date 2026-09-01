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

TARGET="monitor.exe"
ZIG_TARGET="${ZIG_TARGET:-x86_64-windows-gnu}"

if ! command -v zig &>/dev/null; then
    printf "${BOLD}${RED}Error:${RESET} Zig compiler was not found in PATH.\n" >&2
    printf "Download Zig from: ${CYAN}https://ziglang.org/download/${RESET}\n" >&2
    exit 1
fi

printf "${BOLD}${CYAN}==> Building %s with Zig C++ (target: %s)...${RESET}\n" "${TARGET}" "${ZIG_TARGET}"

zig c++ -std=c++20 -O3 -Wall -Wextra -Wpedantic -DNDEBUG \
    -target "${ZIG_TARGET}" \
    src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp \
    -o "${TARGET}" \
    -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 "$@"

rm -f *.obj src/*.gch 2>/dev/null || true

printf "${BOLD}${GREEN}✔ Build succeeded:${RESET} %s\n" "${TARGET}"
