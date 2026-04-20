# Run from this folder after copying your Cursor screenshot PNGs here.
# Adjust $pairs if your filenames differ slightly (only the *-GUID part may vary).

$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot

$pairs = @(
    @{ Old = 'Build-*.png'; New = '01-build-success.png' }
    @{ Old = 'Test2-*.png'; New = '02-unit-tests-29-passed.png' }
    @{ Old = 'Test1-*.png'; New = '02b-unit-tests-partial.png' }
    @{ Old = 'tex_and_txt-*.png'; New = '03-sample-txt-and-tex-output.png' }
    @{ Old = 'DoesNotExistFile-*.png'; New = '04-missing-file-error-stdout.png' }
    @{ Old = 'OverleafA-*.png'; New = '05-overleaf-pdf-page-1.png' }
    @{ Old = 'OverleafB-*.png'; New = '06-overleaf-pdf-page-2.png' }
)

foreach ($p in $pairs) {
    $found = Get-ChildItem -Path $p.Old -File -ErrorAction SilentlyContinue | Select-Object -First 1
    if (-not $found) {
        Write-Warning "Skip (not found): $($p.Old)"
        continue
    }
    $dest = Join-Path $PSScriptRoot $p.New
    if (Test-Path $dest) {
        Write-Warning "Already exists, skip: $($p.New)"
        continue
    }
    Rename-Item -LiteralPath $found.FullName -NewName $p.New
    Write-Host "OK: $($found.Name) -> $($p.New)"
}

Write-Host "`nDone. Attach 01-06 (skip 02b if you only want one test screenshot)."
