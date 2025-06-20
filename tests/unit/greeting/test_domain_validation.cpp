#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "greeting/validation.hpp"

using namespace greeting;
using Catch::Matchers::Equals;

SCENARIO("Domain validation functions", "[validation][domain]") {
    
    GIVEN("length validation function") {
        
        WHEN("validating string within bounds") {
            auto result = validateLength("Hello", 1, 10);
            
            THEN("validation succeeds") {
                REQUIRE_FALSE(result.has_value());
            }
        }
        
        WHEN("validating string too short") {
            auto result = validateLength("", 1, 10);
            
            THEN("NameTooShort error is returned") {
                REQUIRE(result.has_value());
                REQUIRE(result.value() == GreetingError::NameTooShort);
            }
        }
        
        WHEN("validating string too long") {
            std::string longString(101, 'a');
            auto result = validateLength(longString, 1, 100);
            
            THEN("NameTooLong error is returned") {
                REQUIRE(result.has_value());
                REQUIRE(result.value() == GreetingError::NameTooLong);
            }
        }
    }
    
    GIVEN("character validation function") {
        
        WHEN("validating valid characters") {
            auto result = validateCharacters("John Doe-Smith");
            
            THEN("validation succeeds") {
                REQUIRE_FALSE(result.has_value());
            }
        }
        
        WHEN("validating invalid characters") {
            auto result = validateCharacters("John@Domain");
            
            THEN("InvalidCharacters error is returned") {
                REQUIRE(result.has_value());
                REQUIRE(result.value() == GreetingError::InvalidCharacters);
            }
        }
    }
    
    GIVEN("person name validation function") {
        
        WHEN("validating valid person name") {
            auto result = validatePersonName("Alice Johnson");
            
            THEN("validation succeeds") {
                REQUIRE_FALSE(result.has_value());
            }
        }
        
        WHEN("validating empty person name") {
            auto result = validatePersonName("");
            
            THEN("EmptyName error is returned") {
                REQUIRE(result.has_value());
                REQUIRE(result.value() == GreetingError::EmptyName);
            }
        }
        
        WHEN("validating whitespace-only person name") {
            auto result = validatePersonName("   \t  ");
            
            THEN("EmptyName error is returned") {
                REQUIRE(result.has_value());
                REQUIRE(result.value() == GreetingError::EmptyName);
            }
        }
    }
    
    GIVEN("greeting message validation function") {
        
        WHEN("validating valid greeting message") {
            auto result = validateGreetingMessage("Hello, World!");
            
            THEN("validation succeeds") {
                REQUIRE_FALSE(result.has_value());
            }
        }
        
        WHEN("validating empty greeting message") {
            auto result = validateGreetingMessage("");
            
            THEN("EmptyMessage error is returned") {
                REQUIRE(result.has_value());
                REQUIRE(result.value() == GreetingError::EmptyMessage);
            }
        }
        
        WHEN("validating message with punctuation") {
            auto result = validateGreetingMessage("Hello, world! How are you?");
            
            THEN("validation succeeds") {
                REQUIRE_FALSE(result.has_value());
            }
        }
    }
}
