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

Result<GreetingMessage> StrategyContext::greet(
    const PersonName& person) const noexcept {
    
    return greetWith(person, config_.default_strategy);
}

Result<GreetingMessage> StrategyContext::greetWith(
    const PersonName& person,
    StrategyType strategy_type) const noexcept {
    
    // Select appropriate strategy
    StrategyType selected_strategy = selectStrategy(person, strategy_type);
    
    // Get strategy implementation
    IGreetingStrategy* strategy = getStrategy(selected_strategy);
    if (!strategy) {
        if (config_.enable_fallback && selected_strategy != config_.default_strategy) {
            // Try fallback to default strategy
            strategy = getStrategy(config_.default_strategy);
            selected_strategy = config_.default_strategy;
        }
        
        if (!strategy) {
            return createContextError(
                GreetingError::StrategyNotFound,
                "No strategy available",
                toString(strategy_type)
            );
        }
    }
    
    // Execute greeting with metrics tracking
    ExecutionContext execution_context{
        .selected_strategy = selected_strategy,
        .strategy_name = std::string(strategy->getStrategyName()),
        .fallback_used = (selected_strategy != strategy_type)
    };
    
    auto result = executeGreeting(strategy, person, execution_context);
    
    // Store execution context for debugging
    if constexpr (greeting::config::is_debug_build()) {
        if (config_.enable_metrics) {
            last_execution_ = execution_context;
            total_greetings_++;
            #ifdef HELLOWORLD_DEBUG_BUILD
            total_execution_time_ += execution_context.execution_time;
            #endif
        }
    }
    
    return result;
}

Result<GreetingMessage> StrategyContext::greetWithValidation(
    std::string_view person_name,
    StrategyType strategy_type) const noexcept {
    
    // Validate person name using configuration-aware validation
    auto person_result = PersonName::create(person_name);
    if (!person_result.has_value()) {
        return person_result.error();
    }
    
    // Track validation steps in debug builds
    if constexpr (greeting::config::is_debug_build()) {
        if (config_.enable_metrics && last_execution_.has_value()) {
            #ifdef HELLOWORLD_DEBUG_BUILD
            last_execution_->validation_steps++;
            #endif
        }
    }
    
    return greetWith(person_result.value(), strategy_type);
}

// ============================================================================
// Strategy Management Implementation
// ============================================================================

bool StrategyContext::registerStrategy(
    StrategyType strategy_type,
    std::unique_ptr<IGreetingStrategy> strategy) noexcept {
    
    if (!strategy || strategy_type == StrategyType::Auto) {
        return false;
    }
    
    // Validate strategy before registration
    if (!strategy->isValid()) {
        return false;
    }
    
    strategies_[strategy_type] = std::move(strategy);
    return true;
}

bool StrategyContext::hasStrategy(StrategyType strategy_type) const noexcept {
    if (strategy_type == StrategyType::Auto) {
        // Auto is not a real strategy, it's a selection mechanism
        return false;
    }
    
    auto it = strategies_.find(strategy_type);
    return it != strategies_.end() && it->second && it->second->isValid();
}

std::vector<StrategyType> StrategyContext::getAvailableStrategies() const noexcept {
    std::vector<StrategyType> available;
    available.reserve(strategies_.size());
    
    for (const auto& [type, strategy] : strategies_) {
        if (strategy && strategy->isValid()) {
            available.push_back(type);
        }
    }
    
    return available;
}

bool StrategyContext::setDefaultStrategy(StrategyType strategy_type) noexcept {
    if (strategy_type == StrategyType::Auto || !hasStrategy(strategy_type)) {
        return false;
    }
    
    config_.default_strategy = strategy_type;
    return true;
}

// ============================================================================
// Context Management Implementation
// ============================================================================

const ContextConfig& StrategyContext::getConfig() const noexcept {
    return config_;
}

bool StrategyContext::updateConfig(const ContextConfig& config) noexcept {
    if (!config.isValid()) {
        return false;
    }
    
    config_ = config;
    
    // Initialize metrics tracking if enabled
    if constexpr (greeting::config::is_debug_build()) {
        if (config_.enable_metrics && !last_execution_.has_value()) {
            last_execution_.emplace();
        }
    }
    
    return true;
}

std::optional<ExecutionContext> StrategyContext::getLastExecution() const noexcept {
    if constexpr (greeting::config::is_debug_build()) {
        return last_execution_;
    } else {
        return std::nullopt;
    }
}

bool StrategyContext::isValid() const noexcept {
    return config_.isValid() && 
           hasStrategy(config_.default_strategy) &&
           !strategies_.empty();
}

// ============================================================================
// Performance and Diagnostics Implementation
// ============================================================================

void StrategyContext::clearCache() const noexcept {
    if constexpr (greeting::config::is_debug_build()) {
        if (config_.enable_caching) {
            performance_cache_.clear();
        }
    }
}

std::string StrategyContext::getPerformanceMetrics() const noexcept {
    if constexpr (greeting::config::is_debug_build()) {
        if (!config_.enable_metrics) {
            return "Metrics disabled";
        }
        
        std::ostringstream oss;
        oss << "Strategy Context Performance Metrics:\n";
        oss << "  Total greetings: " << total_greetings_ << "\n";
        
        #ifdef HELLOWORLD_DEBUG_BUILD
        oss << "  Total execution time: " << total_execution_time_.count() << " microseconds\n";
        if (total_greetings_ > 0) {
            auto avg_time = total_execution_time_.count() / total_greetings_;
            oss << "  Average execution time: " << avg_time << " microseconds\n";
        }
        #endif
        
        oss << "  Available strategies: " << strategies_.size() << "\n";
        oss << "  Default strategy: " << toString(config_.default_strategy) << "\n";
        oss << "  Fallback enabled: " << (config_.enable_fallback ? "yes" : "no") << "\n";
        
        return oss.str();
    } else {
        return "Performance metrics not available in release builds";
    }
}

// ============================================================================
// Internal Implementation
// ============================================================================

void StrategyContext::initializeDefaultStrategies() noexcept {
    // Register console strategy
    auto console_strategy = createConsoleStrategy();
    if (console_strategy) {
        strategies_[StrategyType::Console] = std::move(console_strategy);
    }
    
    // Register formal strategy
    auto formal_strategy = createFormalStrategy();
    if (formal_strategy) {
        strategies_[StrategyType::Formal] = std::move(formal_strategy);
    }
    
    // Register casual strategy - use static create method
    // Note: Need to check if CasualStrategy factory exists or adapt based on actual interface
    // For now, only register if the factory function is available
}

StrategyType StrategyContext::selectStrategy(
    const PersonName& person,
    StrategyType requested_type) const noexcept {
    
    if (requested_type != StrategyType::Auto) {
        return requested_type;
    }
    
    // Automatic strategy selection based on person name characteristics
    const auto& name = person.value();
    
    // Simple heuristics for automatic selection
    if (name.length() <= 3) {
        // Short names often work better with casual strategy
        if (hasStrategy(StrategyType::Casual)) {
            return StrategyType::Casual;
        }
    }
    
    // Check for formal indicators (titles, multiple words)
    if (name.find("Dr.") != std::string::npos ||
        name.find("Prof.") != std::string::npos ||
        name.find("Mr.") != std::string::npos ||
        name.find("Mrs.") != std::string::npos ||
        name.find("Ms.") != std::string::npos) {
        if (hasStrategy(StrategyType::Formal)) {
            return StrategyType::Formal;
        }
    }
    
    // Default to configured default strategy
    return config_.default_strategy;
}

IGreetingStrategy* StrategyContext::getStrategy(
    StrategyType strategy_type) const noexcept {
    
    if (strategy_type == StrategyType::Auto) {
        strategy_type = config_.default_strategy;
    }
    
    auto it = strategies_.find(strategy_type);
    if (it != strategies_.end() && it->second) {
        return it->second.get();
    }
    
    return nullptr;
}

Result<GreetingMessage> StrategyContext::executeGreeting(
    IGreetingStrategy* strategy,
    const PersonName& person,
    ExecutionContext& context) const noexcept {
    
    if constexpr (greeting::config::is_debug_build()) {
        #ifdef HELLOWORLD_DEBUG_BUILD
        auto start_time = std::chrono::high_resolution_clock::now();
        #endif
        
        auto result = strategy->greet(person);
        
        #ifdef HELLOWORLD_DEBUG_BUILD
        auto end_time = std::chrono::high_resolution_clock::now();
        context.execution_time = std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - start_time);
        #endif
        
        if (!result.has_value()) {
            return handleStrategyError(result.error(), person, context.selected_strategy);
        }
        
        return result;
    } else {
        // Release builds skip timing overhead
        return strategy->greet(person);
    }
}

Result<GreetingMessage> StrategyContext::handleStrategyError(
    GreetingError error,
    const PersonName& person,
    StrategyType failed_strategy) const noexcept {
    
    if (!config_.enable_fallback || failed_strategy == config_.default_strategy) {
        // No fallback available or fallback already failed
        return createContextError(error, "Strategy execution failed", toString(failed_strategy));
    }
    
    // Try fallback strategy
    IGreetingStrategy* fallback_strategy = getStrategy(config_.default_strategy);
    if (!fallback_strategy) {
        return createContextError(error, "Fallback strategy not available");
    }
    
    ExecutionContext fallback_context{
        .selected_strategy = config_.default_strategy,
        .strategy_name = std::string(fallback_strategy->getStrategyName()),
        .fallback_used = true
    };
    
    auto fallback_result = executeGreeting(fallback_strategy, person, fallback_context);
    if (!fallback_result.has_value()) {
        return createContextError(
            fallback_result.error(),
            "Both primary and fallback strategies failed"
        );
    }
    
    return fallback_result;
}

GreetingError StrategyContext::createContextError(
    GreetingError error,
    std::string_view context,
    std::string_view strategy_info) const noexcept {
    
    if constexpr (greeting::config::is_debug_build()) {
        // In debug builds, we could enhance the error with additional context
        // For now, just return the original error
        return error;
    } else {
        // Release builds return minimal error information
        return error;
    }
}

// ============================================================================
// Utility Functions Implementation
// ============================================================================

std::optional<StrategyType> parseStrategyType(
    std::string_view strategy_name) noexcept {
    
    if (strategy_name == "Console") return StrategyType::Console;
    if (strategy_name == "Formal")  return StrategyType::Formal;
    if (strategy_name == "Casual")  return StrategyType::Casual;
    if (strategy_name == "Auto")    return StrategyType::Auto;
    
    return std::nullopt;
}

std::unique_ptr<StrategyContext> createDefaultContext() noexcept {
    return std::make_unique<StrategyContext>();
}

std::unique_ptr<StrategyContext> createOptimizedContext(
    const ContextConfig& config) noexcept {
    
    return std::make_unique<StrategyContext>(config);
}

} // namespace greeting::strategy
