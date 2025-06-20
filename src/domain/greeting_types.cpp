/**
 * @file greeting_types.cpp
 * @brief Core domain types implementation
 * 
 * Contains the implementation of domain services that orchestrate
 * greeting business logic while maintaining clean architecture principles.
 */

#include "greeting/types.hpp"

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
    [[nodiscard]] Result<GreetingMessage> generateFormalGreeting(
        const PersonName& person) const noexcept {
        
        if (person.empty()) {
            return createEmptyPersonError();
        }
        
        const std::string greeting = "Hello, " + person.value() + "!";
        return GreetingMessage::create(greeting);
    }

private:
    /**
     * @brief Create greeting message with compile-time optimization
     * @param text The greeting text
     * @return Expected greeting message
     */
    [[nodiscard]] constexpr Result<GreetingMessage> createGreetingMessage(
        const std::string& text) const noexcept {
        return GreetingMessage::create(text);
    }
    
    /**
     * @brief Create standardized empty person error
     * @return Error for empty person input
     */
    [[nodiscard]] GreetingError createEmptyPersonError() const noexcept {
        return GreetingError::EmptyName;
    }
};

} // namespace greeting
