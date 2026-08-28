@echo off
setlocal EnableDelayedExpansion

pushd "%~dp0"

set "TARGET=system_monitor.exe"
set "SRCS=src\main.cpp src\logic.cpp src\interaction.cpp src\style.cpp"
set "LIBS=-lpsapi -liphlpapi -lws2_32 -ldxgi -ldxguid -lole32"
set "BUILD_MODE=release"
set "TOOLCHAIN=auto"

:parse_args
if "%~1"=="" goto after_args
if /i "%~1"=="clean" goto do_clean
if /i "%~1"=="help" goto do_help
if /i "%~1"=="--help" goto do_help
if /i "%~1"=="-h" goto do_help
if /i "%~1"=="debug" (
    set "BUILD_MODE=debug"
    shift
    goto parse_args
)
if /i "%~1"=="release" (
    set "BUILD_MODE=release"
    shift
    goto parse_args
)
if /i "%~1"=="g++" (
    set "TOOLCHAIN=g++"
    shift
    goto parse_args
)
if /i "%~1"=="gcc" (
    set "TOOLCHAIN=g++"
    shift
    goto parse_args
)
if /i "%~1"=="clang" (
    set "TOOLCHAIN=clang"
    shift
    goto parse_args
)
if /i "%~1"=="clang++" (
    set "TOOLCHAIN=clang"
    shift
    goto parse_args
)
if /i "%~1"=="zig" (
    set "TOOLCHAIN=zig"
    shift
    goto parse_args
)
echo Unknown option: %~1
goto do_help

:after_args

if "%BUILD_MODE%"=="debug" (
    set "FLAGS=-std=c++20 -g -O0 -Wall -Wextra -DDEBUG"
) else (
    set "FLAGS=-std=c++20 -O3 -Wall -Wextra -DNDEBUG"
)

if not "%TOOLCHAIN%"=="auto" goto run_build

where g++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    set "TOOLCHAIN=g++"
    goto run_build
)
where clang++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    set "TOOLCHAIN=clang"
    goto run_build
)
where zig >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    set "TOOLCHAIN=zig"
    goto run_build
)
echo Error: No supported compiler found (g++, clang++, zig).
popd
exit /b 1

:run_build
echo Building %TARGET% with %TOOLCHAIN% (%BUILD_MODE%)...

if "%TOOLCHAIN%"=="g++" goto compile_gpp
if "%TOOLCHAIN%"=="clang" goto compile_clang
if "%TOOLCHAIN%"=="zig" goto compile_zig

echo Error: Unsupported toolchain: %TOOLCHAIN%
popd
exit /b 1

:compile_gpp
where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: g++ not found in PATH.
    popd
    exit /b 1
)
g++ %FLAGS% %SRCS% -o %TARGET% %LIBS% -static -static-libgcc -static-libstdc++
goto check_status

:compile_clang
where clang++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: clang++ not found in PATH.
    popd
    exit /b 1
)
clang++ %FLAGS% %SRCS% -o %TARGET% %LIBS%
goto check_status

:compile_zig
where zig >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: zig not found in PATH.
    popd
    exit /b 1
)
zig c++ %FLAGS% -target x86_64-windows-gnu %SRCS% -o %TARGET% %LIBS%
goto check_status

:check_status
if %ERRORLEVEL% EQU 0 (
    echo Build succeeded: %TARGET%
    popd
    exit /b 0
) else (
    echo Build failed with error code %ERRORLEVEL%.
    popd
    exit /b %ERRORLEVEL%
)

:do_clean
if exist system_monitor.exe del /q system_monitor.exe
if exist monitor.exe del /q monitor.exe
if exist *.obj del /q *.obj
if exist *.o del /q *.o
if exist *.pdb del /q *.pdb
if exist *.gch del /q *.gch
if exist src\*.gch del /q src\*.gch
if exist build rmdir /s /q build
echo Clean complete.
popd
exit /b 0

:do_help
echo Usage: build.bat [g++|clang|zig] [release|debug] [clean]
popd
exit /b 0
