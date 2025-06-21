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
#include "config/release_config.hpp"

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
            // Allow extended ASCII for international names (release mode)
            unsigned char uc = static_cast<unsigned char>(c);
            return uc >= 128;  // Allow extended ASCII
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
    // Fast empty check
    if (name.empty()) {
        return Expected<NameType, GreetingError>{GreetingError::EmptyName};
    }
    
    // Fast whitespace-only check for release builds (simplified)
    bool has_non_whitespace = false;
    for (char c : name) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            has_non_whitespace = true;
            break;
        }
    }
    if (!has_non_whitespace) {
        return Expected<NameType, GreetingError>{GreetingError::EmptyName};
    }
    
    // Fast length check
    if (name.length() > 100) {
        return Expected<NameType, GreetingError>{GreetingError::NameTooLong};
    }
    if (name.length() < 2) {
        return Expected<NameType, GreetingError>{GreetingError::NameTooShort};
    }
    
    // Fast character validation - optimized loop
    for (char c : name) {
        if (!validate_name_character_release(c)) {
            return Expected<NameType, GreetingError>{GreetingError::InvalidName};
        }
    }
    
    // Check for names starting/ending with special characters (release mode - minimal checks)
    char first_char = name.front();
    char last_char = name.back();
    
    if (first_char == '-' || first_char == '\'' || first_char == '.') {
        return Expected<NameType, GreetingError>{GreetingError::InvalidName};
    }
    if (last_char == '-' || last_char == '\'' || (last_char == '.' && name != "Dr.")) {
        return Expected<NameType, GreetingError>{GreetingError::InvalidName};
    }
    
    // Direct construction for performance - use internal constructor
    NameType validated_name{std::string{name}, typename NameType::InternalTag{}};
    return Expected<NameType, GreetingError>{std::move(validated_name)};
}

// ============================================================================
// Release Greeting Message Validation (Optimized)
// ============================================================================

/**
 * @brief Release greeting message validation optimized for performance
 */
template<typename MessageType>
ConfigAwareValidationResult<MessageType> validate_greeting_message_release(std::string_view message) {
    // Fast empty check
    if (message.empty()) {
        return Expected<MessageType, GreetingError>{GreetingError::EmptyMessage};
    }
    
    // Fast whitespace-only check for release builds
    bool has_non_whitespace = false;
    for (char c : message) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            has_non_whitespace = true;
            break;
        }
    }
    if (!has_non_whitespace) {
        return Expected<MessageType, GreetingError>{GreetingError::EmptyMessage};
    }
    
    // Fast length check
    if (message.length() > 500) {
        return Expected<MessageType, GreetingError>{GreetingError::MessageTooLong};
    }
    
    // Minimal content validation in release mode
    // Only check for actual control characters that could cause display issues
    for (char c : message) {
        // Only reject actual control characters (0-31) except allowed whitespace
        // Allow all characters >= 32 (including extended ASCII and UTF-8 sequences)
        if (c >= 0 && c < 32 && c != '\t' && c != '\n' && c != '\r') {
            return Expected<MessageType, GreetingError>{GreetingError::InvalidMessage};
        }
    }
    
    // Direct construction for performance - use internal constructor
    MessageType validated_message{std::string{message}, typename MessageType::InternalTag{}};
    return Expected<MessageType, GreetingError>{std::move(validated_message)};
}

// ============================================================================
// Explicit Template Instantiations
// ============================================================================

// Ensure the templates are available for common types
template ConfigAwareValidationResult<PersonName> validate_person_name_release<PersonName>(std::string_view);
template ConfigAwareValidationResult<GreetingMessage> validate_greeting_message_release<GreetingMessage>(std::string_view);

} // namespace greeting::validation
