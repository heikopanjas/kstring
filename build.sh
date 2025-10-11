#!/usr/bin/env bash
# KString Library - Build Script for Linux/macOS
# This script provides a comprehensive build system with multiple options

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/_build"

# Default options
CLEAN=false
VERBOSE=false
RUN_TESTS=false
RUN_EXAMPLES=false
BUILD_TYPE="Release"
GENERATOR="Ninja"

# Print colored message
print_message() {
    local color=$1
    shift
    echo -e "${color}$@${NC}"
}

# Print usage information
usage() {
    cat << EOF
Usage: $0 [OPTIONS]

KString Library Build Script

OPTIONS:
    -c, --clean         Clean build directory before building
    -d, --debug         Build in Debug mode (default: Release)
    -v, --verbose       Enable verbose build output
    -t, --test          Run tests after building
    -e, --examples      Run examples after building
    -g, --generator     Specify CMake generator (default: Ninja)
                        Options: Ninja, Unix Makefiles
    -h, --help          Show this help message

EXAMPLES:
    $0                  # Standard release build
    $0 --clean          # Clean and build
    $0 --debug --verbose # Debug build with verbose output
    $0 -c -e            # Clean, build, and run examples

EOF
}

# Parse command line arguments
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -c|--clean)
                CLEAN=true
                shift
                ;;
            -d|--debug)
                BUILD_TYPE="Debug"
                shift
                ;;
            -v|--verbose)
                VERBOSE=true
                shift
                ;;
            -t|--test)
                RUN_TESTS=true
                shift
                ;;
            -e|--examples)
                RUN_EXAMPLES=true
                shift
                ;;
            -g|--generator)
                GENERATOR="$2"
                shift 2
                ;;
            -h|--help)
                usage
                exit 0
                ;;
            *)
                print_message "$RED" "Unknown option: $1"
                usage
                exit 1
                ;;
        esac
    done
}

# Check for required tools
check_dependencies() {
    print_message "$BLUE" "Checking dependencies..."

    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        print_message "$RED" "Error: CMake is not installed"
        print_message "$YELLOW" "Please install CMake 3.30 or higher"
        exit 1
    fi

    local CMAKE_VERSION=$(cmake --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
    print_message "$GREEN" "Found CMake $CMAKE_VERSION"

    # Check for Ninja if selected
    if [[ "$GENERATOR" == "Ninja" ]]; then
        if ! command -v ninja &> /dev/null; then
            print_message "$YELLOW" "Warning: Ninja not found, falling back to Unix Makefiles"
            GENERATOR="Unix Makefiles"
        else
            local NINJA_VERSION=$(ninja --version)
            print_message "$GREEN" "Found Ninja $NINJA_VERSION"
        fi
    fi

    # Check for C compiler
    if ! command -v cc &> /dev/null && ! command -v gcc &> /dev/null && ! command -v clang &> /dev/null; then
        print_message "$RED" "Error: No C compiler found"
        exit 1
    fi

    print_message "$GREEN" "All dependencies satisfied"
}

# Clean build directory
clean_build() {
    if [[ "$CLEAN" == true ]]; then
        print_message "$YELLOW" "Cleaning build directory..."
        if [[ -d "$BUILD_DIR" ]]; then
            rm -rf "$BUILD_DIR"
            print_message "$GREEN" "Build directory cleaned"
        else
            print_message "$YELLOW" "Build directory does not exist, nothing to clean"
        fi
    fi
}

# Configure the project
configure_project() {
    print_message "$BLUE" "Configuring project..."

    local CMAKE_ARGS=(
        -G "$GENERATOR"
        -B "$BUILD_DIR"
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
        -DBUILD_EXAMPLES=ON
    )

    if [[ "$RUN_TESTS" == true ]]; then
        CMAKE_ARGS+=(-DBUILD_TESTS=ON)
    fi

    if [[ "$VERBOSE" == true ]]; then
        CMAKE_ARGS+=(-DCMAKE_VERBOSE_MAKEFILE=ON)
    fi

    cmake "${CMAKE_ARGS[@]}"

    print_message "$GREEN" "Configuration complete"
}

# Build the project
build_project() {
    print_message "$BLUE" "Building project..."

    local BUILD_ARGS=(--build "$BUILD_DIR")

    if [[ "$VERBOSE" == true ]]; then
        BUILD_ARGS+=(--verbose)
    fi

    # Add parallel build flag
    if [[ "$GENERATOR" == "Ninja" ]]; then
        BUILD_ARGS+=(--parallel)
    else
        # For Unix Makefiles, detect CPU count
        if [[ "$(uname)" == "Darwin" ]]; then
            local NCPU=$(sysctl -n hw.ncpu)
        else
            local NCPU=$(nproc)
        fi
        BUILD_ARGS+=(--parallel "$NCPU")
    fi

    cmake "${BUILD_ARGS[@]}"

    print_message "$GREEN" "Build complete"
}

# Display build results
show_results() {
    print_message "$BLUE" "Build Results:"
    echo ""

    if [[ -d "$BUILD_DIR" ]]; then
        print_message "$GREEN" "Generated libraries:"
        ls -lh "$BUILD_DIR"/libkstring* 2>/dev/null | while read line; do
            echo "  $line"
        done

        echo ""
        print_message "$GREEN" "Generated binaries:"
        if [[ -f "$BUILD_DIR/_examples/kstring_demo" ]]; then
            ls -lh "$BUILD_DIR/_examples/kstring_demo"
        fi
    fi

    echo ""
    print_message "$GREEN" "Build directory: $BUILD_DIR"
}

# Run tests
run_tests() {
    if [[ "$RUN_TESTS" == true ]]; then
        print_message "$BLUE" "Running tests..."
        cd "$BUILD_DIR"
        ctest --output-on-failure
        cd "$SCRIPT_DIR"
        print_message "$GREEN" "Tests complete"
    fi
}

# Run examples
run_examples() {
    if [[ "$RUN_EXAMPLES" == true ]]; then
        print_message "$BLUE" "Running example program..."
        echo ""

        if [[ -f "$BUILD_DIR/_examples/kstring_demo" ]]; then
            "$BUILD_DIR/_examples/kstring_demo"
            echo ""
            print_message "$GREEN" "Example execution complete"
        else
            print_message "$YELLOW" "Warning: Example program not found"
        fi
    fi
}

# Main execution
main() {
    print_message "$GREEN" "========================================="
    print_message "$GREEN" "  KString Library Build Script"
    print_message "$GREEN" "========================================="
    echo ""

    parse_args "$@"
    check_dependencies
    clean_build
    configure_project
    build_project
    show_results
    run_tests
    run_examples

    echo ""
    print_message "$GREEN" "========================================="
    print_message "$GREEN" "  Build Complete!"
    print_message "$GREEN" "========================================="
}

# Run main function
main "$@"
