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
# C++23 Feature Detection and Validation
# ============================================================================

include(cmake/FeatureDetection.cmake)

# Run C++23 feature validation
validate_cpp23_build_system()

# Check if build configuration system can be enabled
if(NOT HELLOWORLD_CPP23_BUILD_CONFIG_READY)
    message(WARNING "C++23 Build Configuration System not available - using basic configuration")
    set(HELLOWORLD_USE_BASIC_CONFIG ON CACHE BOOL "Use basic configuration fallback")
else()
    message(STATUS "C++23 Build Configuration System ready for implementation")
    set(HELLOWORLD_USE_BASIC_CONFIG OFF CACHE BOOL "Use basic configuration fallback")
    
    # Initialize the advanced C++23 build configuration system
    include(cmake/BuildConfigurations.cmake)
    initialize_build_configuration_system()
endif()

# ============================================================================
# Compiler Configuration
# ============================================================================

if(HELLOWORLD_USE_BASIC_CONFIG)
    # Basic compiler flags fallback
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
else()
    # Advanced configuration is handled via configuration targets
    # Base flags are applied through HelloWorld_ConfigBase target
    message(STATUS "Using advanced C++23 build configuration system")
endif()

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
    src/domain/console_strategy.cpp
    src/domain/formal_strategy.cpp
    src/config/debug_validation.cpp
    src/config/release_validation.cpp
)
add_library(HelloWorld::Domain ALIAS HelloWorld_Domain)

target_link_libraries(HelloWorld_Domain 
    PUBLIC HelloWorld_Core
)

target_compile_features(HelloWorld_Domain 
    PUBLIC cxx_std_23
)

# Apply build configuration if advanced system is available
if(NOT HELLOWORLD_USE_BASIC_CONFIG)
    apply_build_configuration(HelloWorld_Domain)
else()
    target_compile_options(HelloWorld_Domain
        PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-unused-parameter>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
endif()

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

# Apply build configuration if advanced system is available
if(NOT HELLOWORLD_USE_BASIC_CONFIG)
    apply_build_configuration(HelloWorld_Application)
endif()

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

# Apply build configuration if advanced system is available
if(NOT HELLOWORLD_USE_BASIC_CONFIG)
    apply_build_configuration(HelloWorld_Infrastructure)
endif()

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

# Apply build configuration if advanced system is available
if(NOT HELLOWORLD_USE_BASIC_CONFIG)
    apply_build_configuration(hello_simple)
endif()

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

# Apply build configuration if advanced system is available
if(NOT HELLOWORLD_USE_BASIC_CONFIG)
    apply_build_configuration(hello_world)
endif()

# Configuration System Demo (disabled for now due to linking issues)
# add_executable(demo_configuration demo_configuration.cpp)
# target_link_libraries(demo_configuration PRIVATE HelloWorld_Core)
# target_compile_features(demo_configuration PRIVATE cxx_std_23)

# Console Strategy Demo
add_executable(demo_console_strategy tools/demo_console_strategy.cpp)
target_link_libraries(demo_console_strategy PRIVATE HelloWorld_Domain)
target_compile_features(demo_console_strategy PRIVATE cxx_std_23)

# Formal Strategy Demo
add_executable(demo_formal_strategy tools/demo_formal_strategy.cpp)
target_link_libraries(demo_formal_strategy PRIVATE HelloWorld_Domain)
target_compile_features(demo_formal_strategy PRIVATE cxx_std_23)

# Apply build configuration if advanced system is available
if(NOT HELLOWORLD_USE_BASIC_CONFIG)
    # apply_build_configuration(demo_configuration)
    apply_build_configuration(demo_console_strategy)
    apply_build_configuration(demo_formal_strategy)
endif()

# ============================================================================
# Testing Configuration
# ============================================================================

# Include testing setup
include(cmake/Testing.cmake)

# ============================================================================
# Build Configuration Validation Tools
# ============================================================================

# Add configuration validator executable
add_executable(config_validator tools/config_validator.cpp)
target_compile_features(config_validator PRIVATE cxx_std_23)
target_link_libraries(config_validator PRIVATE HelloWorld::Core)

# Apply build configuration if advanced system is available
if(NOT HELLOWORLD_USE_BASIC_CONFIG)
    apply_build_configuration(config_validator)
else()
    # Set compiler flags for validation tool (fallback)
    target_compile_options(config_validator PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
endif()

# Add custom target to run validation
add_custom_target(validate_config
    COMMAND config_validator
    DEPENDS config_validator
    COMMENT "Running C++23 build configuration validation"
    VERBATIM
)

# Add validation to the build process if requested
if(HELLOWORLD_RUN_VALIDATION_ON_BUILD)
    add_dependencies(HelloWorld_Domain validate_config)
endif()

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