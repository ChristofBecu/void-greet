#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

#include "greeting/types.hpp"

using namespace greeting;
using Catch::Matchers::Equals;
using Catch::Matchers::ContainsSubstring;

SCENARIO("PersonName creation and validation", "[PersonName][validation]") {
    
    GIVEN("a valid person name") {
        const std::string validName = "John Doe";
        
        WHEN("creating PersonName with valid input") {
            auto result = PersonName::create(validName);
            
            THEN("creation succeeds") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), Equals(validName));
                REQUIRE_THAT(std::string{result.value().view()}, Equals(validName));
                REQUIRE(result.value().length() == validName.length());
                REQUIRE_FALSE(result.value().empty());
            }
        }
    }
    
    GIVEN("various invalid person names") {
        
        WHEN("creating PersonName with empty string") {
            auto result = PersonName::create("");
            
            THEN("creation fails with EmptyName error") {
                REQUIRE_FALSE(result.has_value());
                REQUIRE(result.error() == GreetingError::EmptyName);
                REQUIRE_THAT(std::string{toString(result.error())}, 
                           ContainsSubstring("cannot be empty"));
            }
        }
        
        WHEN("creating PersonName with whitespace-only string") {
            auto result = PersonName::create("   \t\n  ");
            
            THEN("creation fails with EmptyName error") {
                REQUIRE_FALSE(result.has_value());
                REQUIRE(result.error() == GreetingError::EmptyName);
            }
        }
        
        WHEN("creating PersonName with invalid characters") {
            auto result = PersonName::create("John@Doe#123");
            
            THEN("creation fails with InvalidName error") {
                REQUIRE_FALSE(result.has_value());
                REQUIRE(result.error() == GreetingError::InvalidName);
                REQUIRE_THAT(std::string{toString(result.error())}, 
                           ContainsSubstring("invalid characters"));
            }
        }
    }
    
    GIVEN("edge cases for person names") {
        
        WHEN("creating PersonName with valid special characters") {
            auto result1 = PersonName::create("Mary-Jane O'Connor");
            auto result2 = PersonName::create("Dr. Smith");
            
            THEN("creation succeeds") {
                REQUIRE(result1.has_value());
                REQUIRE(result2.has_value());
                REQUIRE_THAT(result1.value().value(), Equals("Mary-Jane O'Connor"));
                REQUIRE_THAT(result2.value().value(), Equals("Dr. Smith"));
            }
        }
        
        WHEN("creating PersonName with single character") {
            auto result = PersonName::create("X");
            
            THEN("creation fails with NameTooShort error") {
                REQUIRE_FALSE(result.has_value());
                REQUIRE(result.error() == GreetingError::NameTooShort);
            }
        }
    }
}

SCENARIO("PersonName comparison operations", "[PersonName][comparison]") {
    
    GIVEN("two PersonName instances") {
        auto name1 = PersonName::create("Alice").value();
        auto name2 = PersonName::create("Alice").value();
        auto name3 = PersonName::create("Bob").value();
        
        WHEN("comparing equal names") {
            THEN("equality operators work correctly") {
                REQUIRE(name1 == name2);
                REQUIRE_FALSE(name1 != name2);
            }
        }
        
        WHEN("comparing different names") {
            THEN("inequality operators work correctly") {
                REQUIRE(name1 != name3);
                REQUIRE_FALSE(name1 == name3);
                REQUIRE(name1 < name3);  // Alice < Bob alphabetically
            }
        }
    }
}

SCENARIO("GreetingMessage creation and validation", "[GreetingMessage][validation]") {
    
    GIVEN("a valid greeting message") {
        const std::string validMessage = "Hello, World!";
        
        WHEN("creating GreetingMessage with valid input") {
            auto result = GreetingMessage::create(validMessage);
            
            THEN("creation succeeds") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), Equals(validMessage));
                REQUIRE_THAT(std::string{result.value().view()}, Equals(validMessage));
                REQUIRE(result.value().length() == validMessage.length());
                REQUIRE_FALSE(result.value().empty());
            }
        }
    }
    
    GIVEN("invalid greeting messages") {
        
        WHEN("creating GreetingMessage with empty string") {
            auto result = GreetingMessage::create("");
            
            THEN("creation fails with EmptyMessage error") {
                REQUIRE_FALSE(result.has_value());
                REQUIRE(result.error() == GreetingError::EmptyMessage);
                REQUIRE_THAT(std::string{toString(result.error())}, 
                           ContainsSubstring("cannot be empty"));
            }
        }
        
        WHEN("creating GreetingMessage with whitespace-only string") {
            auto result = GreetingMessage::create("   \t\n  ");
            
            THEN("creation fails with EmptyMessage error") {
                REQUIRE_FALSE(result.has_value());
                REQUIRE(result.error() == GreetingError::EmptyMessage);
            }
        }
    }
}

SCENARIO("GreetingMessage comparison operations", "[GreetingMessage][comparison]") {
    
    GIVEN("two GreetingMessage instances") {
        auto msg1 = GreetingMessage::create("Hello").value();
        auto msg2 = GreetingMessage::create("Hello").value();
        auto msg3 = GreetingMessage::create("Hi").value();
        
        WHEN("comparing equal messages") {
            THEN("equality operators work correctly") {
                REQUIRE(msg1 == msg2);
                REQUIRE_FALSE(msg1 != msg2);
            }
        }
        
        WHEN("comparing different messages") {
            THEN("inequality operators work correctly") {
                REQUIRE(msg1 != msg3);
                REQUIRE_FALSE(msg1 == msg3);
                REQUIRE(msg1 < msg3);  // Hello < Hi alphabetically
            }
        }
    }
}

TEST_CASE("Convenience factory functions", "[factory][convenience]") {
    
    SECTION("makePersonName function") {
        auto result = makePersonName("Test User");
        
        REQUIRE(result.has_value());
        REQUIRE_THAT(result.value().value(), Equals("Test User"));
        
        auto errorResult = makePersonName("");
        REQUIRE_FALSE(errorResult.has_value());
        REQUIRE(errorResult.error() == GreetingError::EmptyName);
    }
    
    SECTION("makeGreetingMessage function") {
        auto result = makeGreetingMessage("Test Message");
        
        REQUIRE(result.has_value());
        REQUIRE_THAT(result.value().value(), Equals("Test Message"));
        
        auto errorResult = makeGreetingMessage("");
        REQUIRE_FALSE(errorResult.has_value());
        REQUIRE(errorResult.error() == GreetingError::EmptyMessage);
    }
}
