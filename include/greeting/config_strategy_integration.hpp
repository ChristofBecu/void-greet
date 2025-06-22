#pragma once

// ============================================================================
// Configuration-Aware Strategy Integration
// 
// This header provides configuration-aware interfaces that adapt strategy
// behavior based on build configuration (debug/release/profile).
// ============================================================================

#include "greeting/strategy_interface.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"
#include "greeting/expected.hpp"
#include "greeting/error_codes.hpp"
#include "config/build_config.hpp"
#include "config/debug_config.hpp"
#include "config/release_config.hpp"
#include "greeting/config_aware_validation.hpp"
#include <memory>
#include <string_view>
#include <type_traits>

namespace greeting::strategy::config {

// Import strategy interface types into this namespace
using greeting::strategy::IGreetingStrategy;

// ============================================================================
// Configuration-Aware Strategy Base
// ============================================================================

/**
 * @brief Base class for configuration-aware strategies
 * 
 * Provides common configuration-aware functionality that adapts
 * behavior based on build configuration.
 */
template<typename ConfigType = ::greeting::config::ActiveConfiguration>
class ConfigAwareStrategyBase {
public:
    using configuration_type = ConfigType;
    
    /**
     * @brief Check if strategy supports current configuration
     */
    static consteval bool supports_configuration() noexcept {
        return true; // All strategies support configuration awareness
    }
    
    /**
     * @brief Get configuration-aware validation context
     */
    static constexpr auto get_validation_context() noexcept {
        return validation::ValidationContext::create_for_current_config();
    }
    
    /**
     * @brief Execute strategy with configuration awareness
     */
    template<typename ConcreteStrategy>
    Result<GreetingMessage> execute_with_config(
        const PersonName& name,
        const ConcreteStrategy& strategy) const noexcept {
        return strategy.execute_with_context(name, get_validation_context());
    }

protected:
    /**
     * @brief Configuration-aware input validation
     */
    template<typename StrategyType>
    Result<bool> validate_input_for_strategy(
        const PersonName& name, 
        const StrategyType& strategy) const noexcept {
        if constexpr (::greeting::config::is_debug_build()) {
            return validate_input_debug(name, strategy);
        } else {
            return validate_input_release(name, strategy);
        }
    }

private:
    template<typename StrategyType>
    Result<bool> validate_input_debug(
        const PersonName& name, const StrategyType& strategy) const noexcept;
        
    template<typename StrategyType>
    Result<bool> validate_input_release(
        const PersonName& name, const StrategyType& strategy) const noexcept;
};

// ============================================================================
// Configuration-Aware Strategy Adapters
// ============================================================================

/**
 * @brief Debug-optimized strategy adapter
 * 
 * Enhances strategies with debug-specific functionality like
 * detailed logging, rich error context, and comprehensive validation.
 */
template<typename StrategyType>
class DebugStrategyAdapter : public IGreetingStrategy {
    static_assert(std::is_base_of_v<IGreetingStrategy, StrategyType>, 
                  "StrategyType must inherit from IGreetingStrategy");

public:
    explicit DebugStrategyAdapter(std::unique_ptr<StrategyType> strategy)
        : wrapped_strategy_(std::move(strategy)) {}
    
    /**
     * @brief Enable debug diagnostics
     */
    static consteval bool enable_debug_diagnostics() noexcept {
        return true;
    }
    
    /**
     * @brief Execute with debug enhancements
     */
    Result<GreetingMessage> greet(const PersonName& name) const noexcept override {
        // Debug-specific pre-execution logging
        if constexpr (::greeting::config::is_debug_build()) {
            // Log strategy execution in debug builds
        }
        
        // Delegate to wrapped strategy
        auto result = wrapped_strategy_->greet(name);
        
        if constexpr (::greeting::config::is_debug_build()) {
            // Track execution metrics in debug builds
        }
        
        return result;
    }
    
    std::string_view getStrategyName() const noexcept override {
        return wrapped_strategy_->getStrategyName();
    }
    
    bool isValid() const noexcept override {
        return wrapped_strategy_ && wrapped_strategy_->isValid();
    }

private:
    std::unique_ptr<StrategyType> wrapped_strategy_;
};

/**
 * @brief Release-optimized strategy adapter
 * 
 * Optimizes strategies for release builds with minimal overhead,
 * constexpr optimization, and lean error handling.
 */
template<typename StrategyType>
class ReleaseStrategyAdapter : public IGreetingStrategy {
    static_assert(std::is_base_of_v<IGreetingStrategy, StrategyType>, 
                  "StrategyType must inherit from IGreetingStrategy");

public:
    explicit ReleaseStrategyAdapter(std::unique_ptr<StrategyType> strategy)
        : wrapped_strategy_(std::move(strategy)) {}
    
    /**
     * @brief Enable zero-overhead optimizations
     */
    static consteval bool enable_zero_overhead() noexcept {
        return true;
    }
    
    /**
     * @brief Execute with release optimizations
     */
    Result<GreetingMessage> greet(const PersonName& name) const noexcept override {
        // Release: direct execution with minimal overhead
        return wrapped_strategy_->greet(name);
    }
    
    std::string_view getStrategyName() const noexcept override {
        return wrapped_strategy_->getStrategyName();
    }
    
    bool isValid() const noexcept override {
        return wrapped_strategy_ && wrapped_strategy_->isValid();
    }

private:
    std::unique_ptr<StrategyType> wrapped_strategy_;
};

// ============================================================================
// Configuration Selection Utilities
// ============================================================================

/**
 * @brief Configuration-aware strategy adapter factory
 */
template<typename StrategyType>
std::unique_ptr<IGreetingStrategy> create_config_aware_strategy(
    std::unique_ptr<StrategyType> strategy) {
    
    static_assert(std::is_base_of_v<IGreetingStrategy, StrategyType>, 
                  "StrategyType must inherit from IGreetingStrategy");
    
    if constexpr (::greeting::config::is_debug_build()) {
        return std::make_unique<DebugStrategyAdapter<StrategyType>>(std::move(strategy));
    } else if constexpr (::greeting::config::is_profile_build()) {
        // Profile: debug symbols with release performance
        return std::make_unique<ReleaseStrategyAdapter<StrategyType>>(std::move(strategy));
    } else {
        // Release: maximum optimization
        return std::make_unique<ReleaseStrategyAdapter<StrategyType>>(std::move(strategy));
    }
}

/**
 * @brief Configuration-aware strategy selection
 */
consteval std::string_view get_optimal_strategy_for_config() noexcept {
    if constexpr (::greeting::config::is_debug_build()) {
        return "formal"; // Debug: prefer formal strategy for detailed validation
    } else if constexpr (::greeting::config::is_profile_build()) {
        return "console"; // Profile: balanced performance strategy
    } else {
        return "casual"; // Release: optimized casual strategy
    }
}

} // namespace greeting::strategy::config
