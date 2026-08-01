#include <iostream>
#include <thread>
#include <chrono>

#include "logic.hpp"
#include "interaction.hpp"
#include "style.hpp"

int main() {
    MonitorState state;
    WindowsInteraction winApi;

    // Enable Virtual Terminal / ANSI mode
    winApi.InitConsoleSettings();

    // Initial Screen Clear
    std::cout << "\033[2J\033[H" << std::flush;

    // Initial metrics fetch
    winApi.SampleSystemMetrics(state);

    while (state.running) {
        bool forceRefresh = winApi.PollKeyInput(state);

        if (!state.isPaused || forceRefresh) {
            winApi.SampleSystemMetrics(state);
        }

        TerminalStyle::RenderDashboard(state);

        std::this_thread::sleep_for(std::chrono::milliseconds(state.refreshIntervalMs));
    }

    std::cout << "\033[2J\033[HExiting System Monitor. Goodbye!\n";
    return 0;
}
