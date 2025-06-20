# ============================================================================
# C++23 Build Configuration System
# 
# This CMake module sets up sophisticated debug/release build configurations
# leveraging C++23 features for compile-time configuration decisions.
# ============================================================================

# ============================================================================
# Configuration Interface Libraries
# ============================================================================

function(setup_configuration_targets)
    message(STATUS "Setting up C++23 build configuration targets...")
    
    # Base configuration interface
    add_library(HelloWorld_ConfigBase INTERFACE)
    target_compile_features(HelloWorld_ConfigBase INTERFACE cxx_std_23)
    
    # Common C++23 compile definitions
    target_compile_definitions(HelloWorld_ConfigBase INTERFACE
        $<$<BOOL:${HELLOWORLD_HAS_CONSTEVAL}>:HELLOWORLD_HAS_CONSTEVAL=1>
        $<$<BOOL:${HELLOWORLD_HAS_IF_CONSTEVAL}>:HELLOWORLD_HAS_IF_CONSTEVAL=1>
        $<$<BOOL:${HELLOWORLD_HAS_EXPECTED}>:HELLOWORLD_HAS_EXPECTED=1>
        $<$<BOOL:${HELLOWORLD_HAS_STACKTRACE}>:HELLOWORLD_HAS_STACKTRACE=1>
        $<$<BOOL:${HELLOWORLD_HAS_CONCEPTS}>:HELLOWORLD_HAS_CONCEPTS=1>
    )
    
    # Apply consteval optimization flags if available
    if(HELLOWORLD_CONSTEVAL_FLAGS)
        target_compile_options(HelloWorld_ConfigBase INTERFACE ${HELLOWORLD_CONSTEVAL_FLAGS})
    endif()
    
    # ========================================================================
    # Debug Configuration Target
    # ========================================================================
    
    add_library(HelloWorld_DebugConfig INTERFACE)
    target_link_libraries(HelloWorld_DebugConfig INTERFACE HelloWorld_ConfigBase)
    
    target_compile_definitions(HelloWorld_DebugConfig INTERFACE
        # Debug mode indicators
        DEBUG=1
        HELLOWORLD_DEBUG_BUILD=1
        HELLOWORLD_ENABLE_ASSERTIONS=1
        HELLOWORLD_ENABLE_LOGGING=1
        HELLOWORLD_ENABLE_VALIDATION=1
        
        # Error handling configuration
        HELLOWORLD_RICH_ERROR_CONTEXT=1
        HELLOWORLD_DETAILED_DIAGNOSTICS=1
        
        # Performance monitoring
        HELLOWORLD_ENABLE_PERFORMANCE_MONITORING=1
        HELLOWORLD_ENABLE_MEMORY_DEBUGGING=1
        
        # C++23 feature usage in debug
        $<$<BOOL:${HELLOWORLD_HAS_STACKTRACE}>:HELLOWORLD_USE_STACKTRACE=1>
    )
    
    target_compile_options(HelloWorld_DebugConfig INTERFACE
        $<$<CXX_COMPILER_ID:GNU>:-g3 -O0 -fno-omit-frame-pointer>
        $<$<CXX_COMPILER_ID:Clang>:-g3 -O0 -fno-omit-frame-pointer>
        $<$<CXX_COMPILER_ID:MSVC>:/Od /Zi>
    )
    
    # Debug-specific sanitizers (optional)
    option(HELLOWORLD_ENABLE_SANITIZERS "Enable sanitizers in debug builds" OFF)
    if(HELLOWORLD_ENABLE_SANITIZERS)
        target_compile_options(HelloWorld_DebugConfig INTERFACE
            $<$<CXX_COMPILER_ID:GNU>:-fsanitize=address -fsanitize=undefined>
            $<$<CXX_COMPILER_ID:Clang>:-fsanitize=address -fsanitize=undefined>
        )
        target_link_options(HelloWorld_DebugConfig INTERFACE
            $<$<CXX_COMPILER_ID:GNU>:-fsanitize=address -fsanitize=undefined>
            $<$<CXX_COMPILER_ID:Clang>:-fsanitize=address -fsanitize=undefined>
        )
    endif()
    
    # ========================================================================
    # Release Configuration Target
    # ========================================================================
    
    add_library(HelloWorld_ReleaseConfig INTERFACE)
    target_link_libraries(HelloWorld_ReleaseConfig INTERFACE HelloWorld_ConfigBase)
    
    target_compile_definitions(HelloWorld_ReleaseConfig INTERFACE
        # Release mode indicators
        NDEBUG=1
        HELLOWORLD_RELEASE_BUILD=1
        HELLOWORLD_DISABLE_ASSERTIONS=1
        HELLOWORLD_DISABLE_LOGGING=1
        HELLOWORLD_MINIMAL_VALIDATION=1
        
        # Error handling configuration
        HELLOWORLD_LIGHTWEIGHT_ERRORS=1
        HELLOWORLD_MINIMAL_DIAGNOSTICS=1
        
        # Performance optimization
        HELLOWORLD_MAXIMIZE_CONSTEVAL=1
        HELLOWORLD_ZERO_OVERHEAD_ABSTRACTIONS=1
        HELLOWORLD_DISABLE_PERFORMANCE_MONITORING=1
        
        # C++23 optimization hints
        $<$<BOOL:${HELLOWORLD_HAS_CONSTEVAL}>:HELLOWORLD_PREFER_CONSTEVAL=1>
    )
    
    target_compile_options(HelloWorld_ReleaseConfig INTERFACE
        $<$<CXX_COMPILER_ID:GNU>:-O3 -DNDEBUG -march=native -flto>
        $<$<CXX_COMPILER_ID:Clang>:-O3 -DNDEBUG -march=native -flto>
        $<$<CXX_COMPILER_ID:MSVC>:/O2 /DNDEBUG /GL>
    )
    
    target_link_options(HelloWorld_ReleaseConfig INTERFACE
        $<$<CXX_COMPILER_ID:GNU>:-flto -s>
        $<$<CXX_COMPILER_ID:Clang>:-flto -s>
        $<$<CXX_COMPILER_ID:MSVC>:/LTCG>
    )
    
    # ========================================================================
    # Profile Configuration Target
    # ========================================================================
    
    add_library(HelloWorld_ProfileConfig INTERFACE)
    target_link_libraries(HelloWorld_ProfileConfig INTERFACE HelloWorld_ConfigBase)
    
    target_compile_definitions(HelloWorld_ProfileConfig INTERFACE
        # Profile mode combines debug symbols with release performance
        RELWITHDEBINFO=1
        HELLOWORLD_PROFILE_BUILD=1
        HELLOWORLD_ENABLE_PERFORMANCE_MONITORING=1
        HELLOWORLD_MINIMAL_VALIDATION=1
        HELLOWORLD_LIGHTWEIGHT_ERRORS=1
    )
    
    target_compile_options(HelloWorld_ProfileConfig INTERFACE
        $<$<CXX_COMPILER_ID:GNU>:-O2 -g -fno-omit-frame-pointer>
        $<$<CXX_COMPILER_ID:Clang>:-O2 -g -fno-omit-frame-pointer>
        $<$<CXX_COMPILER_ID:MSVC>:/O2 /Zi>
    )
    
    message(STATUS "✅ Configuration targets created successfully")
    
endfunction()

# ============================================================================
# Configuration Selection Logic
# ============================================================================

function(apply_build_configuration TARGET_NAME)
    if(NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR "Target ${TARGET_NAME} does not exist")
    endif()
    
    message(STATUS "Applying build configuration to target: ${TARGET_NAME}")
    
    # Check if target is an INTERFACE library
    get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
    
    # Apply configuration based on CMAKE_BUILD_TYPE
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "  → Debug configuration applied")
        if(TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
            target_link_libraries(${TARGET_NAME} INTERFACE HelloWorld_DebugConfig)
        else()
            target_link_libraries(${TARGET_NAME} PRIVATE HelloWorld_DebugConfig)
        endif()
        
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "  → Release configuration applied")
        if(TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
            target_link_libraries(${TARGET_NAME} INTERFACE HelloWorld_ReleaseConfig)
        else()
            target_link_libraries(${TARGET_NAME} PRIVATE HelloWorld_ReleaseConfig)
        endif()
        
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        message(STATUS "  → Profile configuration applied")
        if(TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
            target_link_libraries(${TARGET_NAME} INTERFACE HelloWorld_ProfileConfig)
        else()
            target_link_libraries(${TARGET_NAME} PRIVATE HelloWorld_ProfileConfig)
        endif()
        
    else()
        message(STATUS "  → Default (Debug) configuration applied")
        if(TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
            target_link_libraries(${TARGET_NAME} INTERFACE HelloWorld_DebugConfig)
        else()
            target_link_libraries(${TARGET_NAME} PRIVATE HelloWorld_DebugConfig)
        endif()
    endif()
    
endfunction()

# ============================================================================
# Compiler-Specific Optimizations
# ============================================================================

function(setup_compiler_optimizations)
    message(STATUS "Setting up compiler-specific optimizations...")
    
    # GCC-specific optimizations
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(STATUS "  → GCC optimizations enabled")
        
        # Check GCC version for C++23 support
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "12.0")
            set(HELLOWORLD_GCC_CPP23_READY ON CACHE BOOL "GCC C++23 ready")
            target_compile_options(HelloWorld_ConfigBase INTERFACE
                -fconstexpr-ops-limit=100000000
                -fconstexpr-depth=2000
            )
        else()
            message(WARNING "GCC version ${CMAKE_CXX_COMPILER_VERSION} may have limited C++23 support")
        endif()
        
    # Clang-specific optimizations  
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(STATUS "  → Clang optimizations enabled")
        
        # Check Clang version for C++23 support
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "15.0")
            set(HELLOWORLD_CLANG_CPP23_READY ON CACHE BOOL "Clang C++23 ready")
            target_compile_options(HelloWorld_ConfigBase INTERFACE
                -fconstexpr-steps=100000000
                -fconstexpr-depth=2000
            )
        else()
            message(WARNING "Clang version ${CMAKE_CXX_COMPILER_VERSION} may have limited C++23 support")
        endif()
        
    # MSVC-specific optimizations
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(STATUS "  → MSVC optimizations enabled")
        
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.30")
            set(HELLOWORLD_MSVC_CPP23_READY ON CACHE BOOL "MSVC C++23 ready")
            target_compile_options(HelloWorld_ConfigBase INTERFACE
                /constexpr:depth2000
                /constexpr:backtrace10
            )
        else()
            message(WARNING "MSVC version ${CMAKE_CXX_COMPILER_VERSION} may have limited C++23 support")
        endif()
    endif()
    
endfunction()

# ============================================================================
# Configuration Validation
# ============================================================================

function(validate_configuration_setup)
    message(STATUS "Validating build configuration setup...")
    
    # Check if configuration targets exist
    set(REQUIRED_TARGETS 
        HelloWorld_ConfigBase
        HelloWorld_DebugConfig 
        HelloWorld_ReleaseConfig
        HelloWorld_ProfileConfig
    )
    
    set(VALIDATION_PASSED ON)
    foreach(target ${REQUIRED_TARGETS})
        if(NOT TARGET ${target})
            message(WARNING "Configuration target ${target} not found")
            set(VALIDATION_PASSED OFF)
        endif()
    endforeach()
    
    if(VALIDATION_PASSED)
        message(STATUS "✅ Build configuration validation passed")
        set(HELLOWORLD_BUILD_CONFIG_VALID ON CACHE BOOL "Build configuration valid")
    else()
        message(WARNING "❌ Build configuration validation failed")
        set(HELLOWORLD_BUILD_CONFIG_VALID OFF CACHE BOOL "Build configuration valid")
    endif()
    
endfunction()

# ============================================================================
# Main Setup Function
# ============================================================================

function(initialize_build_configuration_system)
    message(STATUS "========================================")
    message(STATUS "Initializing C++23 Build Configuration System")
    message(STATUS "========================================")
    
    # Only proceed if C++23 features are available
    if(NOT HELLOWORLD_CPP23_BUILD_CONFIG_READY)
        message(WARNING "C++23 build configuration system not ready - skipping advanced configuration")
        return()
    endif()
    
    setup_configuration_targets()
    setup_compiler_optimizations()
    validate_configuration_setup()
    
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "C++23 Build Configuration System initialized successfully")
    message(STATUS "========================================")
    
endfunction()
