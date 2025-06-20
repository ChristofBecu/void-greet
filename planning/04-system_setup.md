# Setting up C++23 on Arch Linux (Keeping C++17)

## 🎯 Current Situation Assessment

You're on Arch Linux with C++17, which means you likely have GCC 7.x or 8.x. We need to install modern compilers that support C++23 features.

## 🔧 Compiler Options for C++23

### **Option 1: GCC 13+ (Recommended)**
- Best C++23 support currently available
- Most features implemented
- Great compatibility with existing code

### **Option 2: Clang 17+ (Alternative)**
- Good C++23 support
- Excellent error messages
- LLVM ecosystem benefits

## 📦 Installation Steps

### **Step 1: Update System**
```bash
sudo pacman -Syu
```

### **Step 2: Install GCC 13+ (Primary Option)**
```bash
# Install latest GCC
sudo pacman -S gcc

# Check version (should be 13.x or 14.x)
gcc --version

# Install additional development tools
sudo pacman -S base-devel cmake ninja gdb
```

### **Step 3: Install Clang 17+ (Secondary Option)**
```bash
# Install latest Clang
sudo pacman -S clang llvm

# Check version
clang++ --version

# Install additional LLVM tools
sudo pacman -S lldb lld
```

### **Step 4: Install Multiple GCC Versions (If Needed)**
If you need to keep specific versions:

```bash
# Check available GCC versions
yay -Ss gcc

# Install specific versions if available
yay -S gcc11 gcc12 gcc13

# Use update-alternatives to manage versions
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 60
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 80
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 60
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 80
```

## 🛠️ Development Environment Setup

### **CMake Configuration**
Create a `CMakeLists.txt` for C++23:

```cmake
cmake_minimum_required(VERSION 3.25)
project(HelloWorldCleanCode CXX)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "13.0")
        message(FATAL_ERROR "GCC 13+ required for C++23 features")
    endif()
    target_compile_options(your_target PRIVATE -std=c++23 -fcoroutines)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "17.0")
        message(FATAL_ERROR "Clang 17+ required for C++23 features")
    endif()
    target_compile_options(your_target PRIVATE -std=c++23 -stdlib=libc++)
endif()
```

### **Compiler Version Management Script**
Create `~/bin/cpp-version.sh`:

```bash
#!/bin/bash
# C++ Compiler Version Manager

case "$1" in
    "17")
        export CC=gcc-8
        export CXX=g++-8
        echo "Switched to C++17 (GCC 8)"
        ;;
    "23")
        export CC=gcc-13
        export CXX=g++-13
        echo "Switched to C++23 (GCC 13)"
        ;;
    "clang23")
        export CC=clang-17
        export CXX=clang++-17
        echo "Switched to C++23 (Clang 17)"
        ;;
    *)
        echo "Usage: $0 {17|23|clang23}"
        echo "Current versions:"
        gcc --version | head -1
        g++ --version | head -1
        clang++ --version | head -1
        ;;
esac
```

Make it executable:
```bash
chmod +x ~/bin/cpp-version.sh
```

## 🔍 C++23 Feature Verification

### **Feature Test Script**
Create `test_cpp23_features.cpp`:

```cpp
#include <version>
#include <iostream>

int main() {
    std::cout << "C++23 Feature Support Check:\n";
    
    #ifdef __cpp_modules
    std::cout << "✅ Modules: " << __cpp_modules << "\n";
    #else
    std::cout << "❌ Modules: Not supported\n";
    #endif
    
    #ifdef __cpp_if_consteval
    std::cout << "✅ if consteval: " << __cpp_if_consteval << "\n";
    #else
    std::cout << "❌ if consteval: Not supported\n";
    #endif
    
    #ifdef __cpp_multidimensional_subscript
    std::cout << "✅ Multidimensional subscript: " << __cpp_multidimensional_subscript << "\n";
    #else
    std::cout << "❌ Multidimensional subscript: Not supported\n";
    #endif
    
    #ifdef __cpp_auto_cast
    std::cout << "✅ Auto cast: " << __cpp_auto_cast << "\n";
    #else
    std::cout << "❌ Auto cast: Not supported\n";
    #endif
    
    #ifdef __cpp_deducing_this
    std::cout << "✅ Deducing this: " << __cpp_deducing_this << "\n";
    #else
    std::cout << "❌ Deducing this: Not supported\n";
    #endif
    
    return 0;
}
```

Compile and run:
```bash
g++-13 -std=c++23 -o test_features test_cpp23_features.cpp
./test_features
```

## 📚 Additional Development Tools

### **Install Package Managers for C++**
```bash
# Conan (Python-based)
sudo pacman -S python-pip
pip install conan

# vcpkg (Microsoft's package manager)
git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh
```

### **Enhanced Development Environment**
```bash
# Modern build tools
sudo pacman -S meson ninja ccache

# Static analysis tools
sudo pacman -S cppcheck clang-tools-extra

# Documentation tools
sudo pacman -S doxygen graphviz

# Debugging and profiling
sudo pacman -S valgrind perf gdb lldb

# Code formatting
sudo pacman -S clang-format
```

## 🚀 Quick Project Setup

### **Create Project Structure**
```bash
mkdir -p ~/dev/projects/helloworldcleancode
cd ~/dev/projects/helloworldcleancode

# Create directory structure
mkdir -p {src,include,tests,docs,build}
mkdir -p src/{domain,application,infrastructure}
mkdir -p include/{greeting,utils,config}
```

### **Basic Build Script**
Create `build.sh`:

```bash
#!/bin/bash
set -e

# Configuration
BUILD_TYPE=${1:-Debug}
COMPILER=${2:-gcc}

echo "Building with $COMPILER in $BUILD_TYPE mode..."

# Choose compiler
if [ "$COMPILER" = "clang" ]; then
    export CC=clang-17
    export CXX=clang++-17
else
    export CC=gcc-13
    export CXX=g++-13
fi

# Build
cmake -B build -S . \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_CXX_STANDARD=23 \
    -G Ninja

ninja -C build

echo "Build complete!"
```

## ⚠️ Potential Issues & Solutions

### **Issue 1: Module Support**
C++23 modules are still experimental. If you encounter issues:

```bash
# Use module-specific flags
g++-13 -std=c++23 -fmodules-ts -x c++-system-header iostream
```

### **Issue 2: Library Compatibility**
Some libraries might not be C++23 ready:

```bash
# Check library compatibility
ldd your_binary
nm -D your_binary | grep GLIBCXX
```

### **Issue 3: IDE Integration**
For VS Code, update `c_cpp_properties.json`:

```json
{
    "configurations": [
        {
            "name": "Linux C++23",
            "includePath": [
                "${workspaceFolder}/**"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/g++-13",
            "cStandard": "c17",
            "cppStandard": "c++23",
            "intelliSenseMode": "gcc-x64"
        }
    ]
}
```

## 🎯 Verification Commands

Run these to verify your setup:

```bash
# Check compiler versions
gcc-13 --version
g++-13 --version
clang++-17 --version

# Test C++23 compilation
echo 'import <iostream>; int main(){ std::cout << "C++23 works!\n"; }' > test.cpp
g++-13 -std=c++23 -fmodules-ts test.cpp -o test
./test

# Check available C++ standards
g++-13 -dM -E -x c++ /dev/null | grep __cplusplus
```

## 📝 Next Steps

1. **Verify Installation**: Run all verification commands
2. **Test C++23 Features**: Compile and run the feature test
3. **Set Up IDE**: Configure your development environment
4. **Start Coding**: Begin implementing the Hello World architecture!
