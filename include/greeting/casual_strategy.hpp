#pragma once

#include "strategy_interface.hpp"
#include "person_name.hpp"
#include "greeting_message.hpp"
#include "config_aware_validation.hpp"
#include "config_aware_type_system.hpp"
#include "expected.hpp"
#include <string>
#include <string_view>
#include <random>
#include <memory>

namespace greeting::strategy {

/**
 * @brief Casual greeting strategy implementation
 * 
 * Provides informal, relaxed greeting generation with more permissive
 * validation rules. Supports casual formats like nicknames, shortened
 * names, and informal expressions.
 * 
 * Features:
 * - Relaxed name validation (supports "Jo", "Al", casual formats)
 * - Multiple casual greeting patterns
 * - Configuration-aware validation integration
 * - Random greeting variation support
 * - C++23 consteval optimization
 */
class CasualStrategy final : public IGreetingStrategy {
public:
    /**
     * @brief Strategy identification
     */
    static constexpr std::string_view strategy_name() noexcept {
        return "casual";
    }
    
    /**
     * @brief Configuration-aware casual greeting generation
     * @param person The person to greet casually
     * @return Expected casual greeting message or configuration-aware error
     */
    [[nodiscard]] Result<GreetingMessage> greet(
        const PersonName& person) const noexcept override;
    
    /**
     * @brief Generate casual greeting with validation
     * @param name Raw name input with relaxed validation
     * @return Expected casual greeting or validation error
     */
    [[nodiscard]] Result<GreetingMessage> generateCasualGreeting(
        std::string_view name) const noexcept;
    
    /**
     * @brief Validate name with casual constraints
     * @param name Name to validate with casual rules
     * @return Expected PersonName or validation error
     */
    [[nodiscard]] Result<PersonName> validateCasualName(
        std::string_view name) const noexcept;
    
    /**
     * @brief Get strategy identification for diagnostics
     * @return Strategy name for debugging/logging
     */
    [[nodiscard]] std::string_view getStrategyName() const noexcept override {
        return strategy_name();
    }
    
    /**
     * @brief Configuration-aware strategy validation
     * @return True if strategy is properly configured
     */
    [[nodiscard]] bool isValid() const noexcept override {
        return true; // Casual strategy is always valid
    }
    
    /**
     * @brief Factory method for strategy creation
     */
    [[nodiscard]] static std::unique_ptr<CasualStrategy> create() noexcept;

private:
    /**
     * @brief Generate base casual greeting
     * @param person Validated person name
     * @return Base greeting message
     */
    [[nodiscard]] std::string createBaseCasualGreeting(
        const PersonName& person) const noexcept;
    
    /**
     * @brief Apply casual greeting variations
     * @param base_greeting Base greeting to vary
     * @param person Person context for variation
     * @return Varied greeting message
     */
    [[nodiscard]] std::string applyCasualVariations(
        std::string_view base_greeting,
        const PersonName& person) const noexcept;
    
    /**
     * @brief Get random casual prefix
     * @return Random casual greeting prefix
     */
    [[nodiscard]] std::string_view getRandomCasualPrefix() const noexcept;
    
    /**
     * @brief Casual name validation implementation
     * @param name Name to validate with relaxed rules
     * @return Validation result with casual constraints
     */
    [[nodiscard]] std::optional<GreetingError> validateCasualNameImpl(
        std::string_view name) const noexcept;
    
    /**
     * @brief Configuration-aware error creation
     * @param error Base error code
     * @param context Error context for debugging
     * @return Configuration-appropriate error
     */
    [[nodiscard]] GreetingError createConfigAwareError(
        GreetingError error,
        std::string_view context) const noexcept;

    // Strategy state
    mutable std::random_device random_device_;
    mutable std::mt19937 random_generator_{random_device_()};
};

} // namespace greeting::strategy

// Specialization must be in greeting::strategy::traits namespace  
namespace greeting::strategy::traits {

/**
 * @brief Casual strategy traits specialization
 */
template<>
struct StrategyTraits<greeting::strategy::CasualStrategy> {
    static constexpr bool supports_debug_info = true;
#ifdef DEBUG
    static constexpr bool enable_performance_monitoring = true;
    static constexpr bool enable_validation_caching = false; // Casual doesn't need caching
#else
    static constexpr bool enable_performance_monitoring = false;
    static constexpr bool enable_validation_caching = false;
#endif
    
    static constexpr size_t expected_message_length = 50; // Shorter for casual greetings
    static constexpr bool is_thread_safe = false; // Uses mutable random state
    static constexpr bool supports_async_operation = true;
    static constexpr bool requires_system_resources = false;
    static constexpr bool supports_localization = true;
    static constexpr bool supports_cultural_context = false; // Casual is universal
    static constexpr bool supports_variations = true;
    static constexpr bool supports_nicknames = true;
    static constexpr size_t min_name_length = 1; // More permissive than formal (was 2)
    static constexpr size_t max_name_length = 100;
};

} // namespace greeting::strategy::traits
