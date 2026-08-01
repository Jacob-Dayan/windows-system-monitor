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
