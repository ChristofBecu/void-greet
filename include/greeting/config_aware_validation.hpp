#pragma once

// ============================================================================
// Configuration-Aware Validation System
// 
// This header provides validation functions that adapt their behavior based
// on build configuration, leveraging C++23 features for optimal performance.
// ============================================================================

#include "config/build_config.hpp"
#include "config/debug_config.hpp"
#include "config/release_config.hpp"
#include "greeting/expected.hpp"
#include "greeting/error_codes.hpp"
#include "greeting/config_aware_type_system.hpp"
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
 * @brief Configuration-aware validation error type
 * 
 * Selects appropriate error type based on build configuration:
 * - Debug: Detailed error context with suggestions and source location
 * - Release: Minimal error context for zero-overhead
 */
#ifdef HELLOWORLD_DEBUG_BUILD
using ValidationError = config::debug::ValidationError;
#else
using ValidationError = config::release::ValidationError;
#endif

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
        #ifdef HELLOWORLD_DEBUG_BUILD
            return config::debug::enable_detailed_errors();
        #else
            return config::release::enable_detailed_errors();
        #endif
    }
    
    /**
     * @brief Check if stacktrace should be captured on errors
     */
    static constexpr bool enable_stacktrace() noexcept {
        #ifdef HELLOWORLD_DEBUG_BUILD
            return config::debug::enable_stacktrace();
        #else
            return config::release::enable_stacktrace();
        #endif
    }
    
    /**
     * @brief Check if performance monitoring is active
     */
    static constexpr bool enable_performance_monitoring() noexcept {
        #ifdef HELLOWORLD_DEBUG_BUILD
            return config::debug::enable_position_tracking();
        #else
            return config::release::enable_position_tracking();
        #endif
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
    requires std::same_as<NameType, PersonName>
ConfigAwareValidationResult<NameType> validate_person_name(std::string_view name) {
    // Assert zero-overhead properties in debug builds
    if constexpr (greeting::types::is_debug_configuration()) {
        greeting::types::performance::assert_zero_overhead<NameType>();
    }
    
    // Delegate to configuration-specific implementations
    return validate_person_name_impl<NameType>(name);
}

/**
 * @brief Configuration-aware greeting message validation
 */
template<typename MessageType>
    requires std::same_as<MessageType, GreetingMessage>  
ConfigAwareValidationResult<MessageType> validate_greeting_message(std::string_view message) {
    // Assert zero-overhead properties in debug builds
    if constexpr (greeting::types::is_debug_configuration()) {
        greeting::types::performance::assert_zero_overhead<MessageType>();
    }
    
    // Delegate to configuration-specific implementations
    return validate_greeting_message_impl<MessageType>(message);
}

// ============================================================================
// Configuration-Aware Error Creation
// ============================================================================

/**
 * @brief Create validation error appropriate for current configuration
 */
#ifdef HELLOWORLD_DEBUG_BUILD
inline ValidationError make_config_aware_validation_error(
    GreetingError code,
    const std::string& message,
    const std::string& input = ""
) {
    return config::debug::make_validation_error(code, message, input);
}

inline ValidationError make_config_aware_empty_name_error(const std::string& input) {
    return config::debug::make_empty_name_error(input);
}

inline ValidationError make_config_aware_invalid_characters_error(
    const std::string& input, 
    size_t position, 
    char invalid_char
) {
    return config::debug::make_invalid_characters_error(input, position, invalid_char);
}

inline ValidationError make_config_aware_name_too_long_error(
    const std::string& input,
    size_t max_length
) {
    return config::debug::make_name_too_long_error(input, max_length);
}

inline ValidationError make_config_aware_empty_message_error(const std::string& input) {
    return config::debug::make_empty_message_error(input);
}

inline ValidationError make_config_aware_message_too_long_error(
    const std::string& input,
    size_t max_length
) {
    return config::debug::make_message_too_long_error(input, max_length);
}

#else

inline ValidationError make_config_aware_validation_error(
    GreetingError code,
    const std::string& = "",
    const std::string& = ""
) {
    return config::release::make_validation_error(code);
}

inline ValidationError make_config_aware_empty_name_error(const std::string& = "") {
    return config::release::make_empty_name_error();
}

inline ValidationError make_config_aware_invalid_characters_error(
    const std::string& = "", 
    size_t = 0, 
    char = '\0'
) {
    return config::release::make_invalid_characters_error();
}

inline ValidationError make_config_aware_name_too_long_error(
    const std::string& = "",
    size_t = 0
) {
    return config::release::make_name_too_long_error();
}

inline ValidationError make_config_aware_empty_message_error(const std::string& = "") {
    return config::release::make_empty_message_error();
}

inline ValidationError make_config_aware_message_too_long_error(
    const std::string& = "",
    size_t = 0
) {
    return config::release::make_message_too_long_error();
}

#endif

// ============================================================================
// Configuration-Aware Validation Concepts
// ============================================================================

#ifdef __cpp_concepts

// Note: ConfigAwareValidatable concept is now defined in config_aware_type_system.hpp
// to avoid circular dependencies and provide better type system organization

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
