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

#ifndef INTERACTION_HPP
#define INTERACTION_HPP

#include <chrono>
#include <cstdint>
#include "logic.hpp"

class WindowsInteraction {
private:
    uint64_t prevIdleTime{0};
    uint64_t prevKernelTime{0};
    uint64_t prevUserTime{0};
    std::chrono::steady_clock::time_point lastSampleTime;

public:
    WindowsInteraction();

    void InitConsoleSettings() noexcept;
    void SampleSystemMetrics(MonitorState& state);
    void TriggerAudioBeep(int frequencyHz = 750, int durationMs = 200) noexcept;
    [[nodiscard]] bool PollKeyInput(MonitorState& state) noexcept;

private:
    void SampleCPU(SystemMetrics& metrics) noexcept;
    void SampleRAM(SystemMetrics& metrics, MonitorState& state) noexcept;
    void SampleGPU(SystemMetrics& metrics) noexcept;
    void SampleDrives(SystemMetrics& metrics);
    void SampleNetwork(SystemMetrics& metrics, double elapsedSec);
    void SampleProcesses(SystemMetrics& metrics);
};

#endif // INTERACTION_HPP
