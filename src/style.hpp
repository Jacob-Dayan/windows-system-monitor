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


#ifndef STYLE_HPP
#define STYLE_HPP

#include <string>
#include <vector>
#include "logic.hpp"

class TerminalStyle {
public:
    static std::string FormatBytes(uint64_t bytes);
    static std::string FormatSpeed(double bytesPerSec);
    static std::string FormatUptime(uint64_t totalSec);

    static std::string RenderProgressBar(double percent, int width = 24);
    static std::string RenderSparkline(const std::vector<double>& history, int width = 30);

    static void RenderDashboard(const MonitorState& state);
};

#endif // STYLE_HPP
