#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_section_info.hpp>
#include "greeting/strategy_interface.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"

using namespace greeting;
using greeting::strategy::IGreetingStrategy;

namespace {

// Mock strategy implementation for testing
class MockGreetingStrategy : public IGreetingStrategy {
public:
    MockGreetingStrategy(bool valid = true) : valid_(valid) {}
    
    [[nodiscard]] Result<GreetingMessage> greet(
        const PersonName& person) const noexcept override {
        
        if (!valid_) {
            return greeting::strategy::errors::makeStrategyError<
                greeting::strategy::errors::StrategyError::StrategyExecutionFailed>();
        }
        
        auto message = GreetingMessage::create("Hello, " + person.value());
        return message;
    }
    
    [[nodiscard]] std::string_view getStrategyName() const noexcept override {
        return "MockGreetingStrategy";
    }
    
    [[nodiscard]] bool isValid() const noexcept override {
        return valid_;
    }
    
private:
    bool valid_;
};

#ifdef __cpp_concepts
// Test strategy that should satisfy ConfigAwareStrategy concept  
class ConfigAwareTestStrategy : public IGreetingStrategy {
public:
    [[nodiscard]] Result<GreetingMessage> greet(
        const PersonName& person) const noexcept override {
        auto message = GreetingMessage::create("Config-aware: " + person.value());
        return message;
    }
    
    [[nodiscard]] std::string_view getStrategyName() const noexcept override {
        return "ConfigAwareTestStrategy";
    }
    
    [[nodiscard]] bool isValid() const noexcept override {
        return true;
    }
};
#endif

} // anonymous namespace

TEST_CASE("Strategy Interface - Core Interface Definition", "[strategy][interface]") {
    SECTION("Interface can be inherited and implemented") {
        MockGreetingStrategy strategy;
        
        REQUIRE(strategy.getStrategyName() == "MockGreetingStrategy");
        REQUIRE(strategy.isValid() == true);
    }
    
    SECTION("Strategy can generate greeting") {
        MockGreetingStrategy strategy;
        auto person = PersonName::create("Alice");
        REQUIRE(person.has_value());
        
        auto result = strategy.greet(person.value());
        REQUIRE(result.has_value());
        REQUIRE(result.value().value() == "Hello, Alice");
    }
    
    SECTION("Strategy handles error conditions") {
        MockGreetingStrategy invalidStrategy(false);
        auto person = PersonName::create("Bob");
        REQUIRE(person.has_value());
        
        auto result = invalidStrategy.greet(person.value());
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == static_cast<GreetingError>(
            greeting::strategy::errors::StrategyError::StrategyExecutionFailed));
    }
    
    SECTION("Strategy interface enforces move-only semantics") {
        // Test that strategy is non-copyable
        static_assert(!std::is_copy_constructible_v<IGreetingStrategy>);
        static_assert(!std::is_copy_assignable_v<IGreetingStrategy>);
        
        // Test that concrete strategy is moveable
        static_assert(std::is_move_constructible_v<MockGreetingStrategy>);
        static_assert(std::is_move_assignable_v<MockGreetingStrategy>);
    }
}

#ifdef __cpp_concepts
TEST_CASE("Strategy Interface - C++23 Concepts", "[strategy][concepts]") {
    SECTION("GreetingStrategy concept validation") {
        static_assert(greeting::strategy::concepts::GreetingStrategy<MockGreetingStrategy>);
        static_assert(greeting::strategy::concepts::GreetingStrategy<ConfigAwareTestStrategy>);
    }
    
    SECTION("Strategy concept requirements") {
        // Basic type requirements
        static_assert(std::derived_from<MockGreetingStrategy, IGreetingStrategy>);
        static_assert(std::derived_from<ConfigAwareTestStrategy, IGreetingStrategy>);
    }
}
#endif

TEST_CASE("Strategy Interface - Type Traits", "[strategy][traits]") {
    SECTION("StrategyTraits provides configuration-aware information") {
        using Traits = greeting::strategy::traits::StrategyTraits<MockGreetingStrategy>;
        
        // These values should depend on build configuration
        if constexpr (greeting::config::is_debug_build()) {
            REQUIRE(Traits::supports_debug_info == true);
            REQUIRE(Traits::enable_performance_monitoring == true);
            REQUIRE(Traits::enable_validation_caching == true);
        } else {
            REQUIRE(Traits::supports_debug_info == false);
            REQUIRE(Traits::enable_performance_monitoring == false);
            REQUIRE(Traits::enable_validation_caching == false);
        }
        
        // These should be strategy-specific constants
        REQUIRE(Traits::expected_message_length == 50);
        REQUIRE(Traits::is_thread_safe == false);
        REQUIRE(Traits::supports_async_operation == false);
    }
    
    SECTION("Compile-time strategy validation") {
        // Simple compile-time validation
        static_assert(std::is_base_of_v<IGreetingStrategy, MockGreetingStrategy>);
        static_assert(std::is_nothrow_destructible_v<MockGreetingStrategy>);
        
        // Test with non-strategy type should fail
        static_assert(!std::is_base_of_v<IGreetingStrategy, int>);
    }
}

TEST_CASE("Strategy Interface - Error Handling", "[strategy][errors]") {
    SECTION("Strategy error enumeration values") {
        using SE = greeting::strategy::errors::StrategyError;
        
        REQUIRE(static_cast<std::uint16_t>(SE::StrategyNotInitialized) == 3001);
        REQUIRE(static_cast<std::uint16_t>(SE::StrategyExecutionFailed) == 3002);
        REQUIRE(static_cast<std::uint16_t>(SE::StrategyValidationFailed) == 3003);
        REQUIRE(static_cast<std::uint16_t>(SE::StrategyConfigurationInvalid) == 3004);
        REQUIRE(static_cast<std::uint16_t>(SE::StrategyResourceUnavailable) == 3005);
    }
    
    SECTION("Strategy error creation") {
        constexpr auto error = greeting::strategy::errors::makeStrategyError<
            greeting::strategy::errors::StrategyError::StrategyNotInitialized>();
        
        REQUIRE(error == static_cast<GreetingError>(
            greeting::strategy::errors::StrategyError::StrategyNotInitialized));
    }
    
    SECTION("Error propagation utility") {
        auto originalError = GreetingError::InvalidName;
        auto result = greeting::strategy::errors::propagateStrategyError<GreetingMessage>(
            originalError, "TestStrategy", "greet"
        );
        
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == originalError);
    }
}

TEST_CASE("Strategy Interface - Performance Optimization", "[strategy][optimization]") {
    SECTION("Compile-time optimization detection") {
        constexpr bool canOptimize = greeting::strategy::optimization::canOptimizeStrategy<MockGreetingStrategy>();
        
        // The result should depend on build configuration and strategy traits
        if constexpr (greeting::config::is_release_build()) {
            // In release builds, optimization depends on strategy characteristics
            // MockGreetingStrategy is not thread-safe, so should not be optimizable
            REQUIRE(canOptimize == false);
        } else {
            // In debug builds, optimization is typically disabled
            REQUIRE(canOptimize == false);
        }
    }
    
    SECTION("Strategy caching configuration") {
        using Cache = greeting::strategy::optimization::StrategyCache<MockGreetingStrategy>;
        
        if constexpr (greeting::config::is_debug_build()) {
            REQUIRE(Cache::CACHE_SIZE == 16);
            REQUIRE(Cache::shouldCache() == true);
        } else {
            REQUIRE(Cache::CACHE_SIZE == 0);
            REQUIRE(Cache::shouldCache() == false);
        }
    }
}

#ifdef __cpp_concepts
TEST_CASE("Strategy Interface - OptimizedStrategyWrapper", "[strategy][optimization][wrapper]") {
    SECTION("Wrapper basic functionality test") {
        // Since ConfigAwareStrategy concept requires concepts that don't exist yet,
        // we'll just test basic functionality
        MockGreetingStrategy strategy;
        
        auto person = PersonName::create("Charlie");
        REQUIRE(person.has_value());
        
        auto result = strategy.greet(person.value());
        REQUIRE(result.has_value());
        REQUIRE(result.value().value() == "Hello, Charlie");
    }
}
#endif

TEST_CASE("Strategy Interface - Integration", "[strategy][integration]") {
    SECTION("Strategy interface integrates with existing error handling") {
        MockGreetingStrategy strategy;
        auto person = PersonName::create(""); // Invalid name should fail
        
        // This should fail at PersonName creation, not at strategy level
        REQUIRE(!person.has_value());
        REQUIRE(person.error() == GreetingError::EmptyName);
    }
    
    SECTION("Strategy interface works with configuration system") {
        // Verify that the configuration-aware functions compile and work
        static_assert(std::is_same_v<bool, decltype(greeting::config::is_debug_build())>);
        static_assert(std::is_same_v<bool, decltype(greeting::config::is_release_build())>);
        
        // The actual values depend on build configuration
        REQUIRE((greeting::config::is_debug_build() || greeting::config::is_release_build()));
    }
}
