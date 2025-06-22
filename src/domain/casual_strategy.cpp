/**
 * @file casual_strategy.cpp
 * @brief Casual greeting strategy implementation
 * 
 * Implements relaxed, informal greeting generation with configuration-aware
 * validation and error handling. Supports casual name formats and varied
 * greeting patterns.
 */

#include "greeting/casual_strategy.hpp"
#include "greeting/config_aware_validation.hpp"
#include "config/build_config.hpp"
#include <array>
#include <algorithm>
#include <optional>
#include <cctype>
#include <memory>

namespace greeting::strategy {

/**
 * @brief Generate casual greeting with configuration awareness
 * @param person The person to greet casually
 * @return Expected casual greeting message or configuration-aware error
 */
[[nodiscard]] Result<GreetingMessage> CasualStrategy::greet(
    const PersonName& person) const noexcept {
    
    if consteval {
        // Compile-time path for constant expressions
        const std::string greeting = "Hi, " + person.value() + "!";
        return GreetingMessage::create(greeting);
    } else {
        // Runtime path with configuration-aware validation
        if (person.value().empty()) {
            return createConfigAwareError(
                GreetingError::EmptyName, 
                "casual greeting generation"
            );
        }
        
        try {
            const auto base_greeting = createBaseCasualGreeting(person);
            const auto varied_greeting = applyCasualVariations(base_greeting, person);
            
            return GreetingMessage::create(varied_greeting);
        } catch (...) {
            return createConfigAwareError(
                GreetingError::InvalidState,
                "casual greeting creation"
            );
        }
    }
}

/**
 * @brief Generate casual greeting with validation
 * @param name Raw name input with relaxed validation
 * @return Expected casual greeting or validation error
 */
[[nodiscard]] Result<GreetingMessage> CasualStrategy::generateCasualGreeting(
    std::string_view name) const noexcept {
    
    // Use casual name validation (more permissive than formal)
    auto person_result = validateCasualName(name);
    if (!person_result.has_value()) {
        return person_result.error();
    }
    
    return greet(person_result.value());
}

/**
 * @brief Validate name with casual constraints
 * @param name Name to validate with casual rules
 * @return Expected PersonName or validation error
 */
[[nodiscard]] Result<PersonName> CasualStrategy::validateCasualName(
    std::string_view name) const noexcept {
    
    // Casual validation is more permissive than standard validation
    if (auto error = validateCasualNameImpl(name)) {
        return createConfigAwareError(*error, "casual name validation");
    }
    
    // Use direct PersonName creation with relaxed constraints
    // Skip standard validation which is too strict for casual use
    if constexpr (greeting::config::is_debug_build()) {
        // Debug: Apply basic validation but allow relaxed constraints
        if (name.empty()) {
            return GreetingError::EmptyName;
        }
        
        // Check for whitespace-only names
        if (std::all_of(name.begin(), name.end(), 
                       [](char c) { return std::isspace(static_cast<unsigned char>(c)); })) {
            return GreetingError::EmptyName;
        }
        
        if (name.length() > 100) {
            return GreetingError::NameTooLong;
        }
        
        // Relaxed character validation for casual names (allows underscores)
        for (size_t i = 0; i < name.length(); ++i) {
            const char c = name[i];
            if (!(std::isalnum(static_cast<unsigned char>(c)) || 
                  c == ' ' || c == '-' || c == '\'' || c == '.' || c == '_')) {
                return GreetingError::InvalidName;
            }
        }
        
        // Create PersonName with casual validation
        return PersonName{std::string{name}, PersonName::InternalTag{}};
    } else {
        // Release: Use optimized validation with casual constraints
        if (name.empty()) {
            return GreetingError::EmptyName;
        }
        
        if (name.length() > 100) {
            return GreetingError::NameTooLong;
        }
        
        // Relaxed character validation for casual names
        for (size_t i = 0; i < name.length(); ++i) {
            const char c = name[i];
            if (!(std::isalnum(static_cast<unsigned char>(c)) || 
                  c == ' ' || c == '-' || c == '\'' || c == '.' || c == '_')) {
                return GreetingError::InvalidName;
            }
        }
        
        // Create PersonName directly for performance
        return PersonName{std::string{name}, PersonName::InternalTag{}};
    }
}

/**
 * @brief Factory method for strategy creation
 */
[[nodiscard]] std::unique_ptr<CasualStrategy> CasualStrategy::create() noexcept {
    return std::make_unique<CasualStrategy>();
}

/**
 * @brief Generate base casual greeting
 * @param person Validated person name
 * @return Base greeting message
 */
[[nodiscard]] std::string CasualStrategy::createBaseCasualGreeting(
    const PersonName& person) const noexcept {
    
    const auto prefix = getRandomCasualPrefix();
    
    // Extract first name for casual greeting
    const auto full_name = person.value();
    const auto space_pos = full_name.find(' ');
    const auto first_name = (space_pos != std::string::npos) 
        ? full_name.substr(0, space_pos)
        : full_name;
    
    return std::string{prefix} + ", " + first_name + "!";
}

/**
 * @brief Apply casual greeting variations
 * @param base_greeting Base greeting to vary
 * @param person Person context for variation
 * @return Varied greeting message
 */
[[nodiscard]] std::string CasualStrategy::applyCasualVariations(
    std::string_view base_greeting,
    const PersonName& person) const noexcept {
    
    std::string varied = std::string{base_greeting};
    
    // Apply random variations based on configuration
    if constexpr (greeting::config::is_debug_build()) {
        // Debug: Apply rich variations for testing
        
        // Add casual suffixes occasionally
        std::uniform_int_distribution<> suffix_dist(1, 4);
        if (suffix_dist(random_generator_) == 1) {
            const std::array<std::string_view, 4> suffixes = {
                " How's it going?", " What's up?", " How are you?", ""
            };
            
            std::uniform_int_distribution<size_t> suffix_choice(0, suffixes.size() - 1);
            varied += suffixes[suffix_choice(random_generator_)];
        }
        
        // Add emoji occasionally (debug only)
        std::uniform_int_distribution<> emoji_dist(1, 5);
        if (emoji_dist(random_generator_) == 1) {
            varied += " 😊";
        }
    } else {
        // Release: Minimal variations for performance
        // Keep base greeting simple in release builds
    }
    
    return varied;
}

/**
 * @brief Get random casual prefix
 * @return Random casual greeting prefix
 */
[[nodiscard]] std::string_view CasualStrategy::getRandomCasualPrefix() const noexcept {
    
    static constexpr std::array<std::string_view, 6> casual_prefixes = {
        "Hi", "Hey", "Hello", "What's up", "Howdy", "Yo"
    };
    
    std::uniform_int_distribution<size_t> dist(0, casual_prefixes.size() - 1);
    return casual_prefixes[dist(random_generator_)];
}

/**
 * @brief Casual name validation implementation
 * @param name Name to validate with relaxed rules
 * @return Validation result with casual constraints
 */
[[nodiscard]] std::optional<GreetingError> CasualStrategy::validateCasualNameImpl(
    std::string_view name) const noexcept {
    
    // Basic validation with relaxed constraints
    if (name.empty()) {
        return GreetingError::EmptyName;
    }
    
    // Check for whitespace-only names
    if (std::all_of(name.begin(), name.end(), 
                   [](char c) { return std::isspace(static_cast<unsigned char>(c)); })) {
        return GreetingError::EmptyName;
    }
    
    // Casual mode allows shorter names (minimum 1 character)
    if (name.length() > 100) {
        return GreetingError::NameTooLong;
    }
    
    // More permissive character validation for casual names
    for (char c : name) {
        // Allow alphanumeric, space, hyphen, apostrophe, period, and some casual chars
        if (!(std::isalnum(static_cast<unsigned char>(c)) || 
              c == ' ' || c == '-' || c == '\'' || c == '.' || c == '_')) {
            return GreetingError::InvalidName;
        }
    }
    
    return std::nullopt; // Valid
}

/**
 * @brief Configuration-aware error creation
 * @param error Base error code
 * @param context Error context for debugging
 * @return Configuration-appropriate error
 */
[[nodiscard]] GreetingError CasualStrategy::createConfigAwareError(
    GreetingError error,
    std::string_view context) const noexcept {
    
    if constexpr (greeting::config::is_debug_build()) {
        // Debug: Could enhance with additional context logging
        // For now, return the base error
        return error;
    } else {
        // Release: Return minimal error for performance
        return error;
    }
}

} // namespace greeting::strategy

// ============================================================================
// Strategy Factory Function
// ============================================================================

namespace greeting::strategy {

/**
 * @brief Factory function for creating casual strategy instances
 * @return Unique pointer to casual strategy implementation
 */
[[nodiscard]] std::unique_ptr<IGreetingStrategy> createCasualStrategy() noexcept {
    return std::make_unique<CasualStrategy>();
}

} // namespace greeting::strategy
