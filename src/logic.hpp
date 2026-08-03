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


#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <string>
#include <vector>
#include <cstdint>

// Drive Info structure
struct DriveInfo {
    std::string name;
    std::string type;
    uint64_t totalBytes{0};
    uint64_t freeBytes{0};
    uint64_t usedBytes{0};
    double usagePercent{0.0};
};

// Process Info structure
struct ProcessItem {
    uint32_t pid{0};
    std::string name;
    uint64_t memoryBytes{0};
    double memoryMB{0.0};
};

// Network Interface info
struct NetInfo {
    uint64_t rxBytes{0};
    uint64_t txBytes{0};
    double rxSpeed{0.0};
    double txSpeed{0.0};
};

// GPU & VRAM info
struct GPUInfo {
    std::string name{"N/A"};
    uint64_t vramTotalBytes{0};
    uint64_t vramUsedBytes{0};
    double vramUsagePercent{0.0};
    bool available{false};
};

// Main Data Container
struct SystemMetrics {
    // Host Info
    std::string hostname;
    int cpuCores{0};
    uint64_t uptimeSeconds{0};

    // CPU
    double cpuLoad{0.0};

    // RAM
    uint64_t ramTotalBytes{0};
    uint64_t ramAvailBytes{0};
    uint64_t ramUsedBytes{0};
    double ramLoadPercent{0.0};
    std::vector<double> ramHistory;

    // Commit / Swap
    uint64_t pageTotalBytes{0};
    uint64_t pageAvailBytes{0};
    uint64_t pageUsedBytes{0};
    double pageLoadPercent{0.0};

    // GPU & VRAM
    GPUInfo gpu;

    // Network & Storage
    NetInfo net;
    std::vector<DriveInfo> drives;

    // Top Processes
    std::vector<ProcessItem> topProcesses;
};

// Application State Controller
class MonitorState {
public:
    SystemMetrics metrics;

    int activeTab{1}; // 1: Overview, 2: CPU/RAM/GPU, 3: Storage/Net, 4: Processes
    bool soundAlertEnabled{true};
    int alertThreshold{90};
    bool isPaused{false};
    int refreshIntervalMs{1000};
    bool running{true};

    const size_t maxHistorySize{30};

    MonitorState();
    void PushRamHistory(double load);
};

#endif // LOGIC_HPP
