# Windows System Monitor

A lightweight terminal system monitor for Windows written in C++20.

Displays real-time CPU, RAM, GPU/VRAM, disk, network, and process metrics in an ANSI terminal dashboard.

## Features

- **CPU**: Usage percentage and core count.
- **RAM & Swap**: Physical memory and pagefile usage with history graph.
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

### Make

```bash
# Static build (default)
make

# Dynamic build
make STATIC=0

# Debug build
make debug

# Custom compiler
make CXX=x86_64-w64-mingw32-g++
```

### Build Scripts

> **Note**: The recommended tool for Unix cross-compilation is the [Zig toolchain](https://ziglang.org/download/) which you can install from [zig's official website](https://ziglang.org/download/), or using your package manager (homebrew, apt, winget, scoop, snap, pacman, and even pip(!))


Scripts are organized under `build_scripts/static/` and `build_scripts/dynamic/`:

- **Static builds:**
  - `build_scripts/static/linux_g++_crosscompile.sh`
  - `build_scripts/static/linux_clang_crosscompile.sh`
  - `build_scripts/static/linux_zig_crosscompile.sh`
  - `build_scripts/static/windows_g++.bat`
  - `build_scripts/static/windows_clang.bat`
  - `build_scripts/static/windows_zig.bat`
  - `build_scripts/static/windows_cl_visualstudio.ps1`

- **Dynamic builds:**
  - `build_scripts/dynamic/linux_g++_crosscompile.sh`
  - `build_scripts/dynamic/linux_clang_crosscompile.sh`
  - `build_scripts/dynamic/linux_zig_crosscompile.sh`
  - `build_scripts/dynamic/windows_g++.bat`
  - `build_scripts/dynamic/windows_clang.bat`
  - `build_scripts/dynamic/windows_zig.bat`
  - `build_scripts/dynamic/windows_cl_visualstudio.ps1`

### CMake

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Running

```cmd
./monitor.exe
```

## License

GNU General Public License v3.0 or later.
