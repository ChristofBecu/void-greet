#pragma once

// ============================================================================
// Release Configuration Header
// 
// This header provides release-specific configuration types optimized for
// minimal memory footprint, zero-overhead error handling, and maximum
// performance in production builds.
// ============================================================================

#include "greeting/error_codes.hpp"

namespace greeting::config::release {

// ============================================================================
// Minimal Error Context Types
// ============================================================================

/**
 * @brief Minimal error context for release builds
 * 
 * Optimized for zero-overhead error handling with minimal memory footprint.
 * Contains only essential error information needed for proper error propagation.
 */
struct MinimalErrorContext {
    GreetingError error_code;           ///< The specific error that occurred
    
    /**
     * @brief Construct minimal error context
     */
    explicit constexpr MinimalErrorContext(GreetingError code) noexcept 
        : error_code(code) {}
    
    /**
     * @brief Default constructor for no error state
     */
    constexpr MinimalErrorContext() noexcept 
        : error_code(static_cast<GreetingError>(0)) {}
    
    /**
     * @brief Check if this represents an error
     */
    constexpr bool has_error() const noexcept {
        return error_code != static_cast<GreetingError>(0);
    }
    
    /**
     * @brief Get basic error message (compile-time known)
     */
    constexpr const char* get_error_message() const noexcept {
        if (!has_error()) {
            return "No error";
        }
        
        switch (error_code) {
            case GreetingError::EmptyName:
                return "Empty name";
            case GreetingError::InvalidCharacters:
                return "Invalid characters";
            case GreetingError::NameTooLong:
                return "Name too long";
            case GreetingError::EmptyMessage:
                return "Empty message";
            case GreetingError::MessageTooLong:
                return "Message too long";
            default:
                return "Unknown error";
        }
    }
};

/**
 * @brief Release-specific validation error with minimal overhead
 */
struct ValidationError {
    MinimalErrorContext context;
    
    /**
     * @brief Construct validation error with minimal context
     */
    explicit constexpr ValidationError(GreetingError code) noexcept 
        : context(code) {}
    
    /**
     * @brief Construct success result
     */
    constexpr ValidationError() noexcept 
        : context() {}
    
    /**
     * @brief Get the underlying error code
     */
    constexpr GreetingError code() const noexcept {
        return context.error_code;
    }
    
    /**
     * @brief Get basic error message
     */
    constexpr const char* message() const noexcept {
        return context.get_error_message();
    }
    
    /**
     * @brief Check if this represents an error
     */
    constexpr bool has_error() const noexcept {
        return context.error_code != static_cast<GreetingError>(0);
    }
    
    /**
     * @brief Implicit conversion to bool (false if error)
     */
    constexpr operator bool() const noexcept {
        return !has_error();
    }
};

// ============================================================================
// Release Configuration Functions
// ============================================================================

/**
 * @brief Check if detailed error context is enabled (always false in release)
 */
constexpr bool enable_detailed_errors() noexcept {
    return false;
}

/**
 * @brief Check if stacktrace capture is enabled (always false in release)
 */
constexpr bool enable_stacktrace() noexcept {
    return false;
}

/**
 * @brief Check if input position tracking is enabled (always false in release)
 */
constexpr bool enable_position_tracking() noexcept {
    return false;
}

/**
 * @brief Check if error suggestions are enabled (always false in release)
 */
constexpr bool enable_error_suggestions() noexcept {
    return false;
}

// ============================================================================
// Release Error Creation Utilities (Zero-Overhead)
// ============================================================================

/**
 * @brief Create a minimal validation error for release builds
 */
constexpr ValidationError make_validation_error(GreetingError code) noexcept {
    return ValidationError(code);
}

/**
 * @brief Create an empty name error with minimal context
 */
constexpr ValidationError make_empty_name_error() noexcept {
    return ValidationError(GreetingError::EmptyName);
}

/**
 * @brief Create an invalid characters error with minimal context
 */
constexpr ValidationError make_invalid_characters_error() noexcept {
    return ValidationError(GreetingError::InvalidCharacters);
}

/**
 * @brief Create a name too long error with minimal context
 */
constexpr ValidationError make_name_too_long_error() noexcept {
    return ValidationError(GreetingError::NameTooLong);
}

/**
 * @brief Create an empty message error with minimal context
 */
constexpr ValidationError make_empty_message_error() noexcept {
    return ValidationError(GreetingError::EmptyMessage);
}

/**
 * @brief Create a message too long error with minimal context
 */
constexpr ValidationError make_message_too_long_error() noexcept {
    return ValidationError(GreetingError::MessageTooLong);
}

// ============================================================================
// Release Optimization Utilities
// ============================================================================

/**
 * @brief Template for zero-overhead error handling
 * 
 * In release builds, this can be optimized away entirely by the compiler
 * when error conditions are compile-time determinable.
 */
template<GreetingError ErrorCode>
constexpr ValidationError make_compile_time_error() noexcept {
    return ValidationError(ErrorCode);
}

/**
 * @brief Check if error handling can be optimized away at compile time
 */
template<typename T>
constexpr bool can_optimize_error_handling() noexcept {
    // This can be specialized for specific types to enable compile-time optimization
    return std::is_trivial_v<T> && std::is_standard_layout_v<T>;
}

} // namespace greeting::config::release
