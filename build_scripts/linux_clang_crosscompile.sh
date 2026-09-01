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
CLANG_TARGET="${CLANG_TARGET:-x86_64-w64-windows-gnu}"

if ! command -v clang++ &>/dev/null; then
    printf "${BOLD}${RED}Error:${RESET} clang++ was not found in PATH.\n" >&2
    printf "Install on Debian/Ubuntu with:\n  ${CYAN}sudo apt update && sudo apt install -y clang llvm${RESET}\n" >&2
    exit 1
fi

printf "${BOLD}${CYAN}==> Building %s with Clang++ (target: %s)...${RESET}\n" "${TARGET}" "${CLANG_TARGET}"

clang++ -std=c++20 -O3 -Wall -Wextra -Wpedantic -DNDEBUG \
    --target="${CLANG_TARGET}" \
    src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp \
    -o "${TARGET}" \
    -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32 "$@"

rm -f *.obj src/*.gch 2>/dev/null || true

printf "${BOLD}${GREEN}✔ Build succeeded:${RESET} %s\n" "${TARGET}"
