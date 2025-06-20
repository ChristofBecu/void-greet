# ============================================================================
# C++23 Feature Detection for Build Configuration System
# 
# This CMake module detects C++23 compiler capabilities and sets appropriate
# configuration flags for the build system.
# ============================================================================

# ============================================================================
# C++23 Feature Detection Module
# ============================================================================

function(detect_cpp23_features)
    message(STATUS "Detecting C++23 feature support...")
    
    # Create feature detection executable
    set(FEATURE_DETECTION_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/cpp23_feature_detection.cpp")
    set(FEATURE_DETECTION_EXEC "${CMAKE_CURRENT_BINARY_DIR}/cpp23_feature_detection")
    
    # Generate feature detection source
    file(WRITE ${FEATURE_DETECTION_SOURCE} "
#include <version>
#include <iostream>

int main() {
    // Test consteval support
    #ifdef __cpp_consteval
    std::cout << \"CONSTEVAL_SUPPORTED=1\\n\";
    std::cout << \"CONSTEVAL_VERSION=\" << __cpp_consteval << \"\\n\";
    #else
    std::cout << \"CONSTEVAL_SUPPORTED=0\\n\";
    #endif
    
    // Test if consteval support
    #ifdef __cpp_if_consteval
    std::cout << \"IF_CONSTEVAL_SUPPORTED=1\\n\";
    std::cout << \"IF_CONSTEVAL_VERSION=\" << __cpp_if_consteval << \"\\n\";
    #else
    std::cout << \"IF_CONSTEVAL_SUPPORTED=0\\n\";
    #endif
    
    // Test std::expected support
    #ifdef __cpp_lib_expected
    std::cout << \"EXPECTED_SUPPORTED=1\\n\";
    std::cout << \"EXPECTED_VERSION=\" << __cpp_lib_expected << \"\\n\";
    #else
    std::cout << \"EXPECTED_SUPPORTED=0\\n\";
    #endif
    
    // Test std::stacktrace support
    #ifdef __cpp_lib_stacktrace
    std::cout << \"STACKTRACE_SUPPORTED=1\\n\";
    std::cout << \"STACKTRACE_VERSION=\" << __cpp_lib_stacktrace << \"\\n\";
    #else
    std::cout << \"STACKTRACE_SUPPORTED=0\\n\";
    #endif
    
    // Test modules support
    #ifdef __cpp_modules
    std::cout << \"MODULES_SUPPORTED=1\\n\";
    std::cout << \"MODULES_VERSION=\" << __cpp_modules << \"\\n\";
    #else
    std::cout << \"MODULES_SUPPORTED=0\\n\";
    #endif
    
    // Test concepts support
    #ifdef __cpp_concepts
    std::cout << \"CONCEPTS_SUPPORTED=1\\n\";
    std::cout << \"CONCEPTS_VERSION=\" << __cpp_concepts << \"\\n\";
    #else
    std::cout << \"CONCEPTS_SUPPORTED=0\\n\";
    #endif
    
    return 0;
}
")
    
    # Try to compile and run feature detection
    try_compile(FEATURE_DETECTION_COMPILED
        ${CMAKE_CURRENT_BINARY_DIR}
        ${FEATURE_DETECTION_SOURCE}
        CMAKE_FLAGS "-DCMAKE_CXX_STANDARD=23"
        OUTPUT_VARIABLE COMPILE_OUTPUT
    )
    
    if(FEATURE_DETECTION_COMPILED)
        # Run the feature detection executable
        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -std=c++23 -o ${FEATURE_DETECTION_EXEC} ${FEATURE_DETECTION_SOURCE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            RESULT_VARIABLE COMPILE_RESULT
            OUTPUT_VARIABLE COMPILE_MSG
            ERROR_VARIABLE COMPILE_ERROR
        )
        
        if(COMPILE_RESULT EQUAL 0)
            execute_process(
                COMMAND ${FEATURE_DETECTION_EXEC}
                OUTPUT_VARIABLE FEATURE_OUTPUT
                ERROR_VARIABLE FEATURE_ERROR
                RESULT_VARIABLE FEATURE_RESULT
            )
            
            if(FEATURE_RESULT EQUAL 0)
                # Parse feature detection output
                string(REPLACE "\n" ";" FEATURE_LINES ${FEATURE_OUTPUT})
                foreach(line ${FEATURE_LINES})
                    if(line MATCHES "^([A-Z_]+)=([0-9]+)$")
                        set(${CMAKE_MATCH_1} ${CMAKE_MATCH_2} CACHE INTERNAL "C++23 Feature: ${CMAKE_MATCH_1}")
                        message(STATUS "  ${CMAKE_MATCH_1}: ${CMAKE_MATCH_2}")
                    endif()
                endforeach()
            else()
                message(WARNING "Failed to run C++23 feature detection")
            endif()
        else()
            message(WARNING "Failed to compile C++23 feature detection")
        endif()
    else()
        message(WARNING "C++23 feature detection compilation failed: ${COMPILE_OUTPUT}")
    endif()
    
    # Set feature availability flags
    if(CONSTEVAL_SUPPORTED)
        set(HELLOWORLD_HAS_CONSTEVAL ON CACHE BOOL "consteval support available")
    endif()
    
    if(IF_CONSTEVAL_SUPPORTED)
        set(HELLOWORLD_HAS_IF_CONSTEVAL ON CACHE BOOL "if consteval support available")
    endif()
    
    if(EXPECTED_SUPPORTED)
        set(HELLOWORLD_HAS_EXPECTED ON CACHE BOOL "std::expected support available")
    endif()
    
    if(STACKTRACE_SUPPORTED)
        set(HELLOWORLD_HAS_STACKTRACE ON CACHE BOOL "std::stacktrace support available")
    endif()
    
    if(MODULES_SUPPORTED)
        set(HELLOWORLD_HAS_MODULES ON CACHE BOOL "C++23 modules support available")
    endif()
    
    if(CONCEPTS_SUPPORTED)
        set(HELLOWORLD_HAS_CONCEPTS ON CACHE BOOL "C++20 concepts support available")
    endif()
    
    # Summary
    message(STATUS "C++23 Feature Detection Summary:")
    message(STATUS "  consteval: ${HELLOWORLD_HAS_CONSTEVAL}")
    message(STATUS "  if consteval: ${HELLOWORLD_HAS_IF_CONSTEVAL}")  
    message(STATUS "  std::expected: ${HELLOWORLD_HAS_EXPECTED}")
    message(STATUS "  std::stacktrace: ${HELLOWORLD_HAS_STACKTRACE}")
    message(STATUS "  modules: ${HELLOWORLD_HAS_MODULES}")
    message(STATUS "  concepts: ${HELLOWORLD_HAS_CONCEPTS}")
    
endfunction()

# ============================================================================
# Consteval Performance Limits Detection
# ============================================================================

function(detect_consteval_limits)
    message(STATUS "Detecting consteval performance limits...")
    
    # Test consteval computation limits
    set(CONSTEVAL_TEST_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/consteval_limits_test.cpp")
    
    file(WRITE ${CONSTEVAL_TEST_SOURCE} "
#include <iostream>

consteval int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

consteval int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n-1);
}

int main() {
    // Test different consteval computation intensities
    constexpr auto fib_10 = fibonacci(10);
    constexpr auto fib_20 = fibonacci(20);
    constexpr auto fact_10 = factorial(10);
    constexpr auto fact_15 = factorial(15);
    
    std::cout << \"CONSTEVAL_FIB_10=\" << fib_10 << \"\\n\";
    std::cout << \"CONSTEVAL_FIB_20=\" << fib_20 << \"\\n\";
    std::cout << \"CONSTEVAL_FACT_10=\" << fact_10 << \"\\n\";
    std::cout << \"CONSTEVAL_FACT_15=\" << fact_15 << \"\\n\";
    std::cout << \"CONSTEVAL_LIMITS_OK=1\\n\";
    
    return 0;
}
")
    
    # Test consteval limits with various compiler flags
    set(CONSTEVAL_FLAGS_LIST
        "-fconstexpr-ops-limit=1000000"
        "-fconstexpr-ops-limit=10000000"  
        "-fconstexpr-depth=1000"
    )
    
    foreach(flag ${CONSTEVAL_FLAGS_LIST})
        try_compile(CONSTEVAL_TEST_RESULT
            ${CMAKE_CURRENT_BINARY_DIR}
            ${CONSTEVAL_TEST_SOURCE}
            CMAKE_FLAGS "-DCMAKE_CXX_STANDARD=23" "-DCMAKE_CXX_FLAGS=${flag}"
            OUTPUT_VARIABLE CONSTEVAL_OUTPUT
        )
        
        if(CONSTEVAL_TEST_RESULT)
            message(STATUS "  consteval limits OK with: ${flag}")
            set(HELLOWORLD_CONSTEVAL_FLAGS "${HELLOWORLD_CONSTEVAL_FLAGS} ${flag}" CACHE STRING "Working consteval flags")
        else()
            message(WARNING "  consteval limits failed with: ${flag}")
        endif()
    endforeach()
    
endfunction()

# ============================================================================
# Module Compilation Test
# ============================================================================

function(test_module_compilation)
    if(NOT HELLOWORLD_HAS_MODULES)
        message(STATUS "Skipping module compilation test - modules not supported")
        return()
    endif()
    
    message(STATUS "Testing C++23 module compilation...")
    
    set(MODULE_TEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/module_test")
    file(MAKE_DIRECTORY ${MODULE_TEST_DIR})
    
    # Create test module interface
    file(WRITE "${MODULE_TEST_DIR}/test_module.cppm" "
export module test_module;

export consteval bool is_module_working() {
    return true;
}

export template<typename T>
concept TestConcept = requires {
    typename T::value_type;
};
")
    
    # Create test module consumer
    file(WRITE "${MODULE_TEST_DIR}/test_consumer.cpp" "
import test_module;
#include <iostream>

int main() {
    if constexpr (is_module_working()) {
        std::cout << \"MODULE_COMPILATION_OK=1\\n\";
    }
    return 0;
}
")
    
    # Test module compilation (this is experimental and may fail)
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -std=c++23 -fmodules-ts -c ${MODULE_TEST_DIR}/test_module.cppm
        WORKING_DIRECTORY ${MODULE_TEST_DIR}
        RESULT_VARIABLE MODULE_COMPILE_RESULT
        OUTPUT_VARIABLE MODULE_COMPILE_OUTPUT
        ERROR_VARIABLE MODULE_COMPILE_ERROR
    )
    
    if(MODULE_COMPILE_RESULT EQUAL 0)
        message(STATUS "  Module interface compilation: SUCCESS")
        set(HELLOWORLD_MODULE_COMPILATION_OK ON CACHE BOOL "Module compilation working")
    else()
        message(WARNING "  Module interface compilation: FAILED")
        message(WARNING "  Error: ${MODULE_COMPILE_ERROR}")
        set(HELLOWORLD_MODULE_COMPILATION_OK OFF CACHE BOOL "Module compilation working")
    endif()
    
endfunction()

# ============================================================================
# Main Detection Function
# ============================================================================

function(validate_cpp23_build_system)
    message(STATUS "========================================")
    message(STATUS "C++23 Build System Validation")
    message(STATUS "========================================")
    
    detect_cpp23_features()
    detect_consteval_limits()
    test_module_compilation()
    
    # Generate validation report
    set(VALIDATION_REPORT "${CMAKE_CURRENT_BINARY_DIR}/cpp23_validation_report.txt")
    file(WRITE ${VALIDATION_REPORT} "C++23 Build System Validation Report\n")
    file(APPEND ${VALIDATION_REPORT} "Generated: ${CMAKE_CURRENT_LIST_FILE}\n")
    file(APPEND ${VALIDATION_REPORT} "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}\n\n")
    
    file(APPEND ${VALIDATION_REPORT} "Feature Support:\n")
    file(APPEND ${VALIDATION_REPORT} "  consteval: ${HELLOWORLD_HAS_CONSTEVAL}\n")
    file(APPEND ${VALIDATION_REPORT} "  if consteval: ${HELLOWORLD_HAS_IF_CONSTEVAL}\n")
    file(APPEND ${VALIDATION_REPORT} "  std::expected: ${HELLOWORLD_HAS_EXPECTED}\n")
    file(APPEND ${VALIDATION_REPORT} "  std::stacktrace: ${HELLOWORLD_HAS_STACKTRACE}\n")
    file(APPEND ${VALIDATION_REPORT} "  modules: ${HELLOWORLD_HAS_MODULES}\n")
    file(APPEND ${VALIDATION_REPORT} "  concepts: ${HELLOWORLD_HAS_CONCEPTS}\n")
    
    file(APPEND ${VALIDATION_REPORT} "\nBuild Configuration Readiness:\n")
    
    set(VALIDATION_PASSED ON)
    
    # Check minimum requirements for build configuration system
    if(NOT HELLOWORLD_HAS_CONSTEVAL)
        file(APPEND ${VALIDATION_REPORT} "  ❌ consteval required for compile-time configuration\n")
        set(VALIDATION_PASSED OFF)
    endif()
    
    if(NOT HELLOWORLD_HAS_CONCEPTS)
        file(APPEND ${VALIDATION_REPORT} "  ❌ concepts required for configuration type constraints\n")
        set(VALIDATION_PASSED OFF)
    endif()
    
    if(NOT HELLOWORLD_HAS_EXPECTED)
        file(APPEND ${VALIDATION_REPORT} "  ❌ std::expected required for error handling configuration\n")
        set(VALIDATION_PASSED OFF)
    endif()
    
    if(VALIDATION_PASSED)
        file(APPEND ${VALIDATION_REPORT} "  ✅ All minimum requirements met\n")
        message(STATUS "✅ C++23 Build Configuration System: VALIDATION PASSED")
        set(HELLOWORLD_CPP23_BUILD_CONFIG_READY ON CACHE BOOL "Ready for C++23 build configuration")
    else()
        file(APPEND ${VALIDATION_REPORT} "  ❌ Missing required features\n")
        message(WARNING "❌ C++23 Build Configuration System: VALIDATION FAILED")
        set(HELLOWORLD_CPP23_BUILD_CONFIG_READY OFF CACHE BOOL "Ready for C++23 build configuration")
    endif()
    
    message(STATUS "Validation report: ${VALIDATION_REPORT}")
    message(STATUS "========================================")
    
endfunction()
