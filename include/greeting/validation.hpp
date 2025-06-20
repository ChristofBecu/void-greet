#pragma once

#include "error_codes.hpp"
#include <string_view>
#include <optional>

namespace greeting {

/**
 * @brief Interface for validation functions
 * 
 * This header defines the basic validation interface used throughout
 * the greeting system for consistent error handling.
 */

/**
 * @brief Validate string length constraints
 * @param sv String view to validate
 * @param minLength Minimum allowed length
 * @param maxLength Maximum allowed length
 * @return Error code if validation fails, nullopt if successful
 */
[[nodiscard]] std::optional<GreetingError> validateLength(
    std::string_view sv, std::size_t minLength, std::size_t maxLength) noexcept;

/**
 * @brief Validate character content of a string
 * @param sv String view to validate
 * @return Error code if validation fails, nullopt if successful
 */
[[nodiscard]] std::optional<GreetingError> validateCharacters(
    std::string_view sv) noexcept;

/**
 * @brief Validate a person name
 * @param name Name to validate
 * @return Error code if validation fails, nullopt if successful
 */
[[nodiscard]] std::optional<GreetingError> validatePersonName(
    std::string_view name) noexcept;

/**
 * @brief Validate a greeting message
 * @param message Message to validate
 * @return Error code if validation fails, nullopt if successful
 */
[[nodiscard]] std::optional<GreetingError> validateGreetingMessage(
    std::string_view message) noexcept;

} // namespace greeting
