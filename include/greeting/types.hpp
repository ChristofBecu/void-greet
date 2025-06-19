#pragma once

#include <string>
#include <string_view>
#include <variant>
#include <system_error>
#include <cctype>

namespace greeting {

/**
 * @brief Error codes for greeting operations
 * 
 * Enumeration of all possible errors that can occur during greeting operations.
 * Each error has a descriptive message and can be used with Expected.
 */
enum class GreetingError {
    EmptyName,           ///< Person name is empty or whitespace only
    InvalidName,         ///< Person name contains invalid characters
    EmptyMessage,        ///< Greeting message is empty
    InvalidMessage,      ///< Greeting message format is invalid
    ConfigurationError,  ///< Configuration-related error
    OutputError         ///< Error during output operation
};

/**
 * @brief Convert GreetingError to descriptive string
 * @param error The error code to convert
 * @return Human-readable error description
 */
[[nodiscard]] constexpr std::string_view toString(GreetingError error) noexcept {
    switch (error) {
        case GreetingError::EmptyName:
            return "Person name cannot be empty or consist only of whitespace";
        case GreetingError::InvalidName:
            return "Person name contains invalid characters";
        case GreetingError::EmptyMessage:
            return "Greeting message cannot be empty";
        case GreetingError::InvalidMessage:
            return "Greeting message format is invalid";
        case GreetingError::ConfigurationError:
            return "Configuration error occurred";
        case GreetingError::OutputError:
            return "Error occurred during output operation";
    }
    return "Unknown error";
}

/**
 * @brief Error category for greeting errors
 * 
 * Provides std::error_code integration for greeting errors.
 */
class GreetingErrorCategory : public std::error_category {
public:
    [[nodiscard]] const char* name() const noexcept override {
        return "greeting";
    }
    
    [[nodiscard]] std::string message(int ev) const override {
        return std::string{toString(static_cast<GreetingError>(ev))};
    }
};

/**
 * @brief Get the singleton instance of GreetingErrorCategory
 * @return Reference to the greeting error category
 */
[[nodiscard]] inline const GreetingErrorCategory& greetingErrorCategory() noexcept {
    static const GreetingErrorCategory instance;
    return instance;
}

/**
 * @brief Create std::error_code from GreetingError
 * @param error The greeting error
 * @return std::error_code representing the error
 */
[[nodiscard]] inline std::error_code make_error_code(GreetingError error) noexcept {
    return {static_cast<int>(error), greetingErrorCategory()};
}

/**
 * @brief Simple Expected-like type for error handling
 * 
 * A variant-based implementation similar to std::expected for C++23 compatibility.
 * Contains either a value of type T or an error of type E.
 */
template<typename T, typename E>
class Expected {
private:
    std::variant<T, E> data_;

public:
    /**
     * @brief Construct with a value
     * @param value The value to store
     */
    Expected(T value) : data_(std::move(value)) {}
    
    /**
     * @brief Construct with an error
     * @param error The error to store
     */
    Expected(E error) : data_(std::move(error)) {}
    
    /**
     * @brief Check if the Expected contains a value
     * @return true if contains value, false if contains error
     */
    [[nodiscard]] bool has_value() const noexcept {
        return std::holds_alternative<T>(data_);
    }
    
    /**
     * @brief Check if the Expected contains a value (bool conversion)
     * @return true if contains value, false if contains error
     */
    [[nodiscard]] explicit operator bool() const noexcept {
        return has_value();
    }
    
    /**
     * @brief Get the contained value
     * @return Reference to the contained value
     * @throws std::bad_variant_access if contains error
     */
    [[nodiscard]] T& value() & {
        return std::get<T>(data_);
    }
    
    /**
     * @brief Get the contained value (const)
     * @return Const reference to the contained value
     * @throws std::bad_variant_access if contains error
     */
    [[nodiscard]] const T& value() const & {
        return std::get<T>(data_);
    }
    
    /**
     * @brief Get the contained value (rvalue)
     * @return Rvalue reference to the contained value
     * @throws std::bad_variant_access if contains error
     */
    [[nodiscard]] T&& value() && {
        return std::get<T>(std::move(data_));
    }
    
    /**
     * @brief Get the contained error
     * @return Reference to the contained error
     * @throws std::bad_variant_access if contains value
     */
    [[nodiscard]] E& error() & {
        return std::get<E>(data_);
    }
    
    /**
     * @brief Get the contained error (const)
     * @return Const reference to the contained error
     * @throws std::bad_variant_access if contains value
     */
    [[nodiscard]] const E& error() const & {
        return std::get<E>(data_);
    }
    
    /**
     * @brief Dereference operator for value access
     * @return Reference to the contained value
     */
    [[nodiscard]] T& operator*() & {
        return value();
    }
    
    /**
     * @brief Dereference operator for value access (const)
     * @return Const reference to the contained value
     */
    [[nodiscard]] const T& operator*() const & {
        return value();
    }
    
    /**
     * @brief Arrow operator for value access
     * @return Pointer to the contained value
     */
    [[nodiscard]] T* operator->() {
        return &value();
    }
    
    /**
     * @brief Arrow operator for value access (const)
     * @return Const pointer to the contained value
     */
    [[nodiscard]] const T* operator->() const {
        return &value();
    }
};

/**
 * @brief Helper function to create an Expected with an error
 * @param error The error value
 * @return Expected containing the error
 */
template<typename E>
[[nodiscard]] auto unexpected(E error) {
    return error;
}

// Template aliases for clean Expected usage
template<typename T>
using Result = Expected<T, GreetingError>;

template<typename T>
using ResultWithCode = Expected<T, std::error_code>;

/**
 * @brief Strong type alias for person names to prevent mixing with other strings
 * 
 * This type ensures type safety by preventing accidental assignment of 
 * arbitrary strings where a person name is expected.
 */
class PersonName {
private:
    struct PrivateTag {};

public:
    /**
     * @brief Private constructor tag - for internal use only
     * @param name The validated person's name
     * @param tag Private construction tag
     */
    explicit PersonName(std::string name, PrivateTag) : value_(std::move(name)) {}

    /**
     * @brief Create a PersonName from a string_view with validation
     * @param name The person's name
     * @return Result<PersonName> containing the PersonName or an error
     */
    [[nodiscard]] static Result<PersonName> create(std::string_view name) noexcept {
        if (name.empty()) {
            return GreetingError::EmptyName;
        }
        
        // Check for whitespace-only names
        bool hasNonWhitespace = false;
        for (char c : name) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                hasNonWhitespace = true;
                break;
            }
        }
        
        if (!hasNonWhitespace) {
            return GreetingError::EmptyName;
        }
        
        // Check for invalid characters (basic validation)
        for (char c : name) {
            if (c < 0 || (!std::isalnum(static_cast<unsigned char>(c)) && 
                         !std::isspace(static_cast<unsigned char>(c)) && 
                         c != '-' && c != '\'' && c != '.')) {
                return GreetingError::InvalidName;
            }
        }
        
        return PersonName{std::string{name}, PrivateTag{}};
    }

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
private:
    struct PrivateTag {};

public:
    /**
     * @brief Private constructor tag - for internal use only
     * @param message The validated greeting message
     * @param tag Private construction tag
     */
    explicit GreetingMessage(std::string message, PrivateTag) : value_(std::move(message)) {}

    /**
     * @brief Create a GreetingMessage from a string_view with validation
     * @param message The greeting message
     * @return Result<GreetingMessage> containing the GreetingMessage or an error
     */
    [[nodiscard]] static Result<GreetingMessage> create(std::string_view message) noexcept {
        if (message.empty()) {
            return GreetingError::EmptyMessage;
        }
        
        // Basic validation - check that message is not just whitespace
        bool hasContent = false;
        for (char c : message) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                hasContent = true;
                break;
            }
        }
        
        if (!hasContent) {
            return GreetingError::EmptyMessage;
        }
        
        return GreetingMessage{std::string{message}, PrivateTag{}};
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
 * @brief Convenience function to create PersonName with validation
 * @param name The person's name
 * @return Result<PersonName> containing the PersonName or an error
 */
[[nodiscard]] inline Result<PersonName> makePersonName(std::string_view name) {
    return PersonName::create(name);
}

/**
 * @brief Convenience function to create GreetingMessage with validation
 * @param message The greeting message
 * @return Result<GreetingMessage> containing the GreetingMessage or an error
 */
[[nodiscard]] inline Result<GreetingMessage> makeGreetingMessage(std::string_view message) {
    return GreetingMessage::create(message);
}

} // namespace greeting
