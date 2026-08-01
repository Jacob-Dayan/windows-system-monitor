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
