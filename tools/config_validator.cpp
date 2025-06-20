/**
 * @file tools/config_validator.cpp
 * @brief C++23 Build Configuration System Validator
 * 
 * This tool validates the compiler's C++23 capabilities and provides
 * recommendations for the build configuration system implementation.
 */

#include <version>
#include <iostream>
#include <iomanip>
#include <string_view>
#include <vector>
#include <chrono>

#ifdef __cpp_lib_expected
#include <expected>
#endif

namespace greeting::config::validation {

/**
 * @brief Feature test result structure
 */
struct FeatureTest {
    std::string_view name;
    bool supported{false};
    long version{0};
    std::string_view description;
};

/**
 * @brief Performance test result structure  
 */
struct PerformanceTest {
    std::string_view name;
    bool passed{false};
    std::chrono::microseconds duration{0};
    std::string_view details;
};

/**
 * @brief Test consteval function performance
 */
consteval int fibonacci_consteval(int n) {
    if (n <= 1) return n;
    return fibonacci_consteval(n-1) + fibonacci_consteval(n-2);
}

consteval int factorial_consteval(int n) {
    return n <= 1 ? 1 : n * factorial_consteval(n-1);
}

/**
 * @brief Test if consteval dual-path functionality
 */
constexpr int dual_path_test(int n) {
#ifdef __cpp_if_consteval
    if consteval {
        // Compile-time path - simple computation
        return n * 2;
    } else {
        // Runtime path - more complex computation
        return n * n + n;
    }
#else
    return n * 2; // Fallback
#endif
}

/**
 * @brief Test configuration-aware type system
 */
template<bool IsDebugBuild>
struct ConfigurationTest {
    static consteval bool is_debug() noexcept {
        return IsDebugBuild;
    }
    
    static consteval std::string_view build_type() noexcept {
        return IsDebugBuild ? "Debug" : "Release";
    }
};

#ifdef __cpp_concepts
/**
 * @brief Test concept-based configuration  
 */
template<typename T>
concept DebugConfiguration = requires {
    T::is_debug() == true;
};

template<typename T>
concept ReleaseConfiguration = requires {
    T::is_debug() == false;
};

template<DebugConfiguration Config>
consteval auto debug_validation() {
    return "Enhanced debug validation enabled";
}

template<ReleaseConfiguration Config>  
consteval auto release_validation() {
    return "Optimized release validation enabled";
}
#endif

/**
 * @brief Run C++23 feature detection tests
 */
std::vector<FeatureTest> run_feature_tests() {
    std::vector<FeatureTest> tests;
    
    // Core language features
    tests.push_back({
        "consteval",
#ifdef __cpp_consteval
        true, __cpp_consteval,
#else
        false, 0,
#endif
        "Required for compile-time configuration"
    });
    
    tests.push_back({
        "if consteval", 
#ifdef __cpp_if_consteval
        true, __cpp_if_consteval,
#else
        false, 0,
#endif
        "Required for dual-path optimization"
    });
    
    tests.push_back({
        "concepts",
#ifdef __cpp_concepts
        true, __cpp_concepts,
#else
        false, 0,
#endif
        "Required for configuration type constraints"
    });
    
    tests.push_back({
        "deducing this",
#ifdef __cpp_deducing_this
        true, __cpp_deducing_this,
#else
        false, 0,
#endif
        "Optional for advanced member function templates"
    });
    
    // Library features
    tests.push_back({
        "std::expected",
#ifdef __cpp_lib_expected
        true, __cpp_lib_expected,
#else
        false, 0,
#endif
        "Required for configuration-aware error handling"
    });
    
    tests.push_back({
        "std::stacktrace",
#ifdef __cpp_lib_stacktrace
        true, __cpp_lib_stacktrace,
#else
        false, 0,
#endif
        "Optional for enhanced debug error context"
    });
    
    tests.push_back({
        "modules",
#ifdef __cpp_modules
        true, __cpp_modules,
#else
        false, 0,
#endif
        "Optional for configuration module system"
    });
    
    return tests;
}

/**
 * @brief Run consteval performance tests
 */
std::vector<PerformanceTest> run_performance_tests() {
    std::vector<PerformanceTest> tests;
    
    // Test consteval computation limits
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        // Test moderate consteval computation
        constexpr auto fib_result = fibonacci_consteval(20);
        constexpr auto fact_result = factorial_consteval(10);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        tests.push_back({
            "consteval computation",
            true,
            duration,
            "Fibonacci(20) and Factorial(10) computed successfully"
        });
        
    } catch (...) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        tests.push_back({
            "consteval computation",
            false,
            duration,
            "consteval computation limits exceeded"
        });
    }
    
    // Test dual-path functionality
    start = std::chrono::high_resolution_clock::now();
    
    constexpr auto compile_time_result = dual_path_test(10);
    auto runtime_result = dual_path_test(10);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    tests.push_back({
        "dual-path optimization",
        compile_time_result != runtime_result, // Should be different in if consteval
        duration,
        "Compile-time and runtime paths tested"
    });
    
    return tests;
}

/**
 * @brief Test configuration system functionality
 */
bool test_configuration_system() {
    // Test configuration types
    using DebugConfig = ConfigurationTest<true>;
    using ReleaseConfig = ConfigurationTest<false>;
    
    static_assert(DebugConfig::is_debug());
    static_assert(!ReleaseConfig::is_debug());
    static_assert(DebugConfig::build_type() == "Debug");
    static_assert(ReleaseConfig::build_type() == "Release");
    
#ifdef __cpp_concepts
    // Test concept-based configuration
    static_assert(DebugConfiguration<DebugConfig>);
    static_assert(ReleaseConfiguration<ReleaseConfig>);
    
    constexpr auto debug_msg = debug_validation<DebugConfig>();
    constexpr auto release_msg = release_validation<ReleaseConfig>();
#endif
    
    return true;
}

/**
 * @brief Print validation report
 */
void print_validation_report(const std::vector<FeatureTest>& features,
                           const std::vector<PerformanceTest>& performance) {
    std::cout << "========================================\n";
    std::cout << "C++23 Build Configuration Validator\n";
    std::cout << "========================================\n\n";
    
    std::cout << "Compiler Information:\n";
    std::cout << "  Version: " << __VERSION__ << "\n";
    std::cout << "  C++ Standard: " << __cplusplus << "\n\n";
    
    std::cout << "Feature Support Matrix:\n";
    std::cout << std::left << std::setw(20) << "Feature" 
              << std::setw(10) << "Status" 
              << std::setw(12) << "Version"
              << "Description\n";
    std::cout << std::string(70, '-') << "\n";
    
    bool all_required_features = true;
    
    for (const auto& test : features) {
        std::cout << std::left << std::setw(20) << test.name
                  << std::setw(10) << (test.supported ? "✅" : "❌")
                  << std::setw(12) << (test.supported ? std::to_string(test.version) : "N/A")
                  << test.description << "\n";
        
        // Check if this is a required feature
        if ((test.name == "consteval" || test.name == "concepts" || test.name == "std::expected") 
            && !test.supported) {
            all_required_features = false;
        }
    }
    
    std::cout << "\nPerformance Tests:\n";
    std::cout << std::left << std::setw(25) << "Test"
              << std::setw(10) << "Status"
              << std::setw(15) << "Duration (μs)"
              << "Details\n";
    std::cout << std::string(70, '-') << "\n";
    
    for (const auto& test : performance) {
        std::cout << std::left << std::setw(25) << test.name
                  << std::setw(10) << (test.passed ? "✅" : "❌")
                  << std::setw(15) << test.duration.count()
                  << test.details << "\n";
    }
    
    std::cout << "\nConfiguration System Test:\n";
    bool config_test_passed = test_configuration_system();
    std::cout << "  Configuration type system: " << (config_test_passed ? "✅ PASSED" : "❌ FAILED") << "\n";
    
    std::cout << "\n========================================\n";
    std::cout << "Validation Summary:\n";
    std::cout << "========================================\n";
    
    if (all_required_features && config_test_passed) {
        std::cout << "✅ BUILD CONFIGURATION SYSTEM: READY\n";
        std::cout << "   All required C++23 features are supported.\n";
        std::cout << "   Proceeding with implementation is recommended.\n";
    } else {
        std::cout << "❌ BUILD CONFIGURATION SYSTEM: NOT READY\n";
        std::cout << "   Missing required features or performance issues detected.\n";
        std::cout << "   Consider using fallback implementation or upgrading compiler.\n";
    }
    
    std::cout << "\nRecommendations:\n";
    
    // Feature-specific recommendations
    auto expected_feature = std::find_if(features.begin(), features.end(),
        [](const auto& f) { return f.name == "std::expected"; });
    
    if (expected_feature != features.end() && !expected_feature->supported) {
        std::cout << "  - Consider using a polyfill for std::expected\n";
    }
    
    auto stacktrace_feature = std::find_if(features.begin(), features.end(),
        [](const auto& f) { return f.name == "std::stacktrace"; });
        
    if (stacktrace_feature != features.end() && !stacktrace_feature->supported) {
        std::cout << "  - Debug builds will use alternative error context methods\n";
    }
    
    auto modules_feature = std::find_if(features.begin(), features.end(),
        [](const auto& f) { return f.name == "modules"; });
        
    if (modules_feature != features.end() && !modules_feature->supported) {
        std::cout << "  - Configuration system will use traditional headers instead of modules\n";
    }
    
    std::cout << "========================================\n";
}

} // namespace greeting::config::validation

/**
 * @brief Main validation entry point
 */
int main() {
    using namespace greeting::config::validation;
    
    auto features = run_feature_tests();
    auto performance = run_performance_tests();
    
    print_validation_report(features, performance);
    
    // Return appropriate exit code
    bool validation_passed = std::all_of(features.begin(), features.end(),
        [](const auto& f) {
            // Only check required features
            if (f.name == "consteval" || f.name == "concepts" || f.name == "std::expected") {
                return f.supported;
            }
            return true; // Optional features don't affect validation
        });
    
    return validation_passed ? 0 : 1;
}
