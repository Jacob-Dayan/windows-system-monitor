#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00
#define _GLIBCXX_PRINT_NEVER_UNICODE

#include <winsock2.h>
#include <windows.h>
#include <psapi.h>
#include <iphlpapi.h>
#include <tlhelp32.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <thread>
#include <sstream>
#include <cmath>

// Helper to convert FILETIME to uint64
static uint64_t FileTimeToUint64(const FILETIME& ft) {
    return (static_cast<uint64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

// Convert bytes to human readable format
static std::string FormatBytes(uint64_t bytes) {
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

// Convert bytes/sec to speed format
static std::string FormatSpeed(double bytesPerSec) {
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
    DWORD pid;
    std::string name;
    uint64_t memoryBytes;
    double memoryMB;
};

// Network Interface info
struct NetInfo {
    uint64_t rxBytes{0};
    uint64_t txBytes{0};
    double rxSpeed{0.0};
    double txSpeed{0.0};
};

// Application State
class SystemMonitor {
private:
    // CPU Tracking
    uint64_t prevIdleTime{0};
    uint64_t prevKernelTime{0};
    uint64_t prevUserTime{0};
    double currentCpuLoad{0.0};
    int cpuCores{0};

    // RAM Metrics
    MEMORYSTATUSEX memState{};
    std::vector<double> ramHistory;
    const size_t maxHistorySize = 30;

    // Network Tracking
    NetInfo netData{};

    // Storage
    std::vector<DriveInfo> drives;

    // Processes
    std::vector<ProcessItem> topProcesses;

    // System Info
    std::string hostname;
    std::string osName{"Windows x64"};
    uint64_t uptimeSeconds{0};

    // App Control
    int activeTab{1}; // 1: Overview, 2: CPU & Memory, 3: Storage & Net, 4: Processes
    bool soundAlertEnabled{true};
    int alertThreshold{93};
    bool isPaused{false};
    int refreshIntervalMs{1000};
    bool running{true};
    std::chrono::steady_clock::time_point lastSampleTime;

public:
    SystemMonitor() {
        // Enable Virtual Terminal Processing for ANSI colors
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
        SetConsoleOutputCP(CP_UTF8);

        // System Info setup
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        cpuCores = sysInfo.dwNumberOfProcessors;

        char hostBuffer[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD hostLen = sizeof(hostBuffer);
        if (GetComputerNameA(hostBuffer, &hostLen)) {
            hostname = hostBuffer;
        } else {
            hostname = "Windows Host";
        }

        memState.dwLength = sizeof(memState);
        lastSampleTime = std::chrono::steady_clock::now();
        
        // Initial sample
        SampleData();
    }

    void SampleData() {
        auto now = std::chrono::steady_clock::now();
        double elapsedSec = std::chrono::duration<double>(now - lastSampleTime).count();
        if (elapsedSec <= 0.001) elapsedSec = 1.0;
        lastSampleTime = now;

        // 1. CPU Load
        FILETIME idleTime, kernelTime, userTime;
        if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
            uint64_t idle = FileTimeToUint64(idleTime);
            uint64_t kernel = FileTimeToUint64(kernelTime);
            uint64_t user = FileTimeToUint64(userTime);

            if (prevKernelTime > 0) {
                uint64_t idleDiff = idle - prevIdleTime;
                uint64_t kernelDiff = kernel - prevKernelTime;
                uint64_t userDiff = user - prevUserTime;
                uint64_t totalSys = kernelDiff + userDiff;

                if (totalSys > 0) {
                    currentCpuLoad = (1.0 - (static_cast<double>(idleDiff) / totalSys)) * 100.0;
                    if (currentCpuLoad < 0.0) currentCpuLoad = 0.0;
                    if (currentCpuLoad > 100.0) currentCpuLoad = 100.0;
                }
            }
            prevIdleTime = idle;
            prevKernelTime = kernel;
            prevUserTime = user;
        }

        // 2. RAM & Virtual Memory
        if (GlobalMemoryStatusEx(&memState)) {
            double load = static_cast<double>(memState.dwMemoryLoad);
            ramHistory.push_back(load);
            if (ramHistory.size() > maxHistorySize) {
                ramHistory.erase(ramHistory.begin());
            }

            // Audio Alert if RAM load exceeds safety threshold (SSD Protection)
            if (soundAlertEnabled && memState.dwMemoryLoad >= static_cast<DWORD>(alertThreshold)) {
                Beep(750, 200);
            }
        }

        // 3. Uptime
        uptimeSeconds = GetTickCount64() / 1000;

        // 4. Storage Drives
        drives.clear();
        char driveBuffer[512];
        DWORD len = GetLogicalDriveStringsA(sizeof(driveBuffer), driveBuffer);
        if (len > 0 && len < sizeof(driveBuffer)) {
            char* pDrive = driveBuffer;
            while (*pDrive) {
                UINT type = GetDriveTypeA(pDrive);
                if (type == DRIVE_FIXED || type == DRIVE_REMOVABLE) {
                    ULARGE_INTEGER freeBytesCaller, totalBytes, totalFreeBytes;
                    if (GetDiskFreeSpaceExA(pDrive, &freeBytesCaller, &totalBytes, &totalFreeBytes)) {
                        DriveInfo info;
                        info.name = pDrive;
                        info.type = (type == DRIVE_FIXED) ? "Fixed Drive" : "Removable";
                        info.totalBytes = totalBytes.QuadPart;
                        info.freeBytes = totalFreeBytes.QuadPart;
                        info.usedBytes = totalBytes.QuadPart - totalFreeBytes.QuadPart;
                        info.usagePercent = (totalBytes.QuadPart > 0) ?
                            (static_cast<double>(info.usedBytes) / totalBytes.QuadPart) * 100.0 : 0.0;
                        drives.push_back(info);
                    }
                }
                pDrive += strlen(pDrive) + 1;
            }
        }

        // 5. Network Traffic
        DWORD dwSize = 0;
        if (GetIfTable(NULL, &dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
            MIB_IFTABLE* pIfTable = (MIB_IFTABLE*)malloc(dwSize);
            if (pIfTable && GetIfTable(pIfTable, &dwSize, FALSE) == NO_ERROR) {
                uint64_t currentRx = 0;
                uint64_t currentTx = 0;

                for (DWORD i = 0; i < pIfTable->dwNumEntries; i++) {
                    const MIB_IFROW& row = pIfTable->table[i];
                    if (row.dwType != MIB_IF_TYPE_LOOPBACK && row.dwOperStatus == MIB_IF_OPER_STATUS_OPERATIONAL) {
                        currentRx += row.dwInOctets;
                        currentTx += row.dwOutOctets;
                    }
                }

                if (netData.rxBytes > 0 && elapsedSec > 0) {
                    netData.rxSpeed = (currentRx >= netData.rxBytes) ? static_cast<double>(currentRx - netData.rxBytes) / elapsedSec : 0.0;
                    netData.txSpeed = (currentTx >= netData.txBytes) ? static_cast<double>(currentTx - netData.txBytes) / elapsedSec : 0.0;
                }
                netData.rxBytes = currentRx;
                netData.txBytes = currentTx;
            }
            if (pIfTable) free(pIfTable);
        }

        // 6. Top Processes by RAM
        topProcesses.clear();
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(hSnapshot, &pe32)) {
                do {
                    if (pe32.th32ProcessID == 0) continue; // System Idle
                    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
                    if (hProc) {
                        PROCESS_MEMORY_COUNTERS pmc;
                        if (GetProcessMemoryInfo(hProc, &pmc, sizeof(pmc))) {
                            ProcessItem item;
                            item.pid = pe32.th32ProcessID;
                            item.name = pe32.szExeFile;
                            item.memoryBytes = pmc.WorkingSetSize;
                            item.memoryMB = static_cast<double>(pmc.WorkingSetSize) / (1024.0 * 1024.0);
                            topProcesses.push_back(item);
                        }
                        CloseHandle(hProc);
                    }
                } while (Process32Next(hSnapshot, &pe32));
            }
            CloseHandle(hSnapshot);
        }

        std::sort(topProcesses.begin(), topProcesses.end(), [](const ProcessItem& a, const ProcessItem& b) {
            return a.memoryBytes > b.memoryBytes;
        });

        if (topProcesses.size() > 15) {
            topProcesses.resize(15);
        }
    }

    std::string RenderProgressBar(double percent, int width = 24) {
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

    std::string RenderSparkline(const std::vector<double>& history, int width = 30) {
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

    std::string FormatUptime(uint64_t totalSec) {
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

    void RenderUI() {
        std::ostringstream out;

        // Reset Cursor to Home Position (No Screen Clear Flicker)
        out << "\033[H";

        // Title Header
        out << "\033[1;37;44m  =================== WINDOWS SYSTEM MONITOR & SSD PROTECTOR ===================  \033[0m\n";
        out << "\033[90m Host: \033[1;36m" << hostname << " \033[90m| Cores: \033[1;36m" << cpuCores
            << " \033[90m| Uptime: \033[1;36m" << FormatUptime(uptimeSeconds) 
            << " \033[90m| Alert: " << (soundAlertEnabled ? "\033[1;32mON (>93%)\033[0m" : "\033[1;31mOFF\033[0m")
            << " \033[90m| Interval: \033[1;33m" << refreshIntervalMs << "ms\033[0m\n";

        // Navigation Tabs Bar
        out << "\n\033[1mTabs: ";
        out << (activeTab == 1 ? "\033[1;37;42m [1] Overview \033[0m" : "\033[90m [1] Overview \033[0m");
        out << (activeTab == 2 ? "\033[1;37;42m [2] CPU & RAM \033[0m" : "\033[90m [2] CPU & RAM \033[0m");
        out << (activeTab == 3 ? "\033[1;37;42m [3] Storage & Net \033[0m" : "\033[90m [3] Storage & Net \033[0m");
        out << (activeTab == 4 ? "\033[1;37;42m [4] Top Processes \033[0m" : "\033[90m [4] Top Processes \033[0m");
        out << "\n\033[90m--------------------------------------------------------------------------------\033[0m\n";

        if (activeTab == 1) {
            // --- OVERVIEW TAB ---
            out << "\033[1;35m[ SYSTEM OVERVIEW ]\033[0m\n\n";

            // CPU Gauge
            out << " CPU Utilization: " << RenderProgressBar(currentCpuLoad, 30)
                << " " << std::fixed << std::setprecision(1) << std::setw(5) << currentCpuLoad << "%\n";

            // RAM Gauge
            double ramTotalGB = static_cast<double>(memState.ullTotalPhys) / (1024.0 * 1024.0 * 1024.0);
            double ramAvailGB = static_cast<double>(memState.ullAvailPhys) / (1024.0 * 1024.0 * 1024.0);
            double ramUsedGB = ramTotalGB - ramAvailGB;
            double ramLoad = static_cast<double>(memState.dwMemoryLoad);

            out << " RAM Load:        " << RenderProgressBar(ramLoad, 30)
                << " " << std::fixed << std::setprecision(1) << std::setw(5) << ramLoad << "%\n";
            out << "                  \033[90mUsed: " << ramUsedGB << " GB / Free: " << ramAvailGB << " GB / Total: " << ramTotalGB << " GB\033[0m\n";
            out << " RAM History:     [" << RenderSparkline(ramHistory, 32) << "]\n\n";

            // Pagefile / Commit Memory
            double pageTotalGB = static_cast<double>(memState.ullTotalPageFile) / (1024.0 * 1024.0 * 1024.0);
            double pageAvailGB = static_cast<double>(memState.ullAvailPageFile) / (1024.0 * 1024.0 * 1024.0);
            double pageUsedGB = pageTotalGB - pageAvailGB;
            double pageLoad = (pageTotalGB > 0) ? (pageUsedGB / pageTotalGB) * 100.0 : 0.0;

            out << " Commit/Swap Load:" << RenderProgressBar(pageLoad, 30)
                << " " << std::fixed << std::setprecision(1) << std::setw(5) << pageLoad << "%\n";
            out << "                  \033[90mUsed: " << pageUsedGB << " GB / Limit: " << pageTotalGB << " GB\033[0m\n\n";

            // Network Quick Stats
            out << " Network Traffic: \033[32m▲ Upload: " << std::left << std::setw(10) << FormatSpeed(netData.txSpeed)
                << "\033[36m▼ Download: " << std::left << std::setw(10) << FormatSpeed(netData.rxSpeed) << "\033[0m\n\n";

            // Storage Quick Summary
            out << "\033[1;33m[ STORAGE SUMMARY ]\033[0m\n";
            for (const auto& d : drives) {
                out << " Drive " << std::left << std::setw(4) << d.name << RenderProgressBar(d.usagePercent, 20)
                    << " " << std::right << std::setw(5) << std::fixed << std::setprecision(1) << d.usagePercent << "% "
                    << "\033[90m(" << FormatBytes(d.usedBytes) << " / " << FormatBytes(d.totalBytes) << ")\033[0m\n";
            }

        } else if (activeTab == 2) {
            // --- CPU & MEMORY TAB ---
            out << "\033[1;35m[ CPU & MEMORY DETAILED MONITOR ]\033[0m\n\n";

            out << " Processor Info:  " << cpuCores << " Logical Cores\n";
            out << " Overall CPU:     " << RenderProgressBar(currentCpuLoad, 35) << " " << currentCpuLoad << "%\n\n";

            out << " --- Physical RAM (SSD Protection Shield) ---\n";
            double ramTotalGB = static_cast<double>(memState.ullTotalPhys) / (1024.0 * 1024.0 * 1024.0);
            double ramAvailGB = static_cast<double>(memState.ullAvailPhys) / (1024.0 * 1024.0 * 1024.0);
            double ramUsedGB = ramTotalGB - ramAvailGB;

            out << " Total Installed: " << std::fixed << std::setprecision(2) << ramTotalGB << " GB\n";
            out << " Currently Used:  " << ramUsedGB << " GB (" << memState.dwMemoryLoad << "%)\n";
            out << " Available Free:  " << ramAvailGB << " GB\n";
            out << " Safety Alert:    " << (memState.dwMemoryLoad >= (DWORD)alertThreshold ? 
                "\033[1;31m[!] WARNING: High RAM usage! Windows swapping to SSD!\033[0m" : "\033[32m[OK] Memory level safe\033[0m") << "\n\n";

            out << " --- Virtual / Commit Memory ---\n";
            double pageTotalGB = static_cast<double>(memState.ullTotalPageFile) / (1024.0 * 1024.0 * 1024.0);
            double pageAvailGB = static_cast<double>(memState.ullAvailPageFile) / (1024.0 * 1024.0 * 1024.0);
            out << " Commit Limit:    " << pageTotalGB << " GB\n";
            out << " Commit Available:" << pageAvailGB << " GB\n\n";

            out << " --- RAM Usage History Graph ---\n";
            out << " [" << RenderSparkline(ramHistory, 45) << "]\n";

        } else if (activeTab == 3) {
            // --- STORAGE & NETWORK TAB ---
            out << "\033[1;35m[ STORAGE & NETWORK INTERFACES ]\033[0m\n\n";

            out << "\033[1mDrive Letter  Type          Total        Free         Used         Usage Bar\033[0m\n";
            out << "\033[90m--------------------------------------------------------------------------------\033[0m\n";
            for (const auto& d : drives) {
                out << " " << std::left << std::setw(13) << d.name
                    << std::setw(14) << d.type
                    << std::setw(13) << FormatBytes(d.totalBytes)
                    << std::setw(13) << FormatBytes(d.freeBytes)
                    << std::setw(13) << FormatBytes(d.usedBytes)
                    << RenderProgressBar(d.usagePercent, 12) << "\n";
            }

            out << "\n\033[1;35m[ REAL-TIME NETWORK MONITOR ]\033[0m\n\n";
            out << " Download Speed (Rx): \033[1;36m▼ " << FormatSpeed(netData.rxSpeed) << "\033[0m\n";
            out << " Upload Speed (Tx):   \033[1;32m▲ " << FormatSpeed(netData.txSpeed) << "\033[0m\n";
            out << " Total Data Received: " << FormatBytes(netData.rxBytes) << "\n";
            out << " Total Data Sent:     " << FormatBytes(netData.txBytes) << "\n";

        } else if (activeTab == 4) {
            // --- PROCESSES TAB ---
            out << "\033[1;35m[ TOP 15 PROCESSES BY MEMORY FOOTPRINT ]\033[0m\n\n";

            out << "\033[1mPID        Process Name                    Memory Usage (Working Set)\033[0m\n";
            out << "\033[90m--------------------------------------------------------------------------------\033[0m\n";
            for (const auto& proc : topProcesses) {
                out << " " << std::left << std::setw(10) << proc.pid
                    << std::setw(32) << proc.name.substr(0, 30)
                    << std::right << std::setw(8) << std::fixed << std::setprecision(1) << proc.memoryMB << " MB  "
                    << RenderProgressBar((proc.memoryMB / (1024.0 * 4.0)) * 100.0, 15) << "\n";
            }
        }

        // Footer / Controls
        out << "\n\033[90m--------------------------------------------------------------------------------\033[0m\n";
        out << "\033[1;37;40m Hotkeys: [Tab/1-4] Switch Tab | [A] Toggle Sound Alert | [P] Pause | [+/-] Speed | [Q] Quit \033[0m\n";
        out << "\033[K"; // Clear rest of line

        std::cout << out.str() << std::flush;
    }

    void HandleInput() {
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 0 || ch == 224) {
                ch = _getch(); // Arrow keys or special
            }

            if (ch == 'q' || ch == 'Q' || ch == 27) { // 27 = ESC
                running = false;
            } else if (ch == '\t') { // Tab
                activeTab = (activeTab % 4) + 1;
            } else if (ch >= '1' && ch <= '4') {
                activeTab = ch - '0';
            } else if (ch == 'a' || ch == 'A') {
                soundAlertEnabled = !soundAlertEnabled;
            } else if (ch == 'p' || ch == 'P') {
                isPaused = !isPaused;
            } else if (ch == '+' || ch == '=') {
                if (refreshIntervalMs > 250) refreshIntervalMs -= 250;
            } else if (ch == '-' || ch == '_') {
                if (refreshIntervalMs < 5000) refreshIntervalMs += 250;
            } else if (ch == 'r' || ch == 'R') {
                SampleData();
            }
        }
    }

    void Run() {
        // Clear screen once at startup
        std::cout << "\033[2J\033[H" << std::flush;

        while (running) {
            HandleInput();
            if (!isPaused) {
                SampleData();
            }
            RenderUI();
            std::this_thread::sleep_for(std::chrono::milliseconds(refreshIntervalMs));
        }

        // Clean exit
        std::cout << "\033[2J\033[HExiting System Monitor. Goodbye!\n";
    }
};

int main() {
    SystemMonitor monitor;
    monitor.Run();
    return 0;
}
