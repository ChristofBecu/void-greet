#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "greeting/config_strategy_integration.hpp"
#include "greeting/strategy_factory_config.hpp"
#include "greeting/strategy_configuration_manager.hpp"
#include "greeting/strategy_context.hpp"
#include "greeting/person_name.hpp"
#include "greeting/console_strategy.hpp"
#include "greeting/formal_strategy.hpp"
#include "greeting/casual_strategy.hpp"
#include "config/build_config.hpp"

using namespace greeting;
using namespace greeting::strategy;
using namespace greeting::strategy::config;

// ============================================================================
// Configuration Integration Core Tests
// ============================================================================

TEST_CASE("Configuration-aware strategy integration", "[integration][config][strategy]") {
    
    SECTION("Strategy adapters can be created for different configurations") {
        auto base_strategy = greeting::strategy::createConsoleStrategy();
        REQUIRE(base_strategy != nullptr);
        
        auto config_aware_strategy = create_config_aware_strategy(std::move(base_strategy));
        REQUIRE(config_aware_strategy != nullptr);
        REQUIRE(config_aware_strategy->isValid());
    }
    
    SECTION("Configuration-aware validation works correctly") {
        auto person = PersonName::create("John Doe");
        REQUIRE(person.has_value());
        
        auto base_strategy = greeting::strategy::createConsoleStrategy();
        auto config_aware_strategy = create_config_aware_strategy(std::move(base_strategy));
        
        auto result = config_aware_strategy->greet(person.value());
        REQUIRE(result.has_value());
        REQUIRE_FALSE(result.value().value().empty());
    }
    
    SECTION("Optimal strategy selection adapts to build configuration") {
        auto optimal_strategy = get_optimal_strategy_for_config();
        REQUIRE_FALSE(optimal_strategy.empty());
        
        // In debug builds, should prefer formal strategy
        // In release builds, should prefer casual strategy
        // In profile builds, should prefer console strategy
        if constexpr (::greeting::config::is_debug_build()) {
            REQUIRE(optimal_strategy == "formal");
        } else if constexpr (::greeting::config::is_profile_build()) {
            REQUIRE(optimal_strategy == "console");
        } else {
            REQUIRE(optimal_strategy == "casual");
        }
    }
}

// ============================================================================
// Build Configuration Specific Tests
// ============================================================================

/*
TEST_CASE("Debug configuration strategy behavior", "[integration][config][debug]") {
    
    SECTION("Debug adapter provides enhanced diagnostics") {
        if constexpr (::greeting::config::is_debug_build()) {
            auto base_strategy = greeting::strategy::createFormalStrategy();
            auto debug_adapter = std::make_unique<DebugStrategyAdapter<decltype(base_strategy)::element_type>>(std::move(base_strategy));
            
            REQUIRE(debug_adapter->enable_debug_diagnostics());
            REQUIRE(debug_adapter->isValid());
            
            auto person = PersonName::create("Dr. Jane Smith");
            REQUIRE(person.has_value());
            
            auto result = debug_adapter->greet(person.value());
            REQUIRE(result.has_value());
        }
    }
    
    SECTION("Debug builds use comprehensive validation") {
        if constexpr (::greeting::config::is_debug_build()) {
            auto factory = std::make_unique<DebugConfigStrategyFactory>();
            
            REQUIRE(factory->get_optimal_strategy() == "formal");
            REQUIRE(factory->supports_strategy("console"));
            REQUIRE(factory->supports_strategy("formal"));
            REQUIRE(factory->supports_strategy("casual"));
            
            auto available = factory->get_available_strategies();
            REQUIRE(available.size() == 3);
        }
    }
}

TEST_CASE("Release configuration strategy behavior", "[integration][config][release]") {
    
    SECTION("Release adapter provides zero-overhead optimization") {
        if constexpr (!::greeting::config::is_debug_build() && !::greeting::config::is_profile_build()) {
            auto base_strategy = greeting::strategy::createCasualStrategy();
            auto release_adapter = std::make_unique<ReleaseStrategyAdapter<decltype(base_strategy)::element_type>>(std::move(base_strategy));
            
            REQUIRE(release_adapter->enable_zero_overhead());
            REQUIRE(release_adapter->isValid());
            
            auto person = PersonName::create("Alex Johnson");
            REQUIRE(person.has_value());
            
            auto result = release_adapter->greet(person.value());
            REQUIRE(result.has_value());
        }
    }
    
    SECTION("Release builds optimize for performance") {
        if constexpr (!::greeting::config::is_debug_build() && !::greeting::config::is_profile_build()) {
            auto factory = std::make_unique<ReleaseConfigStrategyFactory>();
            
            REQUIRE(factory->get_optimal_strategy() == "casual");
            REQUIRE(factory->supports_strategy("console"));
            REQUIRE(factory->supports_strategy("formal"));
            REQUIRE(factory->supports_strategy("casual"));
            
            auto available = factory->get_available_strategies();
            REQUIRE(available.size() == 3);
        }
    }
}
*/

// ============================================================================
// Strategy Context Integration Tests
// ============================================================================

TEST_CASE("Strategy context configuration integration", "[integration][context][config]") {
    
    SECTION("Strategy context can be created with configuration awareness") {
        auto context_result = StrategyConfigurationManager::create_optimized_context();
        REQUIRE(context_result.has_value());
        
        // Test that we can get optimal strategy
        auto optimal_name = get_optimal_strategy_for_config();
        REQUIRE_FALSE(optimal_name.empty());
    }
    
    SECTION("Configuration manager registers strategies correctly") {
        auto context = StrategyContext{};
        
        auto registration_result = StrategyConfigurationManager::register_config_aware_strategies(context);
        REQUIRE(registration_result.has_value());
        
        // Test that strategies are registered and accessible
        auto person = PersonName::create("Test User");
        REQUIRE(person.has_value());
        
        // Try to execute greeting with different strategies
        auto console_result = context.greetWith(person.value(), StrategyType::Console);
        REQUIRE(console_result.has_value());
        
        auto formal_result = context.greetWith(person.value(), StrategyType::Formal);
        REQUIRE(formal_result.has_value());
        
        auto casual_result = context.greetWith(person.value(), StrategyType::Casual);
        REQUIRE(casual_result.has_value());
    }
    
    SECTION("Configuration metrics are correctly collected") {
        auto metrics = StrategyConfigurationManager::get_configuration_metrics();
        
        REQUIRE_FALSE(metrics.build_type.empty());
        REQUIRE_FALSE(metrics.optimal_strategy.empty());
        
        if constexpr (::greeting::config::is_debug_build()) {
            REQUIRE(metrics.build_type == "debug");
            REQUIRE(metrics.optimal_strategy == "formal");
        } else if constexpr (::greeting::config::is_profile_build()) {
            REQUIRE(metrics.build_type == "profile");
            REQUIRE(metrics.optimal_strategy == "console");
        } else {
            REQUIRE(metrics.build_type == "release");
            REQUIRE(metrics.optimal_strategy == "casual");
        }
    }
}
