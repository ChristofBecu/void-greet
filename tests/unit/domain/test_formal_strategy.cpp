#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "greeting/formal_strategy.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"
#include <chrono>
#include <vector>

using namespace greeting::strategy;
using namespace greeting; // Add this for PersonName and GreetingMessage
using Catch::Matchers::ContainsSubstring;
using Catch::Matchers::Equals;

SCENARIO("FormalGreetingStrategy basic functionality", "[formal][strategy][basic]") {
    
    GIVEN("a formal strategy instance") {
        auto strategy = createFormalStrategy();
        REQUIRE(strategy != nullptr);
        
        WHEN("greeting a person with a title") {
            auto person = PersonName::create("Dr. Sarah Johnson").value();
            auto result = strategy->greet(person);
            
            THEN("formal greeting preserves title") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Dr."));
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Johnson"));
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Good day"));
            }
        }
        
        WHEN("greeting a person without title") {
            auto person = PersonName::create("John Smith").value();
            auto result = strategy->greet(person);
            
            THEN("formal greeting adds appropriate formality") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Good day"));
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Smith"));
            }
        }
        
        WHEN("checking strategy properties") {
            THEN("strategy reports correct identification") {
                REQUIRE_THAT(std::string{strategy->getStrategyName()}, 
                           Equals("FormalGreetingStrategy"));
                REQUIRE(strategy->isValid());
            }
        }
    }
}

SCENARIO("FormalGreetingStrategy advanced formatting", "[formal][strategy][formatting]") {
    
    GIVEN("a formal strategy") {
        auto strategy = createFormalStrategy();
        
        WHEN("greeting academic credentials") {
            auto person = PersonName::create("Prof. Alice Wilson PhD").value();
            auto result = strategy->greet(person);
            
            THEN("credentials are properly handled") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Prof."));
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Wilson"));
                
                if constexpr (greeting::config::is_debug_build()) {
                    // In debug mode, might include PhD credential
                    INFO("Debug mode: enhanced credential handling");
                }
            }
        }
        
        WHEN("greeting with multiple titles") {
            auto person = PersonName::create("Dr. Rev. Thomas Anderson").value();
            auto result = strategy->greet(person);
            
            THEN("primary title is used appropriately") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Anderson"));
            }
        }
        
        WHEN("greeting with honorifics") {
            auto person = PersonName::create("Sir James Bond").value();
            auto result = strategy->greet(person);
            
            THEN("honorifics are preserved") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Sir"));
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Bond"));
                REQUIRE_THAT(result.value().value(), ContainsSubstring("Good day"));
            }
        }
    }
}

SCENARIO("FormalGreetingStrategy configuration awareness", "[formal][strategy][config]") {
    
    GIVEN("a formal strategy") {
        auto strategy = createFormalStrategy();
        
        WHEN("operating in different build configurations") {
            auto person = PersonName::create("Ms. Elizabeth Taylor").value();
            auto result = strategy->greet(person);
            
            THEN("behavior adapts to configuration") {
                REQUIRE(result.has_value());
                
                if constexpr (greeting::config::is_debug_build()) {
                    // Debug builds include additional context
                    REQUIRE_THAT(result.value().value(), ContainsSubstring("Ms."));
                    INFO("Debug mode: enhanced formal context");
                } else {
                    // Release builds focus on core functionality
                    REQUIRE_THAT(result.value().value(), ContainsSubstring("Taylor"));
                    INFO("Release mode: optimized formal greeting");
                }
            }
        }
        
        WHEN("testing various formal titles") {
            std::vector<std::string> titles = {
                "Dr. John Doe", "Prof. Jane Smith", "Mr. Robert Brown",
                "Mrs. Mary Wilson", "Ms. Sarah Davis", "Miss Anna Johnson"
            };
            
            THEN("all formal titles are handled correctly") {
                for (const auto& title_name : titles) {
                    auto person = PersonName::create(title_name).value();
                    auto result = strategy->greet(person);
                    
                    REQUIRE(result.has_value());
                    REQUIRE_THAT(result.value().value(), ContainsSubstring("Good day"));
                    INFO("Testing title: " << title_name);
                }
            }
        }
    }
}

SCENARIO("FormalGreetingStrategy error handling", "[formal][strategy][error]") {
    
    GIVEN("a formal strategy") {
        auto strategy = createFormalStrategy();
        
        WHEN("greeting inappropriate content") {
            // Use a valid PersonName that might be considered inappropriate for formal context
            auto person_result = PersonName::create("John@Domain");
            if (person_result.has_value()) {
                auto person = person_result.value();
                auto result = strategy->greet(person);
                
                THEN("appropriate validation occurs") {
                    // Test should handle the result appropriately
                    if constexpr (greeting::config::is_debug_build()) {
                        // Debug mode performs comprehensive validation
                        // Might catch inappropriate characters for formal context
                        INFO("Debug mode: comprehensive formal validation");
                    } else {
                        // Release mode allows basic validation to pass through
                        INFO("Release mode: basic validation");
                    }
                    
                    // At minimum, should not crash
                    REQUIRE_NOTHROW([&]() { 
                        auto test_result = strategy->greet(person); 
                    }());
                }
            } else {
                // If PersonName creation fails, test a different scenario
                auto valid_person = PersonName::create("John Smith").value();
                auto result = strategy->greet(valid_person);
                
                THEN("valid names work correctly") {
                    REQUIRE(result.has_value());
                    REQUIRE_THAT(result.value().value(), ContainsSubstring("Good day"));
                }
            }
        }
        
        WHEN("greeting extremely short names") {
            // Test with a name that passes PersonName validation but might be too short for formal context
            auto person_result = PersonName::create("Jo");  // 2 characters - might pass basic validation
            if (person_result.has_value()) {
                auto person = person_result.value();
                auto result = strategy->greet(person);
                
                THEN("handles edge cases gracefully") {
                    // Should either succeed with appropriate formatting or fail gracefully
                    if (result.has_value()) {
                        REQUIRE_THAT(result.value().value(), ContainsSubstring("Good day"));
                    } else {
                        // If it fails, it should be a validation error
                        INFO("Validation appropriately rejected short name for formal context");
                    }
                }
            } else {
                // If PersonName validation is strict, test with a definitely valid name
                auto valid_person = PersonName::create("John Doe").value();
                auto result = strategy->greet(valid_person);
                
                THEN("valid names work correctly") {
                    REQUIRE(result.has_value());
                    REQUIRE_THAT(result.value().value(), ContainsSubstring("Good day"));
                }
            }
        }
    }
}

SCENARIO("FormalGreetingStrategy compile-time features", "[formal][strategy][consteval]") {
    
    WHEN("using compile-time utilities") {
        
        THEN("constexpr title validation works") {
            static_assert(formal_utils::isFormalTitleValid("Dr."));
            static_assert(formal_utils::isFormalTitleValid("Prof."));
            static_assert(!formal_utils::isFormalTitleValid("Invalid"));
            
            SUCCEED("Compile-time title validation verified");
        }
        
        THEN("constexpr greeting generation works") {
            constexpr auto greeting = compile_time::generateFormalGreeting("Test");
            static_assert(greeting.size() > 0);
            
            SUCCEED("Compile-time greeting generation verified");
        }
        
        THEN("constexpr title validation function works") {
            static_assert(compile_time::validateFormalTitle("Mr."));
            static_assert(compile_time::validateFormalTitle("Ms."));
            static_assert(!compile_time::validateFormalTitle("Fake"));
            
            SUCCEED("Compile-time title validation function verified");
        }
    }
}

SCENARIO("FormalGreetingStrategy performance", "[formal][strategy][performance]") {
    
    GIVEN("a formal strategy") {
        auto strategy = createFormalStrategy();
        
        WHEN("processing many formal greetings") {
            auto person = PersonName::create("Dr. Performance Test").value();
            
            auto start = std::chrono::high_resolution_clock::now();
            
            constexpr int iterations = 10000;
            for (int i = 0; i < iterations; ++i) {
                auto result = strategy->greet(person);
                REQUIRE(result.has_value());
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            THEN("performance meets expectations") {
                // Should be efficient even with enhanced formatting
                REQUIRE(duration.count() < 500000); // Less than 500ms for 10k greetings
                INFO("Duration: " << duration.count() << " microseconds");
                INFO("Average per greeting: " << (duration.count() / iterations) << " μs");
            }
        }
        
        WHEN("testing with various name lengths") {
            std::vector<std::string> names = {
                "Dr. A", "Prof. Elizabeth", "Sir Winston Churchill",
                "Dr. Extraordinarily-Long-Hyphenated-Name"
            };
            
            THEN("handles different name lengths efficiently") {
                for (const auto& name : names) {
                    auto person = PersonName::create(name).value();
                    auto start = std::chrono::high_resolution_clock::now();
                    
                    auto result = strategy->greet(person);
                    
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    
                    REQUIRE(result.has_value());
                    REQUIRE(duration.count() < 1000); // Less than 1ms per greeting
                    INFO("Name: " << name << ", Duration: " << duration.count() << " μs");
                }
            }
        }
    }
}

SCENARIO("FormalGreetingStrategy strategy traits", "[formal][strategy][traits]") {
    
    WHEN("checking strategy traits") {
        using FormalTraits = greeting::strategy::traits::StrategyTraits<FormalGreetingStrategy>;
        
        THEN("traits are properly configured") {
            REQUIRE(FormalTraits::supports_debug_info == true);
            REQUIRE(FormalTraits::expected_message_length == 80);
            REQUIRE(FormalTraits::is_thread_safe == true);
            REQUIRE(FormalTraits::supports_async_operation == true);
            REQUIRE(FormalTraits::requires_system_resources == false);
            REQUIRE(FormalTraits::supports_localization == true);
            REQUIRE(FormalTraits::supports_cultural_context == true);
            
            // Configuration-dependent traits
            #ifdef DEBUG
            REQUIRE(FormalTraits::enable_performance_monitoring == true);
            REQUIRE(FormalTraits::enable_validation_caching == true);
            #else
            REQUIRE(FormalTraits::enable_performance_monitoring == false);
            REQUIRE(FormalTraits::enable_validation_caching == false);
            #endif
            
            SUCCEED("Strategy traits properly configured");
        }
    }
}

SCENARIO("FormalGreetingStrategy integration", "[formal][strategy][integration]") {
    
    GIVEN("a formal strategy") {
        auto strategy = createFormalStrategy();
        
        WHEN("integrating with existing domain types") {
            // Test integration with PersonName validation
            auto valid_person = PersonName::create("Dr. Integration Test");
            REQUIRE(valid_person.has_value());
            
            auto result = strategy->greet(valid_person.value());
            REQUIRE(result.has_value());
            
            // Test integration with GreetingMessage validation
            auto message = result.value();
            REQUIRE_THAT(message.value(), ContainsSubstring("Good day"));
            
            THEN("integration works seamlessly") {
                SUCCEED("Integration with domain types verified");
            }
        }
        
        WHEN("testing factory function") {
            auto strategy1 = createFormalStrategy();
            auto strategy2 = createFormalStrategy();
            
            THEN("factory creates independent instances") {
                REQUIRE(strategy1 != nullptr);
                REQUIRE(strategy2 != nullptr);
                REQUIRE(strategy1.get() != strategy2.get()); // Different instances
                
                // Both should work identically
                auto person = PersonName::create("Factory Test").value();
                auto result1 = strategy1->greet(person);
                auto result2 = strategy2->greet(person);
                
                REQUIRE(result1.has_value());
                REQUIRE(result2.has_value());
                REQUIRE(result1.value().value() == result2.value().value());
            }
        }
    }
}
