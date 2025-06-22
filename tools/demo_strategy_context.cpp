#include "greeting/strategy_context.hpp"
#include "greeting/person_name.hpp"
#include "config/build_config.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace greeting;
using namespace greeting::strategy;

void demonstrateBasicGreeting() {
    std::cout << "\n=== Basic Strategy Context Greeting ===\n";
    
    auto context = createDefaultContext();
    
    std::vector<std::string> names = {"Alice", "Dr. Smith", "Bob", "Prof. Johnson"};
    
    for (const auto& name : names) {
        auto person_result = PersonName::create(name);
        if (person_result.has_value()) {
            auto greeting_result = context->greet(person_result.value());
            if (greeting_result.has_value()) {
                std::cout << "✓ " << greeting_result.value().value() << std::endl;
            } else {
                std::cout << "✗ Error greeting " << name << std::endl;
            }
        }
    }
}

void demonstrateStrategySelection() {
    std::cout << "\n=== Explicit Strategy Selection ===\n";
    
    auto context = createDefaultContext();
    auto person = PersonName::create("Dr. Jane Smith").value();
    
    // Try different strategies if available
    std::vector<StrategyType> strategies = {
        StrategyType::Console,
        StrategyType::Formal,
        StrategyType::Casual
    };
    
    for (auto strategy : strategies) {
        if (context->hasStrategy(strategy)) {
            auto result = context->greetWith(person, strategy);
            if (result.has_value()) {
                std::cout << toString(strategy) << " Strategy: " 
                         << result.value().value() << std::endl;
            }
        } else {
            std::cout << toString(strategy) << " Strategy: Not available" << std::endl;
        }
    }
}

void demonstrateAutomaticSelection() {
    std::cout << "\n=== Automatic Strategy Selection ===\n";
    
    auto context = createDefaultContext();
    
    std::vector<std::string> test_names = {
        "Dr. Thompson",     // Formal pattern
        "Prof. Williams",   // Formal pattern  
        "Bob",             // Casual pattern
        "Jo",              // Casual pattern
        "Alice Johnson"    // Regular pattern
    };
    
    for (const auto& name : test_names) {
        auto person_result = PersonName::create(name);
        if (person_result.has_value()) {
            auto result = context->greetWithValidation(name, StrategyType::Auto);
            if (result.has_value()) {
                std::cout << "Auto-selected for '" << name << "': " 
                         << result.value().value() << std::endl;
            }
        }
    }
}

void demonstrateContextConfiguration() {
    std::cout << "\n=== Context Configuration ===\n";
    
    ContextConfig config{};
    config.default_strategy = StrategyType::Console;
    config.enable_fallback = true;
    config.enable_metrics = greeting::config::is_debug_build();
    config.enable_caching = greeting::config::is_debug_build();
    
    auto context = createOptimizedContext(config);
    
    std::cout << "Context Configuration:" << std::endl;
    std::cout << "  Default Strategy: " << toString(config.default_strategy) << std::endl;
    std::cout << "  Fallback Enabled: " << (config.enable_fallback ? "Yes" : "No") << std::endl;
    std::cout << "  Metrics Enabled: " << (config.enable_metrics ? "Yes" : "No") << std::endl;
    std::cout << "  Caching Enabled: " << (config.enable_caching ? "Yes" : "No") << std::endl;
    
    std::cout << "\nAvailable Strategies: ";
    auto available = context->getAvailableStrategies();
    for (size_t i = 0; i < available.size(); ++i) {
        std::cout << toString(available[i]);
        if (i < available.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
}

void demonstratePerformanceMetrics() {
    std::cout << "\n=== Performance Metrics ===\n";
    
    if constexpr (greeting::config::is_debug_build()) {
        ContextConfig config{};
        config.enable_metrics = true;
        
        auto context = createOptimizedContext(config);
        
        // Perform several greetings to generate metrics
        std::vector<std::string> names = {"Alice", "Bob", "Charlie", "Diana", "Eve"};
        for (const auto& name : names) {
            auto person_result = PersonName::create(name);
            if (person_result.has_value()) {
                auto result = context->greet(person_result.value());
                // We don't need to process the result, just generate metrics
                (void)result;
            }
        }
        
        std::cout << context->getPerformanceMetrics() << std::endl;
        
        auto last_execution = context->getLastExecution();
        if (last_execution.has_value()) {
            std::cout << "Last Execution Details:" << std::endl;
            std::cout << "  Strategy: " << last_execution->strategy_name << std::endl;
            std::cout << "  Fallback Used: " << (last_execution->fallback_used ? "Yes" : "No") << std::endl;
        }
    } else {
        std::cout << "Performance metrics are available only in debug builds." << std::endl;
        std::cout << "Current build: Release (optimized for performance)" << std::endl;
    }
}

void demonstrateErrorHandling() {
    std::cout << "\n=== Error Handling ===\n";
    
    auto context = createDefaultContext();
    
    // Test with invalid input
    std::vector<std::string> invalid_names = {"", "   ", "Test@Name"};
    
    for (const auto& name : invalid_names) {
        auto result = context->greetWithValidation(name);
        if (!result.has_value()) {
            std::cout << "✓ Validation correctly rejected: \"" << name << "\"" << std::endl;
        } else {
            std::cout << "✗ Unexpected success for: \"" << name << "\"" << std::endl;
        }
    }
}

void demonstrateUtilityFunctions() {
    std::cout << "\n=== Utility Functions ===\n";
    
    std::cout << "Strategy Type Conversions:" << std::endl;
    std::vector<StrategyType> types = {
        StrategyType::Console,
        StrategyType::Formal,
        StrategyType::Casual,
        StrategyType::Auto
    };
    
    for (auto type : types) {
        auto name = toString(type);
        auto parsed = parseStrategyType(name);
        std::cout << "  " << static_cast<int>(type) << " -> \"" 
                 << name << "\" -> " 
                 << (parsed.has_value() ? "✓" : "✗") << std::endl;
    }
}

int main() {
    std::cout << "🌟 Strategy Context Integration Demo\n";
    std::cout << "=====================================\n";
    
    if constexpr (greeting::config::is_debug_build()) {
        std::cout << "Build Configuration: Debug (with enhanced diagnostics)\n";
    } else {
        std::cout << "Build Configuration: Release (optimized)\n";
    }
    
    try {
        demonstrateBasicGreeting();
        demonstrateStrategySelection();
        demonstrateAutomaticSelection();
        demonstrateContextConfiguration();
        demonstratePerformanceMetrics();
        demonstrateErrorHandling();
        demonstrateUtilityFunctions();
        
        std::cout << "\n✅ Strategy Context Integration Demo completed successfully!\n";
        std::cout << "\nKey Features Demonstrated:\n";
        std::cout << "  • Basic greeting with default strategy\n";
        std::cout << "  • Explicit strategy selection\n";
        std::cout << "  • Automatic strategy selection based on name patterns\n";
        std::cout << "  • Context configuration management\n";
        std::cout << "  • Performance metrics and diagnostics\n";
        std::cout << "  • Comprehensive error handling\n";
        std::cout << "  • Utility functions for strategy management\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Demo failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Demo failed with unknown exception" << std::endl;
        return 2;
    }
    
    return 0;
}
