CXX ?= g++
CXXFLAGS ?= -O3 -std=c++20 -Wall -Wextra
LDFLAGS ?= -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid

SRCS = src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp
TARGET = system_monitor.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
