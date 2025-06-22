#include "greeting/casual_strategy.hpp"
#include "greeting/person_name.hpp"
#include <iostream>
#include <vector>

using namespace greeting;
using namespace greeting::strategy;

int main() {
    std::cout << "=== Casual Greeting Strategy Demo ===\n\n";
    
    // Create casual strategy
    auto strategy = CasualStrategy::create();
    
    // Test names to demonstrate casual validation
    std::vector<std::string> test_names = {
        "John",
        "Alice Smith", 
        "Bob_Jones",  // Underscore allowed in casual mode
        "X",          // Very short name allowed
        "Jo",         // Short name
        "Mary-Jane"   // Hyphenated name
    };
    
    for (const auto& name : test_names) {
        std::cout << "Testing name: \"" << name << "\"\n";
        
        // Validate with casual rules
        auto person_result = strategy->validateCasualName(name);
        if (!person_result.has_value()) {
            std::cout << "  ❌ Validation failed: " << static_cast<int>(person_result.error()) << "\n";
            continue;
        }
        
        // Generate greeting
        auto greeting_result = strategy->greet(person_result.value());
        if (!greeting_result.has_value()) {
            std::cout << "  ❌ Greeting generation failed: " << static_cast<int>(greeting_result.error()) << "\n";
            continue;
        }
        
        std::cout << "  ✅ " << greeting_result.value().value() << "\n";
        std::cout << "\n";
    }
    
    // Demonstrate random variations
    std::cout << "=== Random Greeting Variations ===\n";
    auto person = PersonName::create("Alex");
    if (person.has_value()) {
        std::cout << "Generating 5 greetings for \"Alex\":\n";
        for (int i = 1; i <= 5; ++i) {
            auto greeting = strategy->greet(person.value());
            if (greeting.has_value()) {
                std::cout << i << ". " << greeting.value().value() << "\n";
            }
        }
    }
    
    std::cout << "\n=== Strategy Information ===\n";
    std::cout << "Strategy Name: " << strategy->getStrategyName() << "\n";
    std::cout << "Is Valid: " << (strategy->isValid() ? "Yes" : "No") << "\n";
    
    // Show traits information
    using traits = greeting::strategy::traits::StrategyTraits<CasualStrategy>;
    std::cout << "\n=== Strategy Traits ===\n";
    std::cout << "Thread Safe: " << (traits::is_thread_safe ? "Yes" : "No") << "\n";
    std::cout << "Supports Variations: " << (traits::supports_variations ? "Yes" : "No") << "\n";
    std::cout << "Supports Nicknames: " << (traits::supports_nicknames ? "Yes" : "No") << "\n";
    std::cout << "Min Name Length: " << traits::min_name_length << "\n";
    std::cout << "Max Name Length: " << traits::max_name_length << "\n";
    std::cout << "Expected Message Length: " << traits::expected_message_length << "\n";
    
    return 0;
}
