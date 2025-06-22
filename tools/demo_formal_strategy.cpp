#include "greeting/formal_strategy.hpp"
#include "greeting/person_name.hpp"
#include <iostream>
#include <vector>

int main() {
    using namespace greeting;
    using namespace greeting::strategy;
    
    std::cout << "🎩 Formal Greeting Strategy Demo\n";
    std::cout << "================================\n\n";
    
    auto strategy = createFormalStrategy();
    
    if (!strategy) {
        std::cerr << "❌ Failed to create formal strategy\n";
        return 1;
    }
    
    std::cout << "✅ Strategy: " << strategy->getStrategyName() << "\n";
    std::cout << "✅ Valid: " << (strategy->isValid() ? "Yes" : "No") << "\n\n";
    
    // Test with various formal names
    std::vector<std::string> test_names = {
        "Dr. Jane Smith",
        "Prof. John Doe", 
        "Mr. Robert Brown",
        "Mrs. Elizabeth Taylor",
        "Ms. Sarah Wilson",
        "Sir Winston Churchill",
        "Rev. Martin Luther",
        "Alice Johnson"  // No title
    };
    
    std::cout << "🧪 Testing formal greetings:\n";
    std::cout << "----------------------------\n";
    
    for (const auto& name : test_names) {
        auto person = PersonName::create(name);
        if (person.has_value()) {
            auto result = strategy->greet(person.value());
            if (result.has_value()) {
                std::cout << "✅ \"" << name << "\"\n";
                std::cout << "   → \"" << result.value().value() << "\"\n\n";
            } else {
                std::cout << "❌ \"" << name << "\" → ERROR\n\n";
            }
        } else {
            std::cout << "❌ Invalid name: \"" << name << "\"\n\n";
        }
    }
    
    // Test compile-time features
    std::cout << "🔧 Compile-time features:\n";
    std::cout << "-------------------------\n";
    
    // Test constexpr title validation
    constexpr bool dr_valid = formal_utils::isFormalTitleValid("Dr.");
    constexpr bool prof_valid = formal_utils::isFormalTitleValid("Prof.");
    constexpr bool invalid_valid = formal_utils::isFormalTitleValid("Invalid");
    
    std::cout << "✅ constexpr title validation:\n";
    std::cout << "   Dr. is valid: " << (dr_valid ? "Yes" : "No") << "\n";
    std::cout << "   Prof. is valid: " << (prof_valid ? "Yes" : "No") << "\n";
    std::cout << "   Invalid is valid: " << (invalid_valid ? "Yes" : "No") << "\n\n";
    
    // Test compile-time greeting generation
    constexpr auto greeting = compile_time::generateFormalGreeting("Test");
    std::cout << "✅ constexpr greeting generation:\n";
    std::cout << "   Generated: \"" << greeting.data() << "\"\n\n";
    
    std::cout << "🎉 Formal Strategy Demo Complete!\n";
    
    return 0;
}
