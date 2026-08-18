# Windows System Monitor

A lightweight, real-time, terminal-based system performance monitor for Windows, written in **C++20**.

It features a non-flickering, double-buffered ANSI visual dashboard monitoring CPU, RAM, GPU, VRAM, Storage Disks, Network interfaces, and active processes.

![C++20](https://img.shields.io/badge/Language-C%2B%2B20-blue.svg)
![Compiler](https://img.shields.io/badge/Compiler-Zig%20C%2B%2B-orange.svg) or ![GCC](https://img.shields.io/badge/Compiler-GCC-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%2010%2F11%20x64-0078D6.svg)
![License](https://img.shields.io/badge/License-GPL_v3_or_later-blue.svg)

---

## Key Features

-  **CPU Utilization**: Real-time overall CPU usage and core detection via Win32 `GetSystemTimes`.
-  **Physical RAM & Commit Memory**: Memory load percentages, working set usage, and live sparkline memory history (` ▂▃▄▅▆▇█`).
-  **GPU & Dedicated VRAM**: DirectX DXGI interface (`IDXGIAdapter3::QueryVideoMemoryInfo`) reporting GPU adapter model, total VRAM, used VRAM, and usage gauges.
-  **Storage Drives**: Automatic enumeration of logical fixed and removable drives with live space utilization.
-  **Network Throughput**: Real-time download/upload speed calculation (`KB/s`, `MB/s`) and total data transferred.
-  **Process Manager**: Top 15 memory-consuming active processes sorted dynamically by memory working set.
-  **Flicker-Free ANSI Dashboard**: Double-buffered VT100 terminal interface with tabbed views and custom update intervals.

---

## Controls & Hotkeys

| Key | Action |
| --- | --- |
| `Tab` / `1` - `4` | Switch Tab (*Overview*, *CPU/RAM/GPU*, *Storage/Net*, *Top Processes*) |
| `A` | Toggle High-RAM Audio Warning Beep |
| `P` | Pause / Resume live refresh |
| `+` / `-` | Speed up / slow down update interval (250ms - 5000ms) |
| `R` | Force immediate refresh |
| `Q` / `Esc` | Cleanly exit application |

---

## Project Structure

```text
windows-system-monitor/
├── src/
│   ├── main.cpp          # Entry point & application lifecycle loop
│   ├── logic.hpp / .cpp  # Metric containers & state management
│   ├── style.hpp / .cpp  # ANSI UI layout, gauge bars & sparkline graphs
│   └── interaction.hpp / .cpp # Win32, IP Helper & DXGI hardware samplers
├── build/
│   ├── build_g++.bat     # One-click build script using GCC
│   └── build_zig.bat     # One-click build script using Zig C++
├── .gitignore            # Git ignore rules
└── README.md             # Project documentation
```

---

## Building & Installation

### Prerequisites
- **[Zig Compiler](https://ziglang.org/)** (v0.10.0 or newer) added to your system `PATH`, or **[GCC](https://gcc.gnu.org/)** (v11 or newer).

### Compile
Run `build/build_g++.bat` or `build/build_zig.bat` or compile manually via command line:
```cmd
zig c++ -O3 -std=c++20 src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp -o system_monitor.exe -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid
```

### Launch
```cmd
.\system_monitor.exe
```

---

## License

Distributed under the [GPLv3 or later License](LICENSE).
