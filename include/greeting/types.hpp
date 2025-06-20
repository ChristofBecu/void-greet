#pragma once

#include <string>
#include <string_view>
#include <variant>
#include <system_error>
#include <cctype>
#include <cstdint>
#include <optional>

namespace greeting {

/**
 * @brief Error severity levels for greeting operations
 * 
 * Categorizes errors by their impact and recovery potential.
 */
enum class ErrorSeverity : std::uint8_t {
    Info,       ///< Informational - operation can continue
    Warning,    ///< Warning - operation continues with degraded functionality
    Error,      ///< Error - operation fails but system remains stable
    Critical    ///< Critical - system-level failure requiring immediate attention
};

/**
 * @brief Error domains for greeting operations
 * 
 * Groups errors by functional area for better organization and handling.
 */
enum class ErrorDomain : std::uint8_t {
    Validation,     ///< Input validation errors
    Configuration,  ///< Configuration and setup errors
    Strategy,       ///< Strategy pattern related errors
    Output,         ///< Output and rendering errors
    System,         ///< System-level errors (memory, I/O, etc.)
    Logic          ///< Business logic errors
};

/**
 * @brief Comprehensive error codes for greeting operations
 * 
 * Enumeration of all possible errors that can occur during greeting operations.
 * Each error has a descriptive message, severity level, and domain classification.
 * Organized by functional area with consistent naming conventions.
 */
enum class GreetingError : std::uint16_t {
    // Validation Errors (1000-1999)
    EmptyName = 1001,           ///< Person name is empty or whitespace only
    InvalidName = 1002,         ///< Person name contains invalid characters
    NameTooLong = 1003,         ///< Person name exceeds maximum length
    NameTooShort = 1004,        ///< Person name is below minimum length
    EmptyMessage = 1005,        ///< Greeting message is empty
    InvalidMessage = 1006,      ///< Greeting message format is invalid
    MessageTooLong = 1007,      ///< Greeting message exceeds maximum length
    InvalidCharacters = 1008,   ///< Input contains prohibited characters
    UnsupportedFormat = 1009,   ///< Input format is not supported
    
    // Configuration Errors (2000-2999)
    ConfigurationMissing = 2001,    ///< Required configuration is missing
    ConfigurationInvalid = 2002,    ///< Configuration values are invalid
    ConfigurationCorrupted = 2003,  ///< Configuration file is corrupted
    SettingsNotFound = 2004,        ///< Specific setting not found
    EnvironmentInvalid = 2005,      ///< Environment variables are invalid
    LocaleUnsupported = 2006,       ///< Requested locale is not supported
    
    // Strategy Errors (3000-3999)
    StrategyNotFound = 3001,        ///< Requested strategy implementation not found
    StrategyCreationFailed = 3002,  ///< Failed to create strategy instance
    StrategyIncompatible = 3003,    ///< Strategy is incompatible with current context
    StrategyRegistrationFailed = 3004, ///< Failed to register strategy
    FactoryNotInitialized = 3005,   ///< Strategy factory is not initialized
    
    // Output Errors (4000-4999)
    OutputStreamError = 4001,       ///< Error writing to output stream
    FormattingError = 4002,         ///< Error during message formatting
    EncodingError = 4003,           ///< Character encoding error
    RenderingFailed = 4004,         ///< Failed to render output
    BufferOverflow = 4005,          ///< Output buffer overflow
    
    // System Errors (5000-5999)
    MemoryAllocationFailed = 5001,  ///< Memory allocation failed
    FileSystemError = 5002,         ///< File system operation failed
    NetworkError = 5003,            ///< Network operation failed
    PermissionDenied = 5004,        ///< Insufficient permissions
    ResourceUnavailable = 5005,     ///< Required resource is unavailable
    
    // Logic Errors (6000-6999)
    InvalidState = 6001,            ///< Object is in invalid state
    PreconditionViolated = 6002,    ///< Method precondition not met
    PostconditionViolated = 6003,   ///< Method postcondition not met
    InvariantViolated = 6004,       ///< Class invariant violated
    NotImplemented = 6005,          ///< Feature not yet implemented
    
    // Generic/Unknown
    Unknown = 9999                  ///< Unknown or unclassified error
};

/**
 * @brief Error metadata structure
 * 
 * Contains additional information about an error for better debugging and logging.
 */
struct ErrorInfo {
    GreetingError code;
    ErrorSeverity severity;
    ErrorDomain domain;
    std::string_view message;
    std::string_view category;
    std::string_view suggestion;  ///< Suggested action to resolve the error
    
    constexpr ErrorInfo(GreetingError c, ErrorSeverity s, ErrorDomain d, 
                       std::string_view msg, std::string_view cat, 
                       std::string_view sug = "") noexcept
        : code(c), severity(s), domain(d), message(msg), category(cat), suggestion(sug) {}
};

/**
 * @brief Comprehensive error information lookup
 * @param error The error code to look up
 * @return Complete error information including metadata
 */
[[nodiscard]] constexpr ErrorInfo getErrorInfo(GreetingError error) noexcept {
    switch (error) {
        // Validation Errors
        case GreetingError::EmptyName:
            return {error, ErrorSeverity::Error, ErrorDomain::Validation,
                   "Person name cannot be empty or consist only of whitespace",
                   "Input Validation",
                   "Provide a non-empty name with at least one non-whitespace character"};
        
        case GreetingError::InvalidName:
            return {error, ErrorSeverity::Error, ErrorDomain::Validation,
                   "Person name contains invalid characters (only letters, spaces, hyphens, apostrophes, and periods allowed)",
                   "Input Validation",
                   "Use only alphabetic characters, spaces, hyphens (-), apostrophes ('), and periods (.)"};
        
        case GreetingError::NameTooLong:
            return {error, ErrorSeverity::Error, ErrorDomain::Validation,
                   "Person name exceeds maximum allowed length of 100 characters",
                   "Input Validation",
                   "Shorten the name to 100 characters or less"};
        
        case GreetingError::NameTooShort:
            return {error, ErrorSeverity::Warning, ErrorDomain::Validation,
                   "Person name is unusually short (less than 2 characters)",
                   "Input Validation",
                   "Consider providing a longer, more complete name"};
        
        case GreetingError::EmptyMessage:
            return {error, ErrorSeverity::Error, ErrorDomain::Validation,
                   "Greeting message cannot be empty or consist only of whitespace",
                   "Input Validation",
                   "Provide a meaningful greeting message"};
        
        case GreetingError::InvalidMessage:
            return {error, ErrorSeverity::Error, ErrorDomain::Validation,
                   "Greeting message contains invalid or unsupported characters",
                   "Input Validation",
                   "Use only printable characters and avoid control characters"};
        
        case GreetingError::MessageTooLong:
            return {error, ErrorSeverity::Warning, ErrorDomain::Validation,
                   "Greeting message is very long and may be truncated during display",
                   "Input Validation",
                   "Consider shortening the message for better readability"};
        
        case GreetingError::InvalidCharacters:
            return {error, ErrorSeverity::Error, ErrorDomain::Validation,
                   "Input contains characters that are not allowed in this context",
                   "Input Validation",
                   "Remove or replace the invalid characters"};
        
        case GreetingError::UnsupportedFormat:
            return {error, ErrorSeverity::Error, ErrorDomain::Validation,
                   "The provided input format is not supported by this operation",
                   "Input Validation",
                   "Use a supported input format or convert the data"};
        
        // Configuration Errors
        case GreetingError::ConfigurationMissing:
            return {error, ErrorSeverity::Critical, ErrorDomain::Configuration,
                   "Required configuration settings are missing or could not be loaded",
                   "Configuration",
                   "Ensure configuration files exist and are accessible"};
        
        case GreetingError::ConfigurationInvalid:
            return {error, ErrorSeverity::Error, ErrorDomain::Configuration,
                   "Configuration contains invalid values or format errors",
                   "Configuration",
                   "Check configuration syntax and value ranges"};
        
        case GreetingError::ConfigurationCorrupted:
            return {error, ErrorSeverity::Error, ErrorDomain::Configuration,
                   "Configuration file is corrupted or unreadable",
                   "Configuration",
                   "Restore configuration from backup or recreate"};
        
        case GreetingError::SettingsNotFound:
            return {error, ErrorSeverity::Error, ErrorDomain::Configuration,
                   "Specific configuration setting could not be found",
                   "Configuration",
                   "Check setting name and ensure it exists in configuration"};
        
        case GreetingError::EnvironmentInvalid:
            return {error, ErrorSeverity::Error, ErrorDomain::Configuration,
                   "Environment variables contain invalid values",
                   "Configuration",
                   "Check and correct environment variable values"};
        
        case GreetingError::LocaleUnsupported:
            return {error, ErrorSeverity::Warning, ErrorDomain::Configuration,
                   "The requested locale is not supported on this system",
                   "Localization",
                   "Use a supported locale or install the required language pack"};
        
        // Strategy Errors
        case GreetingError::StrategyNotFound:
            return {error, ErrorSeverity::Error, ErrorDomain::Strategy,
                   "The requested greeting strategy implementation was not found",
                   "Strategy Pattern",
                   "Ensure the strategy is registered or use a different strategy"};
        
        case GreetingError::StrategyCreationFailed:
            return {error, ErrorSeverity::Error, ErrorDomain::Strategy,
                   "Failed to create an instance of the requested strategy",
                   "Strategy Pattern",
                   "Check strategy dependencies and initialization requirements"};
        
        case GreetingError::StrategyIncompatible:
            return {error, ErrorSeverity::Error, ErrorDomain::Strategy,
                   "Strategy is incompatible with the current context",
                   "Strategy Pattern",
                   "Use a compatible strategy or adjust context"};
        
        case GreetingError::StrategyRegistrationFailed:
            return {error, ErrorSeverity::Error, ErrorDomain::Strategy,
                   "Failed to register strategy in the factory",
                   "Strategy Pattern",
                   "Check registration requirements and factory state"};
        
        case GreetingError::FactoryNotInitialized:
            return {error, ErrorSeverity::Error, ErrorDomain::Strategy,
                   "Strategy factory has not been properly initialized",
                   "Strategy Pattern",
                   "Initialize the factory before using strategies"};
        
        // Output Errors
        case GreetingError::OutputStreamError:
            return {error, ErrorSeverity::Error, ErrorDomain::Output,
                   "Failed to write to the output stream",
                   "Output Operations",
                   "Check stream state and available disk space or permissions"};
        
        case GreetingError::FormattingError:
            return {error, ErrorSeverity::Error, ErrorDomain::Output,
                   "Error occurred during message formatting or template processing",
                   "Output Operations",
                   "Verify format strings and template syntax"};
        
        case GreetingError::EncodingError:
            return {error, ErrorSeverity::Error, ErrorDomain::Output,
                   "Character encoding error during output processing",
                   "Output Operations",
                   "Check character encoding settings and input data"};
        
        case GreetingError::RenderingFailed:
            return {error, ErrorSeverity::Error, ErrorDomain::Output,
                   "Failed to render output to the target format",
                   "Output Operations",
                   "Check rendering parameters and output format support"};
        
        case GreetingError::BufferOverflow:
            return {error, ErrorSeverity::Error, ErrorDomain::Output,
                   "Output buffer overflow occurred during processing",
                   "Output Operations",
                   "Increase buffer size or reduce output data"};
        
        // System Errors  
        case GreetingError::MemoryAllocationFailed:
            return {error, ErrorSeverity::Critical, ErrorDomain::System,
                   "Failed to allocate required memory",
                   "System Resources",
                   "Free up memory or restart the application"};
        
        case GreetingError::FileSystemError:
            return {error, ErrorSeverity::Error, ErrorDomain::System,
                   "File system operation failed",
                   "System Resources",
                   "Check file permissions and available disk space"};
        
        case GreetingError::NetworkError:
            return {error, ErrorSeverity::Error, ErrorDomain::System,
                   "Network operation failed",
                   "System Resources",
                   "Check network connectivity and configuration"};
        
        case GreetingError::PermissionDenied:
            return {error, ErrorSeverity::Error, ErrorDomain::System,
                   "Insufficient permissions to perform the requested operation",
                   "System Security",
                   "Run with appropriate permissions or contact system administrator"};
        
        case GreetingError::ResourceUnavailable:
            return {error, ErrorSeverity::Error, ErrorDomain::System,
                   "Required system resource is unavailable",
                   "System Resources",
                   "Wait for resource to become available or try alternative"};
        
        // Logic Errors
        case GreetingError::InvalidState:
            return {error, ErrorSeverity::Error, ErrorDomain::Logic,
                   "Object is in an invalid state for the requested operation",
                   "Business Logic",
                   "Ensure proper initialization and state transitions"};
        
        case GreetingError::PreconditionViolated:
            return {error, ErrorSeverity::Error, ErrorDomain::Logic,
                   "Method precondition was not satisfied",
                   "Business Logic",
                   "Ensure preconditions are met before calling the method"};
        
        case GreetingError::PostconditionViolated:
            return {error, ErrorSeverity::Error, ErrorDomain::Logic,
                   "Method postcondition was violated",
                   "Business Logic",
                   "Check method implementation and invariants"};
        
        case GreetingError::InvariantViolated:
            return {error, ErrorSeverity::Error, ErrorDomain::Logic,
                   "Class invariant was violated",
                   "Business Logic",
                   "Check object state and method implementations"};
        
        case GreetingError::NotImplemented:
            return {error, ErrorSeverity::Info, ErrorDomain::Logic,
                   "This feature is not yet implemented",
                   "Development",
                   "Use an alternative approach or wait for implementation"};
        
        // Default case
        default:
            return {GreetingError::Unknown, ErrorSeverity::Error, ErrorDomain::System,
                   "An unknown error occurred",
                   "Unknown",
                   "Contact support with error details"};
    }
}

/**
 * @brief Convert GreetingError to descriptive string (backward compatibility)
 * @param error The error code to convert
 * @return Human-readable error description
 */
[[nodiscard]] constexpr std::string_view toString(GreetingError error) noexcept {
    return getErrorInfo(error).message;
}

/**
 * @brief Get error severity level
 * @param error The error code
 * @return Severity level of the error
 */
[[nodiscard]] constexpr ErrorSeverity getErrorSeverity(GreetingError error) noexcept {
    return getErrorInfo(error).severity;
}

/**
 * @brief Get error domain
 * @param error The error code  
 * @return Domain classification of the error
 */
[[nodiscard]] constexpr ErrorDomain getErrorDomain(GreetingError error) noexcept {
    return getErrorInfo(error).domain;
}

/**
 * @brief Convert error severity to string
 * @param severity The severity level
 * @return String representation of severity
 */
[[nodiscard]] constexpr std::string_view toString(ErrorSeverity severity) noexcept {
    switch (severity) {
        case ErrorSeverity::Info:     return "INFO";
        case ErrorSeverity::Warning:  return "WARNING";
        case ErrorSeverity::Error:    return "ERROR";
        case ErrorSeverity::Critical: return "CRITICAL";
    }
    return "UNKNOWN";
}

/**
 * @brief Convert error domain to string
 * @param domain The error domain
 * @return String representation of domain
 */
[[nodiscard]] constexpr std::string_view toString(ErrorDomain domain) noexcept {
    switch (domain) {
        case ErrorDomain::Validation:    return "VALIDATION";
        case ErrorDomain::Configuration: return "CONFIGURATION";
        case ErrorDomain::Strategy:      return "STRATEGY";
        case ErrorDomain::Output:        return "OUTPUT";
        case ErrorDomain::System:        return "SYSTEM";
        case ErrorDomain::Logic:         return "LOGIC";
    }
    return "UNKNOWN";
}

/**
 * @brief Error category for greeting errors
 * 
 * Provides std::error_code integration for greeting errors.
 * Supports the enhanced error enumeration with codes and domains.
 */
class GreetingErrorCategory : public std::error_category {
public:
    [[nodiscard]] const char* name() const noexcept override {
        return "greeting";
    }
    
    [[nodiscard]] std::string message(int ev) const override {
        const auto error = static_cast<GreetingError>(ev);
        const auto info = getErrorInfo(error);
        
        // Create detailed message with category and suggestion
        std::string result{info.message};
        if (!info.suggestion.empty()) {
            result += " (Suggestion: ";
            result += info.suggestion;
            result += ")";
        }
        return result;
    }
    
    [[nodiscard]] bool equivalent(int code, const std::error_condition& condition) const noexcept override {
        const auto error = static_cast<GreetingError>(code);
        const auto severity = getErrorSeverity(error);
        
        // Map to standard error conditions based on severity
        switch (severity) {
            case ErrorSeverity::Info:
            case ErrorSeverity::Warning:
                return condition == std::errc::operation_not_supported;
            case ErrorSeverity::Error:
                return condition == std::errc::invalid_argument;
            case ErrorSeverity::Critical:
                return condition == std::errc::resource_unavailable_try_again;
        }
        return false;
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
        // Use compile-time validation when possible
        if (auto error = consteval_validation::validatePersonName(name)) {
            return *error;
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
        // Use compile-time validation when possible
        if (auto error = consteval_validation::validateGreetingMessage(message)) {
            return *error;
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
