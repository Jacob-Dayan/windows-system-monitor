# Windows System Monitor

A lightweight, real-time terminal system monitor for Windows, written in **C++20**.

Displays system metrics through a double-buffered, flicker-free ANSI terminal interface with tabbed views for CPU, memory, GPU/VRAM, storage, network traffic, and active processes.

![Language](https://img.shields.io/badge/Language-C%2B%2B20-blue.svg)
![Compilers](https://img.shields.io/badge/Compilers-GCC%20%7C%20Clang-green.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%2F%2011%20x64-0078D6.svg)
![License](https://img.shields.io/badge/License-GPL_v3_or_later-blue.svg)

---

## Features

- **CPU Metrics**: Overall CPU utilization calculation via `GetSystemTimes` and physical core count enumeration.
- **Physical RAM & Commit Memory**: Real-time load percentages, working set usage, and live sparkline memory history (` ▂▃▄▅▆▇█`).
- **GPU & Dedicated VRAM**: Hardware adapter detection and dedicated video memory usage via DirectX DXGI (`IDXGIAdapter3::QueryVideoMemoryInfo`).
- **Storage Drives**: Automatic enumeration of fixed and removable logical drives with capacity and usage bars.
- **Network Throughput**: Real-time download/upload rate calculation (KB/s, MB/s) and total data transfer using the IP Helper API.
- **Process List**: Top 15 memory-consuming processes sorted dynamically by working set size.
- **TUI Dashboard**: Double-buffered VT100 / ANSI escape sequence rendering with adjustable refresh rates (250ms–5000ms).

---

## Keybinds

| Key | Action |
| --- | --- |
| `Tab` / `1`–`4` | Switch view (*1: Overview*, *2: CPU/RAM/GPU*, *3: Storage/Net*, *4: Top Processes*) |
| `A` | Toggle audio beep warning on high RAM usage |
| `P` | Pause / resume metric polling |
| `+` / `-` | Increase / decrease refresh interval (250ms – 5000ms) |
| `R` | Force immediate refresh |
| `Q` / `Esc` | Exit application |

---

## Project Structure

```text
windows-system-monitor/
├── src/
│   ├── main.cpp               # Entry point and application event loop
│   ├── logic.hpp / .cpp       # Metric data structures and state handling
│   ├── style.hpp / .cpp       # ANSI UI renderer, gauge bars, and sparklines
│   └── interaction.hpp / .cpp # Win32, IP Helper, and DXGI hardware samplers
├── build_scripts/
│   ├── build_g++.bat          # Windows build script using G++
│   ├── build_clang.bat        # Windows build script using Clang++
│   └── build_zig.bat          # Alternative build script using Zig C++
├── CMakeLists.txt             # CMake project configuration
├── Makefile                   # Makefile for GNU Make / MinGW
├── .gitignore                 # Git ignore rules
└── README.md                  # Documentation
```

---

## Building

### Requirements
- **Windows 10 / 11 (x64)**
- **G++ (MinGW-w64)** 11.0+ or **Clang** 14.0+ with C++20 support
- Terminal with ANSI / VT100 escape code support (e.g., Windows Terminal, PowerShell, ConEmu)

### One-Click Build Scripts

Run either batch script from `build_scripts/`:

```cmd
.\build_scripts\build_g++.bat
```
*or*
```cmd
.\build_scripts\build_clang.bat
```

### Manual Compilation

#### With G++ (MinGW-w64)
```cmd
g++ -O3 -std=c++20 src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp -o system_monitor.exe -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid
```

#### With Clang++
```cmd
clang++ -O3 -std=c++20 src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp -o system_monitor.exe -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid
```

#### With Make
```sh
# Defaults to g++
make

# Or specify clang++
make CXX=clang++
```

#### With CMake
```cmd
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

---

## Running

```cmd
.\system_monitor.exe
```

---

## License

This project is licensed under the [GNU General Public License v3.0 or later](LICENSE).
