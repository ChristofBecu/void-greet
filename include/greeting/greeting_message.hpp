#pragma once

#include "expected.hpp"
#include "config_aware_validation.hpp"
#include <string>
#include <string_view>

namespace greeting {

/**
 * @brief Strong type alias for greeting messages to prevent mixing with other strings
 * 
 * This type ensures type safety by preventing accidental assignment of 
 * arbitrary strings where a greeting message is expected.
 */
class GreetingMessage {
public:
    /**
     * @brief Internal construction tag - for validation framework use only
     * DO NOT USE DIRECTLY - Use GreetingMessage::create() instead
     */
    struct InternalTag {};

    /**
     * @brief Create a GreetingMessage from a string_view with validation
     * @param message The greeting message
     * @return Result<GreetingMessage> containing the GreetingMessage or an error
     */
    [[nodiscard]] static Result<GreetingMessage> create(std::string_view message) noexcept {
        // Use configuration-aware validation
        auto validation_result = greeting::validation::validate_greeting_message<GreetingMessage>(message);
        if (!validation_result.has_value()) {
            return validation_result.error();
        }
        
        return validation_result.value();
    }

    /**
     * @brief Get the underlying string value
     * @return const reference to the stored message
     */
    [[nodiscard]] const std::string& value() const noexcept {
        return value_;
    }
    
    /**
     * @brief Get string_view of the message
     * @return string_view of the stored message
     */
    [[nodiscard]] std::string_view view() const noexcept {
        return value_;
    }
    
    /**
     * @brief Check if the message is empty
     * @return true if message is empty, false otherwise
     */
    [[nodiscard]] bool empty() const noexcept {
        return value_.empty();
    }
    
    /**
     * @brief Get the length of the message
     * @return length of the stored message
     */
    [[nodiscard]] std::size_t length() const noexcept {
        return value_.length();
    }
    
    /**
     * @brief Internal constructor - for validation framework use only
     * DO NOT USE DIRECTLY - Use GreetingMessage::create() instead
     * @param message The validated greeting message
     * @param tag Internal construction tag
     */
    explicit GreetingMessage(std::string message, InternalTag) : value_(std::move(message)) {}

    // Comparison operators
    [[nodiscard]] bool operator==(const GreetingMessage& other) const noexcept {
        return value_ == other.value_;
    }
    
    [[nodiscard]] bool operator!=(const GreetingMessage& other) const noexcept {
        return !(*this == other);
    }
    
    [[nodiscard]] bool operator<(const GreetingMessage& other) const noexcept {
        return value_ < other.value_;
    }

private:
    std::string value_;
};

/**
 * @brief Convenience function to create GreetingMessage with validation
 * @param message The greeting message
 * @return Result<GreetingMessage> containing the GreetingMessage or an error
 */
[[nodiscard]] inline Result<GreetingMessage> makeGreetingMessage(std::string_view message) {
    return GreetingMessage::create(message);
}

} // namespace greeting
