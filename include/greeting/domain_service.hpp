#pragma once

#include "types.hpp"

namespace greeting {

/**
 * @brief Domain service for greeting generation
 * 
 * Responsible for coordinating greeting generation using domain types
 * and business rules. Follows single responsibility principle.
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
};

} // namespace greeting
