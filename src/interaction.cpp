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



#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00
#define _GLIBCXX_PRINT_NEVER_UNICODE

#include <winsock2.h>
#include <windows.h>
#include <psapi.h>
#include <iphlpapi.h>
#include <tlhelp32.h>
#include <conio.h>
#include <dxgi1_4.h>
#include <iostream>
#include <algorithm>

#include "interaction.hpp"

static uint64_t FileTimeToUint64(const FILETIME& ft) {
    return (static_cast<uint64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

WindowsInteraction::WindowsInteraction() {
    lastSampleTime = std::chrono::steady_clock::now();
}

void WindowsInteraction::InitConsoleSettings() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    SetConsoleOutputCP(CP_UTF8);
}

void WindowsInteraction::TriggerAudioBeep(int frequencyHz, int durationMs) {
    Beep(frequencyHz, durationMs);
}

void WindowsInteraction::SampleSystemMetrics(MonitorState& state) {
    auto now = std::chrono::steady_clock::now();
    double elapsedSec = std::chrono::duration<double>(now - lastSampleTime).count();
    if (elapsedSec <= 0.001) elapsedSec = 1.0;
    lastSampleTime = now;

    // 1. Host Info
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    state.metrics.cpuCores = sysInfo.dwNumberOfProcessors;

    char hostBuffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD hostLen = sizeof(hostBuffer);
    if (GetComputerNameA(hostBuffer, &hostLen)) {
        state.metrics.hostname = hostBuffer;
    } else {
        state.metrics.hostname = "Windows Host";
    }

    state.metrics.uptimeSeconds = GetTickCount64() / 1000;

    // 2. CPU
    SampleCPU(state.metrics);

    // 3. RAM & Commit
    SampleRAM(state.metrics, state);

    // 4. GPU & VRAM
    SampleGPU(state.metrics);

    // 5. Drives
    SampleDrives(state.metrics);

    // 6. Network
    SampleNetwork(state.metrics, elapsedSec);

    // 7. Processes
    SampleProcesses(state.metrics);
}

void WindowsInteraction::SampleCPU(SystemMetrics& metrics) {
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
                metrics.cpuLoad = (1.0 - (static_cast<double>(idleDiff) / totalSys)) * 100.0;
                if (metrics.cpuLoad < 0.0) metrics.cpuLoad = 0.0;
                if (metrics.cpuLoad > 100.0) metrics.cpuLoad = 100.0;
            }
        }
        prevIdleTime = idle;
        prevKernelTime = kernel;
        prevUserTime = user;
    }
}

void WindowsInteraction::SampleRAM(SystemMetrics& metrics, MonitorState& state) {
    MEMORYSTATUSEX memState{};
    memState.dwLength = sizeof(memState);
    if (GlobalMemoryStatusEx(&memState)) {
        metrics.ramTotalBytes = memState.ullTotalPhys;
        metrics.ramAvailBytes = memState.ullAvailPhys;
        metrics.ramUsedBytes = memState.ullTotalPhys - memState.ullAvailPhys;
        metrics.ramLoadPercent = static_cast<double>(memState.dwMemoryLoad);

        metrics.pageTotalBytes = memState.ullTotalPageFile;
        metrics.pageAvailBytes = memState.ullAvailPageFile;
        metrics.pageUsedBytes = memState.ullTotalPageFile - memState.ullAvailPageFile;
        metrics.pageLoadPercent = (memState.ullTotalPageFile > 0) ?
            (static_cast<double>(metrics.pageUsedBytes) / memState.ullTotalPageFile) * 100.0 : 0.0;

        state.PushRamHistory(metrics.ramLoadPercent);

        if (state.soundAlertEnabled && memState.dwMemoryLoad >= static_cast<DWORD>(state.alertThreshold)) {
            TriggerAudioBeep(750, 150);
        }
    }
}

void WindowsInteraction::SampleGPU(SystemMetrics& metrics) {
    metrics.gpu.available = false;
    IDXGIFactory4* pFactory = nullptr;
    if (SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pFactory))) {
        IDXGIAdapter1* pAdapter = nullptr;
        if (SUCCEEDED(pFactory->EnumAdapters1(0, &pAdapter))) {
            DXGI_ADAPTER_DESC1 desc;
            if (SUCCEEDED(pAdapter->GetDesc1(&desc))) {
                char nameBuf[256];
                WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1, nameBuf, sizeof(nameBuf), NULL, NULL);
                metrics.gpu.name = nameBuf;
                metrics.gpu.vramTotalBytes = desc.DedicatedVideoMemory;

                IDXGIAdapter3* pAdapter3 = nullptr;
                if (SUCCEEDED(pAdapter->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&pAdapter3))) {
                    DXGI_QUERY_VIDEO_MEMORY_INFO info;
                    if (SUCCEEDED(pAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info))) {
                        metrics.gpu.vramUsedBytes = info.CurrentUsage;
                        if (metrics.gpu.vramTotalBytes > 0) {
                            metrics.gpu.vramUsagePercent = (static_cast<double>(info.CurrentUsage) / metrics.gpu.vramTotalBytes) * 100.0;
                        } else if (info.Budget > 0) {
                            metrics.gpu.vramTotalBytes = info.Budget;
                            metrics.gpu.vramUsagePercent = (static_cast<double>(info.CurrentUsage) / info.Budget) * 100.0;
                        }
                        metrics.gpu.available = true;
                    }
                    pAdapter3->Release();
                } else if (metrics.gpu.vramTotalBytes > 0) {
                    metrics.gpu.available = true;
                }
            }
            pAdapter->Release();
        }
        pFactory->Release();
    }
}

void WindowsInteraction::SampleDrives(SystemMetrics& metrics) {
    metrics.drives.clear();
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
                    metrics.drives.push_back(info);
                }
            }
            pDrive += strlen(pDrive) + 1;
        }
    }
}

void WindowsInteraction::SampleNetwork(SystemMetrics& metrics, double elapsedSec) {
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

            if (metrics.net.rxBytes > 0 && elapsedSec > 0) {
                metrics.net.rxSpeed = (currentRx >= metrics.net.rxBytes) ? static_cast<double>(currentRx - metrics.net.rxBytes) / elapsedSec : 0.0;
                metrics.net.txSpeed = (currentTx >= metrics.net.txBytes) ? static_cast<double>(currentTx - metrics.net.txBytes) / elapsedSec : 0.0;
            }
            metrics.net.rxBytes = currentRx;
            metrics.net.txBytes = currentTx;
        }
        if (pIfTable) free(pIfTable);
    }
}

void WindowsInteraction::SampleProcesses(SystemMetrics& metrics) {
    metrics.topProcesses.clear();
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (pe32.th32ProcessID == 0) continue;
                HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
                if (hProc) {
                    PROCESS_MEMORY_COUNTERS pmc;
                    if (GetProcessMemoryInfo(hProc, &pmc, sizeof(pmc))) {
                        ProcessItem item;
                        item.pid = pe32.th32ProcessID;
                        item.name = pe32.szExeFile;
                        item.memoryBytes = pmc.WorkingSetSize;
                        item.memoryMB = static_cast<double>(pmc.WorkingSetSize) / (1024.0 * 1024.0);
                        metrics.topProcesses.push_back(item);
                    }
                    CloseHandle(hProc);
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }

    std::sort(metrics.topProcesses.begin(), metrics.topProcesses.end(), [](const ProcessItem& a, const ProcessItem& b) {
        return a.memoryBytes > b.memoryBytes;
    });

    if (metrics.topProcesses.size() > 15) {
        metrics.topProcesses.resize(15);
    }
}

bool WindowsInteraction::PollKeyInput(MonitorState& state) {
    if (!_kbhit()) {
        return false;
    }

    int ch = _getch();
    if (ch == 0 || ch == 224) {
        ch = _getch();
    }

    switch (ch) {
        case 'q':
        case 'Q':
        case 27:
            state.running = false;
            break;
        case '\t':
            state.activeTab = (state.activeTab % 4) + 1;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
            state.activeTab = ch - '0';
            break;
        case 'a':
        case 'A':
            state.soundAlertEnabled = !state.soundAlertEnabled;
            break;
        case 'p':
        case 'P':
            state.isPaused = !state.isPaused;
            break;
        case '+':
        case '=':
            if (state.refreshIntervalMs > 250) state.refreshIntervalMs -= 250;
            break;
        case '-':
        case '_':
            if (state.refreshIntervalMs < 5000) state.refreshIntervalMs += 250;
            break;
        case 'r':
        case 'R':
            return true;
        default:
            return false;
    }

    return true;
}
