# Download and set up Catch2 v3 for unit testing
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.7.1  # Use latest stable version
)

FetchContent_MakeAvailable(Catch2)

# Include Catch2 CMake modules for test discovery
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

# Create test executable
add_executable(greeting_tests
    tests/unit/test_greeting_types.cpp
    tests/unit/test_expected.cpp
    tests/unit/test_error_handling.cpp
)

# Link with Catch2 and set up proper includes
target_link_libraries(greeting_tests 
    PRIVATE 
    Catch2::Catch2WithMain
    greeting_types
)

target_include_directories(greeting_tests 
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

# Set C++23 standard for tests
target_compile_features(greeting_tests PRIVATE cxx_std_23)

# Enable compiler warnings for tests
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(greeting_tests PRIVATE
        -Wall -Wextra -Wpedantic -Werror
        -Wno-unused-parameter  # Catch2 may have unused params
    )
endif()

# Auto-discover Catch2 tests
catch_discover_tests(greeting_tests)
