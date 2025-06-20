#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "greeting/types.hpp"

using namespace greeting;
using namespace greeting::consteval_validation;
using Catch::Matchers::Equals;
using Catch::Matchers::ContainsSubstring;

SCENARIO("Compile-time validation using if consteval", "[consteval][validation]") {
    
    GIVEN("compile-time character validation") {
        
        WHEN("validating basic characters") {
            
            THEN("valid characters are accepted") {
                static_assert(isValidNameCharacter('A'));
                static_assert(isValidNameCharacter('z'));
                static_assert(isValidNameCharacter(' '));
                static_assert(isValidNameCharacter('-'));
                static_assert(isValidNameCharacter('\''));
                static_assert(isValidNameCharacter('.'));
                
                REQUIRE(isValidNameCharacter('A'));
                REQUIRE(isValidNameCharacter('z'));
                REQUIRE(isValidNameCharacter(' '));
                REQUIRE(isValidNameCharacter('-'));
                REQUIRE(isValidNameCharacter('\''));
                REQUIRE(isValidNameCharacter('.'));
            }
            
            THEN("invalid characters are rejected") {
                static_assert(!isValidNameCharacter('@'));
                static_assert(!isValidNameCharacter('#'));
                static_assert(!isValidNameCharacter('$'));
                static_assert(!isValidNameCharacter('%'));
                
                REQUIRE_FALSE(isValidNameCharacter('@'));
                REQUIRE_FALSE(isValidNameCharacter('#'));
                REQUIRE_FALSE(isValidNameCharacter('$'));
                REQUIRE_FALSE(isValidNameCharacter('%'));
            }
        }
    }
    
    GIVEN("compile-time length validation") {
        
        WHEN("validating string lengths") {
            
            THEN("valid lengths pass validation") {
                constexpr auto validResult = validateLength("John", 2, 100);
                static_assert(!validResult.has_value());
                
                auto runtimeResult = validateLength("John Doe", 2, 100);
                REQUIRE_FALSE(runtimeResult.has_value());
            }
            
            THEN("empty strings fail validation") {
                constexpr auto emptyResult = validateLength("", 2, 100);
                static_assert(emptyResult.has_value());
                static_assert(emptyResult.value() == GreetingError::EmptyName);
                
                auto runtimeEmpty = validateLength("", 2, 100);
                REQUIRE(runtimeEmpty.has_value());
                REQUIRE(runtimeEmpty.value() == GreetingError::EmptyName);
            }
            
            THEN("too short strings fail validation") {
                constexpr auto shortResult = validateLength("A", 2, 100);
                static_assert(shortResult.has_value());
                static_assert(shortResult.value() == GreetingError::NameTooShort);
                
                auto runtimeShort = validateLength("X", 2, 100);
                REQUIRE(runtimeShort.has_value());
                REQUIRE(runtimeShort.value() == GreetingError::NameTooShort);
            }
            
            THEN("too long strings fail validation") {
                std::string longString(101, 'A');
                auto longResult = validateLength(longString, 2, 100);
                REQUIRE(longResult.has_value());
                REQUIRE(longResult.value() == GreetingError::NameTooLong);
            }
        }
    }
    
    GIVEN("compile-time character content validation") {
        
        WHEN("validating character patterns") {
            
            THEN("valid names pass validation") {
                constexpr auto validResult = validateCharacters("John Doe");
                static_assert(!validResult.has_value());
                
                auto runtimeValid = validateCharacters("Mary-Jane O'Connor");
                REQUIRE_FALSE(runtimeValid.has_value());
            }
            
            THEN("names with invalid characters fail") {
                constexpr auto invalidResult = validateCharacters("John@Doe");
                static_assert(invalidResult.has_value());
                static_assert(invalidResult.value() == GreetingError::InvalidName);
                
                auto runtimeInvalid = validateCharacters("Test#Name");
                REQUIRE(runtimeInvalid.has_value());
                REQUIRE(runtimeInvalid.value() == GreetingError::InvalidName);
            }
            
            THEN("names starting with special characters fail") {
                constexpr auto invalidStart = validateCharacters("-John");
                static_assert(invalidStart.has_value());
                static_assert(invalidStart.value() == GreetingError::InvalidName);
                
                auto runtimeInvalidStart = validateCharacters("'Mary");
                REQUIRE(runtimeInvalidStart.has_value());
                REQUIRE(runtimeInvalidStart.value() == GreetingError::InvalidName);
            }
            
            THEN("names ending with special characters fail") {
                constexpr auto invalidEnd = validateCharacters("John-");
                static_assert(invalidEnd.has_value());
                static_assert(invalidEnd.value() == GreetingError::InvalidName);
            }
        }
    }
}

SCENARIO("PersonName with consteval validation", "[PersonName][consteval]") {
    
    GIVEN("compile-time PersonName validation") {
        
        WHEN("validating at compile time") {
            
            THEN("valid names pass validation") {
                constexpr auto validResult = validatePersonName("John Doe");
                static_assert(!validResult.has_value());
                
                constexpr auto validResult2 = validatePersonName("Dr. Smith");
                static_assert(!validResult2.has_value());
            }
            
            THEN("invalid names fail validation at compile time") {
                constexpr auto emptyResult = validatePersonName("");
                static_assert(emptyResult.has_value());
                static_assert(emptyResult.value() == GreetingError::EmptyName);
                
                constexpr auto shortResult = validatePersonName("X");
                static_assert(shortResult.has_value());
                static_assert(shortResult.value() == GreetingError::NameTooShort);
                
                constexpr auto invalidResult = validatePersonName("Test@Name");
                static_assert(invalidResult.has_value());
                static_assert(invalidResult.value() == GreetingError::InvalidName);
            }
        }
        
        WHEN("using PersonName::create with consteval validation") {
            
            THEN("valid names are created successfully") {
                auto validName = PersonName::create("Alice Johnson");
                REQUIRE(validName.has_value());
                REQUIRE_THAT(validName.value().value(), Equals("Alice Johnson"));
            }
            
            THEN("invalid names fail with appropriate errors") {
                auto emptyName = PersonName::create("");
                REQUIRE_FALSE(emptyName.has_value());
                REQUIRE(emptyName.error() == GreetingError::EmptyName);
                
                auto shortName = PersonName::create("A");
                REQUIRE_FALSE(shortName.has_value());
                REQUIRE(shortName.error() == GreetingError::NameTooShort);
                
                auto invalidName = PersonName::create("John@Doe");
                REQUIRE_FALSE(invalidName.has_value());
                REQUIRE(invalidName.error() == GreetingError::InvalidName);
            }
        }
    }
}

SCENARIO("GreetingMessage with consteval validation", "[GreetingMessage][consteval]") {
    
    GIVEN("compile-time GreetingMessage validation") {
        
        WHEN("validating at compile time") {
            
            THEN("valid messages pass validation") {
                constexpr auto validResult = validateGreetingMessage("Hello, World!");
                static_assert(!validResult.has_value());
                
                constexpr auto validResult2 = validateGreetingMessage("Good morning");
                static_assert(!validResult2.has_value());
            }
            
            THEN("invalid messages fail validation at compile time") {
                constexpr auto emptyResult = validateGreetingMessage("");
                static_assert(emptyResult.has_value());
                static_assert(emptyResult.value() == GreetingError::EmptyMessage);
            }
        }
        
        WHEN("using GreetingMessage::create with consteval validation") {
            
            THEN("valid messages are created successfully") {
                auto validMessage = GreetingMessage::create("Welcome to our service!");
                REQUIRE(validMessage.has_value());
                REQUIRE_THAT(validMessage.value().value(), Equals("Welcome to our service!"));
            }
            
            THEN("invalid messages fail with appropriate errors") {
                auto emptyMessage = GreetingMessage::create("");
                REQUIRE_FALSE(emptyMessage.has_value());
                REQUIRE(emptyMessage.error() == GreetingError::EmptyMessage);
                
                auto whitespaceMessage = GreetingMessage::create("   ");
                REQUIRE_FALSE(whitespaceMessage.has_value());
                REQUIRE(whitespaceMessage.error() == GreetingError::EmptyMessage);
            }
        }
    }
}

SCENARIO("Runtime vs compile-time behavior differences", "[consteval][runtime]") {
    
    GIVEN("validation contexts") {
        
        WHEN("comparing compile-time vs runtime validation") {
            
            THEN("both contexts handle basic cases consistently") {
                // Test whitespace-only string handling
                auto runtimeResult = validatePersonName("   ");
                REQUIRE(runtimeResult.has_value());
                REQUIRE(runtimeResult.value() == GreetingError::EmptyName);
                
                // Test Unicode character handling (runtime only)
                auto unicodeResult = PersonName::create("José María");
                REQUIRE(unicodeResult.has_value()); // Should work at runtime
            }
            
            THEN("runtime provides more sophisticated validation") {
                // Test consecutive special characters (runtime feature)
                auto consecutiveSpecial = PersonName::create("John---Doe");
                REQUIRE_FALSE(consecutiveSpecial.has_value());
                REQUIRE(consecutiveSpecial.error() == GreetingError::InvalidName);
                
                // Test very long message warning
                std::string longMessage(600, 'A');
                auto longMsg = GreetingMessage::create(longMessage);
                REQUIRE_FALSE(longMsg.has_value());
                REQUIRE(longMsg.error() == GreetingError::MessageTooLong);
            }
        }
    }
}

TEST_CASE("Consteval validation performance characteristics", "[consteval][performance]") {
    
    SECTION("compile-time evaluation verification") {
        // These validations should happen at compile time
        constexpr bool validChar = isValidNameCharacter('A');
        constexpr auto validLength = validateLength("Test", 2, 100);
        constexpr auto validChars = validateCharacters("Valid Name");
        constexpr auto validName = validatePersonName("John Doe");
        constexpr auto validMessage = validateGreetingMessage("Hello!");
        
        // Verify compile-time results
        static_assert(validChar);
        static_assert(!validLength.has_value());
        static_assert(!validChars.has_value());
        static_assert(!validName.has_value());
        static_assert(!validMessage.has_value());
        
        REQUIRE(validChar);
        REQUIRE_FALSE(validLength.has_value());
        REQUIRE_FALSE(validChars.has_value());
        REQUIRE_FALSE(validName.has_value());
        REQUIRE_FALSE(validMessage.has_value());
    }
    
    SECTION("runtime behavior verification") {
        // These should execute runtime validation paths
        std::string dynamicName = "Dynamic Name";
        auto result = PersonName::create(dynamicName);
        REQUIRE(result.has_value());
        
        std::string dynamicMessage = "Dynamic Message";
        auto msgResult = GreetingMessage::create(dynamicMessage);
        REQUIRE(msgResult.has_value());
    }
}

SCENARIO("Consteval validation edge cases", "[consteval][edge_cases]") {
    
    GIVEN("boundary conditions") {
        
        WHEN("testing minimum valid lengths") {
            
            THEN("exactly minimum length passes") {
                constexpr auto minValid = validatePersonName("Jo");
                static_assert(!minValid.has_value());
                
                auto runtimeMin = PersonName::create("Ed");
                REQUIRE(runtimeMin.has_value());
            }
        }
        
        WHEN("testing maximum valid lengths") {
            
            THEN("exactly maximum length passes") {
                std::string maxLengthName(100, 'A');
                auto maxValid = validatePersonName(maxLengthName);
                REQUIRE_FALSE(maxValid.has_value());
                
                auto maxName = PersonName::create(maxLengthName);
                REQUIRE(maxName.has_value());
            }
        }
        
        WHEN("testing special character boundaries") {
            
            THEN("valid special character combinations work") {
                constexpr auto hyphenValid = validatePersonName("Mary-Jane");
                static_assert(!hyphenValid.has_value());
                
                constexpr auto apostropheValid = validatePersonName("O'Connor");
                static_assert(!apostropheValid.has_value());
                
                constexpr auto periodValid = validatePersonName("Dr. Smith");
                static_assert(!periodValid.has_value());
            }
        }
    }
}
