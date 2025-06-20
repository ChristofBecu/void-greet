#include <iostream>
#include <config/build_config.hpp>

using namespace greeting::config;

int main() {
    std::cout << "=== C++23 Build Configuration System Demo ===\n\n";
    
    // Display current build configuration
    std::cout << "Current Build Configuration: " << build_configuration_name() << "\n";
    std::cout << "Debug Build: " << std::boolalpha << is_debug_build() << "\n";
    std::cout << "Release Build: " << std::boolalpha << is_release_build() << "\n"; 
    std::cout << "Profile Build: " << std::boolalpha << is_profile_build() << "\n\n";
    
    // Display feature availability
    std::cout << "=== C++23 Feature Availability ===\n";
    std::cout << "consteval: " << std::boolalpha << has_consteval() << "\n";
    std::cout << "if consteval: " << std::boolalpha << has_if_consteval() << "\n";
    std::cout << "std::expected: " << std::boolalpha << has_expected() << "\n";
    std::cout << "std::stacktrace: " << std::boolalpha << has_stacktrace() << "\n\n";
    
    // Display configuration flags
    std::cout << "=== Configuration Flags ===\n";
    std::cout << "Assertions Enabled: " << std::boolalpha << assertions_enabled() << "\n";
    std::cout << "Logging Enabled: " << std::boolalpha << logging_enabled() << "\n";
    std::cout << "Validation Enabled: " << std::boolalpha << validation_enabled() << "\n";
    std::cout << "Rich Errors Enabled: " << std::boolalpha << rich_errors_enabled() << "\n";
    std::cout << "Performance Monitoring: " << std::boolalpha << performance_monitoring_enabled() << "\n\n";
    
    // Demonstrate conditional compilation
    std::cout << "=== Conditional Compilation Demo ===\n";
    
    HELLOWORLD_IF_DEBUG(
        std::cout << "✅ This code only runs in DEBUG builds!\n";
    );
    
    HELLOWORLD_IF_RELEASE(
        std::cout << "✅ This code only runs in RELEASE builds!\n";
    );
    
    HELLOWORLD_IF_PROFILE(
        std::cout << "✅ This code only runs in PROFILE builds!\n";
    );
    
    // Demonstrate configuration-aware types
    std::cout << "\n=== Configuration-Aware Types ===\n";
    static_assert(ConfigurationAware<ActiveConfiguration>);
    std::cout << "Active configuration type: ";
    
    if constexpr (DebugBuild<ActiveConfiguration>) {
        std::cout << "DebugConfiguration\n";
    } else if constexpr (ReleaseBuild<ActiveConfiguration>) {
        std::cout << "ReleaseConfiguration or ProfileConfiguration\n";
    }
    
    #if HELLOWORLD_HAS_IF_CONSTEVAL_RUNTIME
    std::cout << "\n=== C++23 if consteval Demo ===\n";
    
    auto compile_time_demo = []() consteval { return "Compile-time path"; };
    auto runtime_demo = []() { return "Runtime path"; };
    
    auto result = dual_path_configuration(compile_time_demo, runtime_demo);
    std::cout << "Dual-path result: " << result << "\n";
    #endif
    
    std::cout << "\n🎉 Phase 1 Implementation Complete!\n";
    
    return 0;
}
