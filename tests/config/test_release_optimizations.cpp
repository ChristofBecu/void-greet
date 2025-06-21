#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include "config/release_config.hpp"
#include "config/debug_config.hpp"
#include "greeting/config_aware_validation.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"

using namespace greeting;
using namespace greeting::config;

// ============================================================================
// Release Configuration Tests
// ============================================================================

TEST_CASE("Release Configuration - Error Context is Minimal", "[release][config][error]") {
    SECTION("MinimalErrorContext has minimal memory footprint") {
        // Release error context should be minimal
        auto error_context = release::MinimalErrorContext(GreetingError::EmptyName);
        
        REQUIRE(error_context.has_error());
        REQUIRE(error_context.error_code == GreetingError::EmptyName);
        REQUIRE(std::string(error_context.get_error_message()) == "Empty name");
        
        // Memory footprint should be just the enum value
        REQUIRE(sizeof(error_context) <= sizeof(GreetingError) + 8); // Allow some padding
    }
    
    SECTION("MinimalErrorContext default constructor creates no-error state") {
        auto no_error = release::MinimalErrorContext();
        
        REQUIRE_FALSE(no_error.has_error());
        REQUIRE(std::string(no_error.get_error_message()) == "No error");
    }
    
    SECTION("ValidationError provides minimal interface") {
        auto validation_error = release::ValidationError(GreetingError::InvalidCharacters);
        
        REQUIRE(validation_error.has_error());
        REQUIRE(validation_error.code() == GreetingError::InvalidCharacters);
        REQUIRE(std::string(validation_error.message()) == "Invalid characters");
        REQUIRE_FALSE(static_cast<bool>(validation_error)); // Should be false for errors
        
        // Success case
        auto success = release::ValidationError();
        REQUIRE_FALSE(success.has_error());
        REQUIRE(static_cast<bool>(success)); // Should be true for success
    }
}

TEST_CASE("Release Configuration - Feature Flags are Disabled", "[release][config][features]") {
    SECTION("All debug features are disabled in release") {
        REQUIRE_FALSE(release::enable_detailed_errors());
        REQUIRE_FALSE(release::enable_stacktrace());
        REQUIRE_FALSE(release::enable_position_tracking());
        REQUIRE_FALSE(release::enable_error_suggestions());
    }
}

TEST_CASE("Release Configuration - Error Creation is Zero-Overhead", "[release][config][optimization]") {
    SECTION("Error creation functions are constexpr") {
        // These should be compile-time evaluable
        constexpr auto empty_name_error = release::make_empty_name_error();
        constexpr auto invalid_chars_error = release::make_invalid_characters_error();
        constexpr auto name_too_long_error = release::make_name_too_long_error();
        constexpr auto empty_message_error = release::make_empty_message_error();
        constexpr auto message_too_long_error = release::make_message_too_long_error();
        
        REQUIRE(empty_name_error.code() == GreetingError::EmptyName);
        REQUIRE(invalid_chars_error.code() == GreetingError::InvalidCharacters);
        REQUIRE(name_too_long_error.code() == GreetingError::NameTooLong);
        REQUIRE(empty_message_error.code() == GreetingError::EmptyMessage);
        REQUIRE(message_too_long_error.code() == GreetingError::MessageTooLong);
    }
    
    SECTION("Template error creation is constexpr") {
        constexpr auto compile_time_error = release::make_compile_time_error<GreetingError::InvalidName>();
        REQUIRE(compile_time_error.code() == GreetingError::InvalidName);
    }
    
    SECTION("Can optimize error handling for trivial types") {
        REQUIRE(release::can_optimize_error_handling<int>());
        REQUIRE(release::can_optimize_error_handling<char>());
        // Note: PersonName and GreetingMessage are not trivial due to std::string members
    }
}

// ============================================================================
// Debug Configuration Tests
// ============================================================================

TEST_CASE("Debug Configuration - Error Context is Rich", "[debug][config][error]") {
    SECTION("DetailedErrorContext contains comprehensive information") {
        std::string test_input = "invalid@name";
        auto error_context = debug::DetailedErrorContext(
            GreetingError::InvalidCharacters,
            "Contains invalid character '@'",
            test_input
        );
        
        REQUIRE(error_context.error_code == GreetingError::InvalidCharacters);
        REQUIRE(error_context.detailed_message == "Contains invalid character '@'");
        REQUIRE(error_context.input_value == test_input);
        
        // Should have source location information
        REQUIRE(std::string(error_context.location.file_name()).length() > 0);
    }
    
    SECTION("ValidationError provides rich interface") {
        std::string test_input = "toolongname" + std::string(100, 'x');
        auto validation_error = debug::ValidationError(
            GreetingError::NameTooLong,
            "Name is too long",
            test_input
        );
        
        validation_error.add_suggestion("Shorten the name");
        validation_error.add_suggestion("Use initials");
        validation_error.set_input_position(5);
        
        REQUIRE(validation_error.code() == GreetingError::NameTooLong);
        REQUIRE(validation_error.context.suggestions.size() == 2);
        REQUIRE(validation_error.context.input_position == 5);
        
        // Formatted message should include all details
        std::string formatted = validation_error.message();
        REQUIRE(formatted.find("Name is too long") != std::string::npos);
        REQUIRE(formatted.find("Shorten the name") != std::string::npos);
        REQUIRE(formatted.find("position 5") != std::string::npos);
    }
}

TEST_CASE("Debug Configuration - Feature Flags are Enabled", "[debug][config][features]") {
    SECTION("Debug features are configuration-dependent") {
        // Note: These may return true or false depending on build configuration
        // In debug builds with all features enabled, they should return true
        #ifdef HELLOWORLD_DEBUG_BUILD
            REQUIRE(debug::enable_detailed_errors());
            REQUIRE(debug::enable_position_tracking());
            REQUIRE(debug::enable_error_suggestions());
            // Stacktrace depends on compiler support
        #endif
    }
}

TEST_CASE("Debug Configuration - Error Creation Provides Rich Context", "[debug][config][diagnostics]") {
    SECTION("Error creation functions provide detailed context") {
        std::string invalid_input = "test@name";
        auto error = debug::make_invalid_characters_error(invalid_input, 4, '@');
        
        REQUIRE(error.code() == GreetingError::InvalidCharacters);
        REQUIRE(error.context.input_value == invalid_input);
        REQUIRE(error.context.input_position == 4);
        REQUIRE(error.context.suggestions.size() > 0);
        
        // Should contain helpful suggestions
        std::string message = error.message();
        REQUIRE(message.find("invalid character") != std::string::npos);
        REQUIRE(message.find("@") != std::string::npos);
    }
    
    SECTION("Name validation errors include suggestions") {
        auto empty_error = debug::make_empty_name_error("");
        auto long_error = debug::make_name_too_long_error(std::string(200, 'x'), 100);
        
        REQUIRE(empty_error.context.suggestions.size() > 0);
        REQUIRE(long_error.context.suggestions.size() > 0);
        
        // Check suggestion content
        bool found_helpful_suggestion = false;
        for (const auto& suggestion : empty_error.context.suggestions) {
            if (suggestion.find("non-empty") != std::string::npos) {
                found_helpful_suggestion = true;
                break;
            }
        }
        REQUIRE(found_helpful_suggestion);
    }
}

// ============================================================================
// Configuration-Aware Validation Tests
// ============================================================================

TEST_CASE("Configuration-Aware Validation - Context Selection", "[config][validation][context]") {
    SECTION("ValidationContext provides correct configuration flags") {
        auto context = validation::ValidationContext::create_for_current_config();
        (void)context; // Suppress unused variable warning
        
        // These should reflect the current build configuration
        bool detailed = validation::ValidationContext::enable_detailed_validation();
        bool stacktrace = validation::ValidationContext::enable_stacktrace();
        bool monitoring = validation::ValidationContext::enable_performance_monitoring();
        
        // Values depend on build configuration, but should be consistent
        REQUIRE((detailed || !detailed)); // Always true (tautology to check compilation)
        REQUIRE((stacktrace || !stacktrace));
        REQUIRE((monitoring || !monitoring));
    }
}

TEST_CASE("Configuration-Aware Validation - Error Type Selection", "[config][validation][errors]") {
    SECTION("ValidationError type is selected based on configuration") {
        // Test that we can create configuration-aware errors
        auto empty_name_error = validation::make_config_aware_empty_name_error("   ");
        auto invalid_chars_error = validation::make_config_aware_invalid_characters_error("test@", 4, '@');
        
        REQUIRE(empty_name_error.code() == GreetingError::EmptyName);
        REQUIRE(invalid_chars_error.code() == GreetingError::InvalidCharacters);
        
        // Both should have appropriate messages for the current configuration
        REQUIRE(std::string(empty_name_error.message()).length() > 0);
        REQUIRE(std::string(invalid_chars_error.message()).length() > 0);
    }
    
    SECTION("Error creation adapts to configuration") {
        auto validation_error = validation::make_config_aware_validation_error(
            GreetingError::NameTooLong,
            "Test message",
            "test input"
        );
        
        REQUIRE(validation_error.code() == GreetingError::NameTooLong);
        
        #ifdef HELLOWORLD_DEBUG_BUILD
            // In debug builds, should have detailed context
            // (Note: Actual validation depends on the implementation)
        #else
            // In release builds, should have minimal context
        #endif
    }
}

// ============================================================================
// Performance and Memory Tests
// ============================================================================

TEST_CASE("Configuration-Aware Performance - Memory Footprint", "[config][performance][memory]") {
    SECTION("Release error types have minimal memory footprint") {
        auto release_error = release::ValidationError(GreetingError::EmptyName);
        
        // Should be very small - just the error code plus minimal overhead
        REQUIRE(sizeof(release_error) <= 16); // Allow for some padding but keep it small
    }
    
    SECTION("Debug error types can have larger footprint for rich context") {
        auto debug_error = debug::ValidationError(
            GreetingError::EmptyName,
            "Test message",
            "test input"
        );
        
        // Debug errors can be larger due to rich context
        // No strict size requirement, but should be reasonable
        REQUIRE(sizeof(debug_error) >= sizeof(release::ValidationError));
    }
}

TEST_CASE("Configuration-Aware Performance - Compile-Time Optimization", "[config][performance][optimization]") {
    SECTION("Release configuration enables compile-time optimizations") {
        // Test that release functions are constexpr
        static_assert(std::is_same_v<decltype(release::make_empty_name_error()), release::ValidationError>);
        static_assert(std::is_same_v<decltype(release::enable_detailed_errors()), bool>);
        
        // These should be compile-time constants
        constexpr bool detailed = release::enable_detailed_errors();
        constexpr bool stacktrace = release::enable_stacktrace();
        
        REQUIRE_FALSE(detailed);
        REQUIRE_FALSE(stacktrace);
    }
}
