/* 
	Copyright (C) 2026 S. Jacob Dayan.S
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.
	
	You should have received me a copy of the GNU General Public License
	along with this program. If not, see <https://www.gnu.org/licenses/>.
*/


#include "style.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

std::string TerminalStyle::FormatBytes(uint64_t bytes) {
    const char* units[] = { "B", "KB", "MB", "GB", "TB" };
    int unitIdx = 0;
    double count = static_cast<double>(bytes);
    while (count >= 1024.0 && unitIdx < 4) {
        count /= 1024.0;
        unitIdx++;
    }
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1) << count << " " << units[unitIdx];
    return ss.str();
}

std::string TerminalStyle::FormatSpeed(double bytesPerSec) {
    const char* units[] = { "B/s", "KB/s", "MB/s", "GB/s" };
    int unitIdx = 0;
    double count = bytesPerSec;
    while (count >= 1024.0 && unitIdx < 3) {
        count /= 1024.0;
        unitIdx++;
    }
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1) << count << " " << units[unitIdx];
    return ss.str();
}

std::string TerminalStyle::FormatUptime(uint64_t totalSec) {
    uint64_t days = totalSec / 86400;
    uint64_t hours = (totalSec % 86400) / 3600;
    uint64_t mins = (totalSec % 3600) / 60;
    uint64_t secs = totalSec % 60;
    std::ostringstream ss;
    if (days > 0) ss << days << "d ";
    ss << std::setfill('0') << std::setw(2) << hours << ":"
       << std::setfill('0') << std::setw(2) << mins << ":"
       << std::setfill('0') << std::setw(2) << secs;
    return ss.str();
}

std::string TerminalStyle::RenderProgressBar(double percent, int width) {
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;
    int filled = static_cast<int>(std::round((percent / 100.0) * width));
    
    std::string bar = "[";
    std::string color = "\033[32m"; // Green
    if (percent >= 75.0 && percent < 90.0) color = "\033[33m"; // Yellow
    else if (percent >= 90.0) color = "\033[31m"; // Red

    bar += color;
    for (int i = 0; i < width; ++i) {
        if (i < filled) bar += "█";
        else bar += "░";
    }
    bar += "\033[0m]";
    return bar;
}

std::string TerminalStyle::RenderSparkline(const std::vector<double>& history, int width) {
    const char* blocks[] = { " ", "▂", "▃", "▄", "▅", "▆", "▇", "█" };
    std::string res = "";
    size_t start = (history.size() > static_cast<size_t>(width)) ? history.size() - width : 0;

    for (size_t i = start; i < history.size(); ++i) {
        double val = history[i];
        int idx = static_cast<int>(std::floor((val / 100.0) * 7.99));
        if (idx < 0) idx = 0;
        if (idx > 7) idx = 7;
        
        if (val >= 90.0) res += "\033[31m";
        else if (val >= 75.0) res += "\033[33m";
        else res += "\033[36m";

        res += blocks[idx];
    }
    res += "\033[0m";
    return res;
}

void TerminalStyle::RenderDashboard(const MonitorState& state) {
    const SystemMetrics& m = state.metrics;
    std::ostringstream out;

    // Reset Cursor to Home Position (No Screen Clear Flicker)
    out << "\033[H";

    // Title Header
    out << "\033[1;37;44m  ====================== WINDOWS SYSTEM MONITOR ======================  \033[0m\n";
    out << "\033[90m Host: \033[1;36m" << m.hostname << " \033[90m| Cores: \033[1;36m" << m.cpuCores
        << " \033[90m| Uptime: \033[1;36m" << FormatUptime(m.uptimeSeconds) 
        << " \033[90m| Alert: " << (state.soundAlertEnabled ? "\033[1;32mON (>90%)\033[0m" : "\033[1;31mOFF\033[0m")
        << " \033[90m| Refresh: \033[1;33m" << state.refreshIntervalMs << "ms\033[0m\n";

    // Navigation Tabs Bar
    out << "\n\033[1mTabs: ";
    out << (state.activeTab == 1 ? "\033[1;37;42m [1] Overview \033[0m" : "\033[90m [1] Overview \033[0m");
    out << (state.activeTab == 2 ? "\033[1;37;42m [2] CPU / RAM / GPU \033[0m" : "\033[90m [2] CPU / RAM / GPU \033[0m");
    out << (state.activeTab == 3 ? "\033[1;37;42m [3] Storage / Net \033[0m" : "\033[90m [3] Storage / Net \033[0m");
    out << (state.activeTab == 4 ? "\033[1;37;42m [4] Top Processes \033[0m" : "\033[90m [4] Top Processes \033[0m");
    out << "\n\033[90m--------------------------------------------------------------------------------\033[0m\n";

    if (state.activeTab == 1) {
        // --- OVERVIEW TAB ---
        out << "\033[1;35m[ SYSTEM OVERVIEW ]\033[0m\n\n";

        // CPU Gauge
        out << " CPU Load:        " << RenderProgressBar(m.cpuLoad, 30)
            << " " << std::fixed << std::setprecision(1) << std::setw(5) << m.cpuLoad << "%\n";

        // RAM Gauge
        out << " RAM Load:        " << RenderProgressBar(m.ramLoadPercent, 30)
            << " " << std::fixed << std::setprecision(1) << std::setw(5) << m.ramLoadPercent << "%\n";
        out << "                  \033[90mUsed: " << FormatBytes(m.ramUsedBytes) << " / Total: " << FormatBytes(m.ramTotalBytes) << "\033[0m\n";
        out << " RAM History:     [" << RenderSparkline(m.ramHistory, 32) << "]\n\n";

        // GPU & VRAM Gauge
        if (m.gpu.available) {
            out << " GPU (" << m.gpu.name.substr(0, 24) << ")\n";
            out << " VRAM Load:       " << RenderProgressBar(m.gpu.vramUsagePercent, 30)
                << " " << std::fixed << std::setprecision(1) << std::setw(5) << m.gpu.vramUsagePercent << "%\n";
            out << "                  \033[90mUsed: " << FormatBytes(m.gpu.vramUsedBytes) << " / Total: " << FormatBytes(m.gpu.vramTotalBytes) << "\033[0m\n\n";
        }

        // Network Quick Stats
        out << " Network Traffic: \033[32m▲ Upload: " << std::left << std::setw(10) << FormatSpeed(m.net.txSpeed)
            << "\033[36m▼ Download: " << std::left << std::setw(10) << FormatSpeed(m.net.rxSpeed) << "\033[0m\n\n";

        // Storage Quick Summary
        out << "\033[1;33m[ STORAGE SUMMARY ]\033[0m\n";
        for (const auto& d : m.drives) {
            out << " Drive " << std::left << std::setw(4) << d.name << RenderProgressBar(d.usagePercent, 20)
                << " " << std::right << std::setw(5) << std::fixed << std::setprecision(1) << d.usagePercent << "% "
                << "\033[90m(" << FormatBytes(d.usedBytes) << " / " << FormatBytes(d.totalBytes) << ")\033[0m\n";
        }

    } else if (state.activeTab == 2) {
        // --- CPU, RAM & GPU TAB ---
        out << "\033[1;35m[ HARDWARE METRICS: CPU / RAM / GPU ]\033[0m\n\n";

        out << " --- Processor (CPU) ---\n";
        out << " Logical Cores:   " << m.cpuCores << "\n";
        out << " Overall Usage:   " << RenderProgressBar(m.cpuLoad, 35) << " " << std::fixed << std::setprecision(1) << m.cpuLoad << "%\n\n";

        out << " --- Memory (RAM) ---\n";
        out << " Installed Total: " << FormatBytes(m.ramTotalBytes) << "\n";
        out << " Currently Used:  " << FormatBytes(m.ramUsedBytes) << " (" << std::fixed << std::setprecision(1) << m.ramLoadPercent << "%)\n";
        out << " Available Free:  " << FormatBytes(m.ramAvailBytes) << "\n";
        out << " RAM Graph:       [" << RenderSparkline(m.ramHistory, 45) << "]\n\n";

        out << " --- Graphics (GPU & VRAM) ---\n";
        if (m.gpu.available) {
            out << " GPU Name:        " << m.gpu.name << "\n";
            out << " VRAM Installed:  " << FormatBytes(m.gpu.vramTotalBytes) << "\n";
            out << " VRAM Used:       " << FormatBytes(m.gpu.vramUsedBytes) << " (" << std::fixed << std::setprecision(1) << m.gpu.vramUsagePercent << "%)\n";
            out << " VRAM Gauge:      " << RenderProgressBar(m.gpu.vramUsagePercent, 35) << "\n";
        } else {
            out << " GPU Info:        DXGI Adapter information unavailable\n";
        }

    } else if (state.activeTab == 3) {
        // --- STORAGE & NETWORK TAB ---
        out << "\033[1;35m[ STORAGE DISKS & NETWORK INTERFACES ]\033[0m\n\n";

        out << "\033[1mDrive Letter  Type          Total        Free         Used         Usage Bar\033[0m\n";
        out << "\033[90m--------------------------------------------------------------------------------\033[0m\n";
        for (const auto& d : m.drives) {
            out << " " << std::left << std::setw(13) << d.name
                << std::setw(14) << d.type
                << std::setw(13) << FormatBytes(d.totalBytes)
                << std::setw(13) << FormatBytes(d.freeBytes)
                << std::setw(13) << FormatBytes(d.usedBytes)
                << RenderProgressBar(d.usagePercent, 12) << "\n";
        }

        out << "\n\033[1;35m[ REAL-TIME NETWORK MONITOR ]\033[0m\n\n";
        out << " Download Speed (Rx): \033[1;36m▼ " << FormatSpeed(m.net.rxSpeed) << "\033[0m\n";
        out << " Upload Speed (Tx):   \033[1;32m▲ " << FormatSpeed(m.net.txSpeed) << "\033[0m\n";
        out << " Total Data Received: " << FormatBytes(m.net.rxBytes) << "\n";
        out << " Total Data Sent:     " << FormatBytes(m.net.txBytes) << "\n";

    } else if (state.activeTab == 4) {
        // --- PROCESSES TAB ---
        out << "\033[1;35m[ TOP 15 PROCESSES BY MEMORY FOOTPRINT ]\033[0m\n\n";

        out << "\033[1mPID        Process Name                    Memory Usage (Working Set)\033[0m\n";
        out << "\033[90m--------------------------------------------------------------------------------\033[0m\n";
        for (const auto& proc : m.topProcesses) {
            out << " " << std::left << std::setw(10) << proc.pid
                << std::setw(32) << proc.name.substr(0, 30)
                << std::right << std::setw(8) << std::fixed << std::setprecision(1) << proc.memoryMB << " MB  "
                << RenderProgressBar((proc.memoryMB / (1024.0 * 4.0)) * 100.0, 15) << "\n";
        }
    }

    // Footer / Controls
    out << "\n\033[90m--------------------------------------------------------------------------------\033[0m\n";
    out << "\033[1;37;40m Hotkeys: [Tab/1-4] Switch Tab | [A] Toggle Audio Alert | [P] Pause | [+/-] Refresh Speed | [Q] Quit \033[0m\n";
    out << "\033[K"; // Clear rest of line

    std::cout << out.str() << std::flush;
}
