# Testing Documentation

## Overview

This project uses **Catch2 v3** as the unit testing framework, providing comprehensive test coverage for the C++23 clean architecture implementation.

## Framework Choice: Catch2 v3 ✅

**Why Catch2 for C++23:**

- ✅ **Modern C++ Native**: Designed for C++14+ with excellent C++23 support
- ✅ **Header-Only**: Easy integration via CMake FetchContent
- ✅ **BDD-Style Syntax**: Natural, readable test descriptions
- ✅ **Rich Assertions**: Built-in matchers and custom assertion support
- ✅ **CMake Integration**: First-class CMake support with auto-discovery
- ✅ **Fast Compilation**: Optimized for quick builds
- ✅ **Active Development**: Well-maintained with regular updates

## Test Structure

### Test Files Organization

```bash
tests/
├── unit/
│   ├── test_greeting_types.cpp    # PersonName & GreetingMessage tests
│   ├── test_expected.cpp          # Expected<T,E> wrapper tests
│   └── test_error_handling.cpp    # Error enumeration & category tests
```

### Test Categories

#### 1. **Greeting Types Tests** (`test_greeting_types.cpp`)

- **PersonName validation**: Empty strings, whitespace-only, invalid characters
- **GreetingMessage validation**: Content validation and error handling
- **Comparison operations**: Equality, inequality, ordering
- **Factory functions**: Convenience creation methods

#### 2. **Expected Wrapper Tests** (`test_expected.cpp`)

- **Success cases**: Value access, dereferencing, arrow operator
- **Error cases**: Error access, exception behavior
- **Move semantics**: Proper move construction and assignment
- **Complex types**: Integration with PersonName and GreetingMessage

#### 3. **Error Handling Tests** (`test_error_handling.cpp`)

- **Error enumeration**: All error codes with descriptive messages
- **Error categories**: std::error_code integration
- **Singleton behavior**: Error category instance management
- **Constexpr evaluation**: Compile-time string generation

## Running Tests

### Build and Run All Tests

```bash
cd build
cmake --build . --target greeting_tests
./greeting_tests
```

### Run with Different Reporters

```bash
# Compact output
./greeting_tests --reporter compact

# XML output (for CI/CD)
./greeting_tests --reporter xml

# JSON output
./greeting_tests --reporter json
```

### Run Specific Test Categories

```bash
# Only PersonName tests
./greeting_tests [PersonName]

# Only validation tests
./greeting_tests [validation]

# Only Expected tests
./greeting_tests [Expected]
```

### List All Available Tests

```bash
./greeting_tests --list-tests
```

## CMake Integration

The testing setup is configured in `cmake/Testing.cmake`:

- **FetchContent** automatically downloads Catch2 v3.7.1
- **Auto-discovery** via `catch_discover_tests()`
- **CTest integration** for CI/CD pipelines
- **Proper C++23 compilation** with warnings as errors

## Test Coverage Summary

**Current Test Results:**

- ✅ **87 assertions** across **14 test cases**
- ✅ **100% pass rate**
- ✅ **Comprehensive scenario coverage**

### Key Test Scenarios

1. **Type Safety**: Strong type validation and error prevention
2. **Error Handling**: Robust error enumeration and std::error_code integration
3. **Expected Wrapper**: Modern error handling with variant-based Expected type
4. **Factory Methods**: Safe object construction with validation
5. **Comparison Operations**: Proper value semantics and ordering
6. **Move Semantics**: Efficient resource management
7. **Constexpr Evaluation**: Compile-time error message generation

## Adding New Tests

### Test File Template

```cpp
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "greeting/types.hpp"

using namespace greeting;
using Catch::Matchers::Equals;
using Catch::Matchers::ContainsSubstring;

SCENARIO("Your feature description", "[tag1][tag2]") {
    GIVEN("initial condition") {
        // Setup
        
        WHEN("action performed") {
            // Action
            
            THEN("expected outcome") {
                // Assertions
                REQUIRE(condition);
                REQUIRE_THAT(value, Equals("expected"));
            }
        }
    }
}
```

### Best Practices

1. Use **descriptive scenario names** that explain business logic
2. Tag tests appropriately for **selective running**
3. Use **BDD structure** (Given-When-Then) for clarity
4. Convert `string_view` to `string` for Catch2 matchers
5. Test both **success and failure paths**
6. Include **edge cases** and **boundary conditions**

## Integration with Build System

The tests are fully integrated with the CMake build system:

- Automatically built with the main project
- CTest integration for CI/CD pipelines  
- Proper dependency management
- C++23 compilation flags applied consistently
