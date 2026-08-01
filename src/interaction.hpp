#ifndef INTERACTION_HPP
#define INTERACTION_HPP

#include "logic.hpp"

class WindowsInteraction {
private:
    uint64_t prevIdleTime{0};
    uint64_t prevKernelTime{0};
    uint64_t prevUserTime{0};
    std::chrono::steady_clock::time_point lastSampleTime;

public:
    WindowsInteraction();

    void InitConsoleSettings();
    void SampleSystemMetrics(MonitorState& state);
    void TriggerAudioBeep(int frequencyHz = 750, int durationMs = 200);
    bool PollKeyInput(MonitorState& state);

private:
    void SampleCPU(SystemMetrics& metrics);
    void SampleRAM(SystemMetrics& metrics, MonitorState& state);
    void SampleGPU(SystemMetrics& metrics);
    void SampleDrives(SystemMetrics& metrics);
    void SampleNetwork(SystemMetrics& metrics, double elapsedSec);
    void SampleProcesses(SystemMetrics& metrics);
};

#endif // INTERACTION_HPP
