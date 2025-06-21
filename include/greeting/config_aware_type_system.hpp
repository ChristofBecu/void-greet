#pragma once

// ============================================================================
// Configuration-Aware Type System
// 
// This header provides modern C++23 concepts, type traits, and utilities
// for configuration-aware validation and type safety.
// ============================================================================

#include "config/build_config.hpp"
#include "greeting/error_codes.hpp"
#include "greeting/expected.hpp"
#include <string_view>
#include <type_traits>

#ifdef __cpp_concepts
#include <concepts>
#endif

// Forward declarations
namespace greeting {
class PersonName;
class GreetingMessage;
}

namespace greeting::types {

// ============================================================================
// Configuration-Aware Type Traits
// ============================================================================

/**
 * @brief Type trait to detect if a type has configuration-aware validation
 */
template<typename T>
struct has_config_aware_validation : std::false_type {};

/**
 * @brief Type trait to detect if a type supports internal construction
 */
template<typename T>
struct has_internal_tag {
private:
    template<typename U>
    static auto test(int) -> decltype(typename U::InternalTag{}, std::true_type{});
    
    template<typename>
    static std::false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

template<typename T>
inline constexpr bool has_internal_tag_v = has_internal_tag<T>::value;

/**
 * @brief Type trait to check if type supports validation result
 */
template<typename T>
struct supports_validation_result {
private:
    template<typename U>
    static auto test(int) -> decltype(
        U::create(std::declval<std::string_view>()),
        std::true_type{}
    );
    
    template<typename>
    static std::false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

template<typename T>
inline constexpr bool supports_validation_result_v = supports_validation_result<T>::value;

/**
 * @brief Type trait for configuration-aware error propagation
 */
template<typename T>
struct supports_error_propagation : std::false_type {};

// Specializations for our domain types
template<>
struct has_config_aware_validation<greeting::PersonName> : std::true_type {};

template<>
struct has_config_aware_validation<greeting::GreetingMessage> : std::true_type {};

template<>
struct supports_error_propagation<greeting::PersonName> : std::true_type {};

template<>
struct supports_error_propagation<greeting::GreetingMessage> : std::true_type {};

// ============================================================================
// Configuration-Aware Concepts (C++23)
// ============================================================================

#ifdef __cpp_concepts

/**
 * @brief Concept for types that support internal construction with validation bypass
 */
template<typename T>
concept InternallyConstructible = requires {
    typename T::InternalTag;
    requires std::constructible_from<T, std::string, typename T::InternalTag>;
};

/**
 * @brief Concept for types that support configuration-aware validation
 */
template<typename T>
concept ConfigAwareValidatable = 
    has_config_aware_validation<T>::value &&
    InternallyConstructible<T> &&
    supports_validation_result<T>::value;

/**
 * @brief Concept for domain types that support error propagation
 */
template<typename T>
concept ErrorPropagatable = 
    supports_error_propagation<T>::value &&
    ConfigAwareValidatable<T>;

/**
 * @brief Concept for validation functions
 */
template<typename F, typename T>
concept ValidationFunction = requires(F func, std::string_view input) {
    { func(input) } -> std::same_as<Expected<T, GreetingError>>;
};

/**
 * @brief Concept for string-like types suitable for validation input
 */
template<typename T>
concept StringLike = 
    std::convertible_to<T, std::string_view> ||
    std::same_as<std::remove_cvref_t<T>, std::string> ||
    std::same_as<std::remove_cvref_t<T>, const char*>;

/**
 * @brief Concept for configuration contexts
 */
template<typename T>
concept ConfigurationContext = requires {
    { T::is_debug_build() } -> std::same_as<bool>;
    { T::is_release_build() } -> std::same_as<bool>;
    { T::enable_detailed_validation() } -> std::same_as<bool>;
    { T::enable_performance_monitoring() } -> std::same_as<bool>;
};

/**
 * @brief Concept for zero-overhead validation
 */
template<typename T>
concept ZeroOverheadValidatable = 
    ConfigAwareValidatable<T> &&
    std::is_move_constructible_v<T> &&
    std::is_move_assignable_v<T>;

#endif // __cpp_concepts

// ============================================================================
// Configuration-Aware Utility Functions
// ============================================================================

/**
 * @brief Compile-time detection of configuration mode
 */
consteval bool is_debug_configuration() noexcept {
    #ifdef HELLOWORLD_DEBUG_BUILD
        return true;
    #else
        return false;
    #endif
}

/**
 * @brief Compile-time detection of release mode
 */
consteval bool is_release_configuration() noexcept {
    return !is_debug_configuration();
}

/**
 * @brief Configuration-aware validation dispatch (declaration only)
 * 
 * This function template provides a unified interface for configuration-aware validation.
 * Implementation is provided in config_aware_validation.hpp to avoid circular dependencies.
 */
template<ConfigAwareValidatable T>
[[nodiscard]] Expected<T, GreetingError> validate_with_config_awareness(
    std::string_view input) noexcept;

// ============================================================================
// Type System Utilities
// ============================================================================

/**
 * @brief SFINAE helper for validation-enabled types
 */
template<typename T, typename = void>
struct is_validatable : std::false_type {};

template<typename T>
struct is_validatable<T, std::void_t<
    decltype(T::create(std::declval<std::string_view>()))
>> : std::true_type {};

template<typename T>
inline constexpr bool is_validatable_v = is_validatable<T>::value;

/**
 * @brief Configuration-aware error handling utilities
 */
namespace error_handling {

/**
 * @brief Enhanced error context for debug builds
 */
template<typename ErrorType>
[[nodiscard]] constexpr ErrorType enhance_error_context(
    ErrorType error, 
    std::string_view context = "") noexcept {
    
    if constexpr (is_debug_configuration()) {
        // In debug builds, we could enhance error with context
        // For now, just return the original error
        return error;
    } else {
        // In release builds, just return the error as-is for performance
        return error;
    }
}

/**
 * @brief Configuration-aware error propagation
 */
template<ErrorPropagatable T>
[[nodiscard]] constexpr GreetingError propagate_error(
    GreetingError original_error,
    std::string_view operation_context = "") noexcept {
    
    return enhance_error_context(original_error, operation_context);
}

} // namespace error_handling

// ============================================================================
// Configuration-Aware Performance Utilities
// ============================================================================

namespace performance {

/**
 * @brief Zero-overhead validation assertion
 */
template<ZeroOverheadValidatable T>
constexpr void assert_zero_overhead() noexcept {
    static_assert(std::is_move_constructible_v<T>, 
                  "Type must be move constructible for zero-overhead");
    static_assert(std::is_move_assignable_v<T>, 
                  "Type must be move assignable for zero-overhead");
    static_assert(has_internal_tag_v<T>, 
                  "Type must support internal construction for zero-overhead");
    
    // Note: We don't require trivially destructible since domain types may contain std::string
    // Zero-overhead refers to validation overhead, not object lifecycle overhead
}

/**
 * @brief Configuration-aware optimization hints
 */
template<typename T>
[[nodiscard]] constexpr bool should_optimize_for_size() noexcept {
    if constexpr (is_release_configuration()) {
        return sizeof(T) > 64; // Optimize larger types for size in release
    } else {
        return false; // Prioritize debugging in debug builds
    }
}

} // namespace performance

} // namespace greeting::types

// ============================================================================
// Global Type Aliases for Configuration-Aware Types
// ============================================================================

namespace greeting {

// Import the concepts into the main greeting namespace for convenience
#ifdef __cpp_concepts
using types::ConfigAwareValidatable;
using types::ErrorPropagatable;
using types::InternallyConstructible;
using types::StringLike;
using types::ValidationFunction;
using types::ZeroOverheadValidatable;
#endif

// Import utility functions
using types::is_debug_configuration;
using types::is_release_configuration;

} // namespace greeting
