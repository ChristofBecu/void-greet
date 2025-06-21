#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "greeting/config_aware_type_system.hpp"
#include "greeting/types.hpp"

using namespace greeting;
using namespace greeting::types;

// ============================================================================
// Configuration-Aware Type System Tests
// ============================================================================

TEST_CASE("Configuration-aware type traits", "[config][types][traits]") {
    
    SECTION("Type trait validation") {
        // Test has_config_aware_validation trait
        REQUIRE(has_config_aware_validation<PersonName>::value);
        REQUIRE(has_config_aware_validation<GreetingMessage>::value);
        REQUIRE_FALSE(has_config_aware_validation<int>::value);
        REQUIRE_FALSE(has_config_aware_validation<std::string>::value);
        
        // Test has_internal_tag trait
        REQUIRE(has_internal_tag_v<PersonName>);
        REQUIRE(has_internal_tag_v<GreetingMessage>);
        REQUIRE_FALSE(has_internal_tag_v<int>);
        REQUIRE_FALSE(has_internal_tag_v<std::string>);
        
        // Test supports_validation_result trait
        REQUIRE(supports_validation_result_v<PersonName>);
        REQUIRE(supports_validation_result_v<GreetingMessage>);
        REQUIRE_FALSE(supports_validation_result_v<int>);
        
        // Test supports_error_propagation trait
        REQUIRE(supports_error_propagation<PersonName>::value);
        REQUIRE(supports_error_propagation<GreetingMessage>::value);
        REQUIRE_FALSE(supports_error_propagation<int>::value);
    }
    
    SECTION("Configuration detection") {
        // Test compile-time configuration detection
        bool is_debug = is_debug_configuration();
        bool is_release = is_release_configuration();
        
        // One should be true, the other false
        REQUIRE(((is_debug && !is_release) || (!is_debug && is_release)));
        
        #ifdef HELLOWORLD_DEBUG_BUILD
        REQUIRE(is_debug);
        REQUIRE_FALSE(is_release);
        #else
        REQUIRE_FALSE(is_debug);
        REQUIRE(is_release);
        #endif
    }
    
    SECTION("Zero-overhead assertions") {
        // These should compile without issues if types meet zero-overhead requirements
        if constexpr (is_debug_configuration()) {
            performance::assert_zero_overhead<PersonName>();
            performance::assert_zero_overhead<GreetingMessage>();
        }
        
        // Test optimization hints
        bool should_optimize_person_name = performance::should_optimize_for_size<PersonName>();
        bool should_optimize_greeting_message = performance::should_optimize_for_size<GreetingMessage>();
        
        // In release builds, might optimize larger types
        if constexpr (is_release_configuration()) {
            // Both types are likely small enough to not need size optimization
            REQUIRE_FALSE(should_optimize_person_name);
            REQUIRE_FALSE(should_optimize_greeting_message);
        } else {
            // Debug builds don't optimize for size
            REQUIRE_FALSE(should_optimize_person_name);
            REQUIRE_FALSE(should_optimize_greeting_message);
        }
    }
}

#ifdef __cpp_concepts

TEST_CASE("Configuration-aware concepts", "[config][types][concepts]") {
    
    SECTION("InternallyConstructible concept") {
        static_assert(InternallyConstructible<PersonName>);
        static_assert(InternallyConstructible<GreetingMessage>);
        static_assert(!InternallyConstructible<int>);
        static_assert(!InternallyConstructible<std::string>);
    }
    
    SECTION("ConfigAwareValidatable concept") {
        static_assert(ConfigAwareValidatable<PersonName>);
        static_assert(ConfigAwareValidatable<GreetingMessage>);
        static_assert(!ConfigAwareValidatable<int>);
        static_assert(!ConfigAwareValidatable<std::string>);
    }
    
    SECTION("ErrorPropagatable concept") {
        static_assert(ErrorPropagatable<PersonName>);
        static_assert(ErrorPropagatable<GreetingMessage>);
        static_assert(!ErrorPropagatable<int>);
        static_assert(!ErrorPropagatable<std::string>);
    }
    
    SECTION("StringLike concept") {
        static_assert(StringLike<std::string_view>);
        static_assert(StringLike<std::string>);
        static_assert(StringLike<const char*>);
        static_assert(StringLike<char*>);
        static_assert(!StringLike<int>);
        static_assert(!StringLike<PersonName>);
    }
    
    SECTION("ZeroOverheadValidatable concept") {
        static_assert(ZeroOverheadValidatable<PersonName>);
        static_assert(ZeroOverheadValidatable<GreetingMessage>);
        static_assert(!ZeroOverheadValidatable<int>);
    }
}

#endif // __cpp_concepts

TEST_CASE("Error handling utilities", "[config][types][error_handling]") {
    
    SECTION("Error context enhancement") {
        auto original_error = GreetingError::InvalidName;
        auto enhanced_error = error_handling::enhance_error_context(original_error, "test context");
        
        // For now, enhancement just returns the original error
        REQUIRE(enhanced_error == original_error);
    }
    
    SECTION("Error propagation") {
        auto original_error = GreetingError::EmptyName;
        auto propagated_error = error_handling::propagate_error<PersonName>(original_error, "test operation");
        
        // For now, propagation just returns the original error
        REQUIRE(propagated_error == original_error);
    }
}

TEST_CASE("Type system utilities", "[config][types][utilities]") {
    
    SECTION("Validatable type detection") {
        REQUIRE(is_validatable_v<PersonName>);
        REQUIRE(is_validatable_v<GreetingMessage>);
        REQUIRE_FALSE(is_validatable_v<int>);
        REQUIRE_FALSE(is_validatable_v<std::string>);
    }
}
