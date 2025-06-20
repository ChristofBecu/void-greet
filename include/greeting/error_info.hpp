#pragma once

#include "error_codes.hpp"
#include <string_view>

namespace greeting {

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

} // namespace greeting
