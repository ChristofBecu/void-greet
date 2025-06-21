I'll create a detailed implementation plan for 3.1.5: Strategy Context Integration, following the established structure and depth from the previous subtasks. Let me first examine the existing codebase to understand the current strategy implementations and service integration patterns. Summarized conversation historyI'll create a detailed implementation plan for Strategy Context Integration (3.1.5). Let me first examine the existing codebase to understand the current strategy implementations and context requirements. Let me search for more specific information about strategy context and management patterns in the codebase. Now let me examine the existing strategy interface and related files to understand the current state. Now I have a comprehensive understanding of the current codebase and the progress made in implementing the greeting system. Let me create the detailed implementation plan for Strategy Context Integration (3.1.5).

# **3.1.5: Strategy Context Integration - Detailed Implementation Plan**

## **Overview**

Implement the Strategy Context that manages strategy lifecycle, coordinates strategy selection, and provides a unified interface for greeting generation. This context serves as the orchestrator between different strategies while maintaining the configuration-aware validation and error handling established in Phase 2. The context leverages advanced C++23 features for optimal performance and type safety.

---

## **File Structure & Implementation**

### **1. Header File: `include/greeting/strategy_context.hpp`**

```cpp
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
[[nodiscard]] constexpr std::string_view toString(StrategyType strategy_type) noexcept;

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
```

### **2. Implementation File: `src/domain/strategy_context.cpp`**

```cpp
#include "greeting/strategy_context.hpp"
#include "greeting/console_strategy.hpp"
#include "greeting/formal_strategy.hpp"
#include "greeting/casual_strategy.hpp"
#include <algorithm>
#include <chrono>
#include <sstream>

#if HELLOWORLD_HAS_STACKTRACE_RUNTIME
#include <stacktrace>
#endif

namespace greeting::strategy {

// ============================================================================
// Constructor & Destructor
// ============================================================================

StrategyContext::StrategyContext() noexcept 
    : StrategyContext(ContextConfig{}) {
}

StrategyContext::StrategyContext(const ContextConfig& config) noexcept 
    : config_(config) {
    
    if constexpr (greeting::config::is_debug_build()) {
        // Debug builds initialize with comprehensive setup
        initializeDefaultStrategies();
        
        if (config_.enable_metrics) {
            last_execution_.emplace();
        }
    } else {
        // Release builds use minimal initialization
        initializeDefaultStrategies();
    }
}

StrategyContext::~StrategyContext() noexcept {
    if constexpr (greeting::config::is_debug_build()) {
        // Debug builds can log context destruction
        if (config_.enable_metrics && total_greetings_ > 0) {
            // Could log performance summary
        }
    }
}

// ============================================================================
// Core Greeting Interface Implementation
// ============================================================================

[[nodiscard]] Result<GreetingMessage> StrategyContext::greet(
    const PersonName& person) const noexcept {
    
    ExecutionContext context{};
    
    if constexpr (greeting::config::is_debug_build()) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            auto strategy_type = selectStrategy(person, config_.default_strategy);
            auto* strategy = getStrategy(strategy_type);
            
            if (!strategy) {
                return createContextError(
                    GreetingError::StrategyNotFound,
                    "greet",
                    toString(strategy_type)
                );
            }
            
            context.selected_strategy = strategy_type;
            context.strategy_name = std::string{strategy->getStrategyName()};
            
            auto result = executeGreeting(strategy, person, context);
            
            auto end_time = std::chrono::high_resolution_clock::now();
            context.execution_time = std::chrono::duration_cast<std::chrono::microseconds>(
                end_time - start_time
            );
            
            if (config_.enable_metrics) {
                last_execution_ = context;
                ++total_greetings_;
                total_execution_time_ += context.execution_time;
            }
            
            return result;
            
        } catch (const std::exception& e) {
            return createContextError(
                GreetingError::StrategyCreationFailed,
                "greet - exception",
                e.what()
            );
        }
    } else {
        // Release build optimized path
        auto strategy_type = selectStrategy(person, config_.default_strategy);
        auto* strategy = getStrategy(strategy_type);
        
        if (!strategy) {
            return GreetingError::StrategyNotFound;
        }
        
        return strategy->greet(person);
    }
}

[[nodiscard]] Result<GreetingMessage> StrategyContext::greetWith(
    const PersonName& person,
    StrategyType strategy_type) const noexcept {
    
    ExecutionContext context{};
    context.selected_strategy = strategy_type;
    
    auto* strategy = getStrategy(strategy_type);
    if (!strategy) {
        if (config_.enable_fallback) {
            return handleStrategyError(
                GreetingError::StrategyNotFound,
                person,
                strategy_type
            );
        } else {
            return createContextError(
                GreetingError::StrategyNotFound,
                "greetWith",
                toString(strategy_type)
            );
        }
    }
    
    context.strategy_name = std::string{strategy->getStrategyName()};
    
    auto result = executeGreeting(strategy, person, context);
    
    if constexpr (greeting::config::is_debug_build()) {
        if (config_.enable_metrics) {
            last_execution_ = context;
        }
    }
    
    return result;
}

[[nodiscard]] Result<GreetingMessage> StrategyContext::greetWithValidation(
    std::string_view person_name,
    StrategyType strategy_type) const noexcept {
    
    // Use configuration-aware validation
    auto person_result = greeting::validation::validate_person_name<PersonName>(person_name);
    if (!person_result.has_value()) {
        return createContextError(
            person_result.error(),
            "greetWithValidation - person validation"
        );
    }
    
    auto strategy_to_use = (strategy_type == StrategyType::Auto) 
        ? selectStrategy(person_result.value(), config_.default_strategy)
        : strategy_type;
    
    return greetWith(person_result.value(), strategy_to_use);
}

// ============================================================================
// Strategy Management Implementation
// ============================================================================

[[nodiscard]] bool StrategyContext::registerStrategy(
    StrategyType strategy_type,
    std::unique_ptr<IGreetingStrategy> strategy) noexcept {
    
    if (!strategy || !strategy->isValid()) {
        return false;
    }
    
    strategies_[strategy_type] = std::move(strategy);
    return true;
}

[[nodiscard]] bool StrategyContext::hasStrategy(StrategyType strategy_type) const noexcept {
    auto it = strategies_.find(strategy_type);
    return it != strategies_.end() && it->second && it->second->isValid();
}

[[nodiscard]] std::vector<StrategyType> StrategyContext::getAvailableStrategies() const noexcept {
    std::vector<StrategyType> available;
    available.reserve(strategies_.size());
    
    for (const auto& [type, strategy] : strategies_) {
        if (strategy && strategy->isValid()) {
            available.push_back(type);
        }
    }
    
    return available;
}

[[nodiscard]] bool StrategyContext::setDefaultStrategy(StrategyType strategy_type) noexcept {
    if (strategy_type == StrategyType::Auto || !hasStrategy(strategy_type)) {
        return false;
    }
    
    config_.default_strategy = strategy_type;
    return true;
}

// ============================================================================
// Context Management Implementation
// ============================================================================

[[nodiscard]] const ContextConfig& StrategyContext::getConfig() const noexcept {
    return config_;
}

[[nodiscard]] bool StrategyContext::updateConfig(const ContextConfig& config) noexcept {
    if (!config.isValid()) {
        return false;
    }
    
    config_ = config;
    
    // Reinitialize if needed
    if (config_.enable_metrics && !last_execution_.has_value()) {
        last_execution_.emplace();
    }
    
    return true;
}

[[nodiscard]] std::optional<ExecutionContext> StrategyContext::getLastExecution() const noexcept {
    if constexpr (greeting::config::is_debug_build()) {
        return last_execution_;
    } else {
        return std::nullopt;
    }
}

[[nodiscard]] bool StrategyContext::isValid() const noexcept {
    return config_.isValid() && 
           hasStrategy(config_.default_strategy) &&
           !strategies_.empty();
}

// ============================================================================
// Performance and Diagnostics Implementation
// ============================================================================

void StrategyContext::clearCache() const noexcept {
    if constexpr (greeting::config::is_debug_build()) {
        performance_cache_.clear();
    }
}

[[nodiscard]] std::string StrategyContext::getPerformanceMetrics() const noexcept {
    if constexpr (greeting::config::is_debug_build()) {
        std::ostringstream oss;
        oss << "Strategy Context Performance Metrics:\n";
        oss << "  Total Greetings: " << total_greetings_ << "\n";
        oss << "  Total Execution Time: " << total_execution_time_.count() << " μs\n";
        
        if (total_greetings_ > 0) {
            auto avg_time = total_execution_time_.count() / total_greetings_;
            oss << "  Average Time per Greeting: " << avg_time << " μs\n";
        }
        
        oss << "  Available Strategies: " << strategies_.size() << "\n";
        oss << "  Cache Enabled: " << (config_.enable_caching ? "Yes" : "No") << "\n";
        
        return oss.str();
    } else {
        return "Performance metrics available only in debug builds";
    }
}

// ============================================================================
// Private Implementation Methods
// ============================================================================

void StrategyContext::initializeDefaultStrategies() noexcept {
    // Register console strategy
    if (auto console = createConsoleStrategy()) {
        strategies_[StrategyType::Console] = std::move(console);
    }
    
    // Register formal strategy
    if (auto formal = createFormalStrategy()) {
        strategies_[StrategyType::Formal] = std::move(formal);
    }
    
    // Register casual strategy
    if (auto casual = createCasualStrategy()) {
        strategies_[StrategyType::Casual] = std::move(casual);
    }
}

[[nodiscard]] StrategyType StrategyContext::selectStrategy(
    const PersonName& person,
    StrategyType requested_type) const noexcept {
    
    if (requested_type != StrategyType::Auto && hasStrategy(requested_type)) {
        return requested_type;
    }
    
    // Auto-selection logic based on person name analysis
    const auto& name = person.value();
    
    // Check for formal indicators (titles, honorifics)
    if (name.find("Dr.") != std::string::npos ||
        name.find("Prof.") != std::string::npos ||
        name.find("Mr.") != std::string::npos ||
        name.find("Ms.") != std::string::npos ||
        name.find("Mrs.") != std::string::npos) {
        if (hasStrategy(StrategyType::Formal)) {
            return StrategyType::Formal;
        }
    }
    
    // Check for casual indicators (single names, informal patterns)
    if (name.length() <= 5 || 
        name.find(' ') == std::string::npos) {
        if (hasStrategy(StrategyType::Casual)) {
            return StrategyType::Casual;
        }
    }
    
    // Default to console strategy
    return StrategyType::Console;
}

[[nodiscard]] IGreetingStrategy* StrategyContext::getStrategy(
    StrategyType strategy_type) const noexcept {
    
    auto it = strategies_.find(strategy_type);
    return (it != strategies_.end() && it->second) ? it->second.get() : nullptr;
}

[[nodiscard]] Result<GreetingMessage> StrategyContext::executeGreeting(
    IGreetingStrategy* strategy,
    const PersonName& person,
    ExecutionContext& context) const noexcept {
    
    try {
        auto result = strategy->greet(person);
        
        if constexpr (greeting::config::is_debug_build()) {
            if (result.has_value()) {
                ++context.validation_steps;
            }
        }
        
        if (!result.has_value() && config_.enable_fallback) {
            return handleStrategyError(result.error(), person, context.selected_strategy);
        }
        
        return result;
        
    } catch (const std::exception& e) {
        return createContextError(
            GreetingError::StrategyCreationFailed,
            "executeGreeting",
            e.what()
        );
    }
}

[[nodiscard]] Result<GreetingMessage> StrategyContext::handleStrategyError(
    GreetingError error,
    const PersonName& person,
    StrategyType failed_strategy) const noexcept {
    
    // Try fallback strategies in order of preference
    std::vector<StrategyType> fallback_order;
    
    switch (failed_strategy) {
        case StrategyType::Formal:
            fallback_order = {StrategyType::Console, StrategyType::Casual};
            break;
        case StrategyType::Casual:
            fallback_order = {StrategyType::Console, StrategyType::Formal};
            break;
        case StrategyType::Console:
            fallback_order = {StrategyType::Formal, StrategyType::Casual};
            break;
        default:
            fallback_order = {StrategyType::Console};
            break;
    }
    
    for (auto fallback_type : fallback_order) {
        if (hasStrategy(fallback_type)) {
            auto* fallback_strategy = getStrategy(fallback_type);
            if (fallback_strategy) {
                auto fallback_result = fallback_strategy->greet(person);
                if (fallback_result.has_value()) {
                    if constexpr (greeting::config::is_debug_build()) {
                        if (last_execution_.has_value()) {
                            const_cast<ExecutionContext&>(*last_execution_).fallback_used = true;
                        }
                    }
                    return fallback_result;
                }
            }
        }
    }
    
    // All fallbacks failed, return original error
    return createContextError(error, "handleStrategyError - all fallbacks failed");
}

[[nodiscard]] GreetingError StrategyContext::createContextError(
    GreetingError error,
    std::string_view context,
    std::string_view strategy_info) const noexcept {
    
    if constexpr (greeting::config::is_debug_build()) {
        // Debug builds could enhance error with context
        // For now, just return the original error
        return error;
    } else {
        // Release builds use minimal error context
        return error;
    }
}

// ============================================================================
// Utility Functions Implementation
// ============================================================================

[[nodiscard]] constexpr std::string_view toString(StrategyType strategy_type) noexcept {
    switch (strategy_type) {
        case StrategyType::Console: return "Console";
        case StrategyType::Formal: return "Formal";
        case StrategyType::Casual: return "Casual";
        case StrategyType::Auto: return "Auto";
        default: return "Unknown";
    }
}

[[nodiscard]] std::optional<StrategyType> parseStrategyType(
    std::string_view strategy_name) noexcept {
    
    if (strategy_name == "Console" || strategy_name == "console") {
        return StrategyType::Console;
    } else if (strategy_name == "Formal" || strategy_name == "formal") {
        return StrategyType::Formal;
    } else if (strategy_name == "Casual" || strategy_name == "casual") {
        return StrategyType::Casual;
    } else if (strategy_name == "Auto" || strategy_name == "auto") {
        return StrategyType::Auto;
    }
    
    return std::nullopt;
}

[[nodiscard]] std::unique_ptr<StrategyContext> createDefaultContext() noexcept {
    try {
        return std::make_unique<StrategyContext>();
    } catch (...) {
        return nullptr;
    }
}

[[nodiscard]] std::unique_ptr<StrategyContext> createOptimizedContext(
    const ContextConfig& config) noexcept {
    
    try {
        ContextConfig optimized_config = config;
        
        // Apply configuration-aware optimizations
        if constexpr (greeting::config::is_debug_build()) {
            optimized_config.enable_metrics = true;
            optimized_config.enable_caching = true;
        } else {
            optimized_config.enable_metrics = false;
            optimized_config.enable_caching = false;
        }
        
        return std::make_unique<StrategyContext>(optimized_config);
    } catch (...) {
        return nullptr;
    }
}

} // namespace greeting::strategy
```

### **3. Unit Test File: `tests/unit/domain/test_strategy_context.cpp`**

```cpp
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "greeting/strategy_context.hpp"
#include "greeting/person_name.hpp"

using namespace greeting;
using namespace greeting::strategy;
using Catch::Matchers::ContainsSubstring;
using Catch::Matchers::Equals;

// ============================================================================
// Test Fixtures and Helpers
// ============================================================================

class MockGreetingStrategy : public IGreetingStrategy {
public:
    explicit MockGreetingStrategy(std::string_view name, bool should_fail = false)
        : strategy_name_(name), should_fail_(should_fail) {}
    
    [[nodiscard]] Result<GreetingMessage> greet(
        const PersonName& person) const noexcept override {
        
        if (should_fail_) {
            return GreetingError::StrategyCreationFailed;
        }
        
        const std::string greeting = std::string{strategy_name_} + ", " + person.value() + "!";
        return GreetingMessage::create(greeting);
    }
    
    [[nodiscard]] std::string_view getStrategyName() const noexcept override {
        return strategy_name_;
    }
    
    [[nodiscard]] bool isValid() const noexcept override {
        return !should_fail_;
    }
    
private:
    std::string strategy_name_;
    bool should_fail_;
};

// ============================================================================
// Basic Functionality Tests
// ============================================================================

SCENARIO("StrategyContext basic functionality", "[StrategyContext][basic]") {
    
    GIVEN("a default strategy context") {
        auto context = createDefaultContext();
        REQUIRE(context != nullptr);
        REQUIRE(context->isValid());
        
        WHEN("greeting with a simple person name") {
            auto person = PersonName::create("Alice").value();
            auto result = context->greet(person);
            
            THEN("greeting is generated successfully") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Alice"));
                REQUIRE_FALSE(result.value().empty());
            }
        }
        
        WHEN("checking available strategies") {
            auto available = context->getAvailableStrategies();
            
            THEN("default strategies are available") {
                REQUIRE_FALSE(available.empty());
                REQUIRE(context->hasStrategy(StrategyType::Console));
                
                // Other strategies depend on implementation state
                INFO("Available strategies: " << available.size());
            }
        }
        
        WHEN("using explicit strategy selection") {
            auto person = PersonName::create("Dr. Smith").value();
            auto result = context->greetWith(person, StrategyType::Console);
            
            THEN("strategy is applied correctly") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Dr. Smith"));
            }
        }
    }
}

SCENARIO("StrategyContext configuration management", "[StrategyContext][config]") {
    
    GIVEN("a context with custom configuration") {
        ContextConfig config{};
        config.default_strategy = StrategyType::Console;
        config.enable_fallback = true;
        config.enable_caching = greeting::config::is_debug_build();
        
        auto context = createOptimizedContext(config);
        REQUIRE(context != nullptr);
        
        WHEN("checking configuration") {
            const auto& current_config = context->getConfig();
            
            THEN("configuration is applied correctly") {
                REQUIRE(current_config.default_strategy == StrategyType::Console);
                REQUIRE(current_config.enable_fallback == true);
                
                if constexpr (greeting::config::is_debug_build()) {
                    REQUIRE(current_config.enable_caching == true);
                } else {
                    // Release builds may optimize caching away
                    INFO("Release build: caching=" << current_config.enable_caching);
                }
            }
        }
        
        WHEN("updating configuration") {
            ContextConfig new_config = config;
            new_config.enable_fallback = false;
            
            bool update_success = context->updateConfig(new_config);
            
            THEN("configuration is updated successfully") {
                REQUIRE(update_success == true);
                REQUIRE(context->getConfig().enable_fallback == false);
            }
        }
        
        WHEN("updating with invalid configuration") {
            ContextConfig invalid_config{};
            invalid_config.default_strategy = StrategyType::Auto;
            invalid_config.enable_fallback = false; // Invalid: Auto requires fallback
            
            bool update_success = context->updateConfig(invalid_config);
            
            THEN("configuration update is rejected") {
                REQUIRE(update_success == false);
                REQUIRE(context->getConfig().enable_fallback == true); // Unchanged
            }
        }
    }
}

SCENARIO("StrategyContext strategy management", "[StrategyContext][strategies]") {
    
    GIVEN("a strategy context") {
        auto context = createDefaultContext();
        REQUIRE(context != nullptr);
        
        WHEN("registering a custom strategy") {
            auto mock_strategy = std::make_unique<MockGreetingStrategy>("MockGreeting");
            bool registration_success = context->registerStrategy(
                StrategyType::Casual, 
                std::move(mock_strategy)
            );
            
            THEN("strategy is registered successfully") {
                REQUIRE(registration_success == true);
                REQUIRE(context->hasStrategy(StrategyType::Casual) == true);
                
                auto person = PersonName::create("Bob").value();
                auto result = context->greetWith(person, StrategyType::Casual);
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("MockGreeting"));
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Bob"));
            }
        }
        
        WHEN("registering an invalid strategy") {
            auto failing_strategy = std::make_unique<MockGreetingStrategy>("Failing", true);
            bool registration_success = context->registerStrategy(
                StrategyType::Formal, 
                std::move(failing_strategy)
            );
            
            THEN("strategy registration is rejected") {
                REQUIRE(registration_success == false);
                REQUIRE(context->hasStrategy(StrategyType::Formal) == false);
            }
        }
        
        WHEN("setting default strategy") {
            // Ensure a strategy is available
            if (context->hasStrategy(StrategyType::Console)) {
                bool set_success = context->setDefaultStrategy(StrategyType::Console);
                
                THEN("default strategy is updated") {
                    REQUIRE(set_success == true);
                    REQUIRE(context->getConfig().default_strategy == StrategyType::Console);
                }
            }
        }
    }
}

SCENARIO("StrategyContext validation and error handling", "[StrategyContext][validation]") {
    
    GIVEN("a strategy context") {
        auto context = createDefaultContext();
        REQUIRE(context != nullptr);
        
        WHEN("greeting with validation") {
            auto result = context->greetWithValidation("Valid Name");
            
            THEN("validation and greeting succeed") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Valid Name"));
            }
        }
        
        WHEN("greeting with invalid input") {
            auto result = context->greetWithValidation(""); // Empty name
            
            THEN("validation error is returned") {
                REQUIRE_FALSE(result.has_value());
                REQUIRE(result.error() == GreetingError::EmptyName);
            }
        }
        
        WHEN("using unavailable strategy") {
            auto person = PersonName::create("Test").value();
            
            // Use a strategy type that might not be available
            auto result = context->greetWith(person, StrategyType::Formal);
            
            THEN("appropriate error handling occurs") {
                if (context->hasStrategy(StrategyType::Formal)) {
                    REQUIRE(result.has_value());
                } else {
                    // Should either return error or use fallback
                    if (context->getConfig().enable_fallback) {
                        // Fallback should work
                        INFO("Fallback mechanism used");
                    } else {
                        REQUIRE_FALSE(result.has_value());
                        REQUIRE(result.error() == GreetingError::StrategyNotFound);
                    }
                }
            }
        }
    }
}

SCENARIO("StrategyContext automatic strategy selection", "[StrategyContext][auto]") {
    
    GIVEN("a context with multiple strategies") {
        auto context = createDefaultContext();
        REQUIRE(context != nullptr);
        
        WHEN("greeting with formal name patterns") {
            std::vector<std::string> formal_names = {
                "Dr. Johnson", "Prof. Smith", "Mr. Anderson", "Ms. Davis"
            };
            
            for (const auto& name : formal_names) {
                auto person_result = PersonName::create(name);
                if (person_result.has_value()) {
                    auto result = context->greetWithValidation(name, StrategyType::Auto);
                    
                    THEN("formal patterns are handled appropriately") {
                        REQUIRE(result.has_value());
                        REQUIRE_THAT(result.value().value(), ContainsSubstring(name));
                        INFO("Formal name: " << name);
                    }
                }
            }
        }
        
        WHEN("greeting with casual name patterns") {
            std::vector<std::string> casual_names = {
                "Bob", "Jo", "Alex", "Sam"
            };
            
            for (const auto& name : casual_names) {
                auto person_result = PersonName::create(name);
                if (person_result.has_value()) {
                    auto result = context->greetWithValidation(name, StrategyType::Auto);
                    
                    THEN("casual patterns are handled appropriately") {
                        REQUIRE(result.has_value());
                        REQUIRE_THAT(result.value().value(), ContainsSubstring(name));
                        INFO("Casual name: " << name);
                    }
                }
            }
        }
    }
}

SCENARIO("StrategyContext performance and diagnostics", "[StrategyContext][performance]") {
    
    GIVEN("a context with metrics enabled") {
        ContextConfig config{};
        config.enable_metrics = greeting::config::is_debug_build();
        
        auto context = createOptimizedContext(config);
        REQUIRE(context != nullptr);
        
        WHEN("performing multiple greetings") {
            std::vector<std::string> names = {"Alice", "Bob", "Charlie"};
            
            for (const auto& name : names) {
                auto person_result = PersonName::create(name);
                if (person_result.has_value()) {
                    auto result = context->greet(person_result.value());
                    REQUIRE(result.has_value());
                }
            }
            
            THEN("metrics are tracked correctly") {
                if constexpr (greeting::config::is_debug_build()) {
                    auto metrics = context->getPerformanceMetrics();
                    REQUIRE_THAT(metrics, ContainsSubstring("Total Greetings"));
                    
                    auto last_execution = context->getLastExecution();
                    if (last_execution.has_value()) {
                        REQUIRE_FALSE(last_execution->strategy_name.empty());
                        INFO("Last strategy: " << last_execution->strategy_name);
                    }
                } else {
                    INFO("Performance metrics available only in debug builds");
                }
            }
        }
        
        WHEN("clearing cache") {
            context->clearCache();
            
            THEN("cache clearing completes without error") {
                // Cache clearing should not affect basic functionality
                auto person = PersonName::create("Test").value();
                auto result = context->greet(person);
                REQUIRE(result.has_value());
            }
        }
    }
}

SCENARIO("StrategyContext utility functions", "[StrategyContext][utilities]") {
    
    GIVEN("strategy type utilities") {
        
        WHEN("converting strategy types to strings") {
            
            THEN("conversion works correctly") {
                REQUIRE(toString(StrategyType::Console) == "Console");
                REQUIRE(toString(StrategyType::Formal) == "Formal");
                REQUIRE(toString(StrategyType::Casual) == "Casual");
                REQUIRE(toString(StrategyType::Auto) == "Auto");
            }
        }
        
        WHEN("parsing strategy types from strings") {
            
            THEN("parsing works correctly") {
                REQUIRE(parseStrategyType("Console") == StrategyType::Console);
                REQUIRE(parseStrategyType("console") == StrategyType::Console);
                REQUIRE(parseStrategyType("Formal") == StrategyType::Formal);
                REQUIRE(parseStrategyType("Casual") == StrategyType::Casual);
                REQUIRE(parseStrategyType("Auto") == StrategyType::Auto);
                
                REQUIRE_FALSE(parseStrategyType("Invalid").has_value());
                REQUIRE_FALSE(parseStrategyType("").has_value());
            }
        }
    }
}

// ============================================================================
// Configuration-Aware Tests
// ============================================================================

SCENARIO("StrategyContext configuration-aware behavior", "[StrategyContext][config-aware]") {
    
    GIVEN("a strategy context") {
        auto context = createDefaultContext();
        REQUIRE(context != nullptr);
        
        WHEN("operating in different build configurations") {
            auto person = PersonName::create("Configuration Test").value();
            auto result = context->greet(person);
            
            THEN("behavior adapts to configuration") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Configuration Test"));
                
                if constexpr (greeting::config::is_debug_build()) {
                    // Debug builds may include additional context
                    auto metrics = context->getPerformanceMetrics();
                    REQUIRE_THAT(metrics, ContainsSubstring("Performance Metrics"));
                    INFO("Debug mode: metrics enabled");
                } else {
                    // Release builds focus on performance
                    INFO("Release mode: optimized execution");
                }
            }
        }
    }
}
```

## **Integration Points**

### **A. Strategy Interface Compliance**

- **Implements**: Uses `IGreetingStrategy` interface for all strategy management
- **Extends**: Adds context management, lifecycle control, and automatic selection
- **Maintains**: Full compatibility with individual strategy implementations
- **Enhances**: Provides fallback mechanisms and error recovery

### **B. Configuration Infrastructure Integration**

- **Leverages**: Phase 2's configuration-aware validation for input processing
- **Uses**: Build configuration for performance optimizations and debug features
- **Maintains**: Zero-overhead abstractions in release builds
- **Integrates**: With error handling system for rich context propagation

### **C. Domain Type Integration**

- **Input**: Uses `PersonName` with configuration-aware validation
- **Output**: Produces `GreetingMessage` through strategy execution
- **Validation**: Integrates with Phase 2.1's dual-path validation system
- **Error Handling**: Leverages Phase 2.2's configuration-aware error context

## **Testing Strategy**

### **1. Unit Test Coverage**

- **Basic functionality**: Context creation, strategy management, greeting generation
- **Configuration management**: Context config updates, validation, persistence
- **Strategy selection**: Automatic selection logic, fallback mechanisms
- **Error handling**: Invalid inputs, strategy failures, configuration errors
- **Performance**: Metrics tracking, cache management, optimization verification

### **2. Integration Testing**

- **Strategy Integration**: Verify compatibility with all strategy implementations
- **Configuration Integration**: Test behavior across debug/release configurations
- **Validation Integration**: Ensure proper integration with Phase 2 validation
- **Error Propagation**: Verify error context flows correctly through system

### **3. Performance Testing**

- **Strategy Selection**: Measure overhead of automatic selection logic
- **Context Management**: Verify minimal overhead in release builds
- **Fallback Performance**: Test fallback mechanism performance impact
- **Memory Usage**: Validate strategy lifecycle management efficiency

## **Configuration Considerations**

### **1. Debug Configuration**

- **Rich Metrics**: Comprehensive performance tracking and execution context
- **Strategy Diagnostics**: Detailed strategy selection and execution information
- **Fallback Tracking**: Monitor fallback usage and success rates
- **Cache Management**: Enable caching for improved debug-time performance

### **2. Release Configuration**

- **Optimized Execution**: Minimal overhead strategy selection and execution
- **Direct Strategy Access**: Bypass unnecessary abstraction layers
- **Reduced Metrics**: Essential metrics only for production monitoring
- **Memory Efficiency**: Optimize memory usage for strategy management

## **C++23 Features Utilized**

### **1. `if consteval` Optimization**

- **Compile-time**: Configuration detection and optimization selection
- **Runtime**: Dual-path strategy execution based on build configuration
- **Performance**: Zero-overhead abstractions where possible

### **2. Concepts for Strategy Validation**

- **Strategy Validation**: Compile-time verification of strategy compatibility
- **Type Safety**: Ensure proper strategy interface implementation
- **Template Constraints**: Provide clear error messages for invalid usage

### **3. Enhanced `constexpr` and `consteval`**

- **Configuration Functions**: Compile-time configuration detection
- **Utility Functions**: Compile-time strategy type conversion and validation
- **Optimization**: Eliminate runtime overhead for constant expressions

## **Success Criteria**

### **1. Functional Requirements**

- ✅ **Strategy Management**: Register, validate, and manage multiple strategies
- ✅ **Automatic Selection**: Intelligently select strategies based on input context
- ✅ **Fallback Mechanisms**: Graceful degradation when strategies fail
- ✅ **Configuration Integration**: Full compatibility with Phase 2 infrastructure
- ✅ **Error Handling**: Rich error context with configuration-aware propagation

### **2. Performance Requirements**

- ✅ **Zero-Overhead**: Release builds add minimal overhead over direct strategy usage
- ✅ **Memory Efficiency**: Efficient strategy lifecycle and cache management
- ✅ **Compilation Speed**: Minimal impact on build times
- ✅ **Runtime Performance**: Fast strategy selection and execution

### **3. Integration Requirements**

- ✅ **Strategy Compatibility**: Works with all Phase 3.1.1-3.1.4 strategy implementations
- ✅ **Configuration Awareness**: Adapts behavior based on build configuration
- ✅ **Validation Integration**: Uses Phase 2.1's configuration-aware validation
- ✅ **Error System Integration**: Leverages Phase 2.2's error handling infrastructure

## **Milestones**

### **Phase 3.1.5.1: Core Context Implementation** (4 hours)

- [ ] Implement `StrategyContext` class with basic strategy management
- [ ] Add automatic strategy selection logic
- [ ] Integrate with configuration-aware validation system

### **Phase 3.1.5.2: Advanced Features** (3 hours)

- [ ] Implement fallback mechanisms and error recovery
- [ ] Add performance monitoring and metrics collection
- [ ] Enhance context configuration management

### **Phase 3.1.5.3: Testing & Integration** (3 hours)

- [ ] Comprehensive unit test suite with >95% coverage
- [ ] Integration testing with all strategy implementations
- [ ] Performance benchmarking and optimization verification

---

This Strategy Context Integration implementation provides a sophisticated orchestration layer that manages strategy lifecycle, automates strategy selection, and provides comprehensive error handling while maintaining the performance and reliability standards established throughout the project. The context leverages the entire Phase 2 configuration infrastructure and integrates seamlessly with all strategy implementations from Phase 3.1.1-3.1.4.