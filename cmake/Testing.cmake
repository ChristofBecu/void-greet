# ============================================================================
# Optimized Catch2 Testing Configuration
# ============================================================================
# Uses external Catch2 installation when available for dramatically faster builds.
# Falls back to FetchContent with optimizations when external not found.
# ============================================================================

# Use optimized external Catch2 configuration
include(cmake/ExternalCatch2.cmake)

# Note: CTest and Catch are now included in ExternalCatch2.cmake

# Create test executable
add_executable(greeting_tests
    tests/unit/greeting/test_greeting_types.cpp
    tests/unit/greeting/test_expected.cpp
    tests/unit/greeting/test_error_handling.cpp
    tests/unit/greeting/test_error_enumeration.cpp
    tests/unit/greeting/test_validation.cpp
    tests/unit/greeting/test_expected_comprehensive.cpp
    tests/unit/greeting/test_domain_service.cpp
    tests/unit/greeting/test_domain_validation.cpp
    tests/config/test_build_configuration.cpp
)

# Link with Catch2 and set up proper includes
target_link_libraries(greeting_tests 
    PRIVATE 
    Catch2::Catch2WithMain
    HelloWorld_Domain
)

target_include_directories(greeting_tests 
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

# Set C++23 standard for tests
target_compile_features(greeting_tests PRIVATE cxx_std_23)

# Apply build configuration if advanced system is available
if(NOT HELLOWORLD_USE_BASIC_CONFIG AND COMMAND apply_build_configuration)
    apply_build_configuration(greeting_tests)
endif()

# Enable compiler warnings for tests
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(greeting_tests PRIVATE
        -Wall -Wextra -Wpedantic -Werror
        -Wno-unused-parameter  # Catch2 may have unused params
    )
endif()

# Auto-discover Catch2 tests
catch_discover_tests(greeting_tests)
