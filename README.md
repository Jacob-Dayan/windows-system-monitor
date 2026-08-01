# Windows C++ System Monitor

A lightweight, real-time, terminal-based Windows System Monitor written in C++20 and compiled cleanly using `zig c++`. Features visual ANSI dashboards, CPU, RAM, GPU/VRAM, Storage, Network, and Process metrics.

![C++20](https://img.shields.io/badge/Language-C%2B%2B20-blue.svg)
![Compiler](https://img.shields.io/badge/Compiler-Zig%20C%2B%2B-orange.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20x64-win.svg)

---

## Features

- **CPU Metrics**: Real-time total CPU utilization percentage and core count detection via Win32 `GetSystemTimes`.
- **Memory (RAM) & Commit**: Total, used, and free RAM tracking with live sparkline history graphs (` ▂▃▄▅▆▇█`).
- **GPU & Dedicated VRAM**: DXGI adapter detection (`IDXGIAdapter3::QueryVideoMemoryInfo`) reporting GPU model name, total VRAM, used VRAM, and VRAM load gauges.
- **Disk & Storage**: Automatic enumeration of logical fixed and removable drives with total vs. free space utilization.
- **Network Traffic**: Live upload and download speed calculation (KB/s, MB/s) and total bandwidth transferred.
- **Process Manager**: Top 15 memory-consuming active processes with PIDs and working set memory usage.
- **Flicker-Free Terminal UI**: Built using double-buffered ANSI escape codes with tabbed navigation views.

---

## Directory Structure

```text
RAMshit/
├── src/
│   ├── main.cpp          # Entry point & application lifecycle loop
│   ├── logic.hpp / .cpp  # Data structures & application state management
│   ├── style.hpp / .cpp  # ANSI UI rendering, gauge bars & sparkline graphs
│   └── interaction.hpp / .cpp # Windows API, IP Helper & DXGI sampler calls
├── build.bat             # Build script executing zig c++
├── .gitignore            # Git ignore rules
└── README.md             # Project documentation
```

---

## Building & Running

### Requirements
- **[Zig Compiler](https://ziglang.org/)** (v0.10.0 or newer) in your system `PATH`.

### Compile with Zig C++
Double-click `build.bat` or run:
```cmd
zig c++ -O3 -std=c++20 src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp -o system_monitor.exe -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid
```

### Launch
```cmd
.\system_monitor.exe
```

---

## Keyboard Controls

| Key | Action |
| --- | --- |
| `Tab` / `1`-`4` | Switch Tab View (*Overview*, *CPU/RAM/GPU*, *Storage/Net*, *Processes*) |
| `A` | Toggle High-RAM Audio Alert on/off |
| `P` | Pause / Resume live refresh |
| `+` / `-` | Speed up / slow down sampling rate (250ms - 5000ms) |
| `R` | Force immediate refresh |
| `Q` / `Esc` | Exit application |

---

## Pushing to GitHub

To push this repository to GitHub, follow these commands in your terminal:

```bash
# 1. Initialize Git repository
git init

# 2. Stage files and create first commit
git add .
git commit -m "Initial commit: Modular Windows C++ System Monitor with GPU/VRAM"

# 3. Create a new repository on GitHub (github.com/new), then link it:
git branch -M main
git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git

# 4. Push code to GitHub
git push -u origin main
```
