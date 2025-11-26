@echo off
REM generate_vs2022_minimal.bat - Generate minimal Visual Studio 2022 solution
REM This creates a lightweight version with ONLY basic ZIP compression/decompression
REM Includes: ZLIB (DEFLATE - standard ZIP compression)
REM Excludes: BZIP2, LZMA, ZSTD, encryption (PKCRYPT, WZAES)

echo ============================================
echo Minizip-ng Minimal VS2022 Generator
echo ============================================
echo.
echo This will create a MINIMAL build with:
echo   [YES] ZLIB (DEFLATE compression - ZIP standard)
echo   [NO]  BZIP2 compression
echo   [NO]  LZMA compression
echo   [NO]  ZSTD compression
echo   [NO]  Encryption (PKCRYPT, WZAES)
echo.

REM Check current path length
set CURRENT_PATH=%CD%
call :strlen result CURRENT_PATH
if %result% GTR 80 (
    echo WARNING: Current path is very long ^(%result% characters^)
    echo.
    echo Windows has a 260-character path limit which may cause build issues.
    echo Current path: %CD%
    echo.
    echo RECOMMENDED: Move this project to a shorter path, such as:
    echo   C:\minizip-ng
    echo   C:\dev\minizip-ng
    echo   D:\projects\minizip-ng
    echo.
    choice /C YN /M "Do you want to continue anyway"
    if errorlevel 2 (
        echo.
        echo Please move the project to a shorter path and try again.
        pause
        exit /b 1
    )
    echo.
)

goto :main

:strlen <resultVar> <stringVar>
(
    setlocal EnableDelayedExpansion
    set "s=!%~2!#"
    set "len=0"
    for %%P in (4096 2048 1024 512 256 128 64 32 16 8 4 2 1) do (
        if "!s:~%%P,1!" NEQ "" (
            set /a "len+=%%P"
            set "s=!s:~%%P!"
        )
    )
)
(
    endlocal
    set "%~1=%len%"
    exit /b
)

:main

REM Check if CMake is installed
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: CMake is not found in PATH
    echo Please install CMake from https://cmake.org/download/
    echo.
    pause
    exit /b 1
)

REM Get CMake version
echo Detecting CMake...
cmake --version
echo.

REM Set build directory
set BUILD_DIR=build_vs2022_minimal

REM Check if build directory exists
if exist %BUILD_DIR% (
    echo Warning: Build directory '%BUILD_DIR%' already exists
    echo.
    choice /C YN /M "Do you want to delete it and regenerate"
    if errorlevel 2 (
        echo Cancelled by user.
        pause
        exit /b 0
    )
    echo Removing old build directory...
    rmdir /S /Q %BUILD_DIR%
)

REM Create build directory
echo Creating build directory: %BUILD_DIR%
mkdir %BUILD_DIR%
if %errorlevel% neq 0 (
    echo Error: Failed to create build directory
    pause
    exit /b 1
)

REM Change to build directory
cd %BUILD_DIR%

echo.
echo Generating MINIMAL Visual Studio 2022 solution...
echo.

REM Generate Visual Studio 2022 project (64-bit) - MINIMAL configuration
cmake -G "Visual Studio 17 2022" -A x64 ^
    -D MZ_BUILD_TESTS=ON ^
    -D MZ_ZLIB=ON ^
    -D MZ_BZIP2=OFF ^
    -D MZ_LZMA=OFF ^
    -D MZ_ZSTD=OFF ^
    -D MZ_PKCRYPT=OFF ^
    -D MZ_WZAES=OFF ^
    -D MZ_COMPAT=ON ^
    ..

if %errorlevel% neq 0 (
    echo.
    echo Error: CMake configuration failed
    cd ..
    pause
    exit /b 1
)

echo.
echo ============================================
echo MINIMAL VS2022 solution generated successfully!
echo ============================================
echo.
echo Solution file: %BUILD_DIR%\minizip-ng.sln
echo.
echo Configuration:
echo   [v] ZLIB (DEFLATE) - Standard ZIP compression
echo   [x] BZIP2 - Disabled
echo   [x] LZMA  - Disabled
echo   [x] ZSTD  - Disabled
echo   [x] Encryption - Disabled
echo.
echo This build supports:
echo   - Basic ZIP compression/decompression (DEFLATE algorithm)
echo   - STORE mode (no compression)
echo   - All helper functions work normally
echo   - NO password protection
echo.
echo You can now:
echo   1. Open the solution in Visual Studio 2022:
echo      start minizip-ng.sln
echo.
echo   2. Or build from command line:
echo      cmake --build . --config Release
echo      cmake --build . --config Debug
echo.
echo Programs that will be built:
echo   - minizip.exe              : Main minizip tool
echo   - mz_zip_helper_example.exe : Simple compression/extraction example
echo   - mz_zip_helper_test.exe   : Test suite
echo.

REM Ask if user wants to open the solution
choice /C YN /M "Do you want to open Visual Studio 2022 now"
if errorlevel 2 (
    echo.
    echo You can open the solution later by running:
    echo   start %BUILD_DIR%\minizip-ng.sln
    echo.
) else (
    echo Opening Visual Studio 2022...
    start minizip-ng.sln
)

cd ..
pause
