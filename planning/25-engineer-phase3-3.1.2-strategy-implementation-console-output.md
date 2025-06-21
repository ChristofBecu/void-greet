# **3.1.2: Strategy Implementation - Console Output - Detailed Implementation Plan**

## **Overview**
Implement the console output strategy as the first concrete implementation of the `IGreetingStrategy` interface. This strategy will serve as the baseline implementation demonstrating direct console output with configuration-aware formatting and robust error handling for I/O operations.

---

## **File: `src/domain/console_strategy.cpp`**

### **Implementation Requirements**

#### **A. Core Console Strategy Implementation**
```cpp
#include "greeting/console_strategy.hpp"
#include "greeting/strategy_interface.hpp"
#include "config/build_config.hpp"
#include <iostream>
#include <sstream>

#if HELLOWORLD_HAS_STACKTRACE_RUNTIME
#include <stacktrace>
#endif

namespace greeting::strategy {

/**
 * @brief Console output strategy implementation
 * 
 * Provides direct console output with configuration-aware formatting.
 * Integrates with existing error handling and validation infrastructure.
 */
class ConsoleGreetingStrategy final : public IGreetingStrategy {
public:
    /**
     * @brief Default constructor with configuration validation
     */
    explicit ConsoleGreetingStrategy() noexcept {
        if constexpr (greeting::config::is_debug_build()) {
            validateConsoleAvailability();
        }
    }

    /**
     * @brief Generate and output greeting to console
     * @param person Validated person name to greet
     * @return Expected greeting message or I/O error
     */
    [[nodiscard]] Result<GreetingMessage> greet(
        const PersonName& person) const noexcept override {
        
        if constexpr (greeting::config::is_debug_build()) {
            return greetWithDebugContext(person);
        } else {
            return greetOptimized(person);
        }
    }

    /**
     * @brief Get strategy identification
     * @return Strategy name for diagnostics
     */
    [[nodiscard]] std::string_view getStrategyName() const noexcept override {
        return "ConsoleGreetingStrategy";
    }

    /**
     * @brief Validate console output capability
     * @return True if console is available for output
     */
    [[nodiscard]] bool isValid() const noexcept override {
        // Check if stdout is available and writable
        return std::cout.good() && !std::cout.bad();
    }

private:
    /**
     * @brief Debug-mode greeting with comprehensive error handling
     */
    [[nodiscard]] Result<GreetingMessage> greetWithDebugContext(
        const PersonName& person) const noexcept {
        
        try {
            // Create greeting message with validation
            auto greeting_result = createFormattedGreeting(person);
            if (!greeting_result.has_value()) {
                return propagateCreationError(greeting_result.error(), person);
            }

            // Output with error checking
            auto output_result = outputToConsoleWithValidation(greeting_result.value());
            if (!output_result.has_value()) {
                return propagateOutputError(output_result.error(), greeting_result.value());
            }

            return greeting_result.value();

        } catch (const std::exception& e) {
            return createExceptionError(e.what(), person);
        }
    }

    /**
     * @brief Release-mode optimized greeting
     */
    [[nodiscard]] Result<GreetingMessage> greetOptimized(
        const PersonName& person) const noexcept {
        
        // Fast path with minimal error checking
        auto greeting_result = createFormattedGreeting(person);
        if (!greeting_result.has_value()) {
            return greeting_result.error();
        }

        // Direct output without extensive validation
        outputToConsoleOptimized(greeting_result.value());
        return greeting_result.value();
    }

    /**
     * @brief Create formatted greeting message
     */
    [[nodiscard]] Result<GreetingMessage> createFormattedGreeting(
        const PersonName& person) const noexcept {
        
        std::string greeting_text;
        
        if constexpr (greeting::config::is_debug_build()) {
            // Rich formatting with timestamp in debug mode
            greeting_text = formatDebugGreeting(person);
        } else {
            // Simple formatting for performance
            greeting_text = "Hello, " + person.value() + "!";
        }

        return GreetingMessage::create(greeting_text);
    }

    /**
     * @brief Configuration-aware console output with validation
     */
    [[nodiscard]] Result<void> outputToConsoleWithValidation(
        const GreetingMessage& message) const noexcept {
        
        try {
            // Pre-output validation
            if (!std::cout.good()) {
                return createOutputStreamError("Console stream not available");
            }

            // Output with formatting
            std::cout << message.value();
            
            if constexpr (greeting::config::is_debug_build()) {
                std::cout << " [DEBUG: ConsoleStrategy]";
            }
            
            std::cout << std::endl;

            // Post-output validation
            if (std::cout.fail()) {
                return createOutputStreamError("Console output failed");
            }

            return Result<void>{};

        } catch (const std::ios_base::failure& e) {
            return createOutputStreamError(std::string{"I/O exception: "} + e.what());
        }
    }

    /**
     * @brief Optimized console output for release builds
     */
    void outputToConsoleOptimized(const GreetingMessage& message) const noexcept {
        // Fast path - minimal error checking
        std::cout << message.value() << std::endl;
    }

    /**
     * @brief Debug-specific greeting formatting
     */
    [[nodiscard]] std::string formatDebugGreeting(const PersonName& person) const {
        std::ostringstream oss;
        oss << "Hello, " << person.value() << "!";
        
        if constexpr (greeting::config::performance_monitoring_enabled()) {
            oss << " [Timestamp: " << getCurrentTimestamp() << "]";
        }
        
        return oss.str();
    }

    /**
     * @brief Validate console availability during construction
     */
    void validateConsoleAvailability() const noexcept {
        if (!std::cout.good()) {
            // In debug builds, log console unavailability
            std::cerr << "WARNING: Console output stream not available\n";
        }
    }

    /**
     * @brief Get current timestamp for debug output
     */
    [[nodiscard]] std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
        return oss.str();
    }

    // Error creation utilities
    [[nodiscard]] Result<GreetingMessage> propagateCreationError(
        GreetingError error, const PersonName& person) const noexcept;
    
    [[nodiscard]] Result<GreetingMessage> propagateOutputError(
        GreetingError error, const GreetingMessage& message) const noexcept;
    
    [[nodiscard]] Result<GreetingMessage> createExceptionError(
        const std::string& what, const PersonName& person) const noexcept;
    
    [[nodiscard]] Result<void> createOutputStreamError(
        const std::string& details) const noexcept;
};

} // namespace greeting::strategy
```

#### **B. Header File: `include/greeting/console_strategy.hpp`**
```cpp
#pragma once

#include "greeting/strategy_interface.hpp"
#include "greeting/expected.hpp"
#include <chrono>
#include <iomanip>

namespace greeting::strategy {

/**
 * @brief Forward declaration for console strategy
 */
class ConsoleGreetingStrategy;

/**
 * @brief Console strategy factory function
 * @return Unique pointer to console strategy instance
 */
[[nodiscard]] std::unique_ptr<IGreetingStrategy> createConsoleStrategy() noexcept;

/**
 * @brief Console strategy traits specialization
 */
template<>
struct StrategyTraits<ConsoleGreetingStrategy> {
    static constexpr bool supports_debug_info = true;
    static constexpr bool enable_performance_monitoring = greeting::config::is_debug_build();
    static constexpr bool enable_validation_caching = false; // I/O operations not cacheable
    
    static constexpr size_t expected_message_length = 50;
    static constexpr bool is_thread_safe = false; // std::cout is not thread-safe
    static constexpr bool supports_async_operation = false;
    static constexpr bool requires_system_resources = true; // Needs console access
};

} // namespace greeting::strategy
```

#### **C. Error Handling Integration**
```cpp
namespace greeting::strategy {

// Error creation implementations
[[nodiscard]] Result<GreetingMessage> ConsoleGreetingStrategy::propagateCreationError(
    GreetingError error, const PersonName& person) const noexcept {
    
    if constexpr (greeting::config::is_debug_build()) {
        return errors::propagateStrategyError<GreetingMessage>(
            error, 
            getStrategyName(), 
            std::string{"greeting creation for '"} + person.value() + "'"
        );
    } else {
        return Expected<GreetingMessage, GreetingError>{error};
    }
}

[[nodiscard]] Result<GreetingMessage> ConsoleGreetingStrategy::propagateOutputError(
    GreetingError error, const GreetingMessage& message) const noexcept {
    
    if constexpr (greeting::config::is_debug_build()) {
        return errors::propagateStrategyError<GreetingMessage>(
            error,
            getStrategyName(),
            std::string{"console output of '"} + message.value() + "'"
        );
    } else {
        return Expected<GreetingMessage, GreetingError>{GreetingError::OutputStreamError};
    }
}

[[nodiscard]] Result<GreetingMessage> ConsoleGreetingStrategy::createExceptionError(
    const std::string& what, const PersonName& person) const noexcept {
    
    if constexpr (greeting::config::is_debug_build()) {
        auto detailed_error = errors::makeStrategyError<errors::StrategyError::StrategyExecutionFailed>(
            std::string{"Exception during console greeting for '"} + person.value() + 
            "': " + what
        );
        return Expected<GreetingMessage, GreetingError>{detailed_error.code()};
    } else {
        return Expected<GreetingMessage, GreetingError>{GreetingError::StrategyExecutionFailed};
    }
}

[[nodiscard]] Result<void> ConsoleGreetingStrategy::createOutputStreamError(
    const std::string& details) const noexcept {
    
    if constexpr (greeting::config::is_debug_build()) {
        // Rich error context with stacktrace if available
        #if HELLOWORLD_HAS_STACKTRACE_RUNTIME
        auto stack = std::stacktrace::current();
        return errors::makeStrategyError<errors::StrategyError::StrategyResourceUnavailable>(
            details + " [Stacktrace: " + std::to_string(stack) + "]"
        );
        #else
        return errors::makeStrategyError<errors::StrategyError::StrategyResourceUnavailable>(details);
        #endif
    } else {
        return Expected<void, GreetingError>{GreetingError::OutputStreamError};
    }
}

} // namespace greeting::strategy
```

#### **D. Factory Function Implementation**
```cpp
namespace greeting::strategy {

[[nodiscard]] std::unique_ptr<IGreetingStrategy> createConsoleStrategy() noexcept {
    try {
        return std::make_unique<ConsoleGreetingStrategy>();
    } catch (...) {
        // In case of allocation failure, return nullptr
        return nullptr;
    }
}

} // namespace greeting::strategy
```

---

## **Integration Points**

### **A. Error Code Extensions**
Extend error_codes.hpp:
```cpp
// Add to existing GreetingError enum
OutputStreamError = 4001,          ///< Console output stream error
StrategyExecutionFailed = 3002,    ///< Strategy execution encountered error
StrategyResourceUnavailable = 3005 ///< Required strategy resources unavailable
```

### **B. Build System Integration**
Update CMakeLists.txt:
```cmake
# Add console strategy to domain library
target_sources(greeting_domain PRIVATE
    src/domain/console_strategy.cpp
)

# Add console strategy header
target_include_directories(greeting_domain PUBLIC
    include/greeting/console_strategy.hpp
)
```

### **C. Configuration Integration**
Leverage existing configuration flags:
- `greeting::config::is_debug_build()` for behavior selection
- `greeting::config::performance_monitoring_enabled()` for timestamp inclusion
- Existing validation infrastructure for error handling

---

## **Testing Requirements**

### **File: `tests/unit/domain/test_console_strategy.cpp`**

#### **Test Categories:**

#### **A. Basic Functionality Tests**
```cpp
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "greeting/console_strategy.hpp"
#include "greeting/person_name.hpp"

using namespace greeting::strategy;
using Catch::Matchers::Equals;

SCENARIO("ConsoleGreetingStrategy basic functionality", "[console][strategy][basic]") {
    
    GIVEN("a console strategy instance") {
        auto strategy = createConsoleStrategy();
        REQUIRE(strategy != nullptr);
        
        WHEN("greeting a valid person") {
            auto person = PersonName::create("Alice Johnson").value();
            auto result = strategy->greet(person);
            
            THEN("greeting succeeds") {
                REQUIRE(result.has_value());
                REQUIRE_THAT(result.value().value(), Catch::Matchers::ContainsSubstring("Alice Johnson"));
            }
        }
        
        WHEN("checking strategy validity") {
            THEN("strategy reports as valid") {
                REQUIRE(strategy->isValid());
            }
        }
        
        WHEN("getting strategy name") {
            THEN("correct name is returned") {
                REQUIRE_THAT(std::string{strategy->getStrategyName()}, 
                           Equals("ConsoleGreetingStrategy"));
            }
        }
    }
}
```

#### **B. Configuration-Aware Behavior Tests**
```cpp
SCENARIO("ConsoleGreetingStrategy configuration awareness", "[console][strategy][config]") {
    
    GIVEN("a console strategy") {
        auto strategy = createConsoleStrategy();
        
        WHEN("greeting in different build configurations") {
            auto person = PersonName::create("Bob Smith").value();
            auto result = strategy->greet(person);
            
            THEN("behavior adapts to configuration") {
                REQUIRE(result.has_value());
                
                if constexpr (greeting::config::is_debug_build()) {
                    // Debug builds may include additional context
                    INFO("Debug build - enhanced output expected");
                } else {
                    // Release builds should be minimal
                    INFO("Release build - optimized output expected");
                }
                
                REQUIRE_THAT(result.value().value(), 
                           Catch::Matchers::ContainsSubstring("Bob Smith"));
            }
        }
    }
}
```

#### **C. Error Handling Tests**
```cpp
SCENARIO("ConsoleGreetingStrategy error handling", "[console][strategy][error]") {
    
    GIVEN("a console strategy") {
        auto strategy = createConsoleStrategy();
        
        WHEN("console output fails") {
            // Test scenario where std::cout might fail
            // This is challenging to test directly, but we can test error paths
            
            THEN("appropriate errors are returned") {
                // Test will depend on ability to simulate I/O failures
                // In practice, this might require dependency injection of output stream
                REQUIRE(strategy->isValid()); // At minimum, validate this doesn't throw
            }
        }
    }
}
```

#### **D. Performance Tests**
```cpp
SCENARIO("ConsoleGreetingStrategy performance", "[console][strategy][performance]") {
    
    GIVEN("a console strategy") {
        auto strategy = createConsoleStrategy();
        
        WHEN("greeting multiple people") {
            auto person = PersonName::create("Performance Test").value();
            
            auto start = std::chrono::high_resolution_clock::now();
            
            constexpr int iterations = 1000;
            for (int i = 0; i < iterations; ++i) {
                auto result = strategy->greet(person);
                REQUIRE(result.has_value());
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            THEN("performance is acceptable") {
                // Ensure reasonable performance (adjust threshold as needed)
                REQUIRE(duration.count() < 100000); // Less than 100ms for 1000 greetings
                INFO("Duration: " << duration.count() << " microseconds");
            }
        }
    }
}
```

#### **E. Interface Compliance Tests**
```cpp
SCENARIO("ConsoleGreetingStrategy interface compliance", "[console][strategy][interface]") {
    
    GIVEN("a console strategy") {
        auto strategy = createConsoleStrategy();
        
        WHEN("used through base interface") {
            IGreetingStrategy* base_strategy = strategy.get();
            auto person = PersonName::create("Interface Test").value();
            
            THEN("all interface methods work correctly") {
                REQUIRE(base_strategy->isValid());
                REQUIRE_FALSE(base_strategy->getStrategyName().empty());
                
                auto result = base_strategy->greet(person);
                REQUIRE(result.has_value());
            }
        }
    }
    
    #ifdef __cpp_concepts
    WHEN("validating against concepts") {
        THEN("strategy satisfies all required concepts") {
            static_assert(concepts::GreetingStrategy<ConsoleGreetingStrategy>);
            static_assert(concepts::ConfigAwareStrategy<ConsoleGreetingStrategy>);
        }
    }
    #endif
}
```

---

## **Implementation Milestones**

### **Milestone 1: Core Implementation** (3 hours)
- [ ] Implement `ConsoleGreetingStrategy` class
- [ ] Add basic console output functionality
- [ ] Implement interface methods (`greet()`, `getStrategyName()`, `isValid()`)
- [ ] Create factory function

### **Milestone 2: Configuration Integration** (2 hours)
- [ ] Add configuration-aware formatting
- [ ] Implement debug vs release behavior paths
- [ ] Add performance monitoring integration
- [ ] Validate zero-overhead abstractions

### **Milestone 3: Error Handling** (2 hours)
- [ ] Implement comprehensive error handling for I/O operations
- [ ] Add error propagation utilities
- [ ] Integrate with existing error enumeration
- [ ] Test exception safety

### **Milestone 4: Testing** (2 hours)
- [ ] Create comprehensive unit test suite
- [ ] Test configuration-aware behavior
- [ ] Add performance benchmarks
- [ ] Validate interface compliance

### **Milestone 5: Integration & Documentation** (1 hour)
- [ ] Update build system configuration
- [ ] Add header file documentation
- [ ] Validate integration with existing infrastructure
- [ ] Performance validation

---

## **Success Criteria**

### **Functional Requirements**
- ✅ Implements `IGreetingStrategy` interface correctly
- ✅ Provides console output functionality
- ✅ Configuration-aware formatting (debug vs release)
- ✅ Proper error handling for I/O operations
- ✅ Factory function for strategy creation

### **Performance Requirements**
- ✅ Zero-overhead abstractions in release builds
- ✅ Minimal allocation and copying
- ✅ Efficient string formatting
- ✅ Fast console output operations

### **Quality Requirements**
- ✅ Comprehensive unit test coverage (>95%)
- ✅ Exception safety guarantees
- ✅ Integration with existing error handling
- ✅ Consistent with project coding standards
- ✅ Proper resource management (RAII)

### **System Integration**
- ✅ Leverages existing configuration infrastructure
- ✅ Uses established validation patterns
- ✅ Integrates with error handling system
- ✅ Maintains compatibility with existing domain services

This console strategy implementation serves as the reference implementation for the strategy pattern, demonstrating proper integration with the configuration infrastructure while providing robust console output capabilities with comprehensive error handling.