#pragma once

#include <cstdint>

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

} // namespace greeting
