#include "include/greeting/types.hpp"
#include <iostream>

// Test compile-time validation
constexpr auto test_compile_time() {
    using namespace greeting;
    
    // This should be evaluated at compile time
    auto result = consteval_validation::validatePersonName("John Doe");
    
    // Return whether validation passed (no error)
    return !result.has_value();
}

int main() {
    using namespace greeting;
    
    // Test compile-time evaluation
    constexpr bool compile_time_valid = test_compile_time();
    std::cout << "Compile-time validation result: " << (compile_time_valid ? "PASSED" : "FAILED") << std::endl;
    
    // Test runtime validation
    auto runtime_result = PersonName::create("Jane Smith");
    if (runtime_result.has_value()) {
        std::cout << "Runtime validation result: PASSED (" << runtime_result.value().value() << ")" << std::endl;
    } else {
        std::cout << "Runtime validation result: FAILED" << std::endl;
    }
    
    // Test invalid name
    auto invalid_result = PersonName::create("");
    if (!invalid_result.has_value()) {
        std::cout << "Invalid name handling: PASSED (correctly rejected empty name)" << std::endl;
    } else {
        std::cout << "Invalid name handling: FAILED (should have rejected empty name)" << std::endl;
    }
    
    return 0;
}
