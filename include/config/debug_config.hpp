#pragma once

// ============================================================================
// Debug Configuration Header
// 
// This header provides debug-specific configuration types and utilities for
// enhanced error handling, detailed diagnostics, and comprehensive validation
// in debug builds.
// ============================================================================

#include "greeting/error_codes.hpp"
#include <string>
#include <vector>

// Check for C++20/23 source_location support
#if __has_include(<source_location>) && __cpp_lib_source_location >= 201907L
#include <source_location>
#define HELLOWORLD_HAS_SOURCE_LOCATION 1
#else
#define HELLOWORLD_HAS_SOURCE_LOCATION 0
#endif

// Note: Disabling stacktrace for now due to linker issues in some GCC versions
// #ifdef HELLOWORLD_HAS_STACKTRACE
// #include <stacktrace>
// #else
// // Disable stacktrace for now due to linker issues in some GCC versions
// #undef HELLOWORLD_HAS_STACKTRACE
// #endif

namespace greeting::config::debug {

// ============================================================================
// Debug Error Context Types
// ============================================================================

#if HELLOWORLD_HAS_SOURCE_LOCATION
using SourceLocation = std::source_location;
#else
// Fallback source location for older compilers
struct SourceLocation {
    static constexpr SourceLocation current() noexcept {
        return SourceLocation{};
    }
    
    constexpr const char* file_name() const noexcept {
        return "unknown";
    }
    
    constexpr std::uint_least32_t line() const noexcept {
        return 0;
    }
    
    constexpr std::uint_least32_t column() const noexcept {
        return 0;
    }
};
#endif

/**
 * @brief Extended error context for debug builds
 * 
 * Provides comprehensive error information including:
 * - Detailed error messages
 * - Source location information (when available)
 * - Input validation context
 * - Optional stacktrace (when available)
 */
struct DetailedErrorContext {
    GreetingError error_code;           ///< The specific error that occurred
    std::string detailed_message;      ///< Human-readable error description
    std::string input_value;           ///< The input that caused the error
    SourceLocation location;           ///< Source location where error occurred
    
    // Note: Stacktrace disabled due to linker issues in some GCC versions
    // #ifdef HELLOWORLD_HAS_STACKTRACE
    // std::stacktrace trace;              ///< Call stack when error occurred
    // #endif
    
    // Additional debug context
    size_t input_position = 0;          ///< Position in input where error occurred
    std::vector<std::string> suggestions; ///< Suggested fixes or alternatives
    
    /**
     * @brief Construct detailed error context
     */
    DetailedErrorContext(
        GreetingError code,
        std::string message,
        std::string input = "",
        SourceLocation loc = SourceLocation::current()
    ) : error_code(code), 
        detailed_message(std::move(message)), 
        input_value(std::move(input)),
        location(loc) {
        // Note: Stacktrace disabled due to linker issues in some GCC versions
        // #ifdef HELLOWORLD_HAS_STACKTRACE
        //     trace = std::stacktrace::current();
        // #endif
    }
    
    /**
     * @brief Get formatted error message with all context
     */
    std::string format_detailed_message() const {
        std::string result = "Error: " + detailed_message;
        result += " (Code: " + std::to_string(static_cast<int>(error_code)) + ")";
        
        if (!input_value.empty()) {
            result += "\nInput: \"" + input_value + "\"";
            if (input_position > 0) {
                result += " (position " + std::to_string(input_position) + ")";
            }
        }
        
        result += "\nLocation: " + std::string(location.file_name()) + 
                  ":" + std::to_string(location.line()) + 
                  ":" + std::to_string(location.column());
        
        if (!suggestions.empty()) {
            result += "\nSuggestions:";
            for (const auto& suggestion : suggestions) {
                result += "\n  - " + suggestion;
            }
        }
        
        // Note: Stacktrace disabled due to linker issues in some GCC versions
        // #ifdef HELLOWORLD_HAS_STACKTRACE
        //     result += "\nStacktrace:\n" + std::to_string(trace);
        // #endif
        
        return result;
    }
};

/**
 * @brief Debug-specific validation error with rich context
 */
struct ValidationError {
    DetailedErrorContext context;
    
    explicit ValidationError(
        GreetingError code,
        const std::string& message,
        const std::string& input = "",
        SourceLocation loc = SourceLocation::current()
    ) : context(code, message, input, loc) {}
    
    /**
     * @brief Add a suggestion for fixing the error
     */
    void add_suggestion(const std::string& suggestion) {
        context.suggestions.push_back(suggestion);
    }
    
    /**
     * @brief Set the position in input where error occurred
     */
    void set_input_position(size_t position) {
        context.input_position = position;
    }
    
    /**
     * @brief Get the underlying error code
     */
    GreetingError code() const noexcept {
        return context.error_code;
    }
    
    /**
     * @brief Get formatted error message
     */
    std::string message() const {
        return context.format_detailed_message();
    }
};

// ============================================================================
// Debug Configuration Functions
// ============================================================================

/**
 * @brief Check if detailed error context is enabled
 */
constexpr bool enable_detailed_errors() noexcept {
    #ifdef HELLOWORLD_DEBUG_BUILD
        return true;
    #else
        return false;
    #endif
}

/**
 * @brief Check if stacktrace capture is enabled
 */
constexpr bool enable_stacktrace() noexcept {
    // Note: Stacktrace disabled due to linker issues in some GCC versions
    // #if defined(HELLOWORLD_DEBUG_BUILD) && defined(HELLOWORLD_HAS_STACKTRACE)
    //     return true;
    // #else
        return false;
    // #endif
}

/**
 * @brief Check if input position tracking is enabled
 */
constexpr bool enable_position_tracking() noexcept {
    #ifdef HELLOWORLD_DEBUG_BUILD
        return true;
    #else
        return false;
    #endif
}

/**
 * @brief Check if error suggestions are enabled
 */
constexpr bool enable_error_suggestions() noexcept {
    #ifdef HELLOWORLD_DEBUG_BUILD
        return true;
    #else
        return false;
    #endif
}

// ============================================================================
// Debug Error Creation Utilities
// ============================================================================

/**
 * @brief Create a detailed validation error for debug builds
 */
inline ValidationError make_validation_error(
    GreetingError code,
    const std::string& message,
    const std::string& input = "",
    SourceLocation loc = SourceLocation::current()
) {
    return ValidationError(code, message, input, loc);
}

/**
 * @brief Create an empty name error with debug context
 */
inline ValidationError make_empty_name_error(
    const std::string& input,
    SourceLocation loc = SourceLocation::current()
) {
    auto error = make_validation_error(
        GreetingError::EmptyName,
        "Person name cannot be empty or contain only whitespace",
        input,
        loc
    );
    error.add_suggestion("Provide a non-empty name with at least one alphabetic character");
    error.add_suggestion("Remove leading and trailing whitespace");
    return error;
}

/**
 * @brief Create an invalid characters error with debug context
 */
inline ValidationError make_invalid_characters_error(
    const std::string& input,
    size_t position,
    char invalid_char,
    SourceLocation loc = SourceLocation::current()
) {
    auto error = make_validation_error(
        GreetingError::InvalidCharacters,
        "Name contains invalid character '" + std::string(1, invalid_char) + "'",
        input,
        loc
    );
    error.set_input_position(position);
    error.add_suggestion("Use only alphabetic characters, spaces, hyphens, apostrophes, and periods");
    error.add_suggestion("Remove or replace the invalid character");
    return error;
}

/**
 * @brief Create a name too long error with debug context
 */
inline ValidationError make_name_too_long_error(
    const std::string& input,
    size_t max_length,
    SourceLocation loc = SourceLocation::current()
) {
    auto error = make_validation_error(
        GreetingError::NameTooLong,
        "Name length (" + std::to_string(input.length()) + 
        ") exceeds maximum allowed length (" + std::to_string(max_length) + ")",
        input,
        loc
    );
    error.add_suggestion("Shorten the name to " + std::to_string(max_length) + " characters or less");
    error.add_suggestion("Consider using initials or abbreviated forms");
    return error;
}

/**
 * @brief Create an empty message error with debug context
 */
inline ValidationError make_empty_message_error(
    const std::string& input,
    SourceLocation loc = SourceLocation::current()
) {
    auto error = make_validation_error(
        GreetingError::EmptyMessage,
        "Greeting message cannot be empty",
        input,
        loc
    );
    error.add_suggestion("Provide a non-empty greeting message");
    error.add_suggestion("Use a standard greeting like 'Hello' or 'Hi'");
    return error;
}

/**
 * @brief Create a message too long error with debug context
 */
inline ValidationError make_message_too_long_error(
    const std::string& input,
    size_t max_length,
    SourceLocation loc = SourceLocation::current()
) {
    auto error = make_validation_error(
        GreetingError::MessageTooLong,
        "Message length (" + std::to_string(input.length()) + 
        ") exceeds maximum allowed length (" + std::to_string(max_length) + ")",
        input,
        loc
    );
    error.add_suggestion("Shorten the message to " + std::to_string(max_length) + " characters or less");
    error.add_suggestion("Split into multiple shorter messages");
    return error;
}

} // namespace greeting::config::debug
