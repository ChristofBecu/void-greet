#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "greeting/strategy_context.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"
#include "greeting/error_codes.hpp"
#include "config/build_config.hpp"

using namespace greeting;
using namespace greeting::strategy;
using Catch::Matchers::ContainsSubstring;

// ============================================================================
// Mock Strategy for Testing
// ============================================================================

class MockGreetingStrategy : public IGreetingStrategy {
private:
    std::string prefix_;
    bool should_fail_;
    
public:
    explicit MockGreetingStrategy(std::string prefix, bool should_fail = false) 
        : prefix_(std::move(prefix)), should_fail_(should_fail) {}
    
    Result<GreetingMessage> greet(const PersonName& person) const noexcept override {
        if (should_fail_) {
            return GreetingError::StrategyCreationFailed;
        }
        
        auto message_text = prefix_ + " " + person.value();
        return GreetingMessage::create(message_text);
    }
    
    std::string_view getStrategyName() const noexcept override {
        return prefix_;
    }
    
    bool isValid() const noexcept override {
        return !should_fail_;
    }
};

// ============================================================================
// Basic Functionality Tests
// ============================================================================

SCENARIO("StrategyContext basic functionality", "[StrategyContext][basic]") {
    
    GIVEN("a default strategy context") {
        auto context = createDefaultContext();
        REQUIRE(context != nullptr);
        REQUIRE(context->isValid());
        
        WHEN("greeting with a simple person name") {
            auto person = PersonName::create("Alice").value();
            auto result = context->greet(person);
            
            THEN("greeting is generated successfully") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Alice"));
                REQUIRE_FALSE(result.value().empty());
            }
        }
        
        WHEN("checking available strategies") {
            auto available = context->getAvailableStrategies();
            
            THEN("default strategies are available") {
                REQUIRE_FALSE(available.empty());
                REQUIRE(context->hasStrategy(StrategyType::Console));
                
                // Other strategies depend on implementation state
                INFO("Available strategies: " << available.size());
            }
        }
        
        WHEN("using explicit strategy selection") {
            auto person = PersonName::create("Dr. Smith").value();
            auto result = context->greetWith(person, StrategyType::Console);
            
            THEN("strategy is applied correctly") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Dr. Smith"));
            }
        }
    }
}

SCENARIO("StrategyContext configuration management", "[StrategyContext][config]") {
    
    GIVEN("a context with custom configuration") {
        ContextConfig config{};
        config.default_strategy = StrategyType::Console;
        config.enable_fallback = true;
        config.enable_caching = greeting::config::is_debug_build();
        
        auto context = createOptimizedContext(config);
        REQUIRE(context != nullptr);
        
        WHEN("checking configuration") {
            const auto& current_config = context->getConfig();
            
            THEN("configuration is applied correctly") {
                REQUIRE(current_config.default_strategy == StrategyType::Console);
                REQUIRE(current_config.enable_fallback == true);
                
                if constexpr (greeting::config::is_debug_build()) {
                    REQUIRE(current_config.enable_caching == true);
                } else {
                    // Release builds may optimize caching away
                    INFO("Release build: caching=" << current_config.enable_caching);
                }
            }
        }
        
        WHEN("updating configuration") {
            ContextConfig new_config = config;
            new_config.enable_fallback = false;
            
            bool update_success = context->updateConfig(new_config);
            
            THEN("configuration is updated successfully") {
                REQUIRE(update_success == true);
                REQUIRE(context->getConfig().enable_fallback == false);
            }
        }
        
        WHEN("updating with invalid configuration") {
            ContextConfig invalid_config{};
            invalid_config.default_strategy = StrategyType::Auto;
            invalid_config.enable_fallback = false; // Invalid: Auto requires fallback
            
            bool update_success = context->updateConfig(invalid_config);
            
            THEN("configuration update is rejected") {
                REQUIRE(update_success == false);
                REQUIRE(context->getConfig().enable_fallback == true); // Unchanged
            }
        }
    }
}

SCENARIO("StrategyContext strategy management", "[StrategyContext][strategies]") {
    
    GIVEN("a strategy context") {
        auto context = createDefaultContext();
        REQUIRE(context != nullptr);
        
        WHEN("registering a custom strategy") {
            auto mock_strategy = std::make_unique<MockGreetingStrategy>("MockGreeting");
            bool registration_success = context->registerStrategy(
                StrategyType::Casual, 
                std::move(mock_strategy)
            );
            
            THEN("strategy is registered successfully") {
                REQUIRE(registration_success == true);
                REQUIRE(context->hasStrategy(StrategyType::Casual) == true);
                
                auto person = PersonName::create("Bob").value();
                auto result = context->greetWith(person, StrategyType::Casual);
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("MockGreeting"));
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Bob"));
            }
        }
        
        WHEN("registering an invalid strategy") {
            auto failing_strategy = std::make_unique<MockGreetingStrategy>("Failing", true);
            
            // Check if Formal strategy is already available
            bool was_available = context->hasStrategy(StrategyType::Formal);
            
            bool registration_success = context->registerStrategy(
                StrategyType::Formal, 
                std::move(failing_strategy)
            );
            
            THEN("strategy registration is rejected") {
                REQUIRE(registration_success == false);
                // Strategy availability should remain the same as before registration attempt
                REQUIRE(context->hasStrategy(StrategyType::Formal) == was_available);
            }
        }
        
        WHEN("setting default strategy") {
            // Ensure a strategy is available
            if (context->hasStrategy(StrategyType::Console)) {
                bool set_success = context->setDefaultStrategy(StrategyType::Console);
                
                THEN("default strategy is updated") {
                    REQUIRE(set_success == true);
                    REQUIRE(context->getConfig().default_strategy == StrategyType::Console);
                }
            }
        }
    }
}

SCENARIO("StrategyContext validation and error handling", "[StrategyContext][validation]") {
    
    GIVEN("a strategy context") {
        auto context = createDefaultContext();
        REQUIRE(context != nullptr);
        
        WHEN("greeting with validation") {
            auto result = context->greetWithValidation("Valid Name");
            
            THEN("validation and greeting succeed") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Valid Name"));
            }
        }
        
        WHEN("greeting with invalid input") {
            auto result = context->greetWithValidation(""); // Empty name
            
            THEN("validation error is returned") {
                REQUIRE_FALSE(result.has_value());
                REQUIRE(result.error() == GreetingError::EmptyName);
            }
        }
        
        WHEN("using unavailable strategy") {
            auto person = PersonName::create("Test").value();
            
            // Use a strategy type that might not be available
            auto result = context->greetWith(person, StrategyType::Formal);
            
            THEN("appropriate error handling occurs") {
                if (context->hasStrategy(StrategyType::Formal)) {
                    REQUIRE(result.has_value());
                } else {
                    // Should either return error or use fallback
                    if (context->getConfig().enable_fallback) {
                        // Fallback should work
                        INFO("Fallback mechanism used");
                    } else {
                        REQUIRE_FALSE(result.has_value());
                    }
                }
            }
        }
    }
}

SCENARIO("StrategyContext automatic strategy selection", "[StrategyContext][auto]") {
    
    GIVEN("a context with multiple strategies") {
        auto context = createDefaultContext();
        REQUIRE(context != nullptr);
        
        WHEN("greeting with formal name patterns") {
            std::vector<std::string> formal_names = {
                "Dr. Johnson", "Prof. Smith", "Mr. Anderson", "Ms. Davis"
            };
            
            for (const auto& name : formal_names) {
                auto person_result = PersonName::create(name);
                if (person_result.has_value()) {
                    auto result = context->greetWithValidation(name, StrategyType::Auto);
                    
                    THEN("formal patterns are handled appropriately") {
                        REQUIRE(result.has_value());
                        REQUIRE_THAT(result.value().value(), ContainsSubstring(name));
                        INFO("Formal name: " << name);
                    }
                }
            }
        }
        
        WHEN("greeting with casual name patterns") {
            std::vector<std::string> casual_names = {
                "Bob", "Jo", "Alex", "Sam"
            };
            
            for (const auto& name : casual_names) {
                auto person_result = PersonName::create(name);
                if (person_result.has_value()) {
                    auto result = context->greetWithValidation(name, StrategyType::Auto);
                    
                    THEN("casual patterns are handled appropriately") {
                        REQUIRE(result.has_value());
                        REQUIRE_THAT(result.value().value(), ContainsSubstring(name));
                        INFO("Casual name: " << name);
                    }
                }
            }
        }
    }
}

SCENARIO("StrategyContext performance and diagnostics", "[StrategyContext][performance]") {
    
    GIVEN("a context with metrics enabled") {
        ContextConfig config{};
        config.enable_metrics = greeting::config::is_debug_build();
        
        auto context = createOptimizedContext(config);
        REQUIRE(context != nullptr);
        
        WHEN("performing multiple greetings") {
            std::vector<std::string> names = {"Alice", "Bob", "Charlie"};
            
            for (const auto& name : names) {
                auto person_result = PersonName::create(name);
                if (person_result.has_value()) {
                    auto result = context->greet(person_result.value());
                    REQUIRE(result.has_value());
                }
            }
            
            THEN("metrics are tracked correctly") {
                if constexpr (greeting::config::is_debug_build()) {
                    auto metrics = context->getPerformanceMetrics();
                    REQUIRE_THAT(metrics, ContainsSubstring("Performance Metrics"));
                    
                    auto last_execution = context->getLastExecution();
                    if (last_execution.has_value()) {
                        REQUIRE_FALSE(last_execution->strategy_name.empty());
                        INFO("Last strategy: " << last_execution->strategy_name);
                    }
                } else {
                    INFO("Performance metrics available only in debug builds");
                }
            }
        }
        
        WHEN("clearing cache") {
            context->clearCache();
            
            THEN("cache clearing completes without error") {
                // Cache clearing should not affect basic functionality
                auto person = PersonName::create("Test").value();
                auto result = context->greet(person);
                REQUIRE(result.has_value());
            }
        }
    }
}

SCENARIO("StrategyContext utility functions", "[StrategyContext][utilities]") {
    
    GIVEN("strategy type utilities") {
        
        WHEN("converting strategy types to strings") {
            
            THEN("conversion works correctly") {
                REQUIRE(toString(StrategyType::Console) == "Console");
                REQUIRE(toString(StrategyType::Formal) == "Formal");
                REQUIRE(toString(StrategyType::Casual) == "Casual");
                REQUIRE(toString(StrategyType::Auto) == "Auto");
            }
        }
        
        WHEN("parsing strategy types from strings") {
            
            THEN("parsing works correctly") {
                REQUIRE(parseStrategyType("Console") == StrategyType::Console);
                REQUIRE(parseStrategyType("Formal") == StrategyType::Formal);
                REQUIRE(parseStrategyType("Casual") == StrategyType::Casual);
                REQUIRE(parseStrategyType("Auto") == StrategyType::Auto);
                
                REQUIRE_FALSE(parseStrategyType("Invalid").has_value());
                REQUIRE_FALSE(parseStrategyType("").has_value());
            }
        }
    }
}

// ============================================================================
// Configuration-Aware Tests
// ============================================================================

SCENARIO("StrategyContext configuration-aware behavior", "[StrategyContext][config-aware]") {
    
    GIVEN("a strategy context") {
        auto context = createDefaultContext();
        REQUIRE(context != nullptr);
        
        WHEN("operating in different build configurations") {
            auto person = PersonName::create("Configuration Test").value();
            auto result = context->greet(person);
            
            THEN("behavior adapts to configuration") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Configuration Test"));
                
                if constexpr (greeting::config::is_debug_build()) {
                    // Debug builds may include additional context
                    auto metrics = context->getPerformanceMetrics();
                    REQUIRE_THAT(metrics, ContainsSubstring("Performance Metrics"));
                    INFO("Debug mode: metrics enabled");
                } else {
                    // Release builds focus on performance
                    INFO("Release mode: optimized execution");
                }
            }
        }
    }
}

// ============================================================================
// Edge Case and Error Scenarios
// ============================================================================

SCENARIO("StrategyContext edge cases", "[StrategyContext][edge-cases]") {
    
    GIVEN("a strategy context") {
        auto context = createDefaultContext();
        REQUIRE(context != nullptr);
        
        WHEN("using invalid strategy registration") {
            // Try to register with Auto type (should be rejected)
            auto mock_strategy = std::make_unique<MockGreetingStrategy>("Invalid");
            bool registration_success = context->registerStrategy(
                StrategyType::Auto, 
                std::move(mock_strategy)
            );
            
            THEN("registration is rejected") {
                REQUIRE(registration_success == false);
                REQUIRE_FALSE(context->hasStrategy(StrategyType::Auto));
            }
        }
        
        WHEN("setting invalid default strategy") {
            bool set_success = context->setDefaultStrategy(StrategyType::Auto);
            
            THEN("setting is rejected") {
                REQUIRE(set_success == false);
                REQUIRE(context->getConfig().default_strategy != StrategyType::Auto);
            }
        }
        
        WHEN("working with empty context configuration") {
            ContextConfig empty_config{};
            empty_config.default_strategy = StrategyType::Console;
            
            THEN("configuration is valid") {
                REQUIRE(empty_config.isValid());
            }
        }
    }
}

SCENARIO("StrategyContext context lifecycle", "[StrategyContext][lifecycle]") {
    
    GIVEN("strategy context creation and destruction") {
        
        WHEN("creating multiple contexts") {
            auto context1 = createDefaultContext();
            auto context2 = createOptimizedContext();
            
            THEN("both contexts are independent") {
                REQUIRE(context1 != nullptr);
                REQUIRE(context2 != nullptr);
                REQUIRE(context1->isValid());
                REQUIRE(context2->isValid());
                
                // They should have independent state
                auto person = PersonName::create("Independent Test").value();
                auto result1 = context1->greet(person);
                auto result2 = context2->greet(person);
                
                REQUIRE(result1.has_value());
                REQUIRE(result2.has_value());
            }
        }
        
        WHEN("moving contexts") {
            auto context1 = createDefaultContext();
            auto context2 = std::move(context1);
            
            THEN("move semantics work correctly") {
                REQUIRE(context2 != nullptr);
                REQUIRE(context2->isValid());
                
                auto person = PersonName::create("Move Test").value();
                auto result = context2->greet(person);
                REQUIRE(result.has_value());
            }
        }
    }
}
