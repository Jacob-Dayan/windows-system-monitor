if (-not (Test-Path "build")) {
    $null = New-Item -ItemType Directory -Path "build"
}

Write-Host "Building monitor.exe (dynamic, cl)..."

cl /nologo /std:c++20 /MP /D_HAS_EXCEPTIONS=0 /MD `
   /Ox /O2 /Ob3 /Oi /Ot /Oy /GF /Gy /Gw /GL /GR- /permissive- /fp:fast /DNDEBUG `
   /Fo:"build\" /Fe:"monitor.exe" `
   (Get-ChildItem src/*.cpp) `
   /link /nologo /LTCG /OPT:REF /OPT:ICF /INCREMENTAL:NO `
   dxgi.lib iphlpapi.lib psapi.lib ws2_32.lib ole32.lib $args

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Write-Host "Done."
