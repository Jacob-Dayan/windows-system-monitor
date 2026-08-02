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
