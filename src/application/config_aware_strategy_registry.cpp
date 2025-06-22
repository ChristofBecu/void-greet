// ============================================================================
// Configuration-Aware Strategy Registry Implementation
// 
// Implements build-specific strategy registration and management with
// configuration-aware optimization and validation.
// ============================================================================

#include "greeting/strategy_factory_config.hpp"
#include "greeting/config_strategy_integration.hpp"
#include "greeting/console_strategy.hpp"
#include "greeting/formal_strategy.hpp"
#include "greeting/casual_strategy.hpp"
#include "config/build_config.hpp"
#include <unordered_map>
#include <memory>

namespace greeting::factory::config {

// Import strategy interface types into this namespace
using greeting::strategy::IGreetingStrategy;

// ============================================================================
// Debug Configuration Factory Implementation
// ============================================================================

std::unique_ptr<IGreetingStrategy> DebugConfigStrategyFactory::create_debug_console_strategy() const {
    auto base_strategy = greeting::strategy::createConsoleStrategy();
    return greeting::strategy::config::create_config_aware_strategy(std::move(base_strategy));
}

std::unique_ptr<IGreetingStrategy> DebugConfigStrategyFactory::create_debug_formal_strategy() const {
    auto base_strategy = greeting::strategy::createFormalStrategy();
    return greeting::strategy::config::create_config_aware_strategy(std::move(base_strategy));
}

std::unique_ptr<IGreetingStrategy> DebugConfigStrategyFactory::create_debug_casual_strategy() const {
    auto base_strategy = greeting::strategy::createCasualStrategy();
    return greeting::strategy::config::create_config_aware_strategy(std::move(base_strategy));
}

// ============================================================================
// Release Configuration Factory Implementation
// ============================================================================

std::unique_ptr<IGreetingStrategy> ReleaseConfigStrategyFactory::create_release_console_strategy() const {
    auto base_strategy = greeting::strategy::createConsoleStrategy();
    return greeting::strategy::config::create_config_aware_strategy(std::move(base_strategy));
}

std::unique_ptr<IGreetingStrategy> ReleaseConfigStrategyFactory::create_release_casual_strategy() const {
    auto base_strategy = greeting::strategy::createCasualStrategy();
    return greeting::strategy::config::create_config_aware_strategy(std::move(base_strategy));
}

std::unique_ptr<IGreetingStrategy> ReleaseConfigStrategyFactory::create_release_formal_strategy() const {
    auto base_strategy = greeting::strategy::createFormalStrategy();
    return greeting::strategy::config::create_config_aware_strategy(std::move(base_strategy));
}

// ============================================================================
// Profile Configuration Factory Implementation
// ============================================================================

std::unique_ptr<IGreetingStrategy> ProfileConfigStrategyFactory::create_profile_console_strategy() const {
    auto base_strategy = greeting::strategy::createConsoleStrategy();
    return greeting::strategy::config::create_config_aware_strategy(std::move(base_strategy));
}

std::unique_ptr<IGreetingStrategy> ProfileConfigStrategyFactory::create_profile_casual_strategy() const {
    auto base_strategy = greeting::strategy::createCasualStrategy();
    return greeting::strategy::config::create_config_aware_strategy(std::move(base_strategy));
}

std::unique_ptr<IGreetingStrategy> ProfileConfigStrategyFactory::create_profile_formal_strategy() const {
    auto base_strategy = greeting::strategy::createFormalStrategy();
    return greeting::strategy::config::create_config_aware_strategy(std::move(base_strategy));
}

// ============================================================================
// Configuration-Aware Strategy Registry
// ============================================================================

/**
 * @brief Registry for configuration-aware strategy management
 * 
 * Manages strategy instances with build-specific optimization and
 * configuration-aware selection policies.
 */
class ConfigAwareStrategyRegistry {
public:
    /**
     * @brief Register strategy with configuration awareness
     */
    void register_strategy(std::string_view name, std::unique_ptr<IGreetingStrategy> strategy) {
        strategies_[std::string(name)] = std::move(strategy);
    }
    
    /**
     * @brief Get strategy by name with configuration optimization
     */
    IGreetingStrategy* get_strategy(std::string_view name) const {
        auto it = strategies_.find(std::string(name));
        return (it != strategies_.end()) ? it->second.get() : nullptr;
    }
    
    /**
     * @brief Get optimal strategy for current configuration
     */
    IGreetingStrategy* get_optimal_strategy() const {
        auto optimal_name = greeting::strategy::config::get_optimal_strategy_for_config();
        return get_strategy(optimal_name);
    }
    
    /**
     * @brief Get all available strategy names
     */
    std::vector<std::string> get_available_strategies() const {
        std::vector<std::string> names;
        names.reserve(strategies_.size());
        
        for (const auto& [name, strategy] : strategies_) {
            names.push_back(name);
        }
        
        return names;
    }
    
    /**
     * @brief Check if strategy is available
     */
    bool has_strategy(std::string_view name) const {
        return strategies_.find(std::string(name)) != strategies_.end();
    }
    
    /**
     * @brief Initialize registry with configuration-aware strategies
     */
    static ConfigAwareStrategyRegistry create_for_current_config() {
        ConfigAwareStrategyRegistry registry;
        
        // Register console strategy
        auto console_strategy = greeting::strategy::createConsoleStrategy();
        auto config_console = greeting::strategy::config::create_config_aware_strategy(std::move(console_strategy));
        registry.register_strategy("console", std::move(config_console));
        
        // Register formal strategy
        auto formal_strategy = greeting::strategy::createFormalStrategy();
        auto config_formal = greeting::strategy::config::create_config_aware_strategy(std::move(formal_strategy));
        registry.register_strategy("formal", std::move(config_formal));
        
        // Register casual strategy
        auto casual_strategy = greeting::strategy::createCasualStrategy();
        auto config_casual = greeting::strategy::config::create_config_aware_strategy(std::move(casual_strategy));
        registry.register_strategy("casual", std::move(config_casual));
        
        return registry;
    }
    
    /**
     * @brief Get configuration-specific metrics
     */
    struct RegistryMetrics {
        size_t total_strategies = 0;
        std::string_view build_configuration;
        std::string_view optimal_strategy;
        bool configuration_aware = true;
    };
    
    RegistryMetrics get_metrics() const {
        RegistryMetrics metrics;
        metrics.total_strategies = strategies_.size();
        metrics.optimal_strategy = greeting::strategy::config::get_optimal_strategy_for_config();
        
        if constexpr (greeting::config::is_debug_build()) {
            metrics.build_configuration = "debug";
        } else if constexpr (greeting::config::is_profile_build()) {
            metrics.build_configuration = "profile";
        } else {
            metrics.build_configuration = "release";
        }
        
        return metrics;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<IGreetingStrategy>> strategies_;
};

// ============================================================================
// Global Registry Access
// ============================================================================

/**
 * @brief Get singleton registry instance for current configuration
 */
ConfigAwareStrategyRegistry& get_global_strategy_registry() {
    static auto registry = ConfigAwareStrategyRegistry::create_for_current_config();
    return registry;
}

/**
 * @brief Convenience function to get optimal strategy from global registry
 */
IGreetingStrategy* get_optimal_strategy_from_registry() {
    return get_global_strategy_registry().get_optimal_strategy();
}

/**
 * @brief Convenience function to get strategy by name from global registry
 */
IGreetingStrategy* get_strategy_from_registry(std::string_view name) {
    return get_global_strategy_registry().get_strategy(name);
}

} // namespace greeting::factory::config
