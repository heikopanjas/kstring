# KString Library - Build Script for Windows (PowerShell)
# This script provides a comprehensive build system with multiple options

[CmdletBinding()]
param(
    [switch]$Clean,
    [switch]$Debug,
    [switch]$Verbose,
    [switch]$Test,
    [switch]$Examples,
    [ValidateSet("Ninja", "Visual Studio 17 2022", "Visual Studio 16 2019", "NMake Makefiles")]
    [string]$Generator = "Ninja",
    [switch]$Help
)

# Script configuration
$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $ScriptDir "_build"
$BuildType = if ($Debug) { "Debug" } else { "Release" }

# Color functions for output
function Write-ColorMessage {
    param(
        [string]$Message,
        [string]$Color = "White"
    )
    Write-Host $Message -ForegroundColor $Color
}

# Print usage information
function Show-Usage {
    Write-Host @"

KString Library Build Script for Windows

USAGE:
    .\build.ps1 [OPTIONS]

OPTIONS:
    -Clean              Clean build directory before building
    -Debug              Build in Debug mode (default: Release)
    -Verbose            Enable verbose build output
    -Test               Run tests after building
    -Examples           Run examples after building
    -Generator <name>   Specify CMake generator (default: Ninja)
                        Options: Ninja, "Visual Studio 17 2022",
                                "Visual Studio 16 2019", "NMake Makefiles"
    -Help               Show this help message

EXAMPLES:
    .\build.ps1                    # Standard release build
    .\build.ps1 -Clean             # Clean and build
    .\build.ps1 -Debug -Verbose    # Debug build with verbose output
    .\build.ps1 -Clean -Examples   # Clean, build, and run examples

"@
}

# Check for required tools
function Test-Dependencies {
    Write-ColorMessage "Checking dependencies..." -Color Cyan

    # Check for CMake
    $cmake = Get-Command cmake -ErrorAction SilentlyContinue
    if (-not $cmake) {
        Write-ColorMessage "Error: CMake is not installed" -Color Red
        Write-ColorMessage "Please install CMake 3.30 or higher" -Color Yellow
        exit 1
    }

    $cmakeVersion = (cmake --version | Select-Object -First 1) -replace 'cmake version ', ''
    Write-ColorMessage "Found CMake $cmakeVersion" -Color Green

    # Check for Ninja if selected
    if ($Generator -eq "Ninja") {
        $ninja = Get-Command ninja -ErrorAction SilentlyContinue
        if (-not $ninja) {
            Write-ColorMessage "Warning: Ninja not found, falling back to Visual Studio" -Color Yellow
            $script:Generator = "Visual Studio 17 2022"
        } else {
            $ninjaVersion = ninja --version
            Write-ColorMessage "Found Ninja $ninjaVersion" -Color Green
        }
    }

    # Check for Visual Studio if needed
    if ($Generator -like "Visual Studio*") {
        $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
        if (Test-Path $vsWhere) {
            $vsPath = & $vsWhere -latest -property installationPath
            if ($vsPath) {
                Write-ColorMessage "Found Visual Studio at: $vsPath" -Color Green
            }
        }
    }

    Write-ColorMessage "All dependencies satisfied" -Color Green
}

# Clean build directory
function Remove-BuildDirectory {
    if ($Clean) {
        Write-ColorMessage "Cleaning build directory..." -Color Yellow
        if (Test-Path $BuildDir) {
            Remove-Item -Recurse -Force $BuildDir
            Write-ColorMessage "Build directory cleaned" -Color Green
        } else {
            Write-ColorMessage "Build directory does not exist, nothing to clean" -Color Yellow
        }
    }
}

# Configure the project
function Invoke-Configure {
    Write-ColorMessage "Configuring project..." -Color Cyan

    $cmakeArgs = @(
        "-G", $Generator,
        "-B", $BuildDir,
        "-DCMAKE_BUILD_TYPE=$BuildType",
        "-DBUILD_EXAMPLES=ON"
    )

    if ($Test) {
        $cmakeArgs += "-DBUILD_TESTS=ON"
    }

    if ($Verbose) {
        $cmakeArgs += "-DCMAKE_VERBOSE_MAKEFILE=ON"
    }

    & cmake $cmakeArgs
    if ($LASTEXITCODE -ne 0) {
        Write-ColorMessage "Configuration failed!" -Color Red
        exit $LASTEXITCODE
    }

    Write-ColorMessage "Configuration complete" -Color Green
}

# Build the project
function Invoke-Build {
    Write-ColorMessage "Building project..." -Color Cyan

    $buildArgs = @(
        "--build", $BuildDir,
        "--config", $BuildType
    )

    if ($Verbose) {
        $buildArgs += "--verbose"
    }

    # Add parallel build flag
    $buildArgs += "--parallel"

    & cmake $buildArgs
    if ($LASTEXITCODE -ne 0) {
        Write-ColorMessage "Build failed!" -Color Red
        exit $LASTEXITCODE
    }

    Write-ColorMessage "Build complete" -Color Green
}

# Display build results
function Show-BuildResults {
    Write-ColorMessage "Build Results:" -Color Cyan
    Write-Host ""

    if (Test-Path $BuildDir) {
        Write-ColorMessage "Generated libraries:" -Color Green
        Get-ChildItem -Path $BuildDir -Filter "kstring.*" -File | ForEach-Object {
            Write-Host "  $($_.FullName) ($($_.Length) bytes)"
        }

        Write-Host ""
        Write-ColorMessage "Generated binaries:" -Color Green
        $examplePath = Join-Path $BuildDir "_examples\$BuildType\kstring_demo.exe"
        if (Test-Path $examplePath) {
            $exampleFile = Get-Item $examplePath
            Write-Host "  $($exampleFile.FullName) ($($exampleFile.Length) bytes)"
        } else {
            # Try without BuildType subdirectory
            $examplePath = Join-Path $BuildDir "_examples\kstring_demo.exe"
            if (Test-Path $examplePath) {
                $exampleFile = Get-Item $examplePath
                Write-Host "  $($exampleFile.FullName) ($($exampleFile.Length) bytes)"
            }
        }
    }

    Write-Host ""
    Write-ColorMessage "Build directory: $BuildDir" -Color Green
}

# Run tests
function Invoke-Tests {
    if ($Test) {
        Write-ColorMessage "Running tests..." -Color Cyan
        Push-Location $BuildDir
        & ctest --output-on-failure -C $BuildType
        $testResult = $LASTEXITCODE
        Pop-Location

        if ($testResult -eq 0) {
            Write-ColorMessage "Tests complete" -Color Green
        } else {
            Write-ColorMessage "Tests failed!" -Color Red
            exit $testResult
        }
    }
}

# Run examples
function Invoke-Examples {
    if ($Examples) {
        Write-ColorMessage "Running example program..." -Color Cyan
        Write-Host ""

        # Try with BuildType subdirectory first
        $examplePath = Join-Path $BuildDir "_examples\$BuildType\kstring_demo.exe"
        if (-not (Test-Path $examplePath)) {
            # Try without BuildType subdirectory
            $examplePath = Join-Path $BuildDir "_examples\kstring_demo.exe"
        }

        if (Test-Path $examplePath) {
            & $examplePath
            Write-Host ""
            Write-ColorMessage "Example execution complete" -Color Green
        } else {
            Write-ColorMessage "Warning: Example program not found" -Color Yellow
            Write-Host "Searched paths:"
            Write-Host "  $(Join-Path $BuildDir "_examples\$BuildType\kstring_demo.exe")"
            Write-Host "  $(Join-Path $BuildDir "_examples\kstring_demo.exe")"
        }
    }
}

# Main execution
function Main {
    if ($Help) {
        Show-Usage
        exit 0
    }

    Write-Host ""
    Write-ColorMessage "=========================================" -Color Green
    Write-ColorMessage "  KString Library Build Script" -Color Green
    Write-ColorMessage "=========================================" -Color Green
    Write-Host ""

    Test-Dependencies
    Remove-BuildDirectory
    Invoke-Configure
    Invoke-Build
    Show-BuildResults
    Invoke-Tests
    Invoke-Examples

    Write-Host ""
    Write-ColorMessage "=========================================" -Color Green
    Write-ColorMessage "  Build Complete!" -Color Green
    Write-ColorMessage "=========================================" -Color Green
}

# Run main function
Main
