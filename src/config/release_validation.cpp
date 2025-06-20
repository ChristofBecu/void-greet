// ============================================================================
// Release Validation Implementation
// 
// This file provides optimized validation logic for release builds,
// focusing on performance while maintaining correctness.
// ============================================================================

#include "greeting/config_aware_validation.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"
#include "config/build_config.hpp"

namespace greeting::validation {

// ============================================================================
// Release Character Validation (Optimized)
// ============================================================================

/**
 * @brief Release character validation optimized for performance
 * 
 * In release builds, we perform fast, essential validation only:
 * - ASCII character validation
 * - Minimal character set checking
 * - Performance-optimized logic
 */
bool validate_name_character_release(char c) noexcept {
    // Fast ASCII alphabetic validation - optimized for common cases
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
        return true;
    }
    
    // Common name characters - optimized lookup
    switch (c) {
        case ' ':
        case '-':
        case '\'':
        case '.':
            return true;
        default:
            return false;
    }
}

/**
 * @brief Release string length validation (fast path)
 */
bool validate_string_length_release(std::string_view str, size_t min_len, size_t max_len) noexcept {
    const size_t len = str.length();
    return len >= min_len && len <= max_len && len > 0;
}

// ============================================================================
// Release Person Name Validation (Optimized)
// ============================================================================

/**
 * @brief Release person name validation optimized for performance
 */
template<typename NameType>
ConfigAwareValidationResult<NameType> validate_person_name_release(std::string_view name) {
    // Fast length check
    if (!validate_string_length_release(name, 1, 100)) {
        if (name.empty()) {
            return Expected<NameType, GreetingError>{GreetingError::EmptyName};
        } else if (name.length() > 100) {
            return Expected<NameType, GreetingError>{GreetingError::NameTooLong};
        } else {
            return Expected<NameType, GreetingError>{GreetingError::NameTooShort};
        }
    }
    
    // Fast character validation - optimized loop
    for (char c : name) {
        if (!validate_name_character_release(c)) {
            return Expected<NameType, GreetingError>{GreetingError::InvalidCharacters};
        }
    }
    
    // Direct construction for performance - use the create method
    auto result = NameType::create(name);
    if (result.has_value()) {
        return Expected<NameType, GreetingError>{result.value()};
    } else {
        return Expected<NameType, GreetingError>{GreetingError::InvalidName};
    }
}

// ============================================================================
// Release Greeting Message Validation (Optimized)
// ============================================================================

/**
 * @brief Release greeting message validation optimized for performance
 */
template<typename MessageType>
ConfigAwareValidationResult<MessageType> validate_greeting_message_release(std::string_view message) {
    // Fast length check
    if (!validate_string_length_release(message, 1, 500)) {
        if (message.empty()) {
            return Expected<MessageType, GreetingError>{GreetingError::EmptyMessage};
        } else if (message.length() > 500) {
            return Expected<MessageType, GreetingError>{GreetingError::MessageTooLong};
        } else {
            return Expected<MessageType, GreetingError>{GreetingError::InvalidMessage};
        }
    }
    
    // Minimal content validation in release mode
    // Only check for completely invalid characters (control characters)
    for (char c : message) {
        if (c < 32 && c != '\t' && c != '\n' && c != '\r') {
            return Expected<MessageType, GreetingError>{GreetingError::InvalidCharacters};
        }
    }
    
    // Direct construction for performance - use the create method
    auto result = MessageType::create(message);
    if (result.has_value()) {
        return Expected<MessageType, GreetingError>{result.value()};
    } else {
        return Expected<MessageType, GreetingError>{GreetingError::InvalidMessage};
    }
}

// ============================================================================
// Explicit Template Instantiations
// ============================================================================

// Ensure the templates are available for common types
template ConfigAwareValidationResult<PersonName> validate_person_name_release<PersonName>(std::string_view);
template ConfigAwareValidationResult<GreetingMessage> validate_greeting_message_release<GreetingMessage>(std::string_view);

} // namespace greeting::validation
