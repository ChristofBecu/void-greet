#pragma once

// ============================================================================
// Configuration-Aware Strategy Registry Header
// 
// Provides build-specific strategy registration and management with
// configuration-aware optimization.
// ============================================================================

#include "greeting/strategy_interface.hpp"
#include <string_view>

namespace greeting::factory::config {

// Import strategy interface types into this namespace
using greeting::strategy::IGreetingStrategy;

// ============================================================================
// Forward Declarations
// ============================================================================

class ConfigAwareStrategyRegistry;

// ============================================================================
// Global Registry Access Functions
// ============================================================================

/**
 * @brief Get singleton registry instance for current configuration
 */
ConfigAwareStrategyRegistry& get_global_strategy_registry();

/**
 * @brief Convenience function to get optimal strategy from global registry
 */
IGreetingStrategy* get_optimal_strategy_from_registry();

/**
 * @brief Convenience function to get strategy by name from global registry
 */
IGreetingStrategy* get_strategy_from_registry(std::string_view name);

} // namespace greeting::factory::config
