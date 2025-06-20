/**
 * @file validation.cpp
 * @brief Input validation logic implementation
 * 
 * Implements validation functions for greeting system inputs.
 * Follows single responsibility principle with focused validation logic.
 */

#include "greeting/validation.hpp"
#include <algorithm>
#include <cctype>

namespace greeting {

// ============================================================================
// Forward declarations for helper functions
// ============================================================================

[[nodiscard]] bool isWhitespaceOnly(std::string_view sv) noexcept;
[[nodiscard]] constexpr bool isValidNameCharacter(char c) noexcept;
[[nodiscard]] bool isValidSpecialCharacter(char c) noexcept;
[[nodiscard]] std::optional<GreetingError> 
    performFullCharacterValidation(std::string_view sv) noexcept;
[[nodiscard]] std::optional<GreetingError> 
    validateMessageCharacters(std::string_view sv) noexcept;

// ============================================================================
// Public validation interface implementations
// ============================================================================

/**
 * @brief Validate string length constraints
 * @param sv String view to validate
 * @param minLength Minimum allowed length
 * @param maxLength Maximum allowed length
 * @return Error code if validation fails, nullopt if successful
 */
[[nodiscard]] std::optional<GreetingError> validateLength(
    std::string_view sv, std::size_t minLength, std::size_t maxLength) noexcept {
    
    const std::size_t length = sv.length();
    
    if (length < minLength) {
        return GreetingError::NameTooShort;
    }
    
    if (length > maxLength) {
        return GreetingError::NameTooLong;
    }
    
    return std::nullopt;
}

/**
 * @brief Validate character content of a string
 * @param sv String view to validate
 * @return Error code if validation fails, nullopt if successful
 */
[[nodiscard]] std::optional<GreetingError> validateCharacters(
    std::string_view sv) noexcept {
    
    // Use runtime validation with full support
    return performFullCharacterValidation(sv);
}

/**
 * @brief Validate a person name
 * @param name Name to validate
 * @return Error code if validation fails, nullopt if successful
 */
[[nodiscard]] std::optional<GreetingError> validatePersonName(
    std::string_view name) noexcept {
    
    // Check for empty name
    if (name.empty() || isWhitespaceOnly(name)) {
        return GreetingError::EmptyName;
    }
    
    // Validate length (1-100 characters)
    if (auto lengthError = validateLength(name, 1, 100)) {
        return lengthError;
    }
    
    // Validate character content
    return validateCharacters(name);
}

/**
 * @brief Validate a greeting message
 * @param message Message to validate
 * @return Error code if validation fails, nullopt if successful
 */
[[nodiscard]] std::optional<GreetingError> validateGreetingMessage(
    std::string_view message) noexcept {
    
    // Check for empty message
    if (message.empty() || isWhitespaceOnly(message)) {
        return GreetingError::EmptyMessage;
    }
    
    // Validate length (1-500 characters)
    if (auto lengthError = validateLength(message, 1, 500)) {
        return lengthError;
    }
    
    // Messages have more relaxed character requirements
    return validateMessageCharacters(message);
}

// ============================================================================
// Private validation helper implementations
// ============================================================================

/**
 * @brief Check if string contains only whitespace
 * @param sv String to check
 * @return true if only whitespace, false otherwise
 */
[[nodiscard]] bool isWhitespaceOnly(std::string_view sv) noexcept {
    return std::all_of(sv.begin(), sv.end(), 
        [](char c) { return std::isspace(static_cast<unsigned char>(c)); });
}

/**
 * @brief Check if character is valid for names at compile-time
 * @param c Character to check
 * @return true if valid, false otherwise
 */
[[nodiscard]] constexpr bool isValidNameCharacter(char c) noexcept {
    return (c >= 'A' && c <= 'Z') || 
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           c == ' ' || c == '-' || c == '\'' || c == '.';
}

/**
 * @brief Check if character is valid special character
 * @param c Character to check
 * @return true if valid special character, false otherwise
 */
[[nodiscard]] bool isValidSpecialCharacter(char c) noexcept {
    return c == ' ' || c == '-' || c == '\'' || c == '.' || c == ',';
}

/**
 * @brief Perform full character validation at runtime
 * @param sv String to validate
 * @return Error if invalid characters found, nullopt if valid
 */
[[nodiscard]] std::optional<GreetingError> 
performFullCharacterValidation(std::string_view sv) noexcept {
    
    for (char c : sv) {
        // Use runtime character validation with Unicode support
        if (!std::isalnum(static_cast<unsigned char>(c)) && 
            !isValidSpecialCharacter(c)) {
            return GreetingError::InvalidCharacters;
        }
    }
    
    return std::nullopt;
}

/**
 * @brief Validate characters in greeting messages
 * @param sv Message to validate
 * @return Error if invalid characters found, nullopt if valid
 */
[[nodiscard]] std::optional<GreetingError> 
validateMessageCharacters(std::string_view sv) noexcept {
    
    for (char c : sv) {
        // Messages allow more characters including punctuation
        if (!std::isprint(static_cast<unsigned char>(c))) {
            return GreetingError::InvalidCharacters;
        }
    }
    
    return std::nullopt;
}

} // namespace greeting
