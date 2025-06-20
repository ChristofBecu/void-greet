#pragma once

#include "error_codes.hpp"
#include <variant>
#include <system_error>

namespace greeting {

/**
 * @brief Simple Expected-like type for error handling
 * 
 * A variant-based implementation similar to std::expected for C++23 compatibility.
 * Contains either a value of type T or an error of type E.
 */
template<typename T, typename E>
class Expected {
private:
    std::variant<T, E> data_;

public:
    /**
     * @brief Construct with a value
     * @param value The value to store
     */
    Expected(T value) : data_(std::move(value)) {}
    
    /**
     * @brief Construct with an error
     * @param error The error to store
     */
    Expected(E error) : data_(std::move(error)) {}
    
    /**
     * @brief Check if the Expected contains a value
     * @return true if contains value, false if contains error
     */
    [[nodiscard]] bool has_value() const noexcept {
        return std::holds_alternative<T>(data_);
    }
    
    /**
     * @brief Check if the Expected contains a value (bool conversion)
     * @return true if contains value, false if contains error
     */
    [[nodiscard]] explicit operator bool() const noexcept {
        return has_value();
    }
    
    /**
     * @brief Get the contained value
     * @return Reference to the contained value
     * @throws std::bad_variant_access if contains error
     */
    [[nodiscard]] T& value() & {
        return std::get<T>(data_);
    }
    
    /**
     * @brief Get the contained value (const)
     * @return Const reference to the contained value
     * @throws std::bad_variant_access if contains error
     */
    [[nodiscard]] const T& value() const & {
        return std::get<T>(data_);
    }
    
    /**
     * @brief Get the contained value (rvalue)
     * @return Rvalue reference to the contained value
     * @throws std::bad_variant_access if contains error
     */
    [[nodiscard]] T&& value() && {
        return std::get<T>(std::move(data_));
    }
    
    /**
     * @brief Get the contained error
     * @return Reference to the contained error
     * @throws std::bad_variant_access if contains value
     */
    [[nodiscard]] E& error() & {
        return std::get<E>(data_);
    }
    
    /**
     * @brief Get the contained error (const)
     * @return Const reference to the contained error
     * @throws std::bad_variant_access if contains value
     */
    [[nodiscard]] const E& error() const & {
        return std::get<E>(data_);
    }
    
    /**
     * @brief Dereference operator for value access
     * @return Reference to the contained value
     */
    [[nodiscard]] T& operator*() & {
        return value();
    }
    
    /**
     * @brief Dereference operator for value access (const)
     * @return Const reference to the contained value
     */
    [[nodiscard]] const T& operator*() const & {
        return value();
    }
    
    /**
     * @brief Arrow operator for value access
     * @return Pointer to the contained value
     */
    [[nodiscard]] T* operator->() {
        return &value();
    }
    
    /**
     * @brief Arrow operator for value access (const)
     * @return Const pointer to the contained value
     */
    [[nodiscard]] const T* operator->() const {
        return &value();
    }
};

/**
 * @brief Helper function to create an Expected with an error
 * @param error The error value
 * @return Expected containing the error
 */
template<typename E>
[[nodiscard]] auto unexpected(E error) {
    return error;
}

// Template aliases for clean Expected usage
template<typename T>
using Result = Expected<T, GreetingError>;

template<typename T>
using ResultWithCode = Expected<T, std::error_code>;

} // namespace greeting
