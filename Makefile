# Windows System Monitor - Universal Cross-Platform Makefile

TARGET      ?= monitor.exe
BUILD_DIR   ?= build/obj
BIN_DIR     ?= .

SRCS        := src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp
OBJS        := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS        := $(OBJS:.o=.d)

DEBUG       ?= 0
STATIC      ?= 1
COLOR       ?= 1
ZIG_TARGET  ?= x86_64-windows-gnu

# Host environment detection
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

# Check if CXX was overridden by user
ifeq ($(origin CXX),default)
    AUTO_CXX := 1
else ifeq ($(origin CXX),undefined)
    AUTO_CXX := 1
else
    AUTO_CXX := 0
endif

TOOLCHAIN_STATUS := OK

ifeq ($(AUTO_CXX),1)
    ifeq ($(HOST_OS),Windows)
        ifneq ($(call check_tool,g++),)
            CXX       := g++
            TOOLCHAIN := MinGW GCC (Native)
        else ifneq ($(call check_tool,clang++),)
            CXX       := clang++
            TOOLCHAIN := LLVM Clang++ (Native)
        else ifneq ($(call check_tool,zig),)
            CXX       := zig c++ -target $(ZIG_TARGET)
            TOOLCHAIN := Zig C++ ($(ZIG_TARGET))
        else
            CXX       := g++
            TOOLCHAIN := Default (g++)
            TOOLCHAIN_STATUS := NOT_FOUND
        endif
    else
        ifneq ($(call check_tool,x86_64-w64-mingw32-g++),)
            CXX       := x86_64-w64-mingw32-g++
            TOOLCHAIN := MinGW-w64 GCC (Cross)
        else ifneq ($(call check_tool,x86_64-w64-mingw32-c++),)
            CXX       := x86_64-w64-mingw32-c++
            TOOLCHAIN := MinGW-w64 C++ (Cross)
        else ifneq ($(call check_tool,zig),)
            CXX       := zig c++ -target $(ZIG_TARGET)
            TOOLCHAIN := Zig C++ ($(ZIG_TARGET))
        else ifneq ($(call check_tool,clang++),)
            CXX       := clang++ --target=x86_64-w64-windows-gnu
            TOOLCHAIN := Clang Cross (x86_64-w64-windows-gnu)
        else
            CXX       := g++
            TOOLCHAIN := None (Host g++ cannot compile Windows binaries)
            TOOLCHAIN_STATUS := NOT_FOUND
        endif
    endif
else
    ifeq ($(CXX),zig)
        override CXX := zig c++ -target $(ZIG_TARGET)
        TOOLCHAIN := User Specified (Zig C++ $(ZIG_TARGET))
    else
        TOOLCHAIN := User Specified ($(CXX))
        ifneq ($(findstring zig,$(CXX)),)
            ifeq ($(findstring -target,$(CXX)),)
                override CXX += -target $(ZIG_TARGET)
            endif
        endif
    endif
endif

BASE_CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -MMD -MP
LIBS          := -lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32

ifeq ($(DEBUG),1)
    BUILD_TYPE := Debug
    CXXFLAGS   := $(BASE_CXXFLAGS) -g -O0 -DDEBUG
else
    BUILD_TYPE := Release
    CXXFLAGS   := $(BASE_CXXFLAGS) -O3 -DNDEBUG
endif

ifeq ($(STATIC),1)
    ifneq ($(findstring mingw,$(TOOLCHAIN)),)
        LDFLAGS += -static -static-libgcc -static-libstdc++
    else ifneq ($(findstring MinGW,$(TOOLCHAIN)),)
        LDFLAGS += -static -static-libgcc -static-libstdc++
    endif
endif

ifeq ($(COLOR),1)
    C_RESET   := \033[0m
    C_BOLD    := \033[1m
    C_CYAN    := \033[36m
    C_GREEN   := \033[32m
    C_YELLOW  := \033[33m
    C_RED     := \033[31m
    C_MAGENTA := \033[35m
else
    C_RESET   :=
    C_BOLD    :=
    C_CYAN    :=
    C_GREEN   :=
    C_YELLOW  :=
    C_RED     :=
    C_MAGENTA :=
endif

.PHONY: all check_toolchain release debug clean distclean info help

all: check_toolchain $(TARGET)
	@printf "$(C_BOLD)$(C_GREEN)✔ Build complete:$(C_RESET) %s (%s, %s)\n" "$(TARGET)" "$(TOOLCHAIN)" "$(BUILD_TYPE)"

release:
	@$(MAKE) --no-print-directory DEBUG=0 all

debug:
	@$(MAKE) --no-print-directory DEBUG=1 all

check_toolchain:
ifeq ($(TOOLCHAIN_STATUS),NOT_FOUND)
	@printf "$(C_BOLD)$(C_RED)ERROR: No suitable Windows cross-compiler was found on your system.$(C_RESET)\n\n"
	@printf "To compile $(TARGET), please install one of the following:\n"
	@printf "  $(C_BOLD)1. MinGW-w64 (Recommended on Ubuntu/Debian):$(C_RESET)\n"
	@printf "     $(C_CYAN)sudo apt update && sudo apt install -y g++-mingw-w64-x86-64$(C_RESET)\n\n"
	@printf "  $(C_BOLD)2. Zig Compiler (Zero-dependency cross-compiler):$(C_RESET)\n"
	@printf "     $(C_CYAN)https://ziglang.org/download/$(C_RESET) or install via package manager\n\n"
	@printf "  $(C_BOLD)3. Or explicitly pass your compiler to make:$(C_RESET)\n"
	@printf "     $(C_CYAN)make CXX=x86_64-w64-mingw32-g++$(C_RESET)\n"
	@printf "     $(C_CYAN)make CXX=\"zig c++\" ZIG_TARGET=x86_64-windows-gnu$(C_RESET)\n\n"
	@exit 1
endif

$(TARGET): $(OBJS)
	@printf "$(C_BOLD)$(C_CYAN)[LD]$(C_RESET) %s\n" "$@"
	@$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)

$(BUILD_DIR)/%.o: src/%.cpp
	@$(call MKDIR_P,$(BUILD_DIR))
	@printf "$(C_BOLD)$(C_MAGENTA)[CXX]$(C_RESET) %s\n" "$<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	@printf "$(C_BOLD)$(C_YELLOW)Cleaning build artifacts...$(C_RESET)\n"
ifeq ($(HOST_OS),Windows)
	@$(RM_DIR) $(BUILD_DIR)
	@$(RM) $(TARGET)
else
	@$(RM_DIR) $(BUILD_DIR)
	@$(RM) $(TARGET)
endif
	@printf "$(C_BOLD)$(C_GREEN)✔ Clean complete.$(C_RESET)\n"

distclean: clean
ifeq ($(HOST_OS),Windows)
	@$(RM) *.exe *.pdb *.obj *.o *.gch *.d 2>nul || (exit 0)
else
	@$(RM) *.exe *.pdb *.obj *.o src/*.gch *.d 2>/dev/null || true
endif

info:
	@printf "$(C_BOLD)Windows System Monitor - Build Information$(C_RESET)\n"
	@printf "  $(C_CYAN)Host OS:$(C_RESET)          %s\n" "$(HOST_OS)"
	@printf "  $(C_CYAN)Toolchain:$(C_RESET)        %s\n" "$(TOOLCHAIN)"
	@printf "  $(C_CYAN)Compiler (CXX):$(C_RESET)  %s\n" "$(CXX)"
	@printf "  $(C_CYAN)Build Type:$(C_RESET)      %s\n" "$(BUILD_TYPE)"
	@printf "  $(C_CYAN)Static Link:$(C_RESET)     %s\n" "$(if $(filter 1,$(STATIC)),Enabled,Disabled)"
	@printf "  $(C_CYAN)Target Output:$(C_RESET)   %s\n" "$(TARGET)"
	@printf "  $(C_CYAN)Objects Dir:$(C_RESET)     %s\n" "$(BUILD_DIR)"
	@printf "  $(C_CYAN)CXX Flags:$(C_RESET)       %s\n" "$(CXXFLAGS)"
	@printf "  $(C_CYAN)Link Libraries:$(C_RESET)  %s\n" "$(LIBS) $(LDFLAGS)"

help:
	@printf "$(C_BOLD)Windows System Monitor Build System$(C_RESET)\n\n"
	@printf "$(C_BOLD)Usage:$(C_RESET) make [target] [options]\n\n"
	@printf "$(C_BOLD)Targets:$(C_RESET)\n"
	@printf "  $(C_CYAN)all$(C_RESET)         Build optimized release binary (default)\n"
	@printf "  $(C_CYAN)release$(C_RESET)     Build optimized release binary (-O3, -DNDEBUG)\n"
	@printf "  $(C_CYAN)debug$(C_RESET)       Build debug binary (-g, -O0, -DDEBUG)\n"
	@printf "  $(C_CYAN)clean$(C_RESET)       Remove intermediate objects and target binary\n"
	@printf "  $(C_CYAN)distclean$(C_RESET)   Remove all build artifacts and leftover binaries\n"
	@printf "  $(C_CYAN)info$(C_RESET)        Display detected toolchain and active configuration\n"
	@printf "  $(C_CYAN)help$(C_RESET)        Display this help message\n\n"
	@printf "$(C_BOLD)Variables & Options:$(C_RESET)\n"
	@printf "  $(C_YELLOW)CXX=<compiler>$(C_RESET)         Override compiler (e.g. CXX=x86_64-w64-mingw32-g++ or CXX=\"zig c++\")\n"
	@printf "  $(C_YELLOW)ZIG_TARGET=<triple>$(C_RESET)    Zig target (default: x86_64-windows-gnu or x86_64-windows-msvc)\n"
	@printf "  $(C_YELLOW)DEBUG=1$(C_RESET)                Enable debug symbols and disable optimizations\n"
	@printf "  $(C_YELLOW)STATIC=0$(C_RESET)               Disable static runtime linking (default is STATIC=1)\n"
	@printf "  $(C_YELLOW)COLOR=0$(C_RESET)                Disable colored terminal output\n\n"
	@printf "$(C_BOLD)Examples:$(C_RESET)\n"
	@printf "  make                       # Auto-detects toolchain and builds release\n"
	@printf "  make debug                 # Builds with debug symbols\n"
	@printf "  make CXX=\"zig c++\"         # Compiles using Zig C++\n"
	@printf "  make info                  # Inspect detected configuration\n"
