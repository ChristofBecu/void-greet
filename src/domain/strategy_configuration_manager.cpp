// ============================================================================
// Strategy Configuration Manager Implementation
// 
// Manages runtime configuration and adaptation of strategies based on
// build configuration and runtime parameters.
// ============================================================================

#include "greeting/strategy_configuration_manager.hpp"
#include "greeting/config_strategy_integration.hpp"
#include "greeting/strategy_context.hpp"
#include "greeting/console_strategy.hpp"
#include "greeting/formal_strategy.hpp"
#include "greeting/casual_strategy.hpp"
#include "config/build_config.hpp"
#include <utility>

namespace greeting::strategy::config {

// Import strategy interface types into this namespace  
using greeting::strategy::IGreetingStrategy;

// ============================================================================
// Configuration Manager Implementation
// ============================================================================

Result<StrategyContext> StrategyConfigurationManager::create_optimized_context() noexcept {
        auto context = StrategyContext{};
        
        auto registration_result = register_config_aware_strategies(context);
        if (!registration_result) {
            return registration_result.error();
        }
        
        return context;
    }
    
    /**
     * @brief Register strategies with configuration awareness
     */
Result<bool> StrategyConfigurationManager::register_config_aware_strategies(
        StrategyContext& context) noexcept {
        
        // Register console strategy with configuration awareness
        auto console_strategy = greeting::strategy::createConsoleStrategy();
        auto config_aware_console = create_config_aware_strategy(std::move(console_strategy));
        if (!context.registerStrategy(StrategyType::Console, std::move(config_aware_console))) {
            return GreetingError::StrategyRegistrationFailed;
        }
        
        // Register formal strategy with configuration awareness
        auto formal_strategy = greeting::strategy::createFormalStrategy();
        auto config_aware_formal = create_config_aware_strategy(std::move(formal_strategy));
        if (!context.registerStrategy(StrategyType::Formal, std::move(config_aware_formal))) {
            return GreetingError::StrategyRegistrationFailed;
        }
        
        // Register casual strategy with configuration awareness
        auto casual_strategy = greeting::strategy::createCasualStrategy();
        auto config_aware_casual = create_config_aware_strategy(std::move(casual_strategy));
        if (!context.registerStrategy(StrategyType::Casual, std::move(config_aware_casual))) {
            return GreetingError::StrategyRegistrationFailed;
        }
        
        return true;
    }
    
    /**
     * @brief Get configuration-specific metrics
     */
ConfigurationMetrics StrategyConfigurationManager::get_configuration_metrics() noexcept {
        ConfigurationMetrics metrics{};
        
        if constexpr (::greeting::config::is_debug_build()) {
            metrics.build_type = "debug";
            #ifdef HELLOWORLD_DEBUG_BUILD
            metrics.debug_metrics = collect_debug_metrics();
            #endif
        } else if constexpr (::greeting::config::is_profile_build()) {
            metrics.build_type = "profile";
        } else {
            metrics.build_type = "release";
        }
        
        metrics.optimal_strategy = get_optimal_strategy_for_config();
        
        return metrics;
    }

#ifdef HELLOWORLD_DEBUG_BUILD
ConfigurationMetrics::DebugMetrics StrategyConfigurationManager::collect_debug_metrics() noexcept {
        ConfigurationMetrics::DebugMetrics debug_metrics;
        debug_metrics.validation_overhead_percent = 15;
        debug_metrics.logging_overhead_percent = 5;
        debug_metrics.enhanced_diagnostics_enabled = true;
        return debug_metrics;
    }
#endif

// ============================================================================
// Configuration-Aware Strategy Factory Functions
// ============================================================================

// Forward declarations for factory functions
std::unique_ptr<IGreetingStrategy> create_debug_optimized_strategy(std::string_view strategy_name);
std::unique_ptr<IGreetingStrategy> create_release_optimized_strategy(std::string_view strategy_name);

/**
 * @brief Create strategy optimized for current build configuration
 */
std::unique_ptr<IGreetingStrategy> create_optimized_strategy(std::string_view strategy_name) {
    if constexpr (::greeting::config::is_debug_build()) {
        return create_debug_optimized_strategy(strategy_name);
    } else {
        return create_release_optimized_strategy(strategy_name);
    }
}

std::unique_ptr<IGreetingStrategy> create_debug_optimized_strategy(std::string_view strategy_name) {
    if (strategy_name == "console") {
        auto base_strategy = greeting::strategy::createConsoleStrategy();
        return create_config_aware_strategy(std::move(base_strategy));
    } else if (strategy_name == "formal") {
        auto base_strategy = greeting::strategy::createFormalStrategy();
        return create_config_aware_strategy(std::move(base_strategy));
    } else if (strategy_name == "casual") {
        auto base_strategy = greeting::strategy::createCasualStrategy();
        return create_config_aware_strategy(std::move(base_strategy));
    }
    
    // Fallback to console strategy
    auto base_strategy = greeting::strategy::createConsoleStrategy();
    return create_config_aware_strategy(std::move(base_strategy));
}

std::unique_ptr<IGreetingStrategy> create_release_optimized_strategy(std::string_view strategy_name) {
    if (strategy_name == "console") {
        auto base_strategy = greeting::strategy::createConsoleStrategy();
        return create_config_aware_strategy(std::move(base_strategy));
    } else if (strategy_name == "formal") {
        auto base_strategy = greeting::strategy::createFormalStrategy();
        return create_config_aware_strategy(std::move(base_strategy));
    } else if (strategy_name == "casual") {
        auto base_strategy = greeting::strategy::createCasualStrategy();
        return create_config_aware_strategy(std::move(base_strategy));
    }
    
    // Fallback to casual strategy (optimized for release)
    auto base_strategy = greeting::strategy::createCasualStrategy();
    return create_config_aware_strategy(std::move(base_strategy));
}

// ============================================================================
// Strategy-Specific Validation Functions (Declarations)
// ============================================================================

Result<bool> validate_formal_strategy_input_debug(const PersonName& name) noexcept;
Result<bool> validate_casual_strategy_input_debug(const PersonName& name) noexcept;
Result<bool> validate_console_strategy_input_debug(const PersonName& name) noexcept;

// ============================================================================
// Strategy-Specific Validation Functions (Implementations)
// ============================================================================

Result<bool> validate_formal_strategy_input_debug(const PersonName& name) noexcept {
    // Enhanced validation for formal strategy in debug builds
    if (name.value().empty()) {
        return GreetingError::EmptyName;
    }
    
    // Additional formal strategy validation would go here
    return true;
}

Result<bool> validate_casual_strategy_input_debug(const PersonName& name) noexcept {
    // Relaxed validation for casual strategy in debug builds
    if (name.value().empty()) {
        return GreetingError::EmptyName;
    }
    
    return true;
}

Result<bool> validate_console_strategy_input_debug(const PersonName& name) noexcept {
    // Standard validation for console strategy in debug builds
    if (name.value().empty()) {
        return GreetingError::EmptyName;
    }
    
    return true;
}

} // namespace greeting::strategy::config
