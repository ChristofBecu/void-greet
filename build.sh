#!/bin/bash
set -e

BUILD_TYPE=${1:-Debug}
echo "🔧 Building in $BUILD_TYPE mode..."

# Project-specific C++23 flags
CXXFLAGS="-std=c++2b -Wall -Wextra"
if [[ "$BUILD_TYPE" == "Release" ]]; then
    CXXFLAGS="$CXXFLAGS -O2 -DNDEBUG"
else
    CXXFLAGS="$CXXFLAGS -g -O0"
fi

# Build command
g++ $CXXFLAGS "$@"
echo "✅ Build complete"