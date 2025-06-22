/**
 * @file test_casual_strategy.cpp
 * @brief Unit tests for casual greeting strategy
 * 
 * Tests casual greeting generation, validation, and configuration-aware behavior.
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <set>

#include "greeting/casual_strategy.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"
#include "config/build_config.hpp"

using namespace greeting;
using namespace greeting::strategy;
using namespace Catch::Matchers;

TEST_CASE("CasualStrategy Creation", "[strategy][casual][creation]") {
    SECTION("Factory method creates valid strategy") {
        auto strategy = CasualStrategy::create();
        
        REQUIRE(strategy != nullptr);
        REQUIRE(strategy->isValid());
        REQUIRE(strategy->getStrategyName() == "casual");
    }
    
    SECTION("Strategy name is correct") {
        REQUIRE(CasualStrategy::strategy_name() == "casual");
    }
}

TEST_CASE("CasualStrategy Basic Greeting Generation", "[strategy][casual][greet]") {
    auto strategy = CasualStrategy::create();
    
    SECTION("Generate greeting for simple name") {
        auto person_result = PersonName::create("John");
        REQUIRE(person_result.has_value());
        
        auto greeting_result = strategy->greet(person_result.value());
        REQUIRE(greeting_result.has_value());
        
        const auto greeting = greeting_result.value().value();
        // Should contain casual prefix and name
        REQUIRE_THAT(greeting, ContainsSubstring("John"));
        // In debug mode greetings may have variations; in release mode they end with "!"
        if constexpr (greeting::config::is_debug_build()) {
            // Debug mode: May have additional suffixes
            REQUIRE(greeting.find("John") != std::string::npos);
        } else {
            // Release mode: Should end with "!"
            REQUIRE(greeting.ends_with("!"));
        }
    }
    
    SECTION("Generate greeting for compound name uses first name only") {
        auto person_result = PersonName::create("John Doe");
        REQUIRE(person_result.has_value());
        
        auto greeting_result = strategy->greet(person_result.value());
        REQUIRE(greeting_result.has_value());
        
        const auto greeting = greeting_result.value().value();
        // Should contain only first name in casual greeting
        REQUIRE_THAT(greeting, ContainsSubstring("John"));
        REQUIRE_THAT(greeting, !ContainsSubstring("Doe"));
    }
    
    SECTION("Generate greeting with various casual prefixes") {
        auto person_result = PersonName::create("Alice");
        REQUIRE(person_result.has_value());
        
        std::set<std::string> observed_prefixes;
        
        // Generate multiple greetings to test randomness
        for (int i = 0; i < 50; ++i) {
            auto greeting_result = strategy->greet(person_result.value());
            REQUIRE(greeting_result.has_value());
            
            const auto greeting = greeting_result.value().value();
            // Extract prefix (everything before the comma)
            const auto comma_pos = greeting.find(',');
            if (comma_pos != std::string::npos) {
                observed_prefixes.insert(greeting.substr(0, comma_pos));
            }
        }
        
        // Should have observed multiple different prefixes
        REQUIRE(observed_prefixes.size() >= 2);
    }
}

TEST_CASE("CasualStrategy Name Validation", "[strategy][casual][validation]") {
    auto strategy = CasualStrategy::create();
    
    SECTION("Validates normal names") {
        auto result = strategy->validateCasualName("John");
        REQUIRE(result.has_value());
        REQUIRE(result.value().value() == "John");
    }
    
    SECTION("Validates very short names (relaxed constraint)") {
        auto result = strategy->validateCasualName("A");
        REQUIRE(result.has_value());
        REQUIRE(result.value().value() == "A");
    }
    
    SECTION("Validates names with casual characters") {
        // Underscore is allowed in casual mode
        auto result = strategy->validateCasualName("Jo_Ann");
        REQUIRE(result.has_value());
        REQUIRE(result.value().value() == "Jo_Ann");
    }
    
    SECTION("Rejects empty names") {
        auto result = strategy->validateCasualName("");
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == GreetingError::EmptyName);
    }
    
    SECTION("Rejects whitespace-only names") {
        auto result = strategy->validateCasualName("   ");
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == GreetingError::EmptyName);
    }
    
    SECTION("Rejects names that are too long") {
        std::string long_name(101, 'a'); // 101 characters
        auto result = strategy->validateCasualName(long_name);
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == GreetingError::NameTooLong);
    }
    
    SECTION("Rejects names with invalid characters") {
        auto result = strategy->validateCasualName("John@Doe");
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == GreetingError::InvalidName);
    }
}

TEST_CASE("CasualStrategy End-to-End Greeting Generation", "[strategy][casual][e2e]") {
    auto strategy = CasualStrategy::create();
    
    SECTION("Generate casual greeting from raw name") {
        auto result = strategy->generateCasualGreeting("Bob");
        REQUIRE(result.has_value());
        
        const auto greeting = result.value().value();
        REQUIRE_THAT(greeting, ContainsSubstring("Bob"));
        // In debug mode greetings may have variations; in release mode they end with "!"
        if constexpr (greeting::config::is_debug_build()) {
            // Debug mode: May have additional suffixes
            REQUIRE(greeting.find("Bob") != std::string::npos);
        } else {
            // Release mode: Should end with "!"
            REQUIRE(greeting.ends_with("!"));
        }
    }
    
    SECTION("Handle validation errors in end-to-end flow") {
        auto result = strategy->generateCasualGreeting("");
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == GreetingError::EmptyName);
    }
    
    SECTION("Handle very short names in casual mode") {
        auto result = strategy->generateCasualGreeting("X");
        REQUIRE(result.has_value());
        
        const auto greeting = result.value().value();
        REQUIRE_THAT(greeting, ContainsSubstring("X"));
    }
}

TEST_CASE("CasualStrategy Configuration Awareness", "[strategy][casual][config]") {
    auto strategy = CasualStrategy::create();
    
    SECTION("Strategy reports correct configuration") {
        REQUIRE(strategy->isValid());
        REQUIRE(strategy->getStrategyName() == "casual");
    }
    
    SECTION("Handles names with relaxed validation") {
        // Test that casual strategy is more permissive than formal
        auto result1 = strategy->validateCasualName("J");  // Very short
        auto result2 = strategy->validateCasualName("Jo_Ann");  // With underscore
        
        REQUIRE(result1.has_value());
        REQUIRE(result2.has_value());
    }
}

TEST_CASE("CasualStrategy Error Handling", "[strategy][casual][errors]") {
    auto strategy = CasualStrategy::create();
    
    SECTION("Handles empty person name in greeting") {
        // Create a PersonName with empty value (using internal tag)
        PersonName empty_person{std::string{}, PersonName::InternalTag{}};
        
        auto result = strategy->greet(empty_person);
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == GreetingError::EmptyName);
    }
    
    SECTION("Configuration-aware error creation") {
        auto result = strategy->validateCasualName("John@Doe");
        REQUIRE(!result.has_value());
        // Should return appropriate error for invalid characters
        REQUIRE(result.error() == GreetingError::InvalidName);
    }
}

TEST_CASE("CasualStrategy Traits and Type Information", "[strategy][casual][traits]") {
    using traits = greeting::strategy::traits::StrategyTraits<CasualStrategy>;
    
    SECTION("Traits configuration") {
        REQUIRE(traits::supports_debug_info == true);
        REQUIRE(traits::expected_message_length == 50);
        REQUIRE(traits::is_thread_safe == false); // Due to random state
        REQUIRE(traits::supports_async_operation == true);
        REQUIRE(traits::requires_system_resources == false);
        REQUIRE(traits::supports_localization == true);
        REQUIRE(traits::supports_cultural_context == false);
        REQUIRE(traits::supports_variations == true);
        REQUIRE(traits::supports_nicknames == true);
        REQUIRE(traits::min_name_length == 1);
        REQUIRE(traits::max_name_length == 100);
    }
}

TEST_CASE("CasualStrategy Greeting Variations", "[strategy][casual][variations]") {
    auto strategy = CasualStrategy::create();
    
    SECTION("Casual prefixes are used") {
        auto person_result = PersonName::create("Test");
        REQUIRE(person_result.has_value());
        
        std::set<std::string> prefixes_seen;
        
        // Generate many greetings to test prefix variation
        for (int i = 0; i < 100; ++i) {
            auto greeting_result = strategy->greet(person_result.value());
            REQUIRE(greeting_result.has_value());
            
            const auto greeting = greeting_result.value().value();
            const auto comma_pos = greeting.find(',');
            if (comma_pos != std::string::npos) {
                prefixes_seen.insert(greeting.substr(0, comma_pos));
            }
        }
        
        // Should see multiple casual prefixes
        REQUIRE(prefixes_seen.size() >= 3);
        
        // Check for expected casual prefixes
        std::set<std::string> expected_prefixes = {"Hi", "Hey", "Hello", "What's up", "Howdy", "Yo"};
        for (const auto& seen : prefixes_seen) {
            REQUIRE(expected_prefixes.count(seen) > 0);
        }
    }
}

TEST_CASE("CasualStrategy Random Behavior", "[strategy][casual][random]") {
    auto strategy1 = CasualStrategy::create();
    auto strategy2 = CasualStrategy::create();
    
    SECTION("Different strategy instances can produce different greetings") {
        auto person_result = PersonName::create("RandomTest");
        REQUIRE(person_result.has_value());
        
        std::set<std::string> greetings_from_strategy1;
        std::set<std::string> greetings_from_strategy2;
        
        // Generate multiple greetings from each strategy
        for (int i = 0; i < 20; ++i) {
            auto greeting1 = strategy1->greet(person_result.value());
            auto greeting2 = strategy2->greet(person_result.value());
            
            REQUIRE(greeting1.has_value());
            REQUIRE(greeting2.has_value());
            
            greetings_from_strategy1.insert(greeting1.value().value());
            greetings_from_strategy2.insert(greeting2.value().value());
        }
        
        // Each strategy should produce some variation
        REQUIRE(greetings_from_strategy1.size() >= 2);
        REQUIRE(greetings_from_strategy2.size() >= 2);
    }
}
