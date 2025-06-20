cmake_minimum_required(VERSION 3.20)
project(HelloWorldCleanCode CXX)

# ============================================================================
# Project Configuration
# ============================================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# ============================================================================
# Compiler Configuration
# ============================================================================

# Base compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")

# Debug configuration
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")

# Release configuration  
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# ============================================================================
# Clean Architecture Layer Targets
# ============================================================================

# Core Types Interface Library (Header-only)
add_library(HelloWorld_Core INTERFACE)
add_library(HelloWorld::Core ALIAS HelloWorld_Core)

target_include_directories(HelloWorld_Core
    INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(HelloWorld_Core 
    INTERFACE cxx_std_23
)

# ============================================================================
# Domain Layer (Business Logic)
# ============================================================================

add_library(HelloWorld_Domain STATIC
    src/domain/greeting_types.cpp
    src/domain/validation.cpp
)
add_library(HelloWorld::Domain ALIAS HelloWorld_Domain)

target_link_libraries(HelloWorld_Domain 
    PUBLIC HelloWorld_Core
)

target_compile_features(HelloWorld_Domain 
    PUBLIC cxx_std_23
)

target_compile_options(HelloWorld_Domain
    PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-unused-parameter>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# ============================================================================
# Application Layer (Use Cases)
# ============================================================================

add_library(HelloWorld_Application INTERFACE)
add_library(HelloWorld::Application ALIAS HelloWorld_Application)

target_link_libraries(HelloWorld_Application 
    INTERFACE HelloWorld_Domain
)

target_compile_features(HelloWorld_Application 
    INTERFACE cxx_std_23
)

# ============================================================================
# Infrastructure Layer (External Concerns)
# ============================================================================

add_library(HelloWorld_Infrastructure INTERFACE)
add_library(HelloWorld::Infrastructure ALIAS HelloWorld_Infrastructure)

target_link_libraries(HelloWorld_Infrastructure 
    INTERFACE HelloWorld_Domain
)

target_compile_features(HelloWorld_Infrastructure 
    INTERFACE cxx_std_23
)

# ============================================================================
# Executable Targets
# ============================================================================

# Simple Hello World executable
add_executable(hello_simple main.cpp)

target_link_libraries(hello_simple 
    PRIVATE HelloWorld_Domain
)

target_compile_features(hello_simple 
    PRIVATE cxx_std_23
)

# Advanced Hello World executable (for future use)
add_executable(hello_world)

target_sources(hello_world
    PRIVATE main.cpp
)

target_link_libraries(hello_world 
    PRIVATE 
    HelloWorld_Application
    HelloWorld_Infrastructure
)

target_compile_features(hello_world 
    PRIVATE cxx_std_23
)

# ============================================================================
# Testing Configuration
# ============================================================================

# Include testing setup
include(cmake/Testing.cmake)

# ============================================================================
# Target Properties and Organization
# ============================================================================

# Set target properties for better organization
set_target_properties(HelloWorld_Core PROPERTIES
    INTERFACE_COMPILE_FEATURES cxx_std_23
    EXPORT_NAME Core
)

set_target_properties(HelloWorld_Domain PROPERTIES
    COMPILE_FEATURES cxx_std_23
    EXPORT_NAME Domain
    OUTPUT_NAME greeting_domain
)

set_target_properties(HelloWorld_Application PROPERTIES
    INTERFACE_COMPILE_FEATURES cxx_std_23
    EXPORT_NAME Application
)

set_target_properties(HelloWorld_Infrastructure PROPERTIES
    INTERFACE_COMPILE_FEATURES cxx_std_23
    EXPORT_NAME Infrastructure
)

# ============================================================================
# Installation Configuration (Future Use)
# ============================================================================

# Install configuration will be added when packaging is needed