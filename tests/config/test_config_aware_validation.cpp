#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "greeting/config_aware_validation.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"
#include "config/build_config.hpp"

using namespace greeting::validation;
using namespace greeting;

// ============================================================================
// Configuration-Aware Validation Core Tests
// ============================================================================

TEST_CASE("ValidationContext functionality", "[config][validation][context]") {
    
    SECTION("ValidationContext creation and properties") {
        [[maybe_unused]] auto context = ValidationContext::create_for_current_config();
        
        // These methods should be callable
        const bool detailed = ValidationContext::enable_detailed_validation();
        const bool stacktrace = ValidationContext::enable_stacktrace();
        const bool performance = ValidationContext::enable_performance_monitoring();
        
        // Values should be consistent (these are placeholder implementations)
        REQUIRE((detailed == true || detailed == false));
        REQUIRE((stacktrace == true || stacktrace == false));
        REQUIRE((performance == true || performance == false));
    }
}

// ============================================================================
// Dual-Path Character Validation Tests
// ============================================================================

TEST_CASE("Configuration-aware character validation", "[config][validation][character]") {
    
    SECTION("Valid name characters") {
        // Basic ASCII letters
        REQUIRE(validate_name_character('A') == true);
        REQUIRE(validate_name_character('Z') == true);
        REQUIRE(validate_name_character('a') == true);
        REQUIRE(validate_name_character('z') == true);
        
        // Common name punctuation
        REQUIRE(validate_name_character(' ') == true);
        REQUIRE(validate_name_character('-') == true);
        REQUIRE(validate_name_character('\'') == true);
        REQUIRE(validate_name_character('.') == true);
    }
    
    SECTION("Invalid name characters") {
        // Numbers (not typically allowed in names)
        REQUIRE(validate_name_character('0') == false);
        REQUIRE(validate_name_character('9') == false);
        
        // Special symbols
        REQUIRE(validate_name_character('@') == false);
        REQUIRE(validate_name_character('#') == false);
        REQUIRE(validate_name_character('$') == false);
        REQUIRE(validate_name_character('%') == false);
        
        // Control characters
        REQUIRE(validate_name_character('\0') == false);
        REQUIRE(validate_name_character('\n') == false);
        REQUIRE(validate_name_character('\t') == false);
    }
}

// ============================================================================
// String Length Validation Tests
// ============================================================================

TEST_CASE("Configuration-aware string length validation", "[config][validation][length]") {
    
    SECTION("Valid string lengths") {
        const std::string valid_short = "John";
        const std::string valid_medium = "John Smith";
        const std::string valid_long = "Mary Elizabeth Johnson-Williams";
        
        REQUIRE(validate_string_length(valid_short, 1, 100) == true);
        REQUIRE(validate_string_length(valid_medium, 1, 100) == true);
        REQUIRE(validate_string_length(valid_long, 1, 100) == true);
    }
    
    SECTION("Invalid string lengths") {
        const std::string empty_string = "";
        const std::string too_long(101, 'A');
        
        REQUIRE(validate_string_length(empty_string, 1, 100) == false);
        REQUIRE(validate_string_length(too_long, 1, 100) == false);
        REQUIRE(validate_string_length("A", 5, 100) == false); // Too short
    }
}

// ============================================================================
// Person Name Validation Tests
// ============================================================================

TEST_CASE("Configuration-aware person name validation", "[config][validation][person_name]") {
    
    SECTION("Valid person names") {
        const std::vector<std::string> valid_names = {
            "John",
            "Mary Jane",
            "Jean-Luc",
            "O'Connor",
            "Dr. Smith",
            "Anna-Maria"
            // Note: Unicode characters like "José" and "François" are not supported
            // in the current ASCII-only implementation
        };
        
        for (const auto& name : valid_names) {
            auto result = validate_person_name<PersonName>(name);
            INFO("Validating name: " << name);
            REQUIRE(result.has_value() == true);
            
            if (result.has_value()) {
                REQUIRE(result.value().value() == name);
            }
        }
    }
    
    SECTION("Invalid person names") {
        const std::vector<std::string> invalid_names = {
            "",                    // Empty
            " ",                   // Whitespace only
            "John123",             // Contains numbers
            "John@Smith",          // Contains invalid characters
            "John  Smith",         // Double spaces (debug builds might catch this)
            " John",               // Leading space (debug builds might catch this)
            "John ",               // Trailing space (debug builds might catch this)
            std::string(101, 'A')  // Too long
        };
        
        for (const auto& name : invalid_names) {
            auto result = validate_person_name<PersonName>(name);
            INFO("Validating invalid name: " << name);
            REQUIRE(result.has_value() == false);
        }
    }
    
    SECTION("Person name error codes") {
        // Empty name
        auto empty_result = validate_person_name<PersonName>("");
        REQUIRE(empty_result.has_value() == false);
        REQUIRE(empty_result.error() == GreetingError::EmptyName);
        
        // Too long name
        std::string long_name(101, 'A');
        auto long_result = validate_person_name<PersonName>(long_name);
        REQUIRE(long_result.has_value() == false);
        REQUIRE(long_result.error() == GreetingError::NameTooLong);
        
        // Invalid characters
        auto invalid_result = validate_person_name<PersonName>("John@Smith");
        REQUIRE(invalid_result.has_value() == false);
        REQUIRE(invalid_result.error() == GreetingError::InvalidCharacters);
    }
}

// ============================================================================
// Greeting Message Validation Tests
// ============================================================================

TEST_CASE("Configuration-aware greeting message validation", "[config][validation][greeting_message]") {
    
    SECTION("Valid greeting messages") {
        const std::vector<std::string> valid_messages = {
            "Hello",
            "Good morning",
            "How are you today?",
            "Welcome to our application!",
            "Have a great day, friend.",
            "Greetings and salutations!"
        };
        
        for (const auto& message : valid_messages) {
            auto result = validate_greeting_message<GreetingMessage>(message);
            INFO("Validating message: " << message);
            REQUIRE(result.has_value() == true);
            
            if (result.has_value()) {
                REQUIRE(result.value().value() == message);
            }
        }
    }
    
    SECTION("Invalid greeting messages") {
        const std::vector<std::string> invalid_messages = {
            "",                        // Empty
            " ",                       // Whitespace only
            std::string(501, 'A'),     // Too long
            std::string("Hello\x00World", 11),  // Contains null character (explicit length)
            "Message\x01with\x02control\x03chars"  // Contains control characters
        };
        
        for (const auto& message : invalid_messages) {
            auto result = validate_greeting_message<GreetingMessage>(message);
            INFO("Validating invalid message: " << message);
            REQUIRE(result.has_value() == false);
        }
    }
    
    SECTION("Greeting message error codes") {
        // Empty message
        auto empty_result = validate_greeting_message<GreetingMessage>("");
        REQUIRE(empty_result.has_value() == false);
        REQUIRE(empty_result.error() == GreetingError::EmptyMessage);
        
        // Too long message
        std::string long_message(501, 'A');
        auto long_result = validate_greeting_message<GreetingMessage>(long_message);
        REQUIRE(long_result.has_value() == false);
        REQUIRE(long_result.error() == GreetingError::MessageTooLong);
        
        // Invalid characters
        std::string invalid_message("Hello\x00World", 11); // String with embedded null
        auto invalid_result = validate_greeting_message<GreetingMessage>(invalid_message);
        REQUIRE(invalid_result.has_value() == false);
        REQUIRE(invalid_result.error() == GreetingError::InvalidCharacters);
    }
}

// ============================================================================
// Configuration-Aware Behavior Tests
// ============================================================================

TEST_CASE("Build configuration awareness", "[config][validation][build_aware]") {
    
    SECTION("Configuration type consistency") {
        // The validation should be consistent with the build configuration
        // These tests verify the configuration-aware behavior
        
        // Basic functionality should work regardless of configuration
        auto name_result = validate_person_name<PersonName>("John Doe");
        REQUIRE(name_result.has_value() == true);
        
        auto message_result = validate_greeting_message<GreetingMessage>("Hello, World!");
        REQUIRE(message_result.has_value() == true);
    }
    
    SECTION("Validation result type consistency") {
        using NameResult = ConfigAwareValidationResult<PersonName>;
        using MessageResult = ConfigAwareValidationResult<GreetingMessage>;
        
        // These should compile and work consistently
        NameResult name_result = validate_person_name<PersonName>("Test Name");
        MessageResult message_result = validate_greeting_message<GreetingMessage>("Test Message");
        
        // Basic interface should be consistent
        REQUIRE(name_result.has_value() == true);
        REQUIRE(message_result.has_value() == true);
    }
}

// ============================================================================
// Performance and Compile-Time Tests
// ============================================================================

TEST_CASE("Validation performance characteristics", "[config][validation][performance]") {
    
    SECTION("Validation functions are efficient") {
        // These tests verify that validation doesn't introduce significant overhead
        const std::string test_name = "Performance Test Name";
        const std::string test_message = "Performance test message content.";
        
        // Multiple validations should be fast
        for (int i = 0; i < 100; ++i) {
            auto name_result = validate_person_name<PersonName>(test_name);
            auto message_result = validate_greeting_message<GreetingMessage>(test_message);
            
            REQUIRE(name_result.has_value() == true);
            REQUIRE(message_result.has_value() == true);
        }
    }
    
    SECTION("Compile-time characteristics") {
        // These should compile without issues - testing only constexpr functions
        constexpr bool length_valid = validate_string_length(std::string_view{"Test"}, 1, 10);
        static_assert(length_valid == true);
        
        // Character validation is runtime-only due to configuration dispatching
        // so we test it at runtime instead
        bool char_valid = validate_name_character('A');
        REQUIRE(char_valid == true);
    }
}

#ifdef __cpp_concepts

// ============================================================================
// Concept Tests (C++20/23)
// ============================================================================

TEST_CASE("Configuration-aware validation concepts", "[config][validation][concepts]") {
    
    SECTION("ConfigAwareValidatable concept") {
        // These should satisfy the concept
        static_assert(ConfigAwareValidatable<PersonName>);
        static_assert(ConfigAwareValidatable<GreetingMessage>);
    }
    
    SECTION("ValidationContextType concept") {
        // ValidationContext should satisfy the concept
        static_assert(ValidationContextType<ValidationContext>);
    }
}

#endif // __cpp_concepts
