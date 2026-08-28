#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

BOLD=$'\033[1m'
RESET=$'\033[0m'
CYAN=$'\033[36m'
GREEN=$'\033[32m'
RED=$'\033[31m'

TOOLCHAIN="auto"
BUILD_MODE="release"
STATIC_LINK=true
ZIG_TARGET="x86_64-windows-gnu"
TARGET_OUTPUT="system_monitor.exe"
SRCS=(
    "src/main.cpp"
    "src/logic.cpp"
    "src/interaction.cpp"
    "src/style.cpp"
)
LIBS=(
    "-lpsapi"
    "-liphlpapi"
    "-lws2_32"
    "-ldxgi"
    "-ldxguid"
    "-lole32"
)

show_help() {
    printf "${BOLD}Usage:${RESET} ./build.sh [options]\n\n"
    printf "Options:\n"
    printf "  -a, --auto             Auto-detect compiler (default)\n"
    printf "  -m, --mingw            Use MinGW-w64 GCC (x86_64-w64-mingw32-g++)\n"
    printf "  -z, --zig              Use Zig C++\n"
    printf "  -c, --clang            Use Clang++\n"
    printf "  -d, --debug            Debug build (-g -O0 -DDEBUG)\n"
    printf "  -r, --release          Release build (-O3 -DNDEBUG) (default)\n"
    printf "  --static               Static runtime linking (default)\n"
    printf "  --no-static            Dynamic runtime linking\n"
    printf "  --target <triple>      Target triple (default: ${ZIG_TARGET})\n"
    printf "  --clean                Clean build artifacts\n"
    printf "  -h, --help             Show this message\n"
}

clean_build() {
    rm -rf build system_monitor.exe monitor.exe *.obj *.o src/*.gch *.d *.pdb
    printf "${BOLD}${GREEN}✔ Clean complete.${RESET}\n"
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -a|--auto)
            TOOLCHAIN="auto"
            shift
            ;;
        -m|--mingw)
            TOOLCHAIN="mingw"
            shift
            ;;
        -z|--zig)
            TOOLCHAIN="zig"
            shift
            ;;
        -c|--clang)
            TOOLCHAIN="clang"
            shift
            ;;
        -d|--debug)
            BUILD_MODE="debug"
            shift
            ;;
        -r|--release)
            BUILD_MODE="release"
            shift
            ;;
        --static)
            STATIC_LINK=true
            shift
            ;;
        --no-static)
            STATIC_LINK=false
            shift
            ;;
        --target)
            if [[ $# -lt 2 ]]; then
                printf "${BOLD}${RED}Error: --target requires a value${RESET}\n" >&2
                exit 1
            fi
            ZIG_TARGET="$2"
            shift 2
            ;;
        --clean)
            clean_build
            exit 0
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            printf "${BOLD}${RED}Unknown option: %s${RESET}\n" "$1" >&2
            show_help
            exit 1
            ;;
    esac
done

SELECTED_CMD=""
SELECTED_NAME=""

detect_toolchain() {
    if command -v x86_64-w64-mingw32-g++ &>/dev/null; then
        SELECTED_CMD="x86_64-w64-mingw32-g++"
        SELECTED_NAME="MinGW-w64 GCC"
        return 0
    elif command -v x86_64-w64-mingw32-c++ &>/dev/null; then
        SELECTED_CMD="x86_64-w64-mingw32-c++"
        SELECTED_NAME="MinGW-w64 C++"
        return 0
    elif command -v zig &>/dev/null; then
        SELECTED_CMD="zig c++ -target ${ZIG_TARGET}"
        SELECTED_NAME="Zig C++ (${ZIG_TARGET})"
        return 0
    elif command -v clang++ &>/dev/null; then
        SELECTED_CMD="clang++ --target=x86_64-w64-windows-gnu"
        SELECTED_NAME="Clang++ Cross"
        return 0
    elif [[ "$(uname -s 2>/dev/null)" =~ ^(MINGW|MSYS|CYGWIN) ]] && command -v g++ &>/dev/null; then
        SELECTED_CMD="g++"
        SELECTED_NAME="MinGW GCC"
        return 0
    fi
    return 1
}

if [[ "$TOOLCHAIN" == "auto" ]]; then
    if ! detect_toolchain; then
        printf "${BOLD}${RED}Error: No Windows cross-compiler found.${RESET}\n" >&2
        printf "Install MinGW-w64: sudo apt install g++-mingw-w64-x86-64\n" >&2
        exit 1
    fi
elif [[ "$TOOLCHAIN" == "mingw" ]]; then
    if command -v x86_64-w64-mingw32-g++ &>/dev/null; then
        SELECTED_CMD="x86_64-w64-mingw32-g++"
        SELECTED_NAME="MinGW-w64 GCC"
    elif command -v x86_64-w64-mingw32-c++ &>/dev/null; then
        SELECTED_CMD="x86_64-w64-mingw32-c++"
        SELECTED_NAME="MinGW-w64 C++"
    else
        printf "${BOLD}${RED}Error: x86_64-w64-mingw32-g++ not found.${RESET}\n" >&2
        exit 1
    fi
elif [[ "$TOOLCHAIN" == "zig" ]]; then
    if command -v zig &>/dev/null; then
        SELECTED_CMD="zig c++ -target ${ZIG_TARGET}"
        SELECTED_NAME="Zig C++ (${ZIG_TARGET})"
    else
        printf "${BOLD}${RED}Error: zig not found in PATH.${RESET}\n" >&2
        exit 1
    fi
elif [[ "$TOOLCHAIN" == "clang" ]]; then
    if command -v clang++ &>/dev/null; then
        SELECTED_CMD="clang++ --target=x86_64-w64-windows-gnu"
        SELECTED_NAME="Clang++"
    else
        printf "${BOLD}${RED}Error: clang++ not found in PATH.${RESET}\n" >&2
        exit 1
    fi
fi

FLAGS=("-std=c++20" "-Wall" "-Wextra" "-Wpedantic")

if [[ "$BUILD_MODE" == "debug" ]]; then
    FLAGS+=("-g" "-O0" "-DDEBUG")
else
    FLAGS+=("-O3" "-DNDEBUG")
fi

if [[ "$STATIC_LINK" == true && "$SELECTED_NAME" =~ MinGW ]]; then
    FLAGS+=("-static" "-static-libgcc" "-static-libstdc++")
fi

printf "Building %s using %s (%s)...\n" "$TARGET_OUTPUT" "$SELECTED_NAME" "$BUILD_MODE"
$SELECTED_CMD "${FLAGS[@]}" "${SRCS[@]}" -o "$TARGET_OUTPUT" "${LIBS[@]}"

rm -f *.obj src/*.gch 2>/dev/null || true
printf "${BOLD}${GREEN}✔ Build succeeded:${RESET} %s\n" "$TARGET_OUTPUT"
