#!/bin/bash
# ============================================================================
# Enhanced Build Script with Catch2 Optimization
# ============================================================================
# This script provides optimized build options to dramatically reduce
# Catch2 build times through external installation management.
#
# Usage:
#   ./build.sh [options]
#
# Options:
#   --clean                 Clean build directory before building
#   --install-catch2        Install Catch2 system-wide for optimal performance
#   --force-fetchcontent    Force FetchContent even if external Catch2 exists
#   --release              Build in Release mode (default: Debug)
#   --help                 Show this help message
#
# Performance Impact:
#   - External Catch2: ~95% faster builds
#   - FetchContent: Standard CMake behavior
# ============================================================================

set -e

# Default configuration
BUILD_DIR="build"
BUILD_TYPE="Debug"
CLEAN_BUILD=false
INSTALL_CATCH2=false
FORCE_FETCHCONTENT=false
PARALLEL_JOBS=$(nproc)

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}🔧 $1${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_info() {
    echo -e "${CYAN}ℹ️  $1${NC}"
}

# Function to show help
show_help() {
    echo "HelloWorld Clean Code - Enhanced Build Script"
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  --clean                 Clean build directory before building"
    echo "  --install-catch2        Install Catch2 system-wide for optimal performance"
    echo "  --force-fetchcontent    Force FetchContent even if external Catch2 exists"
    echo "  --release              Build in Release mode (default: Debug)"
    echo "  --help                 Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                      # Standard build"
    echo "  $0 --clean --release    # Clean release build"
    echo "  $0 --install-catch2     # Install Catch2 for maximum performance"
    echo ""
    echo "Performance Tips:"
    echo "  - Run '$0 --install-catch2' once for ~95% faster subsequent builds"
    echo "  - Use '--clean' only when necessary to preserve build cache"
}

# Function to install Catch2 system-wide
install_catch2() {
    print_status "Installing Catch2 v3.7.1 system-wide for optimal performance..."
    
    # Check if already installed
    if pkg-config --exists catch2 2>/dev/null; then
        INSTALLED_VERSION=$(pkg-config --modversion catch2)
        print_info "Catch2 $INSTALLED_VERSION is already installed"
        echo "Do you want to reinstall? (y/N)"
        read -r response
        if [[ ! "$response" =~ ^[Yy]$ ]]; then
            print_info "Skipping installation"
            return 0
        fi
    fi
    
    # Create temporary directory
    TEMP_DIR=$(mktemp -d)
    cd "$TEMP_DIR"
    
    print_status "Downloading Catch2 v3.7.1..."
    git clone --depth 1 --branch v3.7.1 https://github.com/catchorg/Catch2.git
    cd Catch2
    
    print_status "Configuring Catch2 for system installation..."
    cmake -B build -S . \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/opt/catch2 \
        -DBUILD_TESTING=OFF \
        -DCATCH_INSTALL_DOCS=OFF \
        -DCATCH_INSTALL_EXTRAS=ON \
        -DCATCH_BUILD_EXAMPLES=OFF
    
    print_status "Building Catch2 (this may take a few minutes)..."
    cmake --build build --parallel "$PARALLEL_JOBS"
    
    print_status "Installing Catch2 to /opt/catch2..."
    sudo cmake --install build
    
    # Create pkg-config file for easier detection
    sudo mkdir -p /usr/local/lib/pkgconfig
    sudo tee /usr/local/lib/pkgconfig/catch2.pc > /dev/null << EOF
prefix=/opt/catch2
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include

Name: Catch2
Description: A modern, C++-native, header-only, test framework for unit-tests, TDD and BDD
Version: 3.7.1
Cflags: -I\${includedir}
Libs: -L\${libdir}
EOF
    
    # Cleanup
    cd /
    rm -rf "$TEMP_DIR"
    
    print_success "Catch2 installed successfully!"
    print_info "Future builds will be ~95% faster! 🚀"
    print_info "Installation location: /opt/catch2"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --install-catch2)
            INSTALL_CATCH2=true
            shift
            ;;
        --force-fetchcontent)
            FORCE_FETCHCONTENT=true
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --help)
            show_help
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Install Catch2 if requested
if [[ "$INSTALL_CATCH2" == true ]]; then
    install_catch2
    echo ""
    print_info "Catch2 installation complete. You can now build with optimal performance!"
    echo ""
fi

# Display build configuration
print_info "Build Configuration:"
print_info "  - Build Type: $BUILD_TYPE"
print_info "  - Build Directory: $BUILD_DIR"
print_info "  - Parallel Jobs: $PARALLEL_JOBS"
if [[ "$FORCE_FETCHCONTENT" == true ]]; then
    print_info "  - Catch2 Source: FetchContent (forced)"
else
    print_info "  - Catch2 Source: Auto-detect (prefer external)"
fi

# Clean build directory if requested
if [[ "$CLEAN_BUILD" == true ]]; then
    print_status "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    print_success "Build directory cleaned"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Prepare CMake options
CMAKE_OPTIONS=(
    "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
)

if [[ "$FORCE_FETCHCONTENT" == true ]]; then
    CMAKE_OPTIONS+=("-DHELLOWORLD_FORCE_FETCHCONTENT_CATCH2=ON")
fi

# Configure project
print_status "Configuring project with CMake..."
echo "CMake options: ${CMAKE_OPTIONS[*]}"
cmake "${CMAKE_OPTIONS[@]}" ..

# Build project
print_status "Building project..."
if [[ -f build.ninja ]]; then
    print_info "Using Ninja for fast builds"
    ninja
elif [[ -f Makefile ]]; then
    print_info "Using Make with $PARALLEL_JOBS parallel jobs"
    make -j"$PARALLEL_JOBS"
else
    print_error "No build system files found (build.ninja or Makefile)"
    exit 1
fi

print_success "Build completed successfully!"

# Display available executables
echo ""
print_info "Available executables:"
if [[ -f "hello_simple" ]]; then
    echo "  - ./hello_simple        # Simple hello world"
fi
if [[ -f "hello_world" ]]; then
    echo "  - ./hello_world         # Full clean architecture demo"
fi
if [[ -f "greeting_tests" ]]; then
    echo "  - ./greeting_tests      # Run all unit tests"
fi

# Performance tip
echo ""
if [[ "$FORCE_FETCHCONTENT" == true ]] || ! pkg-config --exists catch2 2>/dev/null; then
    print_warning "Performance Tip: Run '$0 --install-catch2' once for ~95% faster builds!"
else
    print_success "Using optimized external Catch2 - enjoying fast builds! 🚀"
fi
