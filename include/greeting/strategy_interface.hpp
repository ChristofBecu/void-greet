#pragma once

#include "greeting/expected.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"
#include "greeting/error_codes.hpp"
#include "config/build_config.hpp"
#include "greeting/config_aware_type_system.hpp"
#include <type_traits>
#include <string_view>

#ifdef __cpp_concepts
#include <concepts>
#endif

namespace greeting::strategy {

/**
 * @brief Pure virtual base interface for greeting strategies
 * 
 * Provides configuration-aware greeting generation with proper error handling.
 * Integrates with existing validation infrastructure and maintains zero-overhead
 * abstractions in release builds.
 */
class IGreetingStrategy {
public:
    /**
     * @brief Configuration-aware virtual destructor
     * @details In debug builds, may log strategy destruction for diagnostics
     */
    virtual ~IGreetingStrategy() noexcept = default;

    /**
     * @brief Generate greeting message for given person
     * @param person Validated person name to greet
     * @return Expected greeting message or error with configuration-aware context
     */
    [[nodiscard]] virtual Result<GreetingMessage> greet(
        const PersonName& person) const noexcept = 0;

    /**
     * @brief Get strategy identification for debugging/logging
     * @return Strategy name for diagnostic purposes
     */
    [[nodiscard]] virtual std::string_view getStrategyName() const noexcept = 0;

    /**
     * @brief Configuration-aware strategy validation
     * @return True if strategy is properly configured
     */
    [[nodiscard]] virtual bool isValid() const noexcept = 0;

protected:
    /**
     * @brief Protected default constructor for inheritance only
     */
    IGreetingStrategy() = default;

    /**
     * @brief Non-copyable to prevent slicing
     */
    IGreetingStrategy(const IGreetingStrategy&) = delete;
    IGreetingStrategy& operator=(const IGreetingStrategy&) = delete;

    /**
     * @brief Moveable for performance
     */
    IGreetingStrategy(IGreetingStrategy&&) = default;
    IGreetingStrategy& operator=(IGreetingStrategy&&) = default;
};

} // namespace greeting::strategy

#ifdef __cpp_concepts

namespace greeting::strategy::concepts {

/**
 * @brief Concept defining requirements for greeting strategy implementations
 */
template<typename T>
concept GreetingStrategy = requires(const T strategy, const PersonName& person) {
    // Must inherit from base interface
    requires std::derived_from<T, IGreetingStrategy>;
    
    // Must implement core greeting functionality
    { strategy.greet(person) } -> std::same_as<Result<GreetingMessage>>;
    
    // Must provide strategy identification
    { strategy.getStrategyName() } -> std::convertible_to<std::string_view>;
    
    // Must support validation
    { strategy.isValid() } -> std::same_as<bool>;
    
    // Must be noexcept for performance
    requires noexcept(strategy.greet(person));
    requires noexcept(strategy.getStrategyName());
    requires noexcept(strategy.isValid());
};

/**
 * @brief Concept for configuration-aware strategies
 */
template<typename T>
concept ConfigAwareStrategy = GreetingStrategy<T> && requires {
    // Must integrate with configuration system
    requires greeting::types::ConfigAwareValidatable<T>;
    
    // Must support zero-overhead optimization
    requires greeting::types::ZeroOverheadValidatable<T>;
};

/**
 * @brief Concept for strategies supporting rich debug context
 */
template<typename T>
concept DebugAwareStrategy = ConfigAwareStrategy<T> && requires(const T strategy) {
    // Must provide debug information when available
    { strategy.getDebugInfo() } -> std::convertible_to<std::string>;
} && greeting::config::is_debug_build();

} // namespace greeting::strategy::concepts

#endif // __cpp_concepts

namespace greeting::strategy::traits {

/**
 * @brief Type traits for strategy optimization
 */
template<typename Strategy>
struct StrategyTraits {
    static constexpr bool supports_debug_info = greeting::config::is_debug_build();
    static constexpr bool enable_performance_monitoring = greeting::config::is_debug_build();
    static constexpr bool enable_validation_caching = greeting::config::is_debug_build();
    
    // Strategy-specific optimization hints
    static constexpr size_t expected_message_length = 50;
    static constexpr bool is_thread_safe = false;
    static constexpr bool supports_async_operation = false;
};

/**
 * @brief Configuration-aware strategy validation
 */
template<typename Strategy>
constexpr bool validateStrategyAtCompileTime() noexcept {
    #ifdef __cpp_concepts
    return concepts::ConfigAwareStrategy<Strategy>;
    #else
    return std::is_base_of_v<IGreetingStrategy, Strategy> &&
           std::is_nothrow_destructible_v<Strategy>;
    #endif
}

} // namespace greeting::strategy::traits

namespace greeting::strategy::errors {

/**
 * @brief Strategy-specific error types extending existing enumeration
 */
enum class StrategyError : std::uint16_t {
    StrategyNotInitialized = 3001,    ///< Strategy not properly initialized
    StrategyExecutionFailed = 3002,   ///< Strategy execution encountered error
    StrategyValidationFailed = 3003,  ///< Strategy validation failed
    StrategyConfigurationInvalid = 3004, ///< Strategy configuration is invalid
    StrategyResourceUnavailable = 3005   ///< Required strategy resources unavailable
};

/**
 * @brief Configuration-aware error creation for strategies
 */
template<StrategyError ErrorCode>
[[nodiscard]] constexpr GreetingError makeStrategyError() noexcept {
    return static_cast<GreetingError>(ErrorCode);
}

/**
 * @brief Strategy error propagation utility
 */
template<typename T>
[[nodiscard]] Result<T> propagateStrategyError(
    GreetingError originalError,
    std::string_view strategyName,
    std::string_view operation) noexcept {
    
    // For now, just propagate the original error
    // Future enhancement: add contextual information in debug builds
    return Expected<T, GreetingError>{originalError};
}

} // namespace greeting::strategy::errors

namespace greeting::strategy::optimization {

/**
 * @brief Compile-time strategy selection optimization
 */
template<typename Strategy>
constexpr bool canOptimizeStrategy() noexcept {
    return greeting::config::is_release_build() &&
           traits::StrategyTraits<Strategy>::is_thread_safe &&
           std::is_trivially_copyable_v<Strategy>;
}

/**
 * @brief Configuration-aware strategy caching
 */
template<typename Strategy>
class StrategyCache {
public:
    static constexpr size_t CACHE_SIZE = 
        greeting::config::is_debug_build() ? 16 : 0;
        
    [[nodiscard]] static constexpr bool shouldCache() noexcept {
        return CACHE_SIZE > 0;
    }
    
    // Cache implementation details would go here
};

#ifdef __cpp_concepts
/**
 * @brief Zero-overhead strategy wrapper for release builds
 */
template<typename ConcreteStrategy>
    requires greeting::strategy::concepts::ConfigAwareStrategy<ConcreteStrategy>
class OptimizedStrategyWrapper {
public:
    template<typename... Args>
    explicit OptimizedStrategyWrapper(Args&&... args) 
        : strategy_(std::forward<Args>(args)...) {
        
        if constexpr (greeting::config::is_debug_build()) {
            validateStrategyConfiguration();
        }
    }
    
    [[nodiscard]] Result<GreetingMessage> greet(const PersonName& person) const noexcept {
        if constexpr (greeting::config::is_debug_build()) {
            // Debug validation and logging
            if (!strategy_.isValid()) {
                return errors::makeStrategyError<errors::StrategyError::StrategyValidationFailed>();
            }
        }
        
        return strategy_.greet(person);
    }
    
private:
    ConcreteStrategy strategy_;
    
    void validateStrategyConfiguration() const {
        // Configuration validation implementation
    }
};
#endif // __cpp_concepts

} // namespace greeting::strategy::optimization
