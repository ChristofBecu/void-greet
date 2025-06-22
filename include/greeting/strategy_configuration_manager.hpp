#pragma once

// ============================================================================
// Strategy Configuration Manager Header
// 
// Provides interfaces for runtime configuration and adaptation of strategies 
// based on build configuration and runtime parameters.
// ============================================================================

#include "greeting/strategy_context.hpp"
#include "greeting/expected.hpp"
#include "greeting/error_codes.hpp"
#include <string_view>

namespace greeting::strategy::config {

// ============================================================================
// Configuration Metrics Structure
// ============================================================================

struct ConfigurationMetrics {
    std::string_view build_type;
    std::string_view optimal_strategy;
    
    #ifdef HELLOWORLD_DEBUG_BUILD
    struct DebugMetrics {
        size_t validation_overhead_percent = 15;
        size_t logging_overhead_percent = 5;
        bool enhanced_diagnostics_enabled = true;
    } debug_metrics;
    #endif
};

// ============================================================================
// Configuration Manager Interface
// ============================================================================

class StrategyConfigurationManager {
public:
    /**
     * @brief Create configuration-optimized strategy context
     */
    static Result<StrategyContext> create_optimized_context() noexcept;
    
    /**
     * @brief Register strategies with configuration awareness
     */
    static Result<bool> register_config_aware_strategies(
        StrategyContext& context) noexcept;
    
    /**
     * @brief Get configuration-specific metrics
     */
    static ConfigurationMetrics get_configuration_metrics() noexcept;

private:
    #ifdef HELLOWORLD_DEBUG_BUILD
    static ConfigurationMetrics::DebugMetrics collect_debug_metrics() noexcept;
    #endif
};

} // namespace greeting::strategy::config
