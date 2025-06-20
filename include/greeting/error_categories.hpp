#pragma once

#include "error_codes.hpp"
#include "error_info.hpp"
#include <system_error>
#include <string>

namespace greeting {

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

} // namespace greeting

// Enable ADL for make_error_code
namespace std {
    template<>
    struct is_error_code_enum<greeting::GreetingError> : true_type {};
}
