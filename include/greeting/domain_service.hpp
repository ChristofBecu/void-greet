#pragma once

#include "types.hpp"
#include "config_aware_validation.hpp"

namespace greeting {

/**
 * @brief Domain service for greeting generation
 * 
 * Responsible for coordinating greeting generation using domain types
 * and business rules. Follows single responsibility principle.
 * Enhanced with configuration-aware validation and error context propagation.
 */
class GreetingService {
public:
    /**
     * @brief Generate a simple greeting message
     * @param person The person to greet
     * @return Expected greeting message or error
     */
    [[nodiscard]] Result<GreetingMessage> generateSimpleGreeting(
        const PersonName& person) const noexcept;
    
    /**
     * @brief Generate a formal greeting message
     * @param person The person to greet
     * @return Expected formal greeting message or error
     */
    [[nodiscard]] Result<GreetingMessage> generateFormalGreeting(
        const PersonName& person) const noexcept;
        
    /**
     * @brief Generate greeting with configuration-aware validation
     * @param person_name Raw person name to validate and greet
     * @return Expected greeting message or validation error with context
     */
    [[nodiscard]] Result<GreetingMessage> generateGreetingWithValidation(
        std::string_view person_name) const noexcept;
        
    /**
     * @brief Generate formal greeting with configuration-aware validation
     * @param person_name Raw person name to validate and greet formally
     * @return Expected formal greeting message or validation error with context
     */
    [[nodiscard]] Result<GreetingMessage> generateFormalGreetingWithValidation(
        std::string_view person_name) const noexcept;

private:
    /**
     * @brief Create greeting message with compile-time optimization
     * @param text The greeting text
     * @return Expected greeting message
     */
    [[nodiscard]] constexpr Result<GreetingMessage> createGreetingMessage(
        const std::string& text) const noexcept;
    
    /**
     * @brief Create standardized empty person error
     * @return Error for empty person input
     */
    [[nodiscard]] GreetingError createEmptyPersonError() const noexcept;
    
    /**
     * @brief Create greeting message with configuration-aware validation
     * @param text The greeting text to validate and create
     * @return Expected greeting message or validation error with context
     */
    [[nodiscard]] Result<GreetingMessage> createValidatedGreetingMessage(
        std::string_view text) const noexcept;
        
    /**
     * @brief Propagate validation error with additional context
     * @param original_error The original validation error
     * @param context Additional context information
     * @return Enhanced error with propagated context
     */
    [[nodiscard]] GreetingError propagateValidationError(
        GreetingError original_error, 
        std::string_view context = "") const noexcept;
};

} // namespace greeting
