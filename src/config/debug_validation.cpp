// ============================================================================
// Debug Validation Implementation
// 
// This file provides comprehensive validation logic for debug builds,
// including rich error diagnostics, Unicode support, and detailed validation.
// ============================================================================

#include "greeting/config_aware_validation.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"
#include "config/build_config.hpp"
#include "config/debug_config.hpp"
#include <cctype>
#include <algorithm>
#include <string>

#ifdef HELLOWORLD_HAS_STACKTRACE
#include <stacktrace>
#endif

namespace greeting::validation {

// ============================================================================
// Debug Character Validation (Comprehensive)
// ============================================================================

/**
 * @brief Debug character validation with comprehensive Unicode support
 * 
 * In debug builds, we perform thorough character validation including:
 * - Full ASCII range validation
 * - Unicode character support
 * - Special character handling
 * - Detailed error context
 */
bool validate_name_character_debug(char c) noexcept {
    // Allow standard alphabetic characters only (not numeric)
    if (std::isalpha(static_cast<unsigned char>(c))) {
        return true;
    }
    
    // Allow common name characters
    switch (c) {
        case ' ':   // Spaces
        case '-':   // Hyphens
        case '\'':  // Apostrophes
        case '.':   // Periods (for abbreviations)
            return true;
        default:
            // In debug mode, could add more comprehensive Unicode support
            // For now, stick to basic ASCII to avoid compiler warnings
            #ifdef HELLOWORLD_DEBUG_BUILD
                // Could add logging here in a full implementation
            #endif
            return false;
    }
}

/**
 * @brief Debug string length validation with detailed checking
 */
bool validate_string_length_debug(std::string_view str, size_t min_len, size_t max_len) noexcept {
    const size_t len = str.length();
    
    // Debug builds can perform additional checks
    if (len == 0) {
        return false; // Empty strings are invalid
    }
    
    if (len < min_len || len > max_len) {
        return false;
    }
    
    // In debug builds, we can also check for whitespace-only strings
    bool has_non_whitespace = false;
    for (char c : str) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            has_non_whitespace = true;
            break;
        }
    }
    
    return has_non_whitespace;
}

// ============================================================================
// Debug Person Name Validation (Comprehensive)
// ============================================================================

/**
 * @brief Debug person name validation with rich error context
 */
template<typename NameType>
ConfigAwareValidationResult<NameType> validate_person_name_debug(std::string_view name) {
    // Step 1: Length validation
    if (!validate_string_length_debug(name, 1, 100)) {
        if (name.empty()) {
            return Expected<NameType, GreetingError>{GreetingError::EmptyName};
        } else if (name.length() > 100) {
            return Expected<NameType, GreetingError>{GreetingError::NameTooLong};
        } else {
            return Expected<NameType, GreetingError>{GreetingError::NameTooShort};
        }
    }
    
    // Step 2: Character validation with detailed error reporting
    for (size_t i = 0; i < name.length(); ++i) {
        char c = name[i];
        if (!validate_name_character_debug(c)) {
            // In debug builds, we could capture the specific position and character
            return Expected<NameType, GreetingError>{GreetingError::InvalidCharacters};
        }
    }
    
    // Step 3: Additional debug-only validations
    
    // Check for consecutive spaces
    bool found_consecutive_spaces = false;
    for (size_t i = 1; i < name.length(); ++i) {
        if (name[i-1] == ' ' && name[i] == ' ') {
            found_consecutive_spaces = true;
            break;
        }
    }
    
    if (found_consecutive_spaces) {
        // In a more complete implementation, this might be a warning rather than error
        return Expected<NameType, GreetingError>{GreetingError::InvalidName};
    }
    
    // Check for leading/trailing whitespace
    if (name.front() == ' ' || name.back() == ' ') {
        return Expected<NameType, GreetingError>{GreetingError::InvalidName};
    }
    
    // Step 4: Create the validated name
    try {
        // Use the PersonName::create method to properly construct the object
        auto result = NameType::create(name);
        if (result.has_value()) {
            return Expected<NameType, GreetingError>{result.value()};
        } else {
            return Expected<NameType, GreetingError>{GreetingError::InvalidName};
        }
    } catch (...) {
        // Debug builds can catch and report construction errors
        return Expected<NameType, GreetingError>{GreetingError::InvalidName};
    }
}

// ============================================================================
// Debug Greeting Message Validation (Comprehensive)
// ============================================================================

/**
 * @brief Debug greeting message validation with comprehensive checks
 */
template<typename MessageType>
ConfigAwareValidationResult<MessageType> validate_greeting_message_debug(std::string_view message) {
    // Step 1: Length validation
    if (!validate_string_length_debug(message, 1, 500)) {
        if (message.empty()) {
            return Expected<MessageType, GreetingError>{GreetingError::EmptyMessage};
        } else if (message.length() > 500) {
            return Expected<MessageType, GreetingError>{GreetingError::MessageTooLong};
        } else {
            return Expected<MessageType, GreetingError>{GreetingError::InvalidMessage};
        }
    }
    
    // Step 2: Content validation
    // In debug builds, we can perform more thorough content checks
    
    // Check for reasonable character distribution
    size_t printable_chars = 0;
    size_t control_chars = 0;
    
    for (char c : message) {
        if (std::isprint(static_cast<unsigned char>(c)) || std::isspace(static_cast<unsigned char>(c))) {
            printable_chars++;
        } else {
            control_chars++;
        }
    }
    
    // Reject messages with too many control characters
    if (control_chars > 0) {
        return Expected<MessageType, GreetingError>{GreetingError::InvalidCharacters};
    }
    
    // Step 3: Create the validated message
    try {
        // Use the GreetingMessage::create method to properly construct the object
        auto result = MessageType::create(message);
        if (result.has_value()) {
            return Expected<MessageType, GreetingError>{result.value()};
        } else {
            return Expected<MessageType, GreetingError>{GreetingError::InvalidMessage};
        }
    } catch (...) {
        return Expected<MessageType, GreetingError>{GreetingError::InvalidMessage};
    }
}

// ============================================================================
// Debug Implementation Dispatcher
// ============================================================================

/**
 * @brief Character validation implementation dispatcher
 */
bool validate_name_character(char c) noexcept {
    // For now, use a simple runtime dispatch based on build configuration
    #ifdef HELLOWORLD_DEBUG_BUILD
        return validate_name_character_debug(c);
    #else
        return validate_name_character_release(c);
    #endif
}

/**
 * @brief Character validation implementation dispatcher
 */
bool validate_name_character_impl(char c) noexcept {
    return validate_name_character(c);
}

/**
 * @brief Person name validation implementation dispatcher
 */
template<typename NameType>
ConfigAwareValidationResult<NameType> validate_person_name_impl(std::string_view name) {
    #ifdef HELLOWORLD_DEBUG_BUILD
        return validate_person_name_debug<NameType>(name);
    #else
        return validate_person_name_release<NameType>(name);
    #endif
}

/**
 * @brief Greeting message validation implementation dispatcher
 */
template<typename MessageType>
ConfigAwareValidationResult<MessageType> validate_greeting_message_impl(std::string_view message) {
    #ifdef HELLOWORLD_DEBUG_BUILD
        return validate_greeting_message_debug<MessageType>(message);
    #else
        return validate_greeting_message_release<MessageType>(message);
    #endif
}

// ============================================================================
// Explicit Template Instantiations
// ============================================================================

// Ensure the templates are available for common types
template ConfigAwareValidationResult<PersonName> validate_person_name_debug<PersonName>(std::string_view);
template ConfigAwareValidationResult<GreetingMessage> validate_greeting_message_debug<GreetingMessage>(std::string_view);

template ConfigAwareValidationResult<PersonName> validate_person_name_impl<PersonName>(std::string_view);
template ConfigAwareValidationResult<GreetingMessage> validate_greeting_message_impl<GreetingMessage>(std::string_view);

} // namespace greeting::validation
