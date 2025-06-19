cmake_minimum_required(VERSION 3.20)
project(HelloWorldCleanCode CXX)

# C++23 settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags for our detected features
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# Include directories
include_directories(include)

# Create header-only library for greeting types
add_library(greeting_types INTERFACE)
target_include_directories(greeting_types 
    INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(greeting_types INTERFACE cxx_std_23)

# Main executable (simple version for now)
add_executable(hello_simple main.cpp)
target_link_libraries(hello_simple PRIVATE greeting_types)

# Include testing setup
include(cmake/Testing.cmake)

# We'll add more sophisticated targets as we build the architecture