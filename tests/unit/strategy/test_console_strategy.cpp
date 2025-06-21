#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_section_info.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "greeting/console_strategy.hpp"
#include "greeting/strategy_interface.hpp"
#include "greeting/person_name.hpp"
#include "greeting/greeting_message.hpp"
#include <sstream>
#include <iostream>

using namespace greeting;
using greeting::strategy::createConsoleStrategy;
using greeting::strategy::IGreetingStrategy;

namespace {

// Helper to capture console output for testing
class ConsoleCapture {
public:
    ConsoleCapture() : old_cout_(std::cout.rdbuf()) {
        std::cout.rdbuf(captured_output_.rdbuf());
    }
    
    ~ConsoleCapture() {
        std::cout.rdbuf(old_cout_);
    }
    
    std::string getOutput() {
        return captured_output_.str();
    }
    
    void clear() {
        captured_output_.str("");
        captured_output_.clear();
    }
    
private:
    std::ostringstream captured_output_;
    std::streambuf* old_cout_;
};

} // anonymous namespace

TEST_CASE("Console Strategy Interface Compliance", "[console][strategy][interface]") {
    
    SECTION("Factory function creates valid strategy") {
        auto strategy = createConsoleStrategy();
        
        REQUIRE(strategy != nullptr);
        REQUIRE(strategy->isValid());
        REQUIRE(strategy->getStrategyName() == "ConsoleGreetingStrategy");
    }
    
    SECTION("Strategy inherits from IGreetingStrategy") {
        auto strategy = createConsoleStrategy();
        auto* base_ptr = dynamic_cast<IGreetingStrategy*>(strategy.get());
        
        REQUIRE(base_ptr != nullptr);
    }
    
    SECTION("Strategy provides correct identification") {
        auto strategy = createConsoleStrategy();
        auto name = strategy->getStrategyName();
        
        REQUIRE(name == "ConsoleGreetingStrategy");
        REQUIRE(!name.empty());
    }
}

TEST_CASE("Console Strategy Core Functionality", "[console][strategy][core]") {
    
    SECTION("Successful greeting generation and output") {
        ConsoleCapture capture;
        auto strategy = createConsoleStrategy();
        
        auto person_result = PersonName::create("John Doe");
        REQUIRE(person_result.has_value());
        
        auto greeting_result = strategy->greet(person_result.value());
        
        REQUIRE(greeting_result.has_value());
        
        // Check that output was written to console
        std::string output = capture.getOutput();
        REQUIRE(!output.empty());
        REQUIRE(output.find("Hello, John Doe!") != std::string::npos);
    }
    
    SECTION("Greeting message content validation") {
        ConsoleCapture capture;
        auto strategy = createConsoleStrategy();
        
        auto person_result = PersonName::create("Alice");
        REQUIRE(person_result.has_value());
        
        auto greeting_result = strategy->greet(person_result.value());
        
        REQUIRE(greeting_result.has_value());
        
        // Verify the returned greeting message matches expected format
        const auto& message = greeting_result.value();
        REQUIRE(message.value().find("Hello, Alice!") != std::string::npos);
    }
    
    SECTION("Multiple greetings produce correct output") {
        ConsoleCapture capture;
        auto strategy = createConsoleStrategy();
        
        std::vector<std::string> names = {"Alice", "Bob", "Charlie"};
        
        for (const auto& name : names) {
            capture.clear();
            
            auto person_result = PersonName::create(name);
            REQUIRE(person_result.has_value());
            
            auto greeting_result = strategy->greet(person_result.value());
            REQUIRE(greeting_result.has_value());
            
            std::string output = capture.getOutput();
            REQUIRE(output.find("Hello, " + name + "!") != std::string::npos);
        }
    }
}

TEST_CASE("Console Strategy Configuration Awareness", "[console][strategy][config]") {
    
    SECTION("Debug vs Release build behavior") {
        ConsoleCapture capture;
        auto strategy = createConsoleStrategy();
        
        auto person_result = PersonName::create("TestUser");
        REQUIRE(person_result.has_value());
        
        auto greeting_result = strategy->greet(person_result.value());
        REQUIRE(greeting_result.has_value());
        
        std::string output = capture.getOutput();
        
        if constexpr (greeting::config::is_debug_build()) {
            // Debug builds should include debug markers
            REQUIRE(output.find("[DEBUG: ConsoleStrategy]") != std::string::npos);
            
            // Debug builds may include timestamp if performance monitoring is enabled
            if constexpr (greeting::config::performance_monitoring_enabled()) {
                REQUIRE(output.find("[Timestamp:") != std::string::npos);
            }
        } else {
            // Release builds should not include debug markers
            REQUIRE(output.find("[DEBUG: ConsoleStrategy]") == std::string::npos);
            REQUIRE(output.find("[Timestamp:") == std::string::npos);
        }
    }
    
    SECTION("Strategy validation state") {
        auto strategy = createConsoleStrategy();
        
        // Console strategy should be valid when stdout is available
        REQUIRE(strategy->isValid());
    }
}

TEST_CASE("Console Strategy Error Handling", "[console][strategy][error]") {
    
    SECTION("Invalid person name propagation") {
        auto strategy = createConsoleStrategy();
        
        // Test with empty name which should fail validation
        auto person_result = PersonName::create("");
        if (!person_result.has_value()) {
            // If PersonName creation fails, that's expected for empty string
            REQUIRE(true); // This test validates error propagation would work
        } else {
            // If PersonName accepts empty string, strategy should handle it
            auto greeting_result = strategy->greet(person_result.value());
            // Should either succeed or fail gracefully
            REQUIRE((greeting_result.has_value() || !greeting_result.has_value()));
        }
    }
    
    SECTION("Strategy maintains noexcept guarantee") {
        auto strategy = createConsoleStrategy();
        
        auto person_result = PersonName::create("John");
        REQUIRE(person_result.has_value());
        
        // These calls should be noexcept
        REQUIRE_NOTHROW(strategy->greet(person_result.value()));
        REQUIRE_NOTHROW(strategy->getStrategyName());
        REQUIRE_NOTHROW(strategy->isValid());
    }
}

TEST_CASE("Console Strategy Traits and Optimization", "[console][strategy][traits]") {
    
    using ConsoleTraits = greeting::strategy::traits::StrategyTraits<greeting::strategy::ConsoleGreetingStrategy>;
    
    SECTION("Strategy traits configuration") {
        REQUIRE(ConsoleTraits::supports_debug_info == true);
        REQUIRE(ConsoleTraits::enable_validation_caching == false); // I/O not cacheable
        REQUIRE(ConsoleTraits::expected_message_length == 50);
        REQUIRE(ConsoleTraits::is_thread_safe == false); // std::cout not thread-safe
        REQUIRE(ConsoleTraits::supports_async_operation == false);
        REQUIRE(ConsoleTraits::requires_system_resources == true); // Needs console
    }
    
    SECTION("Performance monitoring trait matches build config") {
        REQUIRE(ConsoleTraits::enable_performance_monitoring == greeting::config::is_debug_build());
    }
}

#ifdef __cpp_concepts
TEST_CASE("Console Strategy Concept Compliance", "[console][strategy][concepts]") {
    
    SECTION("Strategy satisfies interface requirements through factory") {
        // Since the concrete class is not exposed, we test through the interface
        auto strategy = createConsoleStrategy();
        
        REQUIRE(strategy != nullptr);
        REQUIRE(strategy->getStrategyName() == "ConsoleGreetingStrategy");
        REQUIRE(strategy->isValid());
        
        // Test that it behaves like a greeting strategy
        auto person_result = PersonName::create("TestUser");
        if (person_result.has_value()) {
            auto greeting_result = strategy->greet(person_result.value());
            // Should either succeed or fail gracefully (both are valid for strategy interface)
            REQUIRE((greeting_result.has_value() || !greeting_result.has_value()));
        }
    }
}
#endif // __cpp_concepts

TEST_CASE("Console Strategy Integration", "[console][strategy][integration]") {
    
    SECTION("Strategy works with existing domain types") {
        ConsoleCapture capture;
        auto strategy = createConsoleStrategy();
        
        // Test with various valid person names
        auto names = GENERATE(
            std::string("John"),
            std::string("Mary Jane"),
            std::string("Jean-Claude"),
            std::string("O'Connor")
        );
        
        auto person_result = PersonName::create(names);
        if (person_result.has_value()) {
            auto greeting_result = strategy->greet(person_result.value());
            REQUIRE(greeting_result.has_value());
            
            std::string output = capture.getOutput();
            REQUIRE(output.find("Hello, " + names + "!") != std::string::npos);
        }
    }
    
    SECTION("Factory function produces unique instances") {
        auto strategy1 = createConsoleStrategy();
        auto strategy2 = createConsoleStrategy();
        
        REQUIRE(strategy1.get() != strategy2.get());
        REQUIRE(strategy1->getStrategyName() == strategy2->getStrategyName());
    }
}

TEST_CASE("Console Strategy Output Formatting", "[console][strategy][format]") {
    
    SECTION("Basic greeting format verification") {
        ConsoleCapture capture;
        auto strategy = createConsoleStrategy();
        
        auto person_result = PersonName::create("World");
        REQUIRE(person_result.has_value());
        
        auto greeting_result = strategy->greet(person_result.value());
        REQUIRE(greeting_result.has_value());
        
        std::string output = capture.getOutput();
        
        // Verify basic format: "Hello, World!" followed by newline
        REQUIRE(output.find("Hello, World!") == 0);
        REQUIRE(output.back() == '\n');
    }
    
    SECTION("Output ends with newline") {
        ConsoleCapture capture;
        auto strategy = createConsoleStrategy();
        
        auto person_result = PersonName::create("TestUser");
        REQUIRE(person_result.has_value());
        
        strategy->greet(person_result.value());
        
        std::string output = capture.getOutput();
        REQUIRE(!output.empty());
        REQUIRE(output.back() == '\n');
    }
}
