# Windows System Monitor

A lightweight, real-time terminal system monitor for Windows written in modern C++20.

Renders CPU, memory, GPU/VRAM, disk, network, and process metrics in an ANSI dashboard with double-buffering.

![Version](https://img.shields.io/badge/version-0.1.2-blue.svg)
![Language](https://img.shields.io/badge/Language-C%2B%2B20-blue.svg)
![Compilers](https://img.shields.io/badge/Compilers-GCC%20%7C%20Clang%20%7C%20Zig-green.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%2F%2011%20x64-0078D6.svg)
![License](https://img.shields.io/badge/License-GPL_v3_or_later-blue.svg)

## Features

- **CPU**: Usage percentage via `GetSystemTimes` and core count.
- **RAM & Swap**: Physical memory and pagefile usage with live sparkline history.
- **GPU**: Dedicated VRAM metrics via DirectX DXGI.
- **Storage**: Logical drive capacity and usage bars.
- **Network**: Real-time throughput (Rx/Tx) via IP Helper API.
- **Processes**: Top 15 processes by memory usage.

## Keybinds

| Key | Action |
| --- | --- |
| `Tab` / `1`–`4` | Switch tab (*1: Overview*, *2: CPU/RAM/GPU*, *3: Storage/Net*, *4: Processes*) |
| `A` | Toggle high-memory audio alert |
| `P` | Pause/resume polling |
| `+` / `-` | Adjust refresh interval (250ms – 5000ms) |
| `R` | Force refresh |
| `Q` / `Esc` | Quit |

## Building

The build system automatically detects available compilers (MinGW-w64, Zig C++, Clang, or native GCC).

### Using Make

```bash
# Auto-detects toolchain (MinGW-w64, Zig, Clang, or GCC)
make

# Debug build
make debug

# Pass custom compiler or Zig target
make CXX=x86_64-w64-mingw32-g++
make CXX=zig ZIG_TARGET=x86_64-windows-gnu

# Clean
make clean
```

### Using Build Scripts

On Linux / WSL:
```bash
./build.sh          # Auto-detect compiler
./build.sh --zig    # Build using Zig C++
./build.sh --debug  # Build with debug symbols
```

On Windows:
```cmd
build.bat           # Auto-detect compiler
build.bat zig       # Build using Zig C++
build.bat debug     # Build with debug symbols
```

### Using CMake

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Running

```cmd
monitor.exe
```

## License

GNU General Public License v3.0 or later.
