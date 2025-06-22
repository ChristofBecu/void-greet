#pragma once

// ============================================================================
// Configuration-Aware Strategy Factory
// 
// Provides factory interfaces that adapt strategy creation based on
// build configuration and runtime requirements.
// ============================================================================

#include "greeting/strategy_interface.hpp"
#include "greeting/config_strategy_integration.hpp"
#include "config/build_config.hpp"
#include <memory>
#include <string_view>
#include <vector>

namespace greeting::factory::config {

// Import strategy interface types into this namespace
using greeting::strategy::IGreetingStrategy;

// ============================================================================
// Configuration-Aware Factory Interface
// ============================================================================

/**
 * @brief Abstract factory for configuration-aware strategy creation
 */
class ConfigAwareStrategyFactory {
public:
    virtual ~ConfigAwareStrategyFactory() = default;
    
    /**
     * @brief Create strategy optimized for current configuration
     */
    virtual std::unique_ptr<IGreetingStrategy> create_strategy(
        std::string_view strategy_name) const = 0;
    
    /**
     * @brief Get available strategies for current configuration
     */
    virtual std::vector<std::string_view> get_available_strategies() const = 0;
    
    /**
     * @brief Get optimal strategy name for current configuration
     */
    virtual std::string_view get_optimal_strategy() const = 0;
    
    /**
     * @brief Check if strategy is supported in current configuration
     */
    virtual bool supports_strategy(std::string_view strategy_name) const = 0;
};

// ============================================================================
// Debug Configuration Factory
// ============================================================================

/**
 * @brief Factory optimized for debug builds
 * 
 * Creates strategies with enhanced diagnostics, comprehensive validation,
 * and detailed error reporting suitable for development and debugging.
 */
class DebugConfigStrategyFactory final : public ConfigAwareStrategyFactory {
public:
    std::unique_ptr<IGreetingStrategy> create_strategy(
        std::string_view strategy_name) const override {
        
        if (strategy_name == "console") {
            return create_debug_console_strategy();
        } else if (strategy_name == "formal") {
            return create_debug_formal_strategy();
        } else if (strategy_name == "casual") {
            return create_debug_casual_strategy();
        }
        
        // Default to formal strategy for debug builds (enhanced validation)
        return create_debug_formal_strategy();
    }
    
    std::vector<std::string_view> get_available_strategies() const override {
        return {"console", "formal", "casual"};
    }
    
    std::string_view get_optimal_strategy() const override {
        return "formal"; // Debug builds prefer formal for detailed validation
    }
    
    bool supports_strategy(std::string_view strategy_name) const override {
        auto available = get_available_strategies();
        return std::find(available.begin(), available.end(), strategy_name) != available.end();
    }

private:
    std::unique_ptr<IGreetingStrategy> create_debug_console_strategy() const;
    std::unique_ptr<IGreetingStrategy> create_debug_formal_strategy() const;
    std::unique_ptr<IGreetingStrategy> create_debug_casual_strategy() const;
};

// ============================================================================
// Release Configuration Factory
// ============================================================================

/**
 * @brief Factory optimized for release builds
 * 
 * Creates strategies with minimal overhead, constexpr optimization,
 * and lean error handling suitable for production environments.
 */
class ReleaseConfigStrategyFactory final : public ConfigAwareStrategyFactory {
public:
    std::unique_ptr<IGreetingStrategy> create_strategy(
        std::string_view strategy_name) const override {
        
        if (strategy_name == "console") {
            return create_release_console_strategy();
        } else if (strategy_name == "casual") {
            return create_release_casual_strategy();
        } else if (strategy_name == "formal") {
            return create_release_formal_strategy();
        }
        
        // Default to casual strategy for release builds (optimized performance)
        return create_release_casual_strategy();
    }
    
    std::vector<std::string_view> get_available_strategies() const override {
        return {"console", "casual", "formal"};
    }
    
    std::string_view get_optimal_strategy() const override {
        return "casual"; // Release builds prefer casual for performance
    }
    
    bool supports_strategy(std::string_view strategy_name) const override {
        auto available = get_available_strategies();
        return std::find(available.begin(), available.end(), strategy_name) != available.end();
    }

private:
    std::unique_ptr<IGreetingStrategy> create_release_console_strategy() const;
    std::unique_ptr<IGreetingStrategy> create_release_casual_strategy() const;
    std::unique_ptr<IGreetingStrategy> create_release_formal_strategy() const;
};

// ============================================================================
// Profile Configuration Factory
// ============================================================================

/**
 * @brief Factory optimized for profile builds
 * 
 * Creates strategies with balanced performance and monitoring capabilities
 * suitable for performance analysis and profiling.
 */
class ProfileConfigStrategyFactory final : public ConfigAwareStrategyFactory {
public:
    std::unique_ptr<IGreetingStrategy> create_strategy(
        std::string_view strategy_name) const override {
        
        if (strategy_name == "console") {
            return create_profile_console_strategy();
        } else if (strategy_name == "casual") {
            return create_profile_casual_strategy();
        } else if (strategy_name == "formal") {
            return create_profile_formal_strategy();
        }
        
        // Default to console strategy for profile builds (balanced performance)
        return create_profile_console_strategy();
    }
    
    std::vector<std::string_view> get_available_strategies() const override {
        return {"console", "casual", "formal"};
    }
    
    std::string_view get_optimal_strategy() const override {
        return "console"; // Profile builds prefer console for balanced performance
    }
    
    bool supports_strategy(std::string_view strategy_name) const override {
        auto available = get_available_strategies();
        return std::find(available.begin(), available.end(), strategy_name) != available.end();
    }

private:
    std::unique_ptr<IGreetingStrategy> create_profile_console_strategy() const;
    std::unique_ptr<IGreetingStrategy> create_profile_casual_strategy() const;
    std::unique_ptr<IGreetingStrategy> create_profile_formal_strategy() const;
};

// ============================================================================
// Configuration-Aware Factory Creation
// ============================================================================

/**
 * @brief Create factory appropriate for current build configuration
 */
inline std::unique_ptr<ConfigAwareStrategyFactory> create_config_aware_factory() {
    if constexpr (::greeting::config::is_debug_build()) {
        return std::make_unique<DebugConfigStrategyFactory>();
    } else if constexpr (::greeting::config::is_profile_build()) {
        return std::make_unique<ProfileConfigStrategyFactory>();
    } else {
        return std::make_unique<ReleaseConfigStrategyFactory>();
    }
}

/**
 * @brief Get singleton factory instance for current configuration
 */
inline ConfigAwareStrategyFactory& get_factory_for_config() {
    static auto factory = create_config_aware_factory();
    return *factory;
}

} // namespace greeting::factory::config
