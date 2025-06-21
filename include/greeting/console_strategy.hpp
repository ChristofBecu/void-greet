#pragma once

#include "greeting/strategy_interface.hpp"
#include "greeting/expected.hpp"
#include <chrono>
#include <iomanip>
#include <memory>

namespace greeting::strategy {

/**
 * @brief Forward declaration for console strategy
 */
class ConsoleGreetingStrategy;

/**
 * @brief Console strategy factory function
 * @return Unique pointer to console strategy instance
 */
[[nodiscard]] std::unique_ptr<IGreetingStrategy> createConsoleStrategy() noexcept;

} // namespace greeting::strategy

// Specialization must be in greeting::strategy::traits namespace
namespace greeting::strategy::traits {

/**
 * @brief Console strategy traits specialization
 */
template<>
struct StrategyTraits<greeting::strategy::ConsoleGreetingStrategy> {
    static constexpr bool supports_debug_info = true;
    static constexpr bool enable_performance_monitoring = greeting::config::is_debug_build();
    static constexpr bool enable_validation_caching = false; // I/O operations not cacheable
    
    static constexpr size_t expected_message_length = 50;
    static constexpr bool is_thread_safe = false; // std::cout is not thread-safe
    static constexpr bool supports_async_operation = false;
    static constexpr bool requires_system_resources = true; // Needs console access
};

} // namespace greeting::strategy::traits
