Set-Location "$PSScriptRoot\..\.."

if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Write-Host "Building monitor.exe (static, cl)..."

cl /nologo /std:c++20 /EHsc /MT /Ox /O2 /Ob3 /Oi /Ot /Oy /GF /Gy /Gw /GL /GR- /permissive- /DNDEBUG `
   /Fo:"build\" /Fe:"monitor.exe" `
   src/main.cpp src/logic.cpp src/interaction.cpp src/style.cpp `
   /link /nologo /LTCG /OPT:REF /OPT:ICF /INCREMENTAL:NO `
   dxgi.lib iphlpapi.lib psapi.lib ws2_32.lib ole32.lib
