# generate_vs2022_minimal.ps1 - Generate minimal Visual Studio 2022 solution
# This creates a lightweight version with ONLY basic ZIP compression/decompression
# Includes: ZLIB (DEFLATE - standard ZIP compression)
# Excludes: BZIP2, LZMA, ZSTD, encryption (PKCRYPT, WZAES)

Write-Host "============================================"
Write-Host "Minizip-ng Minimal VS2022 Generator"
Write-Host "============================================"
Write-Host ""
Write-Host "This will create a MINIMAL build with:"
Write-Host "  [YES] ZLIB (DEFLATE compression - ZIP standard)" -ForegroundColor Green
Write-Host "  [NO]  BZIP2 compression" -ForegroundColor Gray
Write-Host "  [NO]  LZMA compression" -ForegroundColor Gray
Write-Host "  [NO]  ZSTD compression" -ForegroundColor Gray
Write-Host "  [NO]  Encryption (PKCRYPT, WZAES)" -ForegroundColor Gray
Write-Host ""

# Check current path length
$currentPath = Get-Location
$pathLength = $currentPath.Path.Length

if ($pathLength -gt 80) {
    Write-Host "WARNING: Current path is very long ($pathLength characters)" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Windows has a 260-character path limit which may cause build issues."
    Write-Host "Current path: $currentPath"
    Write-Host ""
    Write-Host "RECOMMENDED: Move this project to a shorter path, such as:" -ForegroundColor Yellow
    Write-Host "  C:\minizip-ng"
    Write-Host "  C:\dev\minizip-ng"
    Write-Host "  D:\projects\minizip-ng"
    Write-Host ""
    $response = Read-Host "Do you want to continue anyway? (Y/N)"
    if ($response -ne "Y" -and $response -ne "y") {
        Write-Host ""
        Write-Host "Please move the project to a shorter path and try again." -ForegroundColor Yellow
        Read-Host "Press Enter to exit"
        exit 1
    }
    Write-Host ""
}

# Check if CMake is installed
$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmake) {
    Write-Host "Error: CMake is not found in PATH" -ForegroundColor Red
    Write-Host "Please install CMake from https://cmake.org/download/"
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

# Get CMake version
Write-Host "Detecting CMake..."
cmake --version
Write-Host ""

# Set build directory
$buildDir = "build_vs2022_minimal"

# Check if build directory exists
if (Test-Path $buildDir) {
    Write-Host "Warning: Build directory '$buildDir' already exists" -ForegroundColor Yellow
    Write-Host ""
    $response = Read-Host "Do you want to delete it and regenerate? (Y/N)"
    if ($response -ne "Y" -and $response -ne "y") {
        Write-Host "Cancelled by user."
        Read-Host "Press Enter to exit"
        exit 0
    }
    Write-Host "Removing old build directory..."
    Remove-Item -Path $buildDir -Recurse -Force
}

# Create build directory
Write-Host "Creating build directory: $buildDir"
New-Item -ItemType Directory -Path $buildDir | Out-Null
if (-not (Test-Path $buildDir)) {
    Write-Host "Error: Failed to create build directory" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Change to build directory
Set-Location $buildDir

Write-Host ""
Write-Host "Generating MINIMAL Visual Studio 2022 solution..."
Write-Host ""

# Generate Visual Studio 2022 project (64-bit) - MINIMAL configuration
$cmakeArgs = @(
    "-G", "Visual Studio 17 2022",
    "-A", "x64",
    "-D", "MZ_BUILD_TESTS=ON",
    "-D", "MZ_ZLIB=ON",
    "-D", "MZ_BZIP2=OFF",
    "-D", "MZ_LZMA=OFF",
    "-D", "MZ_ZSTD=OFF",
    "-D", "MZ_PKCRYPT=OFF",
    "-D", "MZ_WZAES=OFF",
    "-D", "MZ_COMPAT=ON",
    ".."
)

& cmake $cmakeArgs

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "Error: CMake configuration failed" -ForegroundColor Red
    Set-Location ..
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "============================================"
Write-Host "MINIMAL VS2022 solution generated successfully!" -ForegroundColor Green
Write-Host "============================================"
Write-Host ""
Write-Host "Solution file: $buildDir\minizip-ng.sln"
Write-Host ""
Write-Host "Configuration:"
Write-Host "  [v] ZLIB (DEFLATE) - Standard ZIP compression" -ForegroundColor Green
Write-Host "  [x] BZIP2 - Disabled" -ForegroundColor Gray
Write-Host "  [x] LZMA  - Disabled" -ForegroundColor Gray
Write-Host "  [x] ZSTD  - Disabled" -ForegroundColor Gray
Write-Host "  [x] Encryption - Disabled" -ForegroundColor Gray
Write-Host ""
Write-Host "This build supports:"
Write-Host "  - Basic ZIP compression/decompression (DEFLATE algorithm)"
Write-Host "  - STORE mode (no compression)"
Write-Host "  - All helper functions work normally"
Write-Host "  - NO password protection"
Write-Host ""
Write-Host "You can now:"
Write-Host "  1. Open the solution in Visual Studio 2022:"
Write-Host "     start minizip-ng.sln"
Write-Host ""
Write-Host "  2. Or build from command line:"
Write-Host "     cmake --build . --config Release"
Write-Host "     cmake --build . --config Debug"
Write-Host ""
Write-Host "Programs that will be built:"
Write-Host "  - minizip.exe              : Main minizip tool"
Write-Host "  - mz_zip_helper_example.exe : Simple compression/extraction example"
Write-Host "  - mz_zip_helper_test.exe   : Test suite"
Write-Host ""

# Ask if user wants to open the solution
$response = Read-Host "Do you want to open Visual Studio 2022 now? (Y/N)"
if ($response -eq "Y" -or $response -eq "y") {
    Write-Host "Opening Visual Studio 2022..."
    Start-Process "minizip-ng.sln"
} else {
    Write-Host ""
    Write-Host "You can open the solution later by running:"
    Write-Host "  start $buildDir\minizip-ng.sln"
    Write-Host ""
}

Set-Location ..
Read-Host "Press Enter to exit"
