#pragma once

#include "greeting/strategy_interface.hpp"
#include "greeting/expected.hpp"
#include "config/build_config.hpp"
#include <string_view>
#include <array>
#include <vector>
#include <optional>
#include <memory>

namespace greeting::strategy {

/**
 * @brief Forward declaration for formal strategy
 */
class FormalGreetingStrategy;

/**
 * @brief Formal strategy factory function
 * @return Unique pointer to formal strategy instance
 */
[[nodiscard]] std::unique_ptr<IGreetingStrategy> createFormalStrategy() noexcept;

/**
 * @brief Compile-time formal greeting utilities
 */
namespace formal_utils {

/**
 * @brief Compile-time title validation
 */
constexpr bool isFormalTitleValid(std::string_view title) noexcept {
    constexpr std::array<std::string_view, 12> titles = {{
        "Dr.", "Prof.", "Mr.", "Mrs.", "Ms.", "Miss",
        "Sir", "Madam", "Lord", "Lady", "Hon.", "Rev."
    }};
    
    for (const auto& valid_title : titles) {
        if (title == valid_title) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Compile-time formal greeting template
 */
template<std::size_t N>
constexpr std::array<char, N + 12> createFormalGreetingTemplate(
    const std::array<char, N>& name) noexcept {
    
    std::array<char, N + 12> result{};
    const char prefix[] = "Good day, ";
    const char suffix[] = ".";
    
    // Copy prefix
    for (std::size_t i = 0; i < 10; ++i) {
        result[i] = prefix[i];
    }
    
    // Copy name
    for (std::size_t i = 0; i < N - 1; ++i) { // -1 to exclude null terminator
        result[i + 10] = name[i];
    }
    
    // Copy suffix
    result[N + 9] = '.';
    result[N + 10] = '\0';
    
    return result;
}

} // namespace formal_utils

} // namespace greeting::strategy

// Specialization must be in greeting::strategy::traits namespace
namespace greeting::strategy::traits {

/**
 * @brief Formal strategy traits specialization
 */
template<>
struct StrategyTraits<greeting::strategy::FormalGreetingStrategy> {
    static constexpr bool supports_debug_info = true;
#ifdef DEBUG
    static constexpr bool enable_performance_monitoring = true;
    static constexpr bool enable_validation_caching = true;
#else
    static constexpr bool enable_performance_monitoring = false;
    static constexpr bool enable_validation_caching = false;
#endif
    
    static constexpr size_t expected_message_length = 80; // Longer for formal greetings
    static constexpr bool is_thread_safe = true; // No shared mutable state
    static constexpr bool supports_async_operation = true;
    static constexpr bool requires_system_resources = false;
    static constexpr bool supports_localization = true; // Future enhancement
    static constexpr bool supports_cultural_context = true;
};

} // namespace greeting::strategy::traits

/**
 * @brief Compile-time formal greeting utilities
 */
namespace greeting::strategy::compile_time {

/**
 * @brief Compile-time formal greeting generation
 */
template<std::size_t N>
constexpr auto generateFormalGreeting(const char (&name)[N]) noexcept {
    // Create a compile-time greeting with "Good day, " prefix and "." suffix
    constexpr const char prefix[] = "Good day, ";
    constexpr const char suffix[] = ".";
    constexpr std::size_t prefix_len = sizeof(prefix) - 1;
    constexpr std::size_t suffix_len = sizeof(suffix) - 1;
    constexpr std::size_t name_len = N - 1; // exclude null terminator
    constexpr std::size_t total_len = prefix_len + name_len + suffix_len + 1; // +1 for null terminator
    
    std::array<char, total_len> result{};
    
    // Copy prefix
    for (std::size_t i = 0; i < prefix_len; ++i) {
        result[i] = prefix[i];
    }
    
    // Copy name
    for (std::size_t i = 0; i < name_len; ++i) {
        result[i + prefix_len] = name[i];
    }
    
    // Copy suffix
    for (std::size_t i = 0; i < suffix_len; ++i) {
        result[i + prefix_len + name_len] = suffix[i];
    }
    
    // Null terminator
    result[total_len - 1] = '\0';
    
    return result;
}

/**
 * @brief Compile-time title validation
 */
constexpr bool validateFormalTitle(std::string_view title) noexcept {
    return formal_utils::isFormalTitleValid(title);
}

} // namespace greeting::strategy::compile_time
