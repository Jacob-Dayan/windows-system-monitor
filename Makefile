TARGET      ?= monitor.exe
BUILD_DIR   ?= build/obj

SRCS        := $(wildcard src/*.cpp)
OBJS        := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS        := $(OBJS:.o=.d)

DEBUG       ?= 0
STATIC      ?= 1
ZIG_TARGET  ?= x86_64-windows-gnu

# Host detection
ifeq ($(OS),Windows_NT)
    HOST_OS     := Windows
    SHELL       := cmd.exe
    MKDIR_P      = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
    RM          := del /Q /F 2>nul || (exit 0)
    RM_DIR      := rmdir /S /Q 2>nul || (exit 0)
    check_tool   = $(shell where $(1) 2>nul)
else
    HOST_OS     := $(shell uname -s 2>/dev/null || echo POSIX)
    MKDIR_P      = mkdir -p $(1)
    RM          := rm -f
    RM_DIR      := rm -rf
    check_tool   = $(shell which $(1) 2>/dev/null)
endif

# Toolchain detection
ifeq ($(origin CXX),default)
    AUTO_CXX := 1
else ifeq ($(origin CXX),undefined)
    AUTO_CXX := 1
else
    AUTO_CXX := 0
endif

ifeq ($(AUTO_CXX),1)
    ifeq ($(HOST_OS),Windows)
        ifneq ($(call check_tool,g++),)
            CXX := g++
        else ifneq ($(call check_tool,clang++),)
            CXX := clang++
        else ifneq ($(call check_tool,zig),)
            CXX := zig c++ -target $(ZIG_TARGET)
        else
            CXX := g++
        endif
    else
        ifneq ($(call check_tool,x86_64-w64-mingw32-g++),)
            CXX := x86_64-w64-mingw32-g++
        else ifneq ($(call check_tool,x86_64-w64-mingw32-c++),)
            CXX := x86_64-w64-mingw32-c++
        else ifneq ($(call check_tool,zig),)
            CXX := zig c++ -target $(ZIG_TARGET)
        else ifneq ($(call check_tool,clang++),)
            CXX := clang++ --target=x86_64-w64-windows-gnu
        else
            CXX := g++
        endif
    endif
else
    ifeq ($(CXX),zig)
        override CXX := zig c++ -target $(ZIG_TARGET)
    endif
endif

BASE_CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -MMD -MP -pipe
LIBS          := -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32

ifeq ($(DEBUG),1)
    CXXFLAGS := $(BASE_CXXFLAGS) -g -O0 -DDEBUG
else
    CXXFLAGS := $(BASE_CXXFLAGS) -O3 -flto=auto -fno-fat-lto-objects \
                -funroll-loops -fstrict-aliasing -ffast-math -finline-functions \
                -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions \
                -fomit-frame-pointer -fmerge-all-constants -fno-ident -DNDEBUG
    LDFLAGS  += -Wl,--gc-sections -Wl,--strip-all -s
endif

ifeq ($(STATIC),1)
    LDFLAGS += -static -static-libgcc -static-libstdc++ -Wl,--exclude-libs,ALL
else
    LDFLAGS += -Wl,--as-needed
endif

.PHONY: all release debug clean

all: $(TARGET)

release:
	@$(MAKE) --no-print-directory DEBUG=0 all

debug:
	@$(MAKE) --no-print-directory DEBUG=1 all

$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)

$(BUILD_DIR)/%.o: src/%.cpp
	@$(call MKDIR_P,$(BUILD_DIR))
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	@echo "Cleaning..."
	@$(RM_DIR) $(BUILD_DIR)
	@$(RM) $(TARGET)
