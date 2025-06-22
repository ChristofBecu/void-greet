#pragma once

#include "greeting/strategy_interface.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"
#include "greeting/expected.hpp"
#include "greeting/error_codes.hpp"
#include "greeting/config_aware_validation.hpp"
#include "config/build_config.hpp"
#include <memory>
#include <string_view>
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <optional>
#include <chrono>

#ifdef __cpp_concepts
#include <concepts>
#endif

namespace greeting::strategy {

/**
 * @brief Strategy selection criteria
 */
enum class StrategyType {
    Console,    ///< Direct console output
    Formal,     ///< Formal greeting with titles/honorifics
    Casual,     ///< Casual/informal greeting
    Auto        ///< Automatic selection based on context
};

/**
 * @brief Strategy context configuration
 */
struct ContextConfig {
    StrategyType default_strategy = StrategyType::Console;
    bool enable_fallback = true;
    bool enable_caching = greeting::config::is_debug_build();
    bool enable_metrics = greeting::config::is_debug_build();
    
    /**
     * @brief Validate context configuration
     */
    [[nodiscard]] constexpr bool isValid() const noexcept {
        return default_strategy != StrategyType::Auto || enable_fallback;
    }
};

/**
 * @brief Strategy execution context and metrics
 */
struct ExecutionContext {
    StrategyType selected_strategy;
    std::string strategy_name;
    bool fallback_used = false;
    
    // Debug-only metrics
    #ifdef HELLOWORLD_DEBUG_BUILD
    std::chrono::microseconds execution_time{0};
    size_t validation_steps = 0;
    bool cache_hit = false;
    #endif
};

/**
 * @brief Strategy Context - manages strategy lifecycle and coordination
 * 
 * Provides unified interface for greeting generation using different strategies.
 * Integrates with configuration-aware validation and error handling system.
 * 
 * Features:
 * - Dynamic strategy selection based on context
 * - Automatic fallback strategies
 * - Configuration-aware performance optimizations
 * - Comprehensive error handling with context propagation
 * - Strategy lifecycle management
 * - C++23 consteval optimization support
 */
class StrategyContext {
public:
    /**
     * @brief Default constructor with standard configuration
     */
    explicit StrategyContext() noexcept;
    
    /**
     * @brief Constructor with custom configuration
     */
    explicit StrategyContext(const ContextConfig& config) noexcept;
    
    /**
     * @brief Move constructor
     */
    StrategyContext(StrategyContext&&) noexcept = default;
    
    /**
     * @brief Move assignment
     */
    StrategyContext& operator=(StrategyContext&&) noexcept = default;
    
    /**
     * @brief Non-copyable to prevent strategy duplication
     */
    StrategyContext(const StrategyContext&) = delete;
    StrategyContext& operator=(const StrategyContext&) = delete;
    
    /**
     * @brief Configuration-aware destructor
     */
    ~StrategyContext() noexcept;
    
    // ========================================================================
    // Core Greeting Interface
    // ========================================================================
    
    /**
     * @brief Generate greeting using configured strategy
     * @param person Validated person name to greet
     * @return Expected greeting message or error with context
     */
    [[nodiscard]] Result<GreetingMessage> greet(
        const PersonName& person) const noexcept;
    
    /**
     * @brief Generate greeting with explicit strategy selection
     * @param person Validated person name to greet  
     * @param strategy_type Specific strategy to use
     * @return Expected greeting message or error with context
     */
    [[nodiscard]] Result<GreetingMessage> greetWith(
        const PersonName& person,
        StrategyType strategy_type) const noexcept;
    
    /**
     * @brief Generate greeting with validation and context tracking
     * @param person_name Raw person name to validate and greet
     * @param strategy_type Strategy to use (Auto for automatic selection)
     * @return Expected greeting message or validation error with rich context
     */
    [[nodiscard]] Result<GreetingMessage> greetWithValidation(
        std::string_view person_name,
        StrategyType strategy_type = StrategyType::Auto) const noexcept;
    
    // ========================================================================
    // Strategy Management
    // ========================================================================
    
    /**
     * @brief Register custom strategy implementation
     * @param strategy_type Type identifier for the strategy
     * @param strategy Strategy implementation (moved)
     * @return True if registration succeeded
     */
    [[nodiscard]] bool registerStrategy(
        StrategyType strategy_type,
        std::unique_ptr<IGreetingStrategy> strategy) noexcept;
    
    /**
     * @brief Check if strategy is available
     * @param strategy_type Strategy to check
     * @return True if strategy is registered and valid
     */
    [[nodiscard]] bool hasStrategy(StrategyType strategy_type) const noexcept;
    
    /**
     * @brief Get available strategy types
     * @return Vector of registered strategy types
     */
    [[nodiscard]] std::vector<StrategyType> getAvailableStrategies() const noexcept;
    
    /**
     * @brief Set default strategy for automatic selection
     * @param strategy_type New default strategy
     * @return True if strategy is available and set
     */
    [[nodiscard]] bool setDefaultStrategy(StrategyType strategy_type) noexcept;
    
    // ========================================================================
    // Context Management
    // ========================================================================
    
    /**
     * @brief Get current context configuration
     * @return Current configuration settings
     */
    [[nodiscard]] const ContextConfig& getConfig() const noexcept;
    
    /**
     * @brief Update context configuration
     * @param config New configuration settings
     * @return True if configuration is valid and applied
     */
    [[nodiscard]] bool updateConfig(const ContextConfig& config) noexcept;
    
    /**
     * @brief Get last execution context (debug builds only)
     * @return Execution context with metrics and debug information
     */
    [[nodiscard]] std::optional<ExecutionContext> getLastExecution() const noexcept;
    
    /**
     * @brief Validate context state
     * @return True if context is properly configured and operational
     */
    [[nodiscard]] bool isValid() const noexcept;
    
    // ========================================================================
    // Performance and Diagnostics
    // ========================================================================
    
    /**
     * @brief Clear strategy cache (debug builds only)
     */
    void clearCache() const noexcept;
    
    /**
     * @brief Get performance metrics (debug builds only)
     * @return Performance statistics if available
     */
    [[nodiscard]] std::string getPerformanceMetrics() const noexcept;

private:
    // ========================================================================
    // Internal Implementation
    // ========================================================================
    
    /**
     * @brief Initialize default strategies
     */
    void initializeDefaultStrategies() noexcept;
    
    /**
     * @brief Select appropriate strategy based on context
     * @param person Person name for context analysis
     * @param requested_type Requested strategy type
     * @return Selected strategy type
     */
    [[nodiscard]] StrategyType selectStrategy(
        const PersonName& person,
        StrategyType requested_type) const noexcept;
    
    /**
     * @brief Get strategy implementation
     * @param strategy_type Strategy type to retrieve
     * @return Strategy pointer or nullptr if not available
     */
    [[nodiscard]] IGreetingStrategy* getStrategy(
        StrategyType strategy_type) const noexcept;
    
    /**
     * @brief Execute greeting with error handling and metrics
     * @param strategy Strategy to use
     * @param person Person to greet
     * @param context Execution context for tracking
     * @return Expected greeting message or error
     */
    [[nodiscard]] Result<GreetingMessage> executeGreeting(
        IGreetingStrategy* strategy,
        const PersonName& person,
        ExecutionContext& context) const noexcept;
    
    /**
     * @brief Handle strategy execution error with fallback
     * @param error Original error
     * @param person Person being greeted
     * @param failed_strategy Strategy that failed
     * @return Expected greeting message from fallback or propagated error
     */
    [[nodiscard]] Result<GreetingMessage> handleStrategyError(
        GreetingError error,
        const PersonName& person,
        StrategyType failed_strategy) const noexcept;
    
    /**
     * @brief Create context-aware error with diagnostic information
     * @param error Base error code
     * @param context Operation context
     * @param strategy_info Strategy-specific information
     * @return Enhanced error with context
     */
    [[nodiscard]] GreetingError createContextError(
        GreetingError error,
        std::string_view context,
        std::string_view strategy_info = "") const noexcept;
    
    // ========================================================================
    // Member Variables
    // ========================================================================
    
    ContextConfig config_;
    std::unordered_map<StrategyType, std::unique_ptr<IGreetingStrategy>> strategies_;
    
    // Debug-only state
    #ifdef HELLOWORLD_DEBUG_BUILD
    mutable std::optional<ExecutionContext> last_execution_;
    mutable std::unordered_map<std::string, size_t> performance_cache_;
    mutable size_t total_greetings_ = 0;
    mutable std::chrono::microseconds total_execution_time_{0};
    #endif
};

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Convert strategy type to string
 * @param strategy_type Strategy type to convert
 * @return String representation
 */
[[nodiscard]] constexpr std::string_view toString(StrategyType strategy_type) noexcept {
    switch (strategy_type) {
        case StrategyType::Console: return "Console";
        case StrategyType::Formal:  return "Formal";
        case StrategyType::Casual:  return "Casual";
        case StrategyType::Auto:    return "Auto";
        default:                    return "Unknown";
    }
}

/**
 * @brief Parse strategy type from string
 * @param strategy_name String to parse
 * @return Strategy type or nullopt if invalid
 */
[[nodiscard]] std::optional<StrategyType> parseStrategyType(
    std::string_view strategy_name) noexcept;

/**
 * @brief Create context with default configuration
 * @return Strategy context with standard setup
 */
[[nodiscard]] std::unique_ptr<StrategyContext> createDefaultContext() noexcept;

/**
 * @brief Create context with configuration-aware optimization
 * @param config Context configuration
 * @return Strategy context optimized for current build configuration
 */
[[nodiscard]] std::unique_ptr<StrategyContext> createOptimizedContext(
    const ContextConfig& config = {}) noexcept;

} // namespace greeting::strategy
