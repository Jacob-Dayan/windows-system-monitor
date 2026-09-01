/*
	Copyright (C) 2026 Jacob Dayan

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
#include "consts.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <array>
#include <string_view>

std::string TerminalStyle::FormatBytes(uint64_t bytes) {
    constexpr std::array<std::string_view, 5> units = { "B", "KB", "MB", "GB", "TB" };
    size_t unitIdx = 0;
    double count = static_cast<double>(bytes);
    while (count >= 1024.0 && unitIdx < units.size() - 1) {
        count /= 1024.0;
        unitIdx++;
    }
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1) << count << " " << units[unitIdx];
    return ss.str();
}

std::string TerminalStyle::FormatSpeed(double bytesPerSec) {
    constexpr std::array<std::string_view, 4> units = { "B/s", "KB/s", "MB/s", "GB/s" };
    size_t unitIdx = 0;
    double count = bytesPerSec;
    while (count >= 1024.0 && unitIdx < units.size() - 1) {
        count /= 1024.0;
        unitIdx++;
    }
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1) << count << " " << units[unitIdx];
    return ss.str();
}

std::string TerminalStyle::FormatUptime(uint64_t totalSec) {
    const uint64_t days = totalSec / 86400;
    const uint64_t hours = (totalSec % 86400) / 3600;
    const uint64_t mins = (totalSec % 3600) / 60;
    const uint64_t secs = totalSec % 60;
    std::ostringstream ss;
    if (days > 0) ss << days << "d ";
    ss << std::setfill('0') << std::setw(2) << hours << ":"
       << std::setfill('0') << std::setw(2) << mins << ":"
       << std::setfill('0') << std::setw(2) << secs;
    return ss.str();
}

std::string TerminalStyle::RenderProgressBar(double percent, int width) {
    const double clampedPercent = std::clamp(percent, 0.0, 100.0);
    const int filled = std::clamp(static_cast<int>(std::round((clampedPercent / 100.0) * width)), 0, width);

    std::string_view color = Ansi::Green;
    switch (GetUsageTier(clampedPercent)) {
        case UsageTier::Critical: color = Ansi::Red; break;
        case UsageTier::Warning:  color = Ansi::Yellow; break;
        case UsageTier::Normal:   break;
    }

    std::string bar = "[";
    bar += color;
    for (int i = 0; i < width; ++i) {
        if (i < filled) bar += "█";
        else bar += "░";
    }
    bar += Ansi::Reset;
    bar += "]";
    return bar;
}

std::string TerminalStyle::RenderSparkline(const std::vector<double>& history, int width) {
    constexpr std::array<std::string_view, 8> blocks = { " ", "▂", "▃", "▄", "▅", "▆", "▇", "█" };
    std::string res;
    res.reserve(static_cast<size_t>(width) * 8);

    const size_t start = (history.size() > static_cast<size_t>(width)) ? history.size() - width : 0;

    for (size_t i = start; i < history.size(); ++i) {
        const double val = history[i];
        const int idx = std::clamp(static_cast<int>(std::floor((val / 100.0) * 7.99)), 0, 7);

        switch (GetUsageTier(val)) {
            case UsageTier::Critical: res += Ansi::Red; break;
            case UsageTier::Warning:  res += Ansi::Yellow; break;
            case UsageTier::Normal:   res += Ansi::Cyan; break;
        }

        res += blocks[idx];
    }
    res += Ansi::Reset;
    return res;
}

void TerminalStyle::RenderOverviewTab(std::ostream& out, const MonitorState& state) {
    const SystemMetrics& m = state.metrics;
    out << Ansi::BoldMagenta << "[ SYSTEM OVERVIEW ]" << Ansi::Reset << "\n\n";

    out << " CPU Load:        " << RenderProgressBar(m.cpuLoad, 30)
        << " " << std::fixed << std::setprecision(1) << std::setw(5) << m.cpuLoad << "%\n";

    out << " RAM Load:        " << RenderProgressBar(m.ramLoadPercent, 30)
        << " " << std::fixed << std::setprecision(1) << std::setw(5) << m.ramLoadPercent << "%\n";
    out << "                  " << Ansi::Gray << "Used: " << FormatBytes(m.ramUsedBytes) << " / Total: " << FormatBytes(m.ramTotalBytes) << Ansi::Reset << "\n";
    out << " RAM History:     [" << RenderSparkline(m.ramHistory, 32) << "]\n\n";

    if (m.gpu.available) {
        out << " GPU (" << m.gpu.name.substr(0, 24) << ")\n";
        out << " VRAM Load:       " << RenderProgressBar(m.gpu.vramUsagePercent, 30)
            << " " << std::fixed << std::setprecision(1) << std::setw(5) << m.gpu.vramUsagePercent << "%\n";
        out << "                  " << Ansi::Gray << "Used: " << FormatBytes(m.gpu.vramUsedBytes) << " / Total: " << FormatBytes(m.gpu.vramTotalBytes) << Ansi::Reset << "\n\n";
    }

    out << " Network Traffic: " << Ansi::Green << "▲ Upload: " << std::left << std::setw(10) << FormatSpeed(m.net.txSpeed)
        << Ansi::Cyan << "▼ Download: " << std::left << std::setw(10) << FormatSpeed(m.net.rxSpeed) << Ansi::Reset << "\n\n";

    out << Ansi::BoldYellow << "[ STORAGE SUMMARY ]" << Ansi::Reset << "\n";
    for (const auto& d : m.drives) {
        out << " Drive " << std::left << std::setw(4) << d.name << RenderProgressBar(d.usagePercent, 20)
            << " " << std::right << std::setw(5) << std::fixed << std::setprecision(1) << d.usagePercent << "% "
            << Ansi::Gray << "(" << FormatBytes(d.usedBytes) << " / " << FormatBytes(d.totalBytes) << ")" << Ansi::Reset << "\n";
    }
}

void TerminalStyle::RenderHardwareTab(std::ostream& out, const MonitorState& state) {
    const SystemMetrics& m = state.metrics;
    out << Ansi::BoldMagenta << "[ HARDWARE METRICS: CPU / RAM / GPU ]" << Ansi::Reset << "\n\n";

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
}

void TerminalStyle::RenderStorageNetworkTab(std::ostream& out, const MonitorState& state) {
    const SystemMetrics& m = state.metrics;
    out << Ansi::BoldMagenta << "[ STORAGE DISKS & NETWORK INTERFACES ]" << Ansi::Reset << "\n\n";

    out << Ansi::Bold << "Drive Letter  Type          Total        Free         Used         Usage Bar" << Ansi::Reset << "\n";
    out << Ansi::Gray << "--------------------------------------------------------------------------------" << Ansi::Reset << "\n";
    for (const auto& d : m.drives) {
        out << " " << std::left << std::setw(13) << d.name
            << std::setw(14) << d.type
            << std::setw(13) << FormatBytes(d.totalBytes)
            << std::setw(13) << FormatBytes(d.freeBytes)
            << std::setw(13) << FormatBytes(d.usedBytes)
            << RenderProgressBar(d.usagePercent, 12) << "\n";
    }

    out << "\n" << Ansi::BoldMagenta << "[ REAL-TIME NETWORK MONITOR ]" << Ansi::Reset << "\n\n";
    out << " Download Speed (Rx): " << Ansi::BoldCyan << "▼ " << FormatSpeed(m.net.rxSpeed) << Ansi::Reset << "\n";
    out << " Upload Speed (Tx):   " << Ansi::BoldGreen << "▲ " << FormatSpeed(m.net.txSpeed) << Ansi::Reset << "\n";
    out << " Total Data Received: " << FormatBytes(m.net.rxBytes) << "\n";
    out << " Total Data Sent:     " << FormatBytes(m.net.txBytes) << "\n";
}

void TerminalStyle::RenderProcessesTab(std::ostream& out, const MonitorState& state) {
    const SystemMetrics& m = state.metrics;
    out << Ansi::BoldMagenta << "[ TOP 15 PROCESSES BY MEMORY FOOTPRINT ]" << Ansi::Reset << "\n\n";

    out << Ansi::Bold << "PID        Process Name                    Memory Usage (Working Set)" << Ansi::Reset << "\n";
    out << Ansi::Gray << "--------------------------------------------------------------------------------" << Ansi::Reset << "\n";
    for (const auto& proc : m.topProcesses) {
        out << " " << std::left << std::setw(10) << proc.pid
            << std::setw(32) << proc.name.substr(0, 30)
            << std::right << std::setw(8) << std::fixed << std::setprecision(1) << proc.memoryMB << " MB  "
            << RenderProgressBar((proc.memoryMB / (1024.0 * 4.0)) * 100.0, 15) << "\n";
    }
}

void TerminalStyle::RenderDashboard(const MonitorState& state) {
    const SystemMetrics& m = state.metrics;
    std::ostringstream out;

    out << Ansi::CursorHome;
    out << Ansi::HeaderBanner << "                         WINDOWS SYSTEM MONITOR                         " << Ansi::Reset << "\n";
    out << Ansi::Gray << " Host: " << Ansi::BoldCyan << m.hostname << " " << Ansi::Gray << "| Cores: " << Ansi::BoldCyan << m.cpuCores
        << " " << Ansi::Gray << "| Uptime: " << Ansi::BoldCyan << FormatUptime(m.uptimeSeconds)
        << " " << Ansi::Gray << "| Alert: ";
    if (state.soundAlertEnabled) {
        out << Ansi::BoldGreen << "ON (>90%)";
    } else {
        out << Ansi::BoldRed << "OFF";
    }
    out << Ansi::Reset
        << " " << Ansi::Gray << "| Refresh: " << Ansi::BoldYellow << state.refreshIntervalMs << "ms" << Ansi::Reset << "\n";

    out << "\n" << Ansi::Bold << "Tabs: " << Ansi::Reset;
    out << (state.activeTab == static_cast<int>(TabId::Overview) ? Ansi::TabActive : Ansi::Gray) << " [1] Overview " << Ansi::Reset;
    out << (state.activeTab == static_cast<int>(TabId::Hardware) ? Ansi::TabActive : Ansi::Gray) << " [2] CPU / RAM / GPU " << Ansi::Reset;
    out << (state.activeTab == static_cast<int>(TabId::StorageNetwork) ? Ansi::TabActive : Ansi::Gray) << " [3] Storage / Net " << Ansi::Reset;
    out << (state.activeTab == static_cast<int>(TabId::Processes) ? Ansi::TabActive : Ansi::Gray) << " [4] Top Processes " << Ansi::Reset;
    out << "\n" << Ansi::Gray << "--------------------------------------------------------------------------------" << Ansi::Reset << "\n";

    switch (static_cast<TabId>(state.activeTab)) {
        case TabId::Overview:
            RenderOverviewTab(out, state);
            break;
        case TabId::Hardware:
            RenderHardwareTab(out, state);
            break;
        case TabId::StorageNetwork:
            RenderStorageNetworkTab(out, state);
            break;
        case TabId::Processes:
            RenderProcessesTab(out, state);
            break;
        default:
            break;
    }

    out << "\n" << Ansi::Gray << "--------------------------------------------------------------------------------" << Ansi::Reset << "\n";
    out << Ansi::HotkeysBar << " Hotkeys: [Tab/1-4] Switch Tab | [A] Toggle Audio Alert | [P] Pause | [+/-] Refresh Speed | [Q] Quit " << Ansi::Reset << "\n";
    out << Ansi::ClearLine;

    std::cout << out.str() << std::flush;
}
