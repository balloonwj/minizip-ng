# generate_vs2022.ps1 - Generate Visual Studio 2022 solution for minizip-ng
# This script creates a Visual Studio 2022 project with test programs enabled

Write-Host "========================================"
Write-Host "Minizip-ng Visual Studio 2022 Generator"
Write-Host "========================================"
Write-Host ""

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
$buildDir = "build_vs2022"

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
Write-Host "Generating Visual Studio 2022 solution..."
Write-Host ""

# Generate Visual Studio 2022 project (64-bit)
$cmakeArgs = @(
    "-G", "Visual Studio 17 2022",
    "-A", "x64",
    "-D", "MZ_BUILD_TESTS=ON",
    "-D", "MZ_ZLIB=ON",
    "-D", "MZ_BZIP2=ON",
    "-D", "MZ_LZMA=ON",
    "-D", "MZ_ZSTD=OFF",
    "-D", "MZ_PKCRYPT=ON",
    "-D", "MZ_WZAES=ON",
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
Write-Host "========================================"
Write-Host "Visual Studio 2022 solution generated successfully!" -ForegroundColor Green
Write-Host "========================================"
Write-Host ""
Write-Host "Solution file: $buildDir\minizip-ng.sln"
Write-Host ""
Write-Host "You can now:"
Write-Host "  1. Open the solution in Visual Studio 2022:"
Write-Host "     start minizip-ng.sln"
Write-Host ""
Write-Host "  2. Or build from command line:"
Write-Host "     cmake --build . --config Release"
Write-Host "     cmake --build . --config Debug"
Write-Host ""
Write-Host "Test programs that will be built:"
Write-Host "  - minizip.exe              : Main minizip command-line tool"
Write-Host "  - mz_zip_helper_example.exe : Simple compression/extraction example"
Write-Host "  - mz_zip_helper_test.exe   : Comprehensive test suite with absolute paths"
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
