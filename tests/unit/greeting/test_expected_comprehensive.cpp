#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "greeting/types.hpp"
#include <string>
#include <memory>
#include <vector>

using namespace greeting;
using Catch::Matchers::Equals;
using Catch::Matchers::ContainsSubstring;

SCENARIO("Expected wrapper comprehensive functionality", "[Expected][comprehensive]") {
    
    GIVEN("Expected with successful values") {
        
        WHEN("constructing with different value types") {
            Expected<int, GreetingError> intResult{42};
            Expected<std::string, GreetingError> stringResult{std::string{"hello"}};
            auto nameCreation = PersonName::create("John");
            REQUIRE(nameCreation.has_value());
            Expected<PersonName, GreetingError> nameResult{nameCreation.value()};
            
            THEN("all values are accessible") {
                REQUIRE(intResult.has_value());
                REQUIRE(intResult.value() == 42);
                
                REQUIRE(stringResult.has_value());
                REQUIRE_THAT(stringResult.value(), Equals("hello"));
                
                REQUIRE(nameResult.has_value());
                REQUIRE_THAT(nameResult.value().value(), Equals("John"));
            }
        }
        
        WHEN("using bool conversion") {
            Expected<std::string, GreetingError> successResult{std::string{"success"}};
            
            THEN("bool conversion returns true") {
                REQUIRE(static_cast<bool>(successResult));
                REQUIRE(successResult); // implicit conversion
            }
        }
    }
    
    GIVEN("Expected with error values") {
        
        WHEN("constructing with different error types") {
            Expected<int, GreetingError> validationError{GreetingError::EmptyName};
            Expected<std::string, GreetingError> configError{GreetingError::ConfigurationMissing};
            Expected<PersonName, GreetingError> strategyError{GreetingError::StrategyNotFound};
            
            THEN("errors are accessible") {
                REQUIRE_FALSE(validationError.has_value());
                REQUIRE(validationError.error() == GreetingError::EmptyName);
                
                REQUIRE_FALSE(configError.has_value());
                REQUIRE(configError.error() == GreetingError::ConfigurationMissing);
                
                REQUIRE_FALSE(strategyError.has_value());
                REQUIRE(strategyError.error() == GreetingError::StrategyNotFound);
            }
        }
        
        WHEN("using bool conversion") {
            Expected<std::string, GreetingError> errorResult{GreetingError::EmptyMessage};
            
            THEN("bool conversion returns false") {
                REQUIRE_FALSE(static_cast<bool>(errorResult));
                REQUIRE_FALSE(errorResult); // implicit conversion
            }
        }
    }
}

SCENARIO("Expected move semantics and performance", "[Expected][move][performance]") {
    
    GIVEN("moveable types") {
        
        WHEN("constructing with move semantics") {
            std::string largeString(1000, 'A');
            std::string originalString = largeString;
            
            Expected<std::string, GreetingError> result{std::move(largeString)};
            
            THEN("value is moved correctly") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value(), Equals(originalString));
                // Original string should be empty after move
                REQUIRE(largeString.empty());
            }
        }
        
        WHEN("moving Expected objects") {
            Expected<std::string, GreetingError> original{std::string{"test"}};
            Expected<std::string, GreetingError> moved{std::move(original)};
            
            THEN("move construction works") {
                REQUIRE(moved.has_value());
                REQUIRE_THAT(moved.value(), Equals("test"));
            }
        }
        
        WHEN("getting rvalue references") {
            Expected<std::string, GreetingError> result{std::string{"moveable"}};
            std::string extracted = std::move(result).value();
            
            THEN("rvalue value() works") {
                REQUIRE_THAT(extracted, Equals("moveable"));
            }
        }
    }
}

SCENARIO("Expected with std::error_code integration", "[Expected][error_code]") {
    
    GIVEN("Expected with std::error_code as error type") {
        using ResultWithCode = Expected<std::string, std::error_code>;
        
        WHEN("constructing with error codes") {
            auto errorCode = make_error_code(GreetingError::ConfigurationMissing);
            ResultWithCode errorResult{errorCode};
            
            THEN("error code integration works") {
                REQUIRE_FALSE(errorResult.has_value());
                REQUIRE(errorResult.error() == errorCode);
                REQUIRE_THAT(errorResult.error().message(), ContainsSubstring("configuration"));
                REQUIRE_THAT(errorResult.error().message(), ContainsSubstring("Suggestion:"));
            }
        }
        
        WHEN("constructing with success values") {
            ResultWithCode successResult{std::string{"success"}};
            
            THEN("success case works normally") {
                REQUIRE(successResult.has_value());
                REQUIRE_THAT(successResult.value(), Equals("success"));
            }
        }
    }
}

SCENARIO("Expected exception safety", "[Expected][exceptions]") {
    
    GIVEN("Expected with values") {
        
        WHEN("accessing value when error is stored") {
            Expected<std::string, GreetingError> errorResult{GreetingError::EmptyName};
            
            THEN("value() throws std::bad_variant_access") {
                REQUIRE_THROWS_AS(errorResult.value(), std::bad_variant_access);
            }
        }
        
        WHEN("accessing error when value is stored") {
            Expected<std::string, GreetingError> successResult{std::string{"success"}};
            
            THEN("error() throws std::bad_variant_access") {
                REQUIRE_THROWS_AS(successResult.error(), std::bad_variant_access);
            }
        }
    }
}

SCENARIO("Expected const correctness", "[Expected][const]") {
    
    GIVEN("const Expected objects") {
        
        WHEN("working with const Expected with value") {
            const Expected<std::string, GreetingError> constResult{std::string{"const_value"}};
            
            THEN("const methods work correctly") {
                REQUIRE(constResult.has_value());
                REQUIRE_THAT(constResult.value(), Equals("const_value"));
                
                // Verify return type is const reference
                static_assert(std::is_same_v<decltype(constResult.value()), const std::string&>);
            }
        }
        
        WHEN("working with const Expected with error") {
            const Expected<std::string, GreetingError> constError{GreetingError::EmptyName};
            
            THEN("const error access works correctly") {
                REQUIRE_FALSE(constError.has_value());
                REQUIRE(constError.error() == GreetingError::EmptyName);
                
                // Verify return type is const reference
                static_assert(std::is_same_v<decltype(constError.error()), const GreetingError&>);
            }
        }
    }
}

SCENARIO("Expected with complex types", "[Expected][complex_types]") {
    
    GIVEN("Expected with custom types") {
        
        WHEN("using PersonName as value type") {
            auto nameResult = PersonName::create("John Doe");
            Expected<PersonName, GreetingError> expectedName{nameResult.value()};
            
            THEN("PersonName works as Expected value") {
                REQUIRE(expectedName.has_value());
                REQUIRE_THAT(expectedName.value().value(), Equals("John Doe"));
            }
        }
        
        WHEN("using GreetingMessage as value type") {
            auto messageResult = GreetingMessage::create("Hello, World!");
            Expected<GreetingMessage, GreetingError> expectedMessage{messageResult.value()};
            
            THEN("GreetingMessage works as Expected value") {
                REQUIRE(expectedMessage.has_value());
                REQUIRE_THAT(expectedMessage.value().value(), Equals("Hello, World!"));
            }
        }
    }
}

SCENARIO("Expected type aliases functionality", "[Expected][aliases]") {
    
    GIVEN("predefined type aliases") {
        
        WHEN("using Result<T> alias") {
            Result<std::string> stringResult{std::string{"test"}};
            Result<int> errorResult{GreetingError::EmptyName};
            
            THEN("aliases work correctly") {
                REQUIRE(stringResult.has_value());
                REQUIRE_THAT(stringResult.value(), Equals("test"));
                
                REQUIRE_FALSE(errorResult.has_value());
                REQUIRE(errorResult.error() == GreetingError::EmptyName);
            }
        }
        
        WHEN("using ResultWithCode<T> alias") {
            ResultWithCode<std::string> successResult{std::string{"success"}};
            auto errorCode = make_error_code(GreetingError::InvalidName);
            ResultWithCode<int> errorResult{errorCode};
            
            THEN("error code aliases work correctly") {
                REQUIRE(successResult.has_value());
                REQUIRE_THAT(successResult.value(), Equals("success"));
                
                REQUIRE_FALSE(errorResult.has_value());
                REQUIRE(errorResult.error() == errorCode);
            }
        }
    }
}

// Stress test with various error codes
TEST_CASE("Expected stress test with all error codes", "[Expected][stress]") {
    
    SECTION("cycling through all error types") {
        std::vector<GreetingError> allErrors = {
            // Validation Errors (1000-1999)
            GreetingError::EmptyName,
            GreetingError::InvalidName,
            GreetingError::NameTooLong,
            GreetingError::NameTooShort,
            GreetingError::EmptyMessage,
            GreetingError::InvalidMessage,
            GreetingError::MessageTooLong,
            GreetingError::InvalidCharacters,
            GreetingError::UnsupportedFormat,
            
            // Configuration Errors (2000-2999)
            GreetingError::ConfigurationMissing,
            GreetingError::ConfigurationInvalid,
            GreetingError::ConfigurationCorrupted,
            GreetingError::SettingsNotFound,
            GreetingError::EnvironmentInvalid,
            GreetingError::LocaleUnsupported,
            
            // Strategy Errors (3000-3999)
            GreetingError::StrategyNotFound,
            GreetingError::StrategyCreationFailed,
            GreetingError::StrategyIncompatible,
            GreetingError::StrategyRegistrationFailed,
            GreetingError::FactoryNotInitialized,
            
            // Output Errors (4000-4999)
            GreetingError::OutputStreamError,
            GreetingError::FormattingError,
            GreetingError::EncodingError,
            GreetingError::RenderingFailed,
            GreetingError::BufferOverflow,
            
            // System Errors (5000-5999)
            GreetingError::MemoryAllocationFailed,
            GreetingError::FileSystemError,
            GreetingError::NetworkError,
            GreetingError::PermissionDenied,
            GreetingError::ResourceUnavailable,
            
            // Logic Errors (6000-6999)
            GreetingError::InvalidState,
            GreetingError::PreconditionViolated,
            GreetingError::PostconditionViolated,
            GreetingError::InvariantViolated,
            GreetingError::NotImplemented,
            
            // Unknown
            GreetingError::Unknown
        };
        
        for (const auto& error : allErrors) {
            INFO("Testing Expected with error: " << static_cast<int>(error));
            
            Expected<std::string, GreetingError> errorResult{error};
            
            REQUIRE_FALSE(errorResult.has_value());
            REQUIRE(errorResult.error() == error);
            
            // Verify error has meaningful information
            auto errorInfo = getErrorInfo(error);
            REQUIRE(errorInfo.message.length() > 0);
            REQUIRE(errorInfo.category.length() > 0);
        }
    }
}
