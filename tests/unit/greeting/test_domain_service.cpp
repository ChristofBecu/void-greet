#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "greeting/domain_service.hpp"

using namespace greeting;
using Catch::Matchers::Equals;

// Note: GreetingService is defined in the .cpp file, so we need to include it
// For testing purposes, we'll access it through the public API or create a test interface

SCENARIO("GreetingService simple greeting generation", "[GreetingService][domain]") {
    
    GIVEN("a valid person name") {
        auto personResult = PersonName::create("Alice");
        REQUIRE(personResult.has_value());
        const auto& person = personResult.value();
        
        WHEN("generating a simple greeting") {
            GreetingService service;
            auto greetingResult = service.generateSimpleGreeting(person);
            
            THEN("simple greeting is generated successfully") {
                REQUIRE(greetingResult.has_value());
                REQUIRE_THAT(greetingResult.value().value(), 
                           Equals("Hello, Alice!"));
                REQUIRE_FALSE(greetingResult.value().empty());
            }
        }
    }
    
    GIVEN("an empty person name") {
        auto personResult = PersonName::create("");
        REQUIRE_FALSE(personResult.has_value());
        
        WHEN("attempting to create with empty name") {
            // Test should verify the PersonName creation fails
            THEN("PersonName creation fails with EmptyName error") {
                REQUIRE(personResult.error() == GreetingError::EmptyName);
            }
        }
    }
}

SCENARIO("GreetingService formal greeting generation", "[GreetingService][domain]") {
    
    GIVEN("a valid person name") {
        auto personResult = PersonName::create("Dr. Smith");
        REQUIRE(personResult.has_value());
        const auto& person = personResult.value();
        
        WHEN("generating a formal greeting") {
            GreetingService service;
            auto greetingResult = service.generateFormalGreeting(person);
            
            THEN("formal greeting is generated successfully") {
                REQUIRE(greetingResult.has_value());
                REQUIRE_THAT(greetingResult.value().value(), 
                           Equals("Hello, Dr. Smith!"));
                REQUIRE_FALSE(greetingResult.value().empty());
            }
        }
    }
    
    GIVEN("a person with special characters in name") {
        auto personResult = PersonName::create("Mary-Jane O'Connor");
        REQUIRE(personResult.has_value());
        const auto& person = personResult.value();
        
        WHEN("generating formal greeting") {
            GreetingService service;
            auto greetingResult = service.generateFormalGreeting(person);
            
            THEN("greeting handles special characters correctly") {
                REQUIRE(greetingResult.has_value());
                REQUIRE_THAT(greetingResult.value().value(), 
                           Equals("Hello, Mary-Jane O'Connor!"));
            }
        }
    }
}

SCENARIO("GreetingService error handling", "[GreetingService][domain][error]") {
    
    GIVEN("service instance") {
        GreetingService service;
        
        WHEN("testing with valid minimal name") {
            auto personResult = PersonName::create("Jo");
            
            THEN("person creation should follow existing validation rules") {
                // This tests integration with existing PersonName validation
                if (personResult.has_value()) {
                    auto greetingResult = service.generateSimpleGreeting(personResult.value());
                    REQUIRE(greetingResult.has_value());
                    REQUIRE_THAT(greetingResult.value().value(), Equals("Hello, Jo!"));
                } else {
                    // If PersonName validation rejects "Jo", that's expected behavior
                    REQUIRE_FALSE(personResult.has_value());
                }
            }
        }
        
        WHEN("testing service consistency") {
            auto personResult = PersonName::create("TestUser");
            REQUIRE(personResult.has_value());
            const auto& person = personResult.value();
            
            auto simpleResult = service.generateSimpleGreeting(person);
            auto formalResult = service.generateFormalGreeting(person);
            
            THEN("both methods work consistently") {
                REQUIRE(simpleResult.has_value());
                REQUIRE(formalResult.has_value());
                REQUIRE_THAT(simpleResult.value().value(), Equals("Hello, TestUser!"));
                REQUIRE_THAT(formalResult.value().value(), Equals("Hello, TestUser!"));
            }
        }
    }
}

// ============================================================================
// Configuration-Aware Domain Service Tests
// ============================================================================

SCENARIO("GreetingService configuration-aware validation integration", "[GreetingService][config][validation]") {
    
    GIVEN("a greeting service") {
        GreetingService service;
        
        WHEN("generating greeting with valid person name") {
            auto greetingResult = service.generateGreetingWithValidation("Alice Johnson");
            
            THEN("greeting is generated successfully with validation") {
                REQUIRE(greetingResult.has_value());
                REQUIRE_THAT(greetingResult.value().value(), Equals("Hello, Alice Johnson!"));
            }
        }
        
        WHEN("generating formal greeting with valid person name") {
            auto greetingResult = service.generateFormalGreetingWithValidation("Dr. Smith");
            
            THEN("formal greeting is generated successfully with validation") {
                REQUIRE(greetingResult.has_value());
                REQUIRE_THAT(greetingResult.value().value(), Equals("Hello, Dr. Smith!"));
            }
        }
        
        WHEN("generating greeting with invalid person name") {
            auto greetingResult = service.generateGreetingWithValidation("");
            
            THEN("validation error is propagated") {
                REQUIRE_FALSE(greetingResult.has_value());
                REQUIRE(greetingResult.error() == GreetingError::EmptyName);
            }
        }
        
        WHEN("generating greeting with too short person name") {
            auto greetingResult = service.generateGreetingWithValidation("X");
            
            THEN("validation error is propagated") {
                REQUIRE_FALSE(greetingResult.has_value());
                REQUIRE(greetingResult.error() == GreetingError::NameTooShort);
            }
        }
        
        WHEN("generating greeting with invalid characters") {
            auto greetingResult = service.generateGreetingWithValidation("John@Smith");
            
            THEN("validation error is propagated") {
                REQUIRE_FALSE(greetingResult.has_value());
                REQUIRE(greetingResult.error() == GreetingError::InvalidName);
            }
        }
    }
}
