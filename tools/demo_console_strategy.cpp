#include <iostream>
#include <iomanip>
#include "greeting/console_strategy.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"
#include "config/build_config.hpp"

using namespace greeting;
using greeting::strategy::createConsoleStrategy;

int main() {
    std::cout << "=== Console Strategy Implementation Demo ===" << std::endl;
    
    // Display current build configuration
    std::cout << "\nBuild Configuration:" << std::endl;
    if constexpr (greeting::config::is_debug_build()) {
        std::cout << "- Build Type: Debug" << std::endl;
        std::cout << "- Debug markers: Enabled" << std::endl;
        if constexpr (greeting::config::performance_monitoring_enabled()) {
            std::cout << "- Performance monitoring: Enabled (timestamps)" << std::endl;
        } else {
            std::cout << "- Performance monitoring: Disabled" << std::endl;
        }
    } else {
        std::cout << "- Build Type: Release" << std::endl;
        std::cout << "- Debug markers: Disabled (optimized)" << std::endl;
        std::cout << "- Performance monitoring: Disabled" << std::endl;
    }
    
    std::cout << "\n=== Console Strategy Demo ===" << std::endl;
    
    // Create console strategy instance
    auto strategy = createConsoleStrategy();
    
    std::cout << "\nStrategy Information:" << std::endl;
    std::cout << "- Name: " << strategy->getStrategyName() << std::endl;
    std::cout << "- Valid: " << (strategy->isValid() ? "Yes" : "No") << std::endl;
    
    // Display strategy traits
    using ConsoleTraits = greeting::strategy::traits::StrategyTraits<greeting::strategy::ConsoleGreetingStrategy>;
    std::cout << "\nStrategy Traits:" << std::endl;
    std::cout << "- Supports debug info: " << (ConsoleTraits::supports_debug_info ? "Yes" : "No") << std::endl;
    std::cout << "- Thread safe: " << (ConsoleTraits::is_thread_safe ? "Yes" : "No") << std::endl;
    std::cout << "- Requires system resources: " << (ConsoleTraits::requires_system_resources ? "Yes" : "No") << std::endl;
    std::cout << "- Expected message length: " << ConsoleTraits::expected_message_length << std::endl;
    
    std::cout << "\n=== Greeting Demonstrations ===" << std::endl;
    
    // Test cases with different types of names
    const std::vector<std::string> test_names = {
        "Alice",
        "Bob Smith", 
        "Jean-Claude Van Damme",
        "Mary O'Connor",
        "José María"
    };
    
    for (const auto& name : test_names) {
        std::cout << "\n--- Testing with: \"" << name << "\" ---" << std::endl;
        
        // Create person name
        auto person_result = PersonName::create(name);
        
        if (!person_result.has_value()) {
            std::cout << "Error: Invalid person name - " << static_cast<int>(person_result.error()) << std::endl;
            continue;
        }
        
        std::cout << "Valid PersonName created: " << person_result.value().value() << std::endl;
        std::cout << "Console output: ";
        
        // Use strategy to greet
        auto greeting_result = strategy->greet(person_result.value());
        
        if (greeting_result.has_value()) {
            std::cout << "\nGreeting message object: \"" << greeting_result.value().value() << "\"" << std::endl;
        } else {
            std::cout << "\nError generating greeting: " << static_cast<int>(greeting_result.error()) << std::endl;
        }
    }
    
    std::cout << "\n=== Error Handling Demo ===" << std::endl;
    
    // Test error handling with invalid names
    const std::vector<std::string> invalid_names = {
        "",                    // Empty name
        "   ",                // Only spaces
        "John123",            // Numbers (if validation rejects them)
        "A",                  // Too short (if validation requires min length)
    };
    
    for (const auto& name : invalid_names) {
        std::cout << "\n--- Testing invalid name: \"" << name << "\" ---" << std::endl;
        
        auto person_result = PersonName::create(name);
        
        if (!person_result.has_value()) {
            std::cout << "Expected validation failure: " << static_cast<int>(person_result.error()) << std::endl;
        } else {
            std::cout << "Unexpected: name passed validation" << std::endl;
            
            // If it passes PersonName validation, test strategy handling
            auto greeting_result = strategy->greet(person_result.value());
            
            if (greeting_result.has_value()) {
                std::cout << "Console output: ";
                // Output already happened to console
                std::cout << "\nGreeting created successfully" << std::endl;
            } else {
                std::cout << "Strategy error: " << static_cast<int>(greeting_result.error()) << std::endl;
            }
        }
    }
    
    std::cout << "\n=== Configuration-Aware Behavior Demo ===" << std::endl;
    
    auto person_result = PersonName::create("Configuration Test User");
    if (person_result.has_value()) {
        std::cout << "\nDemonstrating configuration-aware behavior:" << std::endl;
        std::cout << "Console output: ";
        
        auto greeting_result = strategy->greet(person_result.value());
        
        if (greeting_result.has_value()) {
            std::cout << "\nNote: ";
            if constexpr (greeting::config::is_debug_build()) {
                std::cout << "Debug build includes [DEBUG: ConsoleStrategy] marker and optional timestamp" << std::endl;
            } else {
                std::cout << "Release build omits debug markers for optimal performance" << std::endl;
            }
        }
    }
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    return 0;
}
