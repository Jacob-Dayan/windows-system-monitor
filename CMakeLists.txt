cmake_minimum_required(VERSION 3.20)
project(windows_system_monitor VERSION 0.1.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Option for static linking
option(BUILD_STATIC "Link standard libraries statically" ON)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

add_executable(system_monitor
    src/main.cpp
    src/logic.cpp
    src/interaction.cpp
    src/style.cpp
)

target_compile_options(system_monitor PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic -O3>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /O2 /permissive->
)

target_link_libraries(system_monitor PRIVATE
    psapi
    iphlpapi
    ws2_32
    dxgi
    dxguid
    ole32
)

if(BUILD_STATIC)
    if(MINGW OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
        target_link_options(system_monitor PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
endif()
