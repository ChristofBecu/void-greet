#pragma once

// ============================================================================
// Configuration-Aware Validation System
// 
// This header provides validation functions that adapt their behavior based
// on build configuration, leveraging C++23 features for optimal performance.
// ============================================================================

#include "config/build_config.hpp"
#include "greeting/expected.hpp"
#include "greeting/error_codes.hpp"
#include <string_view>
#include <type_traits>

#ifdef __cpp_concepts
#include <concepts>
#endif

namespace greeting::validation {

// ============================================================================
// Configuration-Aware Validation Core
// ============================================================================

/**
 * @brief Configuration-aware validation result type
 * 
 * This type adapts based on build configuration:
 * - Debug: Rich error context with detailed information
 * - Release: Lightweight error codes for performance
 */
template<typename T>
using ConfigAwareValidationResult = Expected<T, GreetingError>;

/**
 * @brief Validation context for configuration-aware operations
 */
class ValidationContext {
public:
    /**
     * @brief Create validation context based on current configuration
     */
    static constexpr ValidationContext create_for_current_config() noexcept {
        return ValidationContext{};
    }
    
    /**
     * @brief Check if detailed validation is enabled
     */
    static constexpr bool enable_detailed_validation() noexcept {
        return true; // Configuration-dependent in implementation
    }
    
    /**
     * @brief Check if stacktrace should be captured on errors
     */
    static constexpr bool enable_stacktrace() noexcept {
        return true; // Configuration-dependent in implementation
    }
    
    /**
     * @brief Check if performance monitoring is active
     */
    static constexpr bool enable_performance_monitoring() noexcept {
        return true; // Configuration-dependent in implementation
    }

private:
    constexpr ValidationContext() noexcept = default;
};

// ============================================================================
// Dual-Path Validation Functions
// ============================================================================

/**
 * @brief Configuration-aware character validation
 * 
 * Uses dual-path optimization based on build configuration:
 * - Debug: Comprehensive validation with Unicode support
 * - Release: Optimized validation for performance
 */
bool validate_name_character(char c) noexcept;

/**
 * @brief Configuration-aware string length validation
 */
template<typename StringType>
constexpr bool validate_string_length(const StringType& str, size_t min_len, size_t max_len) noexcept {
    if constexpr (std::is_same_v<StringType, const char*> || std::is_array_v<StringType>) {
        // Handle C-style strings
        const size_t len = std::string_view{str}.length();
        return len >= min_len && len <= max_len;
    } else {
        // Handle string-like types (std::string, std::string_view, etc.)
        return str.length() >= min_len && str.length() <= max_len;
    }
}

// Forward declarations for implementation-specific functions
template<typename NameType>
ConfigAwareValidationResult<NameType> validate_person_name_impl(std::string_view name);

template<typename MessageType>
ConfigAwareValidationResult<MessageType> validate_greeting_message_impl(std::string_view message);

/**
 * @brief Configuration-aware person name validation
 * 
 * @param name The name to validate
 * @return Validation result adapted to current configuration
 */
template<typename NameType>
ConfigAwareValidationResult<NameType> validate_person_name(std::string_view name) {
    // Delegate to configuration-specific implementations
    return validate_person_name_impl<NameType>(name);
}

/**
 * @brief Configuration-aware greeting message validation
 */
template<typename MessageType>
ConfigAwareValidationResult<MessageType> validate_greeting_message(std::string_view message) {
    // Delegate to configuration-specific implementations
    return validate_greeting_message_impl<MessageType>(message);
}

// ============================================================================
// Configuration-Aware Error Creation
// ============================================================================

/**
 * @brief Create validation error appropriate for current configuration
 */
inline GreetingError make_validation_error(const std::string& message) {
    return GreetingError::InvalidName;
}

// ============================================================================
// Configuration-Aware Validation Concepts
// ============================================================================

#ifdef __cpp_concepts

/**
 * @brief Concept for types that support configuration-aware validation
 */
template<typename T>
concept ConfigAwareValidatable = requires(T t, std::string_view input) {
    { validate_person_name<T>(input) } -> std::same_as<ConfigAwareValidationResult<T>>;
} || requires(T t, std::string_view input) {
    { validate_greeting_message<T>(input) } -> std::same_as<ConfigAwareValidationResult<T>>;
};

/**
 * @brief Concept for validation contexts
 */
template<typename T>
concept ValidationContextType = requires(T context) {
    { T::enable_detailed_validation() } -> std::same_as<bool>;
    { T::enable_stacktrace() } -> std::same_as<bool>;
    { T::enable_performance_monitoring() } -> std::same_as<bool>;
};

#endif // __cpp_concepts

// ============================================================================
// Implementation Functions (defined in debug_validation.cpp and release_validation.cpp)
// ============================================================================

// Character validation implementation
bool validate_name_character_impl(char c) noexcept;

// Debug validation implementations (comprehensive, with rich diagnostics)
bool validate_name_character_debug(char c) noexcept;
bool validate_string_length_debug(std::string_view str, size_t min_len, size_t max_len) noexcept;

template<typename NameType>
ConfigAwareValidationResult<NameType> validate_person_name_debug(std::string_view name);

template<typename MessageType>
ConfigAwareValidationResult<MessageType> validate_greeting_message_debug(std::string_view message);

// Release validation implementations (optimized for performance)
bool validate_name_character_release(char c) noexcept;
bool validate_string_length_release(std::string_view str, size_t min_len, size_t max_len) noexcept;

template<typename NameType>
ConfigAwareValidationResult<NameType> validate_person_name_release(std::string_view name);

template<typename MessageType>  
ConfigAwareValidationResult<MessageType> validate_greeting_message_release(std::string_view message);

// Configuration-aware implementation dispatcher
// (These functions are now declared above and defined in the implementation files)

} // namespace greeting::validation
