# Visual Studio multi-config: ctest needs -C Debug|Release
param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug'
)

$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$buildDir = Join-Path $root 'build'

if (-not (Test-Path $buildDir)) {
    Write-Error "Missing '$buildDir'. Configure and build first, e.g.:`n  cmake -S . -B build`n  cmake --build build --config $Configuration"
}

$ctest = Get-Command ctest -ErrorAction SilentlyContinue
if (-not $ctest) {
    Write-Error 'ctest not found. Use a Visual Studio / CMake command prompt, or add CMake to PATH.'
}

& ctest --test-dir $buildDir -C $Configuration --output-on-failure @args
