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
            bool output_success = outputToConsoleWithValidation(greeting_result.value());
            if (!output_success) {
                return propagateOutputError(greeting_result.value());
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
    [[nodiscard]] bool outputToConsoleWithValidation(
        const GreetingMessage& message) const noexcept {
        
        try {
            // Pre-output validation
            if (!std::cout.good()) {
                return false;
            }

            // Output with formatting
            std::cout << message.value();
            
            if constexpr (greeting::config::is_debug_build()) {
                std::cout << " [DEBUG: ConsoleStrategy]";
            }
            
            std::cout << std::endl;

            // Post-output validation
            if (std::cout.fail()) {
                return false;
            }

            return true;

        } catch (const std::ios_base::failure& e) {
            return false;
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
        GreetingError error, const PersonName& person) const noexcept {
        
        if constexpr (greeting::config::is_debug_build()) {
            return errors::propagateStrategyError<GreetingMessage>(
                error, 
                "ConsoleGreetingStrategy", 
                "create_formatted_greeting"
            );
        } else {
            return Expected<GreetingMessage, GreetingError>{error};
        }
    }
    
    [[nodiscard]] Result<GreetingMessage> propagateOutputError(
        const GreetingMessage& message) const noexcept {
        
        auto error = errors::makeStrategyError<errors::StrategyError::StrategyResourceUnavailable>();
        
        if constexpr (greeting::config::is_debug_build()) {
            return errors::propagateStrategyError<GreetingMessage>(
                error, 
                "ConsoleGreetingStrategy", 
                "output_to_console"
            );
        } else {
            return Expected<GreetingMessage, GreetingError>{error};
        }
    }
    
    [[nodiscard]] Result<GreetingMessage> createExceptionError(
        const std::string& what, const PersonName& person) const noexcept {
        
        auto error = errors::makeStrategyError<errors::StrategyError::StrategyExecutionFailed>();
        return Expected<GreetingMessage, GreetingError>{error};
    }
};

// Factory function implementation
[[nodiscard]] std::unique_ptr<IGreetingStrategy> createConsoleStrategy() noexcept {
    return std::make_unique<ConsoleGreetingStrategy>();
}

} // namespace greeting::strategy
