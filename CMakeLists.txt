cmake_minimum_required(VERSION 3.20)
project(windows_system_monitor VERSION 0.1.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_STATIC "Link standard libraries statically" ON)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

add_executable(system_monitor
    src/main.cpp
    src/logic.cpp
    src/interaction.cpp
    src/style.cpp
)

target_compile_options(system_monitor PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
        -Wall -Wextra -Wpedantic -O3 -ffunction-sections -fdata-sections
        -fno-rtti -fomit-frame-pointer -fmerge-all-constants -DNDEBUG
    >
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /O2 /Ox /Ob3 /Oi /Ot /Oy /GF /Gy /Gw /GL /GR- /permissive- /DNDEBUG
        $<$<BOOL:${BUILD_STATIC}>:/MT>
        $<$<NOT:$<BOOL:${BUILD_STATIC}>>:/MD>
    >
)

target_link_libraries(system_monitor PRIVATE
    psapi
    iphlpapi
    ws2_32
    dxgi
    dxguid
    ole32
)

if(MINGW OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    target_link_options(system_monitor PRIVATE
        -Wl,--gc-sections
        -Wl,--strip-all
        -s
        $<$<BOOL:${BUILD_STATIC}>:-static -static-libgcc -static-libstdc++ -Wl,--exclude-libs,ALL>
        $<$<NOT:$<BOOL:${BUILD_STATIC}>>:-Wl,--as-needed>
    )
elseif(MSVC)
    target_link_options(system_monitor PRIVATE
        /LTCG
        /OPT:REF
        /OPT:ICF
        /INCREMENTAL:NO
    )
endif()
