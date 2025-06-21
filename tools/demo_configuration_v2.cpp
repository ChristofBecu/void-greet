#include <iostream>
#include <iomanip>
#include "greeting/config_aware_validation.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"

using namespace greeting;
using namespace greeting::validation;

int main() {
    std::cout << "=== Configuration-Aware Validation Demo ===" << std::endl;
    
    // Display current build configuration
    std::cout << "\nBuild Configuration:" << std::endl;
    #ifdef HELLOWORLD_DEBUG_BUILD
        std::cout << "- Build Type: Debug" << std::endl;
    #else
        std::cout << "- Build Type: Release" << std::endl;
    #endif
    
    std::cout << "- Detailed validation: " << (ValidationContext::enable_detailed_validation() ? "Yes" : "No") << std::endl;
    std::cout << "- Stacktrace enabled: " << (ValidationContext::enable_stacktrace() ? "Yes" : "No") << std::endl;
    std::cout << "- Performance monitoring: " << (ValidationContext::enable_performance_monitoring() ? "Yes" : "No") << std::endl;
    
    std::cout << "\n=== Person Name Validation Demo ===" << std::endl;
    
    // Test valid names
    const std::vector<std::string> test_names = {
        "John Doe",
        "Mary-Jane Smith",
        "O'Connor",
        "Dr. Watson",
        "",              // Invalid: empty
        "John123",       // Invalid: contains numbers
        "John@Smith",    // Invalid: contains symbols
        std::string(101, 'A')  // Invalid: too long
    };
    
    for (const auto& name : test_names) {
        auto result = validate_person_name<PersonName>(name);
        
        std::cout << "Name: \"" << name << "\" -> ";
        if (result.has_value()) {
            std::cout << "✅ Valid (length: " << name.length() << ")" << std::endl;
        } else {
            std::cout << "❌ Invalid - Error: " << static_cast<int>(result.error()) << std::endl;
        }
    }
    
    std::cout << "\n=== Greeting Message Validation Demo ===" << std::endl;
    
    // Test valid messages
    const std::vector<std::string> test_messages = {
        "Hello, World!",
        "How are you today?",
        "Welcome to our application!",
        "",                          // Invalid: empty
        std::string(501, 'A'),       // Invalid: too long
        std::string("Hello\x00World", 11)  // Invalid: contains null character
    };
    
    for (const auto& message : test_messages) {
        auto result = validate_greeting_message<GreetingMessage>(message);
        
        std::cout << "Message: \"";
        // Print message safely (handle null characters)
        for (size_t i = 0; i < std::min(message.length(), size_t(50)); ++i) {
            char c = message[i];
            if (c == '\0') {
                std::cout << "\\0";
            } else if (std::isprint(static_cast<unsigned char>(c))) {
                std::cout << c;
            } else {
                std::cout << "\\x" << std::hex << static_cast<int>(static_cast<unsigned char>(c)) << std::dec;
            }
        }
        if (message.length() > 50) {
            std::cout << "...";
        }
        std::cout << "\" -> ";
        
        if (result.has_value()) {
            std::cout << "✅ Valid (length: " << message.length() << ")" << std::endl;
        } else {
            std::cout << "❌ Invalid - Error: " << static_cast<int>(result.error()) << std::endl;
        }
    }
    
    std::cout << "\n=== Character Validation Demo ===" << std::endl;
    
    const std::vector<char> test_chars = {
        'A', 'z', ' ', '-', '\'', '.', '0', '9', '@', '#', '\0', '\n'
    };
    
    for (char c : test_chars) {
        bool valid = validate_name_character(c);
        std::cout << "Character: '";
        if (c == '\0') {
            std::cout << "\\0";
        } else if (c == '\n') {
            std::cout << "\\n";
        } else if (std::isprint(static_cast<unsigned char>(c))) {
            std::cout << c;
        } else {
            std::cout << "\\x" << std::hex << static_cast<int>(static_cast<unsigned char>(c)) << std::dec;
        }
        std::cout << "' -> " << (valid ? "✅ Valid" : "❌ Invalid") << std::endl;
    }
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    std::cout << "Configuration-aware validation system is operational!" << std::endl;
    
    return 0;
}
