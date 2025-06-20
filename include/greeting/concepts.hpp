#pragma once

// Check for C++20 concepts support
#if __cpp_concepts >= 201907L
#include <concepts>
#else
#error "C++20 concepts support is required for this header"
#endif

#include <string>
#include <string_view>
#include <type_traits>
#include <cstdint>

/**
 * @file concepts.hpp
 * @brief C++20 concepts for greeting domain contracts
 * 
 * This header defines concepts that enforce contracts and type safety
 * across the greeting domain. These concepts ensure that types and
 * functions conform to expected interfaces and behaviors.
 * 
 * Key concepts include:
 * - StringLike: For types that can be converted to string representations
 * - Validatable: For types that support validation operations
 * - GreetingStrategy: For greeting generation strategies
 * - FormattedOutput: For types that can produce formatted output
 * - PersonIdentifier: For types representing person identification
 * - MessageGenerator: For greeting message generation capabilities
 */

namespace greeting::concepts {

/**
 * @brief Concept for types that behave like strings
 * 
 * A StringLike type must be convertible to string_view and provide
 * basic string operations. This concept is used to constrain template
 * parameters that should accept string-like types.
 */
template<typename T>
concept StringLike = requires(const T& t) {
    { std::string_view{t} } -> std::convertible_to<std::string_view>;
    { t.empty() } -> std::convertible_to<bool>;
    { t.size() } -> std::convertible_to<std::size_t>;
};

/**
 * @brief Concept for types that can be validated
 * 
 * A Validatable type must provide validation functionality that returns
 * a boolean indicating validity. This is used for domain objects that
 * need to ensure their state is valid.
 */
template<typename T>
concept Validatable = requires(const T& t) {
    { t.isValid() } -> std::convertible_to<bool>;
};

/**
 * @brief Concept for types that have a string value
 * 
 * A ValueHolder type must provide access to an underlying string value.
 * This concept is used for strong type wrappers like PersonName and
 * GreetingMessage.
 */
template<typename T>
concept ValueHolder = requires(const T& t) {
    { t.value() } -> std::convertible_to<std::string_view>;
} && Validatable<T>;

/**
 * @brief Concept for person identifier types
 * 
 * A PersonIdentifier must be a ValueHolder and provide person-specific
 * validation. This ensures type safety when working with person names.
 */
template<typename T>
concept PersonIdentifier = ValueHolder<T> && requires(std::string_view sv) {
    { T::create(sv) } -> std::same_as<typename T::Result>;
};

/**
 * @brief Concept for message types
 * 
 * A GreetingMessageType must be a ValueHolder and provide message-specific
 * validation. This ensures type safety when working with greeting messages.
 */
template<typename T>
concept GreetingMessageType = ValueHolder<T> && requires(std::string_view sv) {
    { T::create(sv) } -> std::same_as<typename T::Result>;
};

/**
 * @brief Concept for greeting strategy implementations
 * 
 * A GreetingStrategy must be able to generate a greeting message given
 * a person identifier. This defines the contract for the Strategy pattern
 * implementation in the greeting domain.
 */
template<typename S, typename PersonType, typename MessageType>
concept GreetingStrategy = requires(const S& strategy, const PersonType& person) {
    requires PersonIdentifier<PersonType>;
    requires GreetingMessageType<MessageType>;
    { strategy.generateGreeting(person) } -> std::same_as<MessageType>;
};

/**
 * @brief Concept for factory types that create strategies
 * 
 * A StrategyFactory must be able to create strategy instances based on
 * some configuration or type parameter. This supports the Factory pattern
 * for strategy creation.
 */
template<typename F, typename StrategyType>
concept StrategyFactory = requires(const F& factory) {
    { factory.createStrategy() } -> std::same_as<StrategyType>;
} || requires(const F& factory, std::string_view type) {
    { factory.createStrategy(type) } -> std::same_as<StrategyType>;
};

/**
 * @brief Concept for output formatters
 * 
 * A Formatter must be able to format messages for output. This abstracts
 * the presentation layer from the domain logic.
 */
template<typename F, typename MessageType>
concept Formatter = requires(const F& formatter, const MessageType& message) {
    requires GreetingMessageType<MessageType>;
    { formatter.format(message) } -> StringLike;
};

/**
 * @brief Concept for output destinations
 * 
 * An OutputDestination must be able to write formatted strings.
 * This abstracts the actual output mechanism (console, file, network, etc.).
 */
template<typename D>
concept OutputDestination = requires(D& dest, std::string_view content) {
    { dest.write(content) } -> std::same_as<void>;
    { dest.flush() } -> std::same_as<void>;
};

/**
 * @brief Concept for complete greeting systems
 * 
 * A GreetingSystem must coordinate between strategies, formatters, and
 * output destinations to produce complete greeting functionality.
 */
template<typename G, typename PersonType, typename OutputType>
concept GreetingSystem = requires(G& system, const PersonType& person) {
    requires PersonIdentifier<PersonType>;
    requires OutputDestination<OutputType>;
    { system.greet(person) } -> std::same_as<void>;
};

/**
 * @brief Concept for error types in the greeting domain
 * 
 * A GreetingError must provide error information and be convertible
 * to a string representation for debugging and user feedback.
 */
template<typename E>
concept GreetingError = requires(const E& error) {
    { error.message() } -> StringLike;
    { error.code() } -> std::convertible_to<int>;
    { error.severity() } -> std::convertible_to<int>;
};

/**
 * @brief Concept for result types that can contain values or errors
 * 
 * A Result type must support the expected/optional pattern for error handling.
 * This is used throughout the domain for operations that can fail.
 */
template<typename R, typename ValueType, typename ErrorType>
concept Result = requires(const R& result) {
    requires GreetingError<ErrorType>;
    { result.hasValue() } -> std::convertible_to<bool>;
    { result.value() } -> std::same_as<const ValueType&>;
    { result.error() } -> std::same_as<const ErrorType&>;
    { static_cast<bool>(result) } -> std::convertible_to<bool>;
};

/**
 * @brief Concept for compile-time validatable types
 * 
 * A ConstevalValidatable type must support compile-time validation
 * when used with string literals or constexpr contexts.
 */
template<typename T>
concept ConstevalValidatable = requires {
    { T::validate_consteval(std::string_view{}) } -> std::same_as<bool>;
};

/**
 * @brief Concept for configurable components
 * 
 * A Configurable type must support configuration through a settings object.
 * This enables dependency injection and testability.
 */
template<typename T, typename ConfigType>
concept Configurable = requires(T& component, const ConfigType& config) {
    { component.configure(config) } -> std::same_as<void>;
    { component.isConfigured() } -> std::convertible_to<bool>;
};

/**
 * @brief Concept for loggable events in the greeting system
 * 
 * A LoggableEvent must provide structured information that can be
 * written to logs for debugging and monitoring.
 */
template<typename E>
concept LoggableEvent = requires(const E& event) {
    { event.timestamp() } -> std::convertible_to<std::uint64_t>;
    { event.level() } -> std::convertible_to<int>;
    { event.message() } -> StringLike;
    { event.category() } -> StringLike;
};

} // namespace greeting::concepts

/**
 * @brief Convenience aliases for commonly used concept combinations
 * 
 * These aliases make the code more readable when multiple concepts
 * need to be satisfied simultaneously.
 */
namespace greeting::concepts {

/**
 * @brief A valid domain string that can be used as a person identifier
 */
template<typename T>
concept ValidPersonString = StringLike<T> && 
    requires(T t) { 
        requires t.size() > 0 && t.size() <= 100; 
    };

/**
 * @brief A valid domain string that can be used as a greeting message
 */
template<typename T>
concept ValidMessageString = StringLike<T> && 
    requires(T t) { 
        requires t.size() > 0 && t.size() <= 500; 
    };

/**
 * @brief Concept for complete greeting processor that handles the full workflow
 */
template<typename P, typename PersonType, typename MessageType, typename OutputType>
concept CompleteGreetingProcessor = 
    GreetingSystem<P, PersonType, OutputType> &&
    requires(P& processor) {
        requires PersonIdentifier<PersonType>;
        requires GreetingMessageType<MessageType>;
        requires OutputDestination<OutputType>;
    };

} // namespace greeting::concepts
