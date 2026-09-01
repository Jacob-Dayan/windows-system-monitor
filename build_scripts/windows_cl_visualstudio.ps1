if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

cl /std:c++20 /EHsc /MD /O2 /GL /Fo:"build\" /Fe:"monitor.exe" src/*.cpp /link /LTCG dxgi.lib iphlpapi.lib
