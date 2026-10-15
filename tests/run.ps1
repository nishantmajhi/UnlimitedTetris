# ================================================================
#  UnlimitedTetris — tests/run.ps1
# ================================================================
#  Builds and runs both suites:
#
#    1. tests/run.cpp         against the working tree
#    2. tests/original_test.cpp against the sources as they were in
#       the repository's first commit, fetched with `git show` into
#       a scratch directory. That half asserts the two defects are
#       present in the original, which is how the regression tests
#       above are known to be testing something.
#
#  Usage:   powershell -ExecutionPolicy Bypass -File tests\run.ps1
#           powershell ... -File tests\run.ps1 -SkipOriginal
#
#  Neither binary links OpenGL: tests/stubs/GL/glut.h replaces the
#  real header and glstub.cpp records the drawing.
# ================================================================

param([switch]$SkipOriginal)

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot

# ── Find a C++ compiler ───────────────────────────────────────
#  Preferred first: the MinGW-w64 13.2 that ships inside Strawberry
#  Perl, which is the toolchain this suite was written against.
$candidates = @(
    'D:\strawberry-perl-5.42.0.1-64bit-portable\c\bin\g++.exe',
    'D:\TDM-GCC-64\bin\g++.exe'
)
$cxx = $null
foreach ($c in $candidates) { if (Test-Path $c) { $cxx = $c; break } }
if (-not $cxx) {
    $found = Get-Command g++ -ErrorAction SilentlyContinue
    if ($found) { $cxx = $found.Source }
}
if (-not $cxx) {
    Write-Error "No g++ found. Set one on PATH, or edit the candidate list above."
    exit 2
}
Write-Host "compiler: $cxx"

# -static so the binaries do not need the compiler's runtime DLLs
# on PATH. Without it they fail to start at all, with exit code
# 0xC000007B and no output, which looks like a suite that ran and
# printed nothing rather than one that never started.
$commonFlags = @('-std=c++17', '-static', '-Wall', '-Wextra')

$failed = 0

# ── 1. The working tree ───────────────────────────────────────
Write-Host "`n=== building the regression suite ==="
$testExe = Join-Path $projectRoot 'tests\run_tests.exe'
$sources = @(
    'tests\run.cpp', 'tests\stubs\glstub.cpp',
    'src\globals.cpp', 'src\highscore.cpp',
    'src\unit2_scan.cpp', 'src\unit3_transform.cpp', 'src\unit4_3d.cpp',
    'src\unit5_curves.cpp', 'src\unit8_lighting.cpp', 'src\unit10_opengl.cpp'
) | ForEach-Object { Join-Path $projectRoot $_ }

& $cxx @commonFlags '-I' (Join-Path $projectRoot 'tests\stubs') `
       '-I' (Join-Path $projectRoot 'include') @sources '-o' $testExe
if ($LASTEXITCODE -ne 0) { Write-Error "the regression suite did not build"; exit 2 }

& $testExe
if ($LASTEXITCODE -ne 0) { $failed = 1 }

# ── 2. The baseline commit ────────────────────────────────────
if (-not $SkipOriginal) {
    Write-Host "`n=== building the baseline suite ==="

    Push-Location $projectRoot
    $base = (& git rev-list --max-parents=0 HEAD) | Select-Object -First 1
    if ($LASTEXITCODE -ne 0 -or -not $base) {
        Pop-Location
        Write-Host "no git repository here; skipping the baseline suite"
        exit $failed
    }
    $subject = (& git log -1 --format=%s $base)
    Write-Host "baseline: $($base.Substring(0,7))  $subject"

    $stage = Join-Path ([System.IO.Path]::GetTempPath()) "ut-baseline-$($base.Substring(0,7))"
    New-Item -ItemType Directory -Force (Join-Path $stage 'src')     | Out-Null
    New-Item -ItemType Directory -Force (Join-Path $stage 'include') | Out-Null
    New-Item -ItemType Directory -Force (Join-Path $stage 'tests')   | Out-Null

    $utf8NoBom = New-Object System.Text.UTF8Encoding $false
    $baselineFiles = & git ls-tree -r --name-only $base |
        Where-Object { $_.StartsWith('src/') -or $_.StartsWith('include/') }
    foreach ($f in $baselineFiles) {
        $text = (& git show "${base}:$f") -join "`n"
        $dest = Join-Path $stage ($f -replace '/', '\')
        [System.IO.File]::WriteAllText($dest, $text, $utf8NoBom)
    }
    Pop-Location

    Copy-Item (Join-Path $projectRoot 'tests\harness.h')         (Join-Path $stage 'tests') -Force
    Copy-Item (Join-Path $projectRoot 'tests\original_test.cpp') (Join-Path $stage 'tests') -Force
    Copy-Item (Join-Path $projectRoot 'tests\stubs') (Join-Path $stage 'tests\stubs') -Recurse -Force

    $origExe = Join-Path $stage 'original_test.exe'
    $origSources = @(
        'tests\original_test.cpp', 'tests\stubs\glstub.cpp',
        'src\globals.cpp', 'src\unit2_scan.cpp', 'src\unit3_transform.cpp',
        'src\unit4_3d.cpp', 'src\unit5_curves.cpp', 'src\unit8_lighting.cpp',
        'src\unit10_opengl.cpp'
    ) | ForEach-Object { Join-Path $stage $_ }

    # The baseline is compiled without -Wall: it is not being
    # reviewed, it is being run.
    & $cxx '-std=c++17' '-static' '-I' (Join-Path $stage 'tests\stubs') `
           '-I' (Join-Path $stage 'include') @origSources '-o' $origExe
    if ($LASTEXITCODE -ne 0) { Write-Error "the baseline suite did not build"; exit 2 }

    & $origExe
    if ($LASTEXITCODE -ne 0) { $failed = 1 }
}

if ($failed -ne 0) { Write-Host "`nSOMETHING FAILED" } else { Write-Host "`nall suites passed" }
exit $failed
