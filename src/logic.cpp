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


#include "logic.hpp"

MonitorState::MonitorState() {
    metrics.ramHistory.reserve(maxHistorySize);
}

void MonitorState::PushRamHistory(double load) {
    metrics.ramHistory.push_back(load);
    if (metrics.ramHistory.size() > maxHistorySize) {
        metrics.ramHistory.erase(metrics.ramHistory.begin());
    }
}
