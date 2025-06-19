#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "greeting/types.hpp"

using namespace greeting;
using Catch::Matchers::Equals;
using Catch::Matchers::ContainsSubstring;

SCENARIO("GreetingError enumeration", "[GreetingError][enum]") {
    
    GIVEN("all GreetingError values") {
        
        WHEN("converting errors to strings") {
            THEN("each error has a descriptive message") {
                REQUIRE_THAT(std::string{toString(GreetingError::EmptyName)}, 
                           ContainsSubstring("empty"));
                REQUIRE_THAT(std::string{toString(GreetingError::InvalidName)}, 
                           ContainsSubstring("invalid characters"));
                REQUIRE_THAT(std::string{toString(GreetingError::EmptyMessage)}, 
                           ContainsSubstring("empty"));
                REQUIRE_THAT(std::string{toString(GreetingError::InvalidMessage)}, 
                           ContainsSubstring("invalid"));
                REQUIRE_THAT(std::string{toString(GreetingError::ConfigurationError)}, 
                           ContainsSubstring("Configuration"));
                REQUIRE_THAT(std::string{toString(GreetingError::OutputError)}, 
                           ContainsSubstring("output"));
            }
        }
    }
}

SCENARIO("Error category integration", "[GreetingError][category]") {
    
    GIVEN("GreetingError values") {
        
        WHEN("creating error codes") {
            auto emptyNameCode = make_error_code(GreetingError::EmptyName);
            auto invalidNameCode = make_error_code(GreetingError::InvalidName);
            
            THEN("error codes have correct properties") {
                REQUIRE_THAT(emptyNameCode.category().name(), Equals("greeting"));
                REQUIRE_THAT(emptyNameCode.message(), 
                           ContainsSubstring("empty"));
                REQUIRE_THAT(invalidNameCode.message(), 
                           ContainsSubstring("invalid characters"));
            }
        }
        
        WHEN("comparing error codes") {
            auto code1 = make_error_code(GreetingError::EmptyName);
            auto code2 = make_error_code(GreetingError::EmptyName);
            auto code3 = make_error_code(GreetingError::InvalidName);
            
            THEN("same errors compare equal") {
                REQUIRE(code1 == code2);
                REQUIRE(code1 != code3);
            }
        }
    }
}

SCENARIO("Error category singleton behavior", "[GreetingError][singleton]") {
    
    GIVEN("multiple requests for error category") {
        
        WHEN("getting category instances") {
            const auto& cat1 = greetingErrorCategory();
            const auto& cat2 = greetingErrorCategory();
            
            THEN("same instance is returned") {
                REQUIRE(&cat1 == &cat2);
                REQUIRE_THAT(cat1.name(), Equals("greeting"));
            }
        }
    }
}

SCENARIO("ResultWithCode type alias", "[ResultWithCode][error_code]") {
    
    GIVEN("a ResultWithCode with success") {
        ResultWithCode<std::string> successResult{std::string{"success"}};
        
        WHEN("checking the result") {
            THEN("it contains the value") {
                REQUIRE(successResult.has_value());
                REQUIRE_THAT(successResult.value(), Equals("success"));
            }
        }
    }
    
    GIVEN("a ResultWithCode with error_code") {
        auto errorCode = make_error_code(GreetingError::ConfigurationError);
        ResultWithCode<std::string> errorResult{errorCode};
        
        WHEN("checking the result") {
            THEN("it contains the error code") {
                REQUIRE_FALSE(errorResult.has_value());
                REQUIRE(errorResult.error() == errorCode);
                REQUIRE_THAT(errorResult.error().message(), 
                           ContainsSubstring("Configuration"));
            }
        }
    }
}

TEST_CASE("toString function is constexpr", "[GreetingError][constexpr]") {
    
    SECTION("compile-time evaluation") {
        constexpr auto emptyNameMsg = toString(GreetingError::EmptyName);
        constexpr auto invalidNameMsg = toString(GreetingError::InvalidName);
        
        // These should compile - verifying constexpr evaluation
        static_assert(emptyNameMsg.size() > 0);
        static_assert(invalidNameMsg.size() > 0);
        static_assert(emptyNameMsg != invalidNameMsg);
        
        REQUIRE(emptyNameMsg.size() > 0);
        REQUIRE(invalidNameMsg.size() > 0);
    }
}
