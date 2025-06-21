#pragma once

#include "expected.hpp"
#include "config_aware_validation.hpp"
#include <string>
#include <string_view>

namespace greeting {

/**
 * @brief Strong type alias for person names to prevent mixing with other strings
 * 
 * This type ensures type safety by preventing accidental assignment of 
 * arbitrary strings where a person name is expected.
 */
class PersonName {
public:
    /**
     * @brief Internal construction tag - for validation framework use only
     * DO NOT USE DIRECTLY - Use PersonName::create() instead
     */
    struct InternalTag {};

    /**
     * @brief Create a PersonName from a string_view with validation
     * @param name The person's name
     * @return Result<PersonName> containing the PersonName or an error
     */
    [[nodiscard]] static Result<PersonName> create(std::string_view name) noexcept {
        // Use configuration-aware validation
        auto validation_result = greeting::validation::validate_person_name<PersonName>(name);
        if (!validation_result.has_value()) {
            return validation_result.error();
        }
        
        return validation_result.value();
    }

    /**
     * @brief Get the underlying string value
     * @return const reference to the stored name
     */
    [[nodiscard]] const std::string& value() const noexcept {
        return value_;
    }
    
    /**
     * @brief Get string_view of the name
     * @return string_view of the stored name
     */
    [[nodiscard]] std::string_view view() const noexcept {
        return value_;
    }
    
    /**
     * @brief Check if the name is empty
     * @return true if name is empty, false otherwise
     */
    [[nodiscard]] bool empty() const noexcept {
        return value_.empty();
    }
    
    /**
     * @brief Get the length of the name
     * @return length of the stored name
     */
    [[nodiscard]] std::size_t length() const noexcept {
        return value_.length();
    }
    
    /**
     * @brief Internal constructor - for validation framework use only
     * DO NOT USE DIRECTLY - Use PersonName::create() instead
     * @param name The validated person's name
     * @param tag Internal construction tag
     */
    explicit PersonName(std::string name, InternalTag) : value_(std::move(name)) {}

    // Comparison operators
    [[nodiscard]] bool operator==(const PersonName& other) const noexcept {
        return value_ == other.value_;
    }
    
    [[nodiscard]] bool operator!=(const PersonName& other) const noexcept {
        return !(*this == other);
    }
    
    [[nodiscard]] bool operator<(const PersonName& other) const noexcept {
        return value_ < other.value_;
    }

private:
    std::string value_;
};

/**
 * @brief Convenience function to create PersonName with validation
 * @param name The person's name
 * @return Result<PersonName> containing the PersonName or an error
 */
[[nodiscard]] inline Result<PersonName> makePersonName(std::string_view name) {
    return PersonName::create(name);
}

} // namespace greeting
