#pragma once

#include <string>
#include <string_view>

namespace greeting {

/**
 * @brief Strong type alias for person names to prevent mixing with other strings
 * 
 * This type ensures type safety by preventing accidental assignment of 
 * arbitrary strings where a person name is expected.
 */
class PersonName {
public:
    /**
     * @brief Construct a PersonName from a string_view (preferred)
     * @param name The person's name (must not be empty)
     */
    explicit PersonName(std::string_view name) : value_(name) {}
    
    /**
     * @brief Construct a PersonName from a string  
     * @param name The person's name (must not be empty)
     */
    explicit PersonName(std::string name) : value_(std::move(name)) {}
    
    /**
     * @brief Get the underlying string value
     * @return const reference to the stored name
     */
    [[nodiscard]] const std::string& value() const noexcept {
        return value_;
    }
    
    /**
     * @brief Get string_view of the name
     * @return string_view of the stored name
     */
    [[nodiscard]] std::string_view view() const noexcept {
        return value_;
    }
    
    /**
     * @brief Check if the name is empty
     * @return true if name is empty, false otherwise
     */
    [[nodiscard]] bool empty() const noexcept {
        return value_.empty();
    }
    
    /**
     * @brief Get the length of the name
     * @return length of the stored name
     */
    [[nodiscard]] std::size_t length() const noexcept {
        return value_.length();
    }
    
    // Comparison operators
    [[nodiscard]] bool operator==(const PersonName& other) const noexcept {
        return value_ == other.value_;
    }
    
    [[nodiscard]] bool operator!=(const PersonName& other) const noexcept {
        return !(*this == other);
    }
    
    [[nodiscard]] bool operator<(const PersonName& other) const noexcept {
        return value_ < other.value_;
    }

private:
    std::string value_;
};

/**
 * @brief Strong type alias for greeting messages to prevent mixing with other strings
 * 
 * This type ensures type safety by preventing accidental assignment of 
 * arbitrary strings where a greeting message is expected.
 */
class GreetingMessage {
public:
    /**
     * @brief Construct a GreetingMessage from a string_view (preferred)
     * @param message The greeting message
     */
    explicit GreetingMessage(std::string_view message) 
        : value_(message) {}
    
    /**
     * @brief Construct a GreetingMessage from a string
     * @param message The greeting message
     */
    explicit GreetingMessage(std::string message) 
        : value_(std::move(message)) {}
    
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
 * @brief Convenience function to create PersonName with deduction
 * @param name The person's name
 * @return PersonName instance
 */
[[nodiscard]] PersonName makePersonName(std::string_view name) {
    return PersonName{std::string{name}};
}

/**
 * @brief Convenience function to create GreetingMessage with deduction
 * @param message The greeting message
 * @return GreetingMessage instance
 */
[[nodiscard]] GreetingMessage makeGreetingMessage(std::string_view message) {
    return GreetingMessage{std::string{message}};
}

} // namespace greeting
