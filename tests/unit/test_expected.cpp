#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "greeting/types.hpp"

using namespace greeting;
using Catch::Matchers::Equals;

SCENARIO("Expected type behavior", "[Expected][result]") {
    
    GIVEN("an Expected with a successful value") {
        Expected<int, std::string> successResult{42};
        
        WHEN("checking if it has a value") {
            THEN("has_value returns true") {
                REQUIRE(successResult.has_value());
                REQUIRE(static_cast<bool>(successResult));
            }
        }
        
        WHEN("accessing the value") {
            THEN("value() returns the correct value") {
                REQUIRE(successResult.value() == 42);
                REQUIRE(*successResult == 42);
            }
        }
        
        WHEN("trying to access error") {
            THEN("error() throws") {
                REQUIRE_THROWS_AS(successResult.error(), std::bad_variant_access);
            }
        }
    }
    
    GIVEN("an Expected with an error") {
        Expected<int, std::string> errorResult{std::string{"test error"}};
        
        WHEN("checking if it has a value") {
            THEN("has_value returns false") {
                REQUIRE_FALSE(errorResult.has_value());
                REQUIRE_FALSE(static_cast<bool>(errorResult));
            }
        }
        
        WHEN("accessing the error") {
            THEN("error() returns the correct error") {
                REQUIRE_THAT(errorResult.error(), Equals("test error"));
            }
        }
        
        WHEN("trying to access value") {
            THEN("value() throws") {
                REQUIRE_THROWS_AS(errorResult.value(), std::bad_variant_access);
            }
        }
    }
}

SCENARIO("Result type alias behavior", "[Result][alias]") {
    
    GIVEN("a successful Result") {
        Result<std::string> successResult{std::string{"success"}};
        
        WHEN("checking the result") {
            THEN("it behaves like Expected") {
                REQUIRE(successResult.has_value());
                REQUIRE_THAT(successResult.value(), Equals("success"));
            }
        }
    }
    
    GIVEN("a failed Result") {
        Result<std::string> errorResult{GreetingError::ConfigurationError};
        
        WHEN("checking the result") {
            THEN("it contains the correct error") {
                REQUIRE_FALSE(errorResult.has_value());
                REQUIRE(errorResult.error() == GreetingError::ConfigurationError);
            }
        }
    }
}

SCENARIO("Expected move semantics", "[Expected][move]") {
    
    GIVEN("an Expected with a moveable type") {
        Expected<std::string, GreetingError> result{std::string{"moveable string"}};
        
        WHEN("moving the value out") {
            auto moved = std::move(result).value();
            
            THEN("the value is properly moved") {
                REQUIRE_THAT(moved, Equals("moveable string"));
                // Note: result is now in moved-from state, testing this would be UB
            }
        }
    }
}

SCENARIO("Expected with complex types", "[Expected][complex]") {
    
    GIVEN("an Expected containing a PersonName") {
        auto nameResult = PersonName::create("Complex Test");
        
        WHEN("the PersonName creation succeeds") {
            THEN("Result contains a valid PersonName") {
                REQUIRE(nameResult.has_value());
                REQUIRE_THAT(nameResult->value(), Equals("Complex Test"));
                REQUIRE(nameResult->length() == 12);
            }
        }
    }
    
    GIVEN("an Expected containing a GreetingMessage") {
        auto messageResult = GreetingMessage::create("Complex Message Test");
        
        WHEN("the GreetingMessage creation succeeds") {
            THEN("Result contains a valid GreetingMessage") {
                REQUIRE(messageResult.has_value());
                REQUIRE_THAT(messageResult->value(), Equals("Complex Message Test"));
                REQUIRE_FALSE(messageResult->empty());
            }
        }
    }
}
