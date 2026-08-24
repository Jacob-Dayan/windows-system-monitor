cmake_minimum_required(VERSION 3.20)
project(windows_system_monitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(system_monitor
    src/main.cpp
    src/logic.cpp
    src/interaction.cpp
    src/style.cpp
)

target_compile_options(system_monitor PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -O3>
)

target_link_libraries(system_monitor PRIVATE
    psapi
    iphlpapi
    ws2_32
    dxgi
    dxguid
)
