#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "greeting/types.hpp"
#include <array>
#include <vector>

using namespace greeting;
using Catch::Matchers::Equals;
using Catch::Matchers::ContainsSubstring;

// Test data for comprehensive error code validation
struct ErrorTestData {
    GreetingError code;
    ErrorSeverity expectedSeverity;
    ErrorDomain expectedDomain;
    std::string_view expectedMessageSubstring;
    std::string_view expectedCategorySubstring;
    bool shouldHaveSuggestion;
};

// Comprehensive error test data - Updated to include ALL error codes (35 total)
constexpr std::array<ErrorTestData, 35> ERROR_TEST_DATA = {{
    // Validation Errors (1000-1999)
    {GreetingError::EmptyName, ErrorSeverity::Error, ErrorDomain::Validation, 
     "empty", "Input Validation", true},
    {GreetingError::InvalidName, ErrorSeverity::Error, ErrorDomain::Validation, 
     "invalid characters", "Input Validation", true},
    {GreetingError::NameTooLong, ErrorSeverity::Error, ErrorDomain::Validation, 
     "exceeds maximum", "Input Validation", true},
    {GreetingError::NameTooShort, ErrorSeverity::Warning, ErrorDomain::Validation, 
     "unusually short", "Input Validation", true},
    {GreetingError::EmptyMessage, ErrorSeverity::Error, ErrorDomain::Validation, 
     "empty", "Input Validation", true},
    {GreetingError::InvalidMessage, ErrorSeverity::Error, ErrorDomain::Validation, 
     "invalid", "Input Validation", true},
    {GreetingError::MessageTooLong, ErrorSeverity::Warning, ErrorDomain::Validation, 
     "very long", "Input Validation", true},
    {GreetingError::InvalidCharacters, ErrorSeverity::Error, ErrorDomain::Validation, 
     "not allowed", "Input Validation", true},
    {GreetingError::UnsupportedFormat, ErrorSeverity::Error, ErrorDomain::Validation, 
     "not supported", "Input Validation", true},
    
    // Configuration Errors (2000-2999)
    {GreetingError::ConfigurationMissing, ErrorSeverity::Critical, ErrorDomain::Configuration, 
     "configuration", "Configuration", true},
    {GreetingError::ConfigurationInvalid, ErrorSeverity::Error, ErrorDomain::Configuration, 
     "invalid values", "Configuration", true},
    {GreetingError::ConfigurationCorrupted, ErrorSeverity::Error, ErrorDomain::Configuration, 
     "corrupted", "Configuration", true},
    {GreetingError::SettingsNotFound, ErrorSeverity::Error, ErrorDomain::Configuration, 
     "setting", "Configuration", true},
    {GreetingError::EnvironmentInvalid, ErrorSeverity::Error, ErrorDomain::Configuration, 
     "variables", "Configuration", true},
    {GreetingError::LocaleUnsupported, ErrorSeverity::Warning, ErrorDomain::Configuration, 
     "locale", "Localization", true},
    
    // Strategy Errors (3000-3999)
    {GreetingError::StrategyNotFound, ErrorSeverity::Error, ErrorDomain::Strategy, 
     "strategy", "Strategy Pattern", true},
    {GreetingError::StrategyCreationFailed, ErrorSeverity::Error, ErrorDomain::Strategy, 
     "create", "Strategy Pattern", true},
    {GreetingError::StrategyIncompatible, ErrorSeverity::Error, ErrorDomain::Strategy, 
     "incompatible", "Strategy Pattern", true},
    {GreetingError::StrategyRegistrationFailed, ErrorSeverity::Error, ErrorDomain::Strategy, 
     "register", "Strategy Pattern", true},
    {GreetingError::FactoryNotInitialized, ErrorSeverity::Error, ErrorDomain::Strategy, 
     "factory", "Strategy Pattern", true},
    
    // Output Errors (4000-4999)
    {GreetingError::OutputStreamError, ErrorSeverity::Error, ErrorDomain::Output, 
     "output stream", "Output Operations", true},
    {GreetingError::FormattingError, ErrorSeverity::Error, ErrorDomain::Output, 
     "formatting", "Output Operations", true},
    {GreetingError::EncodingError, ErrorSeverity::Error, ErrorDomain::Output, 
     "encoding", "Output Operations", true},
    {GreetingError::RenderingFailed, ErrorSeverity::Error, ErrorDomain::Output, 
     "render", "Output Operations", true},
    {GreetingError::BufferOverflow, ErrorSeverity::Error, ErrorDomain::Output, 
     "buffer overflow", "Output Operations", true},
    
    // System Errors (5000-5999)
    {GreetingError::MemoryAllocationFailed, ErrorSeverity::Critical, ErrorDomain::System, 
     "memory", "System Resources", true},
    {GreetingError::FileSystemError, ErrorSeverity::Error, ErrorDomain::System, 
     "system", "System Resources", true},
    {GreetingError::NetworkError, ErrorSeverity::Error, ErrorDomain::System, 
     "Network", "System Resources", true},
    {GreetingError::PermissionDenied, ErrorSeverity::Error, ErrorDomain::System, 
     "permission", "System Security", true},
    {GreetingError::ResourceUnavailable, ErrorSeverity::Error, ErrorDomain::System, 
     "resource", "System Resources", true},
     
    // Logic Errors (6000-6999) - Add the missing ones
    {GreetingError::InvalidState, ErrorSeverity::Error, ErrorDomain::Logic, 
     "invalid state", "Business Logic", true},
    {GreetingError::PreconditionViolated, ErrorSeverity::Error, ErrorDomain::Logic, 
     "precondition", "Business Logic", true},
    {GreetingError::PostconditionViolated, ErrorSeverity::Error, ErrorDomain::Logic, 
     "postcondition", "Business Logic", true},
    {GreetingError::InvariantViolated, ErrorSeverity::Error, ErrorDomain::Logic, 
     "invariant", "Business Logic", true},
    {GreetingError::NotImplemented, ErrorSeverity::Info, ErrorDomain::Logic, 
     "not yet implemented", "Development", true}
}};

SCENARIO("Comprehensive GreetingError enumeration validation", "[GreetingError][comprehensive]") {
    
    GIVEN("all defined error codes") {
        
        WHEN("validating error information completeness") {
            
            THEN("each error has complete metadata") {
                for (const auto& testData : ERROR_TEST_DATA) {
                    INFO("Testing error code: " << static_cast<int>(testData.code));
                    
                    const auto errorInfo = getErrorInfo(testData.code);
                    
                    // Verify error code matches
                    REQUIRE(errorInfo.code == testData.code);
                    
                    // Verify severity matches expected
                    REQUIRE(errorInfo.severity == testData.expectedSeverity);
                    
                    // Verify domain matches expected
                    REQUIRE(errorInfo.domain == testData.expectedDomain);
                    
                    // Verify message contains expected substring
                    REQUIRE_THAT(std::string{errorInfo.message}, 
                               ContainsSubstring(std::string{testData.expectedMessageSubstring}));
                    
                    // Verify category contains expected substring
                    REQUIRE_THAT(std::string{errorInfo.category}, 
                               ContainsSubstring(std::string{testData.expectedCategorySubstring}));
                    
                    // Verify suggestion is present when expected
                    if (testData.shouldHaveSuggestion) {
                        REQUIRE_FALSE(errorInfo.suggestion.empty());
                    }
                    
                    // Verify message is not empty and reasonable length
                    REQUIRE(errorInfo.message.length() > 10);
                    REQUIRE(errorInfo.message.length() < 200);
                }
            }
        }
        
        WHEN("using toString() function") {
            
            THEN("messages match getErrorInfo() results") {
                for (const auto& testData : ERROR_TEST_DATA) {
                    INFO("Testing toString for error code: " << static_cast<int>(testData.code));
                    
                    const auto errorInfo = getErrorInfo(testData.code);
                    const auto toStringResult = toString(testData.code);
                    
                    REQUIRE(toStringResult == errorInfo.message);
                }
            }
        }
        
        WHEN("using helper functions") {
            
            THEN("severity and domain functions work correctly") {
                for (const auto& testData : ERROR_TEST_DATA) {
                    INFO("Testing helpers for error code: " << static_cast<int>(testData.code));
                    
                    REQUIRE(getErrorSeverity(testData.code) == testData.expectedSeverity);
                    REQUIRE(getErrorDomain(testData.code) == testData.expectedDomain);
                }
            }
        }
    }
}

SCENARIO("Error severity and domain enum validation", "[ErrorSeverity][ErrorDomain]") {
    
    GIVEN("ErrorSeverity enum values") {
        
        WHEN("converting to string") {
            
            THEN("each severity has a string representation") {
                REQUIRE_THAT(std::string{toString(ErrorSeverity::Info)}, Equals("INFO"));
                REQUIRE_THAT(std::string{toString(ErrorSeverity::Warning)}, Equals("WARNING"));
                REQUIRE_THAT(std::string{toString(ErrorSeverity::Error)}, Equals("ERROR"));
                REQUIRE_THAT(std::string{toString(ErrorSeverity::Critical)}, Equals("CRITICAL"));
            }
        }
    }
    
    GIVEN("ErrorDomain enum values") {
        
        WHEN("converting to string") {
            
            THEN("each domain has a string representation") {
                REQUIRE_THAT(std::string{toString(ErrorDomain::Validation)}, Equals("VALIDATION"));
                REQUIRE_THAT(std::string{toString(ErrorDomain::Configuration)}, Equals("CONFIGURATION"));
                REQUIRE_THAT(std::string{toString(ErrorDomain::Strategy)}, Equals("STRATEGY"));
                REQUIRE_THAT(std::string{toString(ErrorDomain::Output)}, Equals("OUTPUT"));
                REQUIRE_THAT(std::string{toString(ErrorDomain::System)}, Equals("SYSTEM"));
                REQUIRE_THAT(std::string{toString(ErrorDomain::Logic)}, Equals("LOGIC"));
            }
        }
    }
}

SCENARIO("Error code numeric ranges validation", "[GreetingError][ranges]") {
    
    GIVEN("error code numeric values") {
        
        WHEN("checking range assignments") {
            
            THEN("validation errors are in 1000-1999 range") {
                REQUIRE(static_cast<int>(GreetingError::EmptyName) >= 1000);
                REQUIRE(static_cast<int>(GreetingError::EmptyName) < 2000);
                REQUIRE(static_cast<int>(GreetingError::UnsupportedFormat) >= 1000);
                REQUIRE(static_cast<int>(GreetingError::UnsupportedFormat) < 2000);
            }
            
            THEN("configuration errors are in 2000-2999 range") {
                REQUIRE(static_cast<int>(GreetingError::ConfigurationMissing) >= 2000);
                REQUIRE(static_cast<int>(GreetingError::ConfigurationMissing) < 3000);
                REQUIRE(static_cast<int>(GreetingError::LocaleUnsupported) >= 2000);
                REQUIRE(static_cast<int>(GreetingError::LocaleUnsupported) < 3000);
            }
            
            THEN("strategy errors are in 3000-3999 range") {
                REQUIRE(static_cast<int>(GreetingError::StrategyNotFound) >= 3000);
                REQUIRE(static_cast<int>(GreetingError::StrategyNotFound) < 4000);
                REQUIRE(static_cast<int>(GreetingError::FactoryNotInitialized) >= 3000);
                REQUIRE(static_cast<int>(GreetingError::FactoryNotInitialized) < 4000);
            }
            
            THEN("output errors are in 4000-4999 range") {
                REQUIRE(static_cast<int>(GreetingError::OutputStreamError) >= 4000);
                REQUIRE(static_cast<int>(GreetingError::OutputStreamError) < 5000);
                REQUIRE(static_cast<int>(GreetingError::BufferOverflow) >= 4000);
                REQUIRE(static_cast<int>(GreetingError::BufferOverflow) < 5000);
            }
            
            THEN("system errors are in 5000-5999 range") {
                REQUIRE(static_cast<int>(GreetingError::MemoryAllocationFailed) >= 5000);
                REQUIRE(static_cast<int>(GreetingError::MemoryAllocationFailed) < 6000);
                REQUIRE(static_cast<int>(GreetingError::ResourceUnavailable) >= 5000);
                REQUIRE(static_cast<int>(GreetingError::ResourceUnavailable) < 6000);
            }
            
            THEN("logic errors are in 6000-6999 range") {
                REQUIRE(static_cast<int>(GreetingError::InvalidState) >= 6000);
                REQUIRE(static_cast<int>(GreetingError::InvalidState) < 7000);
                REQUIRE(static_cast<int>(GreetingError::NotImplemented) >= 6000);
                REQUIRE(static_cast<int>(GreetingError::NotImplemented) < 7000);
            }
            
            THEN("unknown error is in special range") {
                REQUIRE(static_cast<int>(GreetingError::Unknown) == 9999);
            }
        }
    }
}

SCENARIO("Error system integration validation", "[GreetingError][integration]") {
    
    GIVEN("various error codes") {
        
        WHEN("creating std::error_code instances") {
            
            THEN("error codes integrate properly with standard library") {
                const auto validationError = make_error_code(GreetingError::EmptyName);
                const auto configError = make_error_code(GreetingError::ConfigurationMissing);
                const auto strategyError = make_error_code(GreetingError::StrategyNotFound);
                
                // Verify category is correct
                REQUIRE_THAT(validationError.category().name(), Equals("greeting"));
                REQUIRE_THAT(configError.category().name(), Equals("greeting"));
                REQUIRE_THAT(strategyError.category().name(), Equals("greeting"));
                
                // Verify messages contain suggestions
                REQUIRE_THAT(validationError.message(), ContainsSubstring("Suggestion:"));
                REQUIRE_THAT(configError.message(), ContainsSubstring("Suggestion:"));
                REQUIRE_THAT(strategyError.message(), ContainsSubstring("Suggestion:"));
                
                // Verify error codes are different
                REQUIRE(validationError != configError);
                REQUIRE(configError != strategyError);
                REQUIRE(validationError != strategyError);
            }
        }
        
        WHEN("checking error condition equivalence") {
            
            THEN("severity-based conditions work correctly") {
                // Critical errors map to resource_unavailable_try_again
                const auto criticalError = make_error_code(GreetingError::ConfigurationMissing);
                REQUIRE(criticalError == std::errc::resource_unavailable_try_again);
                
                // Regular errors map to invalid_argument
                const auto regularError = make_error_code(GreetingError::EmptyName);
                REQUIRE(regularError == std::errc::invalid_argument);
                
                // Warning errors map to operation_not_supported
                const auto warningError = make_error_code(GreetingError::NameTooShort);
                REQUIRE(warningError == std::errc::operation_not_supported);
            }
        }
    }
}

SCENARIO("Error unknown/default handling", "[GreetingError][unknown]") {
    
    GIVEN("an unknown error code") {
        
        WHEN("requesting error information") {
            
            THEN("unknown error information is returned") {
                const auto unknownInfo = getErrorInfo(GreetingError::Unknown);
                
                REQUIRE(unknownInfo.code == GreetingError::Unknown);
                REQUIRE(unknownInfo.severity == ErrorSeverity::Error);
                REQUIRE(unknownInfo.domain == ErrorDomain::System);
                REQUIRE_THAT(std::string{unknownInfo.message}, ContainsSubstring("unknown"));
                REQUIRE_THAT(std::string{unknownInfo.category}, Equals("Unknown"));
                REQUIRE_THAT(std::string{unknownInfo.suggestion}, ContainsSubstring("support"));
            }
        }
    }
}

TEST_CASE("ErrorInfo struct validation", "[ErrorInfo][struct]") {
    
    SECTION("ErrorInfo construction") {
        constexpr ErrorInfo testInfo{
            GreetingError::EmptyName,
            ErrorSeverity::Error,
            ErrorDomain::Validation,
            "Test message",
            "Test category",
            "Test suggestion"
        };
        
        REQUIRE(testInfo.code == GreetingError::EmptyName);
        REQUIRE(testInfo.severity == ErrorSeverity::Error);
        REQUIRE(testInfo.domain == ErrorDomain::Validation);
        REQUIRE_THAT(std::string{testInfo.message}, Equals("Test message"));
        REQUIRE_THAT(std::string{testInfo.category}, Equals("Test category"));
        REQUIRE_THAT(std::string{testInfo.suggestion}, Equals("Test suggestion"));
    }
    
    SECTION("ErrorInfo with empty suggestion") {
        constexpr ErrorInfo testInfo{
            GreetingError::EmptyName,
            ErrorSeverity::Error,
            ErrorDomain::Validation,
            "Test message",
            "Test category"
        };
        
        REQUIRE(testInfo.suggestion.empty());
    }
}

TEST_CASE("Constexpr evaluation of error functions", "[GreetingError][constexpr]") {
    
    SECTION("getErrorInfo is constexpr") {
        constexpr auto errorInfo = getErrorInfo(GreetingError::EmptyName);
        static_assert(errorInfo.code == GreetingError::EmptyName);
        static_assert(errorInfo.severity == ErrorSeverity::Error);
        static_assert(errorInfo.domain == ErrorDomain::Validation);
        
        REQUIRE(errorInfo.message.length() > 0);
    }
    
    SECTION("toString functions are constexpr") {
        constexpr auto errorMsg = toString(GreetingError::EmptyName);
        constexpr auto severityStr = toString(ErrorSeverity::Error);
        constexpr auto domainStr = toString(ErrorDomain::Validation);
        
        static_assert(errorMsg.length() > 0);
        static_assert(severityStr.length() > 0);
        static_assert(domainStr.length() > 0);
        
        REQUIRE(errorMsg.length() > 0);
        REQUIRE(severityStr.length() > 0);
        REQUIRE(domainStr.length() > 0);
    }
    
    SECTION("helper functions are constexpr") {
        constexpr auto severity = getErrorSeverity(GreetingError::EmptyName);
        constexpr auto domain = getErrorDomain(GreetingError::EmptyName);
        
        static_assert(severity == ErrorSeverity::Error);
        static_assert(domain == ErrorDomain::Validation);
        
        REQUIRE(severity == ErrorSeverity::Error);
        REQUIRE(domain == ErrorDomain::Validation);
    }
}
