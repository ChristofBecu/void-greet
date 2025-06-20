#pragma once

#include "error_codes.hpp"
#include <string_view>
#include <optional>
#include <cctype>

namespace greeting {

/**
 * @brief Compile-time validation utilities using C++23 if consteval
 * 
 * These functions provide enhanced validation that can perform different
 * checks at compile-time vs runtime, optimizing performance and enabling
 * early error detection.
 */
namespace consteval_validation {

/**
 * @brief Compile-time character validation
 * @param c Character to validate
 * @return true if character is valid for names
 */
[[nodiscard]] constexpr bool isValidNameCharacter(char c) noexcept {
    if consteval {
        // Compile-time: Use constexpr-friendly character validation
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
               c == ' ' || c == '-' || c == '\'' || c == '.';
    } else {
        // Runtime: Use standard library functions for full Unicode support
        if (c >= 0) {
            const auto uc = static_cast<unsigned char>(c);
            return std::isalnum(uc) || std::isspace(uc) || 
                   c == '-' || c == '\'' || c == '.';
        }
        return true; // Allow potential UTF-8 sequences
    }
}

/**
 * @brief Compile-time string length validation
 * @param sv String view to validate
 * @param minLength Minimum allowed length
 * @param maxLength Maximum allowed length
 * @return Error code or success indicator
 */
[[nodiscard]] constexpr std::optional<GreetingError> validateLength(
    std::string_view sv, std::size_t minLength, std::size_t maxLength) noexcept {
    
    if consteval {
        // Compile-time: Direct length checks
        if (sv.empty()) {
            return GreetingError::EmptyName; // Context-dependent
        }
        if (sv.length() < minLength) {
            return GreetingError::NameTooShort;
        }
        if (sv.length() > maxLength) {
            return GreetingError::NameTooLong;
        }
    } else {
        // Runtime: More comprehensive validation
        if (sv.empty()) {
            return GreetingError::EmptyName;
        }
        
        // Check for whitespace-only strings
        bool hasNonWhitespace = false;
        for (char c : sv) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                hasNonWhitespace = true;
                break;
            }
        }
        if (!hasNonWhitespace) {
            return GreetingError::EmptyName;
        }
        
        if (sv.length() < minLength) {
            return GreetingError::NameTooShort;
        }
        if (sv.length() > maxLength) {
            return GreetingError::NameTooLong;
        }
    }
    
    return std::nullopt; // No error
}

/**
 * @brief Compile-time character content validation
 * @param sv String view to validate
 * @return Error code if validation fails
 */
[[nodiscard]] constexpr std::optional<GreetingError> validateCharacters(
    std::string_view sv) noexcept {
    
    if consteval {
        // Compile-time: Basic character validation
        for (char c : sv) {
            if (!isValidNameCharacter(c)) {
                return GreetingError::InvalidName;
            }
        }
        
        // Check for invalid start/end characters
        if (!sv.empty()) {
            char first = sv.front();
            char last = sv.back();
            if (first == '-' || first == '\'' || first == '.' ||
                last == '-' || last == '\'' || last == '.') {
                return GreetingError::InvalidName;
            }
        }
    } else {
        // Runtime: Comprehensive character validation
        for (char c : sv) {
            if (!isValidNameCharacter(c)) {
                return GreetingError::InvalidName;
            }
        }
        
        // Check for invalid patterns at runtime
        if (!sv.empty()) {
            if (sv.front() == '-' || sv.front() == '\'' || sv.front() == '.' ||
                sv.back() == '-' || sv.back() == '\'' || sv.back() == '.') {
                return GreetingError::InvalidName;
            }
        }
        
        // Check for excessive consecutive special characters
        int consecutiveSpecial = 0;
        for (char c : sv) {
            if (c == '-' || c == '\'' || c == '.') {
                if (++consecutiveSpecial > 2) {
                    return GreetingError::InvalidName;
                }
            } else {
                consecutiveSpecial = 0;
            }
        }
    }
    
    return std::nullopt; // No error
}

/**
 * @brief Compile-time name validation function
 * @param name Name to validate
 * @return Error code if validation fails, nullopt if successful
 */
[[nodiscard]] constexpr std::optional<GreetingError> validatePersonName(
    std::string_view name) noexcept {
    
    // Length validation
    if (auto lengthError = validateLength(name, 2, 100)) {
        return lengthError;
    }
    
    // Character validation
    if (auto charError = validateCharacters(name)) {
        return charError;
    }
    
    return std::nullopt; // Valid
}

/**
 * @brief Compile-time message validation function
 * @param message Message to validate
 * @return Error code if validation fails, nullopt if successful
 */
[[nodiscard]] constexpr std::optional<GreetingError> validateGreetingMessage(
    std::string_view message) noexcept {
    
    if consteval {
        // Compile-time: Basic validation
        if (message.empty()) {
            return GreetingError::EmptyMessage;
        }
        
        // Basic printable character check
        for (char c : message) {
            if (c < 32 && c != '\t' && c != '\n' && c != '\r') {
                return GreetingError::InvalidMessage;
            }
        }
    } else {
        // Runtime: Comprehensive validation
        if (message.empty()) {
            return GreetingError::EmptyMessage;
        }
        
        // Check for whitespace-only messages
        bool hasNonWhitespace = false;
        for (char c : message) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                hasNonWhitespace = true;
                break;
            }
        }
        if (!hasNonWhitespace) {
            return GreetingError::EmptyMessage;
        }
        
        // Check for invalid control characters
        for (char c : message) {
            if (c >= 0) {
                const auto uc = static_cast<unsigned char>(c);
                if (uc < 32 && uc != '\t' && uc != '\n' && uc != '\r') {
                    return GreetingError::InvalidMessage;
                }
            }
        }
        
        // Warning for very long messages
        if (message.length() > 500) {
            return GreetingError::MessageTooLong;
        }
    }
    
    return std::nullopt; // Valid
}

} // namespace consteval_validation

} // namespace greeting
