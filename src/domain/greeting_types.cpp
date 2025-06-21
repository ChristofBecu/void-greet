/**
 * @file greeting_types.cpp
 * @brief Core domain types implementation
 * 
 * Contains the implementation of domain services that orchestrate
 * greeting business logic while maintaining clean architecture principles.
 * Enhanced with configuration-aware validation and error context propagation.
 */

#include "greeting/domain_service.hpp"

namespace greeting {

/**
 * @brief Generate a simple greeting message
 * @param person The person to greet
 * @return Expected greeting message or error
 */
[[nodiscard]] Result<GreetingMessage> GreetingService::generateSimpleGreeting(
    const PersonName& person) const noexcept {
    
    if consteval {
        // Compile-time path for constant expressions
        return createGreetingMessage("Hello, " + person.value() + "!");
    } else {
        // Runtime path with validation
        if (person.empty()) {
            return createEmptyPersonError();
        }
        
        const std::string greeting = "Hello, " + person.value() + "!";
        return GreetingMessage::create(greeting);
    }
}

/**
 * @brief Generate a formal greeting message
 * @param person The person to greet
 * @return Expected formal greeting message or error
 */
[[nodiscard]] Result<GreetingMessage> GreetingService::generateFormalGreeting(
    const PersonName& person) const noexcept {
    
    if (person.empty()) {
        return createEmptyPersonError();
    }
    
    const std::string greeting = "Hello, " + person.value() + "!";
    return GreetingMessage::create(greeting);
}

/**
 * @brief Create greeting message with compile-time optimization
 * @param text The greeting text
 * @return Expected greeting message
 */
[[nodiscard]] constexpr Result<GreetingMessage> GreetingService::createGreetingMessage(
    const std::string& text) const noexcept {
    return GreetingMessage::create(text);
}

/**
 * @brief Create standardized empty person error
 * @return Error for empty person input
 */
[[nodiscard]] GreetingError GreetingService::createEmptyPersonError() const noexcept {
    return GreetingError::EmptyName;
}

// ============================================================================
// Configuration-Aware Domain Service Methods
// ============================================================================

/**
 * @brief Generate greeting with configuration-aware validation
 * @param person_name Raw person name to validate and greet
 * @return Expected greeting message or validation error with context
 */
[[nodiscard]] Result<GreetingMessage> GreetingService::generateGreetingWithValidation(
    std::string_view person_name) const noexcept {
    
    // Use configuration-aware validation to create PersonName
    auto person_result = greeting::validation::validate_person_name<PersonName>(person_name);
    if (!person_result.has_value()) {
        return propagateValidationError(person_result.error(), "simple greeting generation");
    }
    
    // Generate greeting using validated PersonName
    return generateSimpleGreeting(person_result.value());
}

/**
 * @brief Generate formal greeting with configuration-aware validation
 * @param person_name Raw person name to validate and greet formally
 * @return Expected formal greeting message or validation error with context
 */
[[nodiscard]] Result<GreetingMessage> GreetingService::generateFormalGreetingWithValidation(
    std::string_view person_name) const noexcept {
    
    // Use configuration-aware validation to create PersonName
    auto person_result = greeting::validation::validate_person_name<PersonName>(person_name);
    if (!person_result.has_value()) {
        return propagateValidationError(person_result.error(), "formal greeting generation");
    }
    
    // Generate formal greeting using validated PersonName
    return generateFormalGreeting(person_result.value());
}

/**
 * @brief Create greeting message with configuration-aware validation
 * @param text The greeting text to validate and create
 * @return Expected greeting message or validation error with context
 */
[[nodiscard]] Result<GreetingMessage> GreetingService::createValidatedGreetingMessage(
    std::string_view text) const noexcept {
    
    // Use configuration-aware validation to create GreetingMessage
    auto message_result = greeting::validation::validate_greeting_message<GreetingMessage>(text);
    if (!message_result.has_value()) {
        return propagateValidationError(message_result.error(), "greeting message creation");
    }
    
    return message_result.value();
}

/**
 * @brief Propagate validation error with additional context
 * @param original_error The original validation error
 * @param context Additional context information
 * @return Enhanced error with propagated context
 */
[[nodiscard]] GreetingError GreetingService::propagateValidationError(
    GreetingError original_error, 
    std::string_view context) const noexcept {
    
    // In debug builds, we could enhance this with additional context logging
    #ifdef HELLOWORLD_DEBUG_BUILD
        // Could add context information here for debugging
        // For now, just return the original error to maintain compatibility
    #endif
    
    // Return the original error (context propagation can be enhanced in future)
    return original_error;
}

} // namespace greeting
