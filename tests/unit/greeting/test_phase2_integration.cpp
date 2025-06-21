#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"
#include "greeting/domain_service.hpp"
#include "greeting/config_aware_validation.hpp"
#include "config/build_config.hpp"

using namespace greeting;
using Catch::Matchers::Equals;

TEST_CASE("Phase 2.3: Configuration-aware domain type integration", "[phase2][integration][domain]") {
    
    SECTION("PersonName uses configuration-aware validation") {
        // Test that PersonName::create() uses the new validation system
        auto valid_result = PersonName::create("Alice Smith");
        REQUIRE(valid_result.has_value());
        REQUIRE_THAT(valid_result.value().value(), Equals("Alice Smith"));
        
        // Test validation error behavior
        auto empty_result = PersonName::create("");
        REQUIRE_FALSE(empty_result.has_value());
        REQUIRE(empty_result.error() == GreetingError::EmptyName);
        
        // Test whitespace-only names
        auto whitespace_result = PersonName::create("   ");
        REQUIRE_FALSE(whitespace_result.has_value());
        REQUIRE(whitespace_result.error() == GreetingError::EmptyName);
    }
    
    SECTION("GreetingMessage uses configuration-aware validation") {
        // Test that GreetingMessage::create() uses the new validation system
        auto valid_result = GreetingMessage::create("Hello, World!");
        REQUIRE(valid_result.has_value());
        REQUIRE_THAT(valid_result.value().value(), Equals("Hello, World!"));
        
        // Test validation error behavior
        auto empty_result = GreetingMessage::create("");
        REQUIRE_FALSE(empty_result.has_value());
        REQUIRE(empty_result.error() == GreetingError::EmptyMessage);
        
        // Test whitespace-only messages
        auto whitespace_result = GreetingMessage::create("   ");
        REQUIRE_FALSE(whitespace_result.has_value());
        REQUIRE(whitespace_result.error() == GreetingError::EmptyMessage);
    }
    
    SECTION("GreetingService integrates with configuration-aware types") {
        GreetingService service;
        
        // Test successful greeting generation
        auto person = PersonName::create("Bob").value();
        auto greeting = service.generateSimpleGreeting(person);
        REQUIRE(greeting.has_value());
        REQUIRE_THAT(greeting.value().value(), Equals("Hello, Bob!"));
        
        // Test validation methods work with new system
        auto validation_result = service.generateGreetingWithValidation("Charlie");
        REQUIRE(validation_result.has_value());
        REQUIRE_THAT(validation_result.value().value(), Equals("Hello, Charlie!"));
        
        auto invalid_validation = service.generateGreetingWithValidation("");
        REQUIRE_FALSE(invalid_validation.has_value());
    }
    
    SECTION("Configuration-aware validation functions work correctly") {
        // Test direct use of configuration-aware validation
        auto name_result = validation::validate_person_name<PersonName>("David Wilson");
        REQUIRE(name_result.has_value());
        REQUIRE_THAT(name_result.value().value(), Equals("David Wilson"));
        
        auto message_result = validation::validate_greeting_message<GreetingMessage>("Welcome!");
        REQUIRE(message_result.has_value());
        REQUIRE_THAT(message_result.value().value(), Equals("Welcome!"));
        
        // Test error cases
        auto invalid_name = validation::validate_person_name<PersonName>("");
        REQUIRE_FALSE(invalid_name.has_value());
        REQUIRE(invalid_name.error() == GreetingError::EmptyName);
    }
}

TEST_CASE("Phase 2.3: Backward compatibility verification", "[phase2][compatibility]") {
    
    SECTION("All existing APIs remain unchanged") {
        // Test that all public APIs work exactly as before
        
        // PersonName API compatibility
        auto person = PersonName::create("Test User");
        REQUIRE(person.has_value());
        REQUIRE_THAT(person.value().value(), Equals("Test User"));
        REQUIRE(person.value().view() == "Test User");
        REQUIRE(person.value().length() == 9);
        REQUIRE_FALSE(person.value().empty());
        
        // GreetingMessage API compatibility
        auto message = GreetingMessage::create("Test Message");
        REQUIRE(message.has_value());
        REQUIRE_THAT(message.value().value(), Equals("Test Message"));
        REQUIRE(message.value().view() == "Test Message");
        REQUIRE(message.value().length() == 12);
        REQUIRE_FALSE(message.value().empty());
        
        // GreetingService API compatibility
        GreetingService service;
        auto greeting = service.generateSimpleGreeting(person.value());
        REQUIRE(greeting.has_value());
        REQUIRE_THAT(greeting.value().value(), Equals("Hello, Test User!"));
        
        // Helper functions compatibility
        auto helper_person = makePersonName("Helper Test");
        REQUIRE(helper_person.has_value());
        
        auto helper_message = makeGreetingMessage("Helper Message");
        REQUIRE(helper_message.has_value());
    }
    
    SECTION("Error codes remain consistent") {
        // Verify that error codes haven't changed
        auto empty_name = PersonName::create("");
        REQUIRE_FALSE(empty_name.has_value());
        REQUIRE(empty_name.error() == GreetingError::EmptyName);
        
        auto short_name = PersonName::create("X");
        REQUIRE_FALSE(short_name.has_value());
        REQUIRE(short_name.error() == GreetingError::NameTooShort);
        
        auto invalid_name = PersonName::create("Test@User");
        REQUIRE_FALSE(invalid_name.has_value());
        REQUIRE(invalid_name.error() == GreetingError::InvalidName);
        
        auto empty_message = GreetingMessage::create("");
        REQUIRE_FALSE(empty_message.has_value());
        REQUIRE(empty_message.error() == GreetingError::EmptyMessage);
    }
}
