#include "greeting/formal_strategy.hpp"
#include "greeting/strategy_interface.hpp"
#include "config/build_config.hpp"
#include <string>
#include <string_view>
#include <algorithm>
#include <array>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

#if HELLOWORLD_HAS_STACKTRACE_RUNTIME
#include <stacktrace>
#endif

namespace greeting::strategy {

/**
 * @brief Formal greeting strategy with enhanced formatting and titles
 * 
 * Provides sophisticated formal greeting generation with:
 * - Title and honorific detection and preservation
 * - Enhanced validation with cultural awareness
 * - Configuration-aware formatting complexity
 * - C++23 compile-time string processing optimizations
 */
class FormalGreetingStrategy final : public IGreetingStrategy {
private:
    // Compile-time title database for formal greetings
    static constexpr std::array<std::string_view, 12> FORMAL_TITLES = {{
        "Dr.", "Prof.", "Mr.", "Mrs.", "Ms.", "Miss",
        "Sir", "Madam", "Lord", "Lady", "Hon.", "Rev."
    }};
    
    static constexpr std::array<std::string_view, 8> ACADEMIC_TITLES = {{
        "PhD", "MD", "JD", "MBA", "MA", "MSc", "BSc", "BA"
    }};

public:
    /**
     * @brief Constructor with formal validation capabilities
     */
    explicit FormalGreetingStrategy() noexcept {
        if constexpr (greeting::config::is_debug_build()) {
            validateFormalCapabilities();
            initializeDebugContext();
        }
    }

    /**
     * @brief Generate formal greeting with enhanced formatting
     * @param person Validated person name to greet formally
     * @return Expected formal greeting message or validation error
     */
    [[nodiscard]] Result<GreetingMessage> greet(
        const PersonName& person) const noexcept override {
        
        if constexpr (greeting::config::is_debug_build()) {
            return generateFormalGreetingWithDebugContext(person);
        } else {
            return generateFormalGreetingOptimized(person);
        }
    }

    /**
     * @brief Get strategy identification
     * @return Strategy name for diagnostics
     */
    [[nodiscard]] std::string_view getStrategyName() const noexcept override {
        return "FormalGreetingStrategy";
    }

    /**
     * @brief Validate formal greeting capabilities
     * @return True if strategy can generate formal greetings
     */
    [[nodiscard]] bool isValid() const noexcept override {
        // Validate that formal greeting infrastructure is available
        return validateTitleDatabase() && validateFormattingCapabilities();
    }

private:
    /**
     * @brief Debug-mode formal greeting with comprehensive validation
     */
    [[nodiscard]] Result<GreetingMessage> generateFormalGreetingWithDebugContext(
        const PersonName& person) const noexcept {
        
        try {
            // Enhanced validation for formal context
            if (auto validation_error = validatePersonForFormalContext(person)) {
                return propagateFormalValidationError(*validation_error, person);
            }

            // Analyze name structure for formal greeting
            auto name_analysis = analyzeNameStructure(person);
            if (!name_analysis.has_value()) {
                return propagateNameAnalysisError(name_analysis.error(), person);
            }

            // Generate formal greeting with enhanced formatting
            auto formal_greeting = formatFormalGreetingWithContext(
                person, name_analysis.value()
            );
            
            if (!formal_greeting.has_value()) {
                return propagateFormattingError(formal_greeting.error(), person);
            }

            return GreetingMessage::create(formal_greeting.value());

        } catch (const std::exception& e) {
            return createFormalExceptionError(e.what(), person);
        }
    }

    /**
     * @brief Release-mode optimized formal greeting
     */
    [[nodiscard]] Result<GreetingMessage> generateFormalGreetingOptimized(
        const PersonName& person) const noexcept {
        
        // Fast path with minimal validation
        auto formal_text = formatFormalGreetingSimple(person);
        return GreetingMessage::create(formal_text);
    }

    /**
     * @brief Validate person name for formal greeting context
     */
    [[nodiscard]] std::optional<GreetingError> validatePersonForFormalContext(
        const PersonName& person) const noexcept {
        
        const auto& name = person.value();
        
        // Enhanced validation for formal context
        if (name.length() < 2) {
            return GreetingError::NameTooShort;
        }
        
        // Check for inappropriate characters in formal context
        if (containsInformalCharacters(name)) {
            return GreetingError::InvalidName;
        }
        
        // Validate title formatting if present
        if (auto title_error = validateTitleFormatting(name)) {
            return *title_error;
        }
        
        return std::nullopt;
    }

    /**
     * @brief Analyze name structure for formal greeting optimization
     */
    struct NameAnalysis {
        std::string_view title;
        std::string_view first_name;
        std::string_view last_name;
        std::string_view suffix;
        bool has_formal_title = false;
        bool has_academic_credential = false;
    };

    [[nodiscard]] Result<NameAnalysis> analyzeNameStructure(
        const PersonName& person) const noexcept {
        
        const auto& name = person.value();
        
        if constexpr (greeting::config::is_debug_build()) {
            // Comprehensive name parsing in debug mode
            return parseNameComponentsDebug(name);
        } else {
            // Simplified name parsing for performance
            return parseNameComponentsSimple(name);
        }
    }

    /**
     * @brief Debug-mode comprehensive name parsing
     */
    [[nodiscard]] Result<NameAnalysis> parseNameComponentsDebug(
        std::string_view name) const noexcept {
        
        NameAnalysis analysis{};
        
        // Tokenize name components
        auto tokens = tokenizeName(name);
        if (tokens.empty()) {
            return Expected<NameAnalysis, GreetingError>{GreetingError::EmptyName};
        }
        
        // Identify title
        if (isFormalTitle(tokens[0])) {
            analysis.title = tokens[0];
            analysis.has_formal_title = true;
            tokens.erase(tokens.begin());
        }
        
        // Identify academic credentials (usually at the end)
        if (!tokens.empty() && isAcademicCredential(tokens.back())) {
            analysis.suffix = tokens.back();
            analysis.has_academic_credential = true;
            tokens.pop_back();
        }
        
        // Assign remaining components
        if (!tokens.empty()) {
            analysis.first_name = tokens[0];
            if (tokens.size() > 1) {
                analysis.last_name = tokens.back();
            }
        }
        
        return Expected<NameAnalysis, GreetingError>{analysis};
    }

    /**
     * @brief Release-mode simplified name parsing
     */
    [[nodiscard]] Result<NameAnalysis> parseNameComponentsSimple(
        std::string_view name) const noexcept {
        
        NameAnalysis analysis{};
        
        // Simple title detection
        for (const auto& title : FORMAL_TITLES) {
            if (name.starts_with(title)) {
                analysis.title = title;
                analysis.has_formal_title = true;
                break;
            }
        }
        
        // Simple first/last name extraction (basic implementation)
        auto space_pos = name.find(' ');
        if (space_pos != std::string_view::npos) {
            analysis.first_name = name.substr(0, space_pos);
            analysis.last_name = name.substr(space_pos + 1);
        } else {
            analysis.first_name = name;
        }
        
        return Expected<NameAnalysis, GreetingError>{analysis};
    }

    /**
     * @brief Format formal greeting with contextual enhancement
     */
    [[nodiscard]] Result<std::string> formatFormalGreetingWithContext(
        const PersonName& person, const NameAnalysis& analysis) const noexcept {
        
        std::string formal_greeting;
        
        if (analysis.has_formal_title) {
            // Use existing title for maximum formality
            formal_greeting = formatGreetingWithTitle(analysis);
        } else if (!analysis.last_name.empty()) {
            // Add appropriate title based on context
            formal_greeting = formatGreetingWithInferredTitle(analysis);
        } else {
            // Fallback to enhanced simple greeting
            formal_greeting = formatEnhancedSimpleGreeting(person);
        }
        
        if constexpr (greeting::config::is_debug_build()) {
            // Add formal context markers in debug mode
            formal_greeting += " [Formal Context]";
            
            // Add timestamp if performance monitoring is enabled
            if constexpr (traits::StrategyTraits<FormalGreetingStrategy>::enable_performance_monitoring) {
                formal_greeting += " [" + getCurrentFormalTimestamp() + "]";
            }
        }
        
        return Expected<std::string, GreetingError>{formal_greeting};
    }

    /**
     * @brief Format greeting preserving existing title
     */
    [[nodiscard]] std::string formatGreetingWithTitle(
        const NameAnalysis& analysis) const noexcept {
        
        std::string greeting = "Good day, ";
        greeting += analysis.title;
        
        if (!analysis.title.ends_with('.')) {
            greeting += ".";
        }
        
        greeting += " ";
        
        if (!analysis.last_name.empty()) {
            greeting += analysis.last_name;
        } else {
            greeting += analysis.first_name;
        }
        
        if (analysis.has_academic_credential) {
            greeting += ", ";
            greeting += analysis.suffix;
        }
        
        greeting += ".";
        
        return greeting;
    }

    /**
     * @brief Format greeting with context-appropriate title inference
     */
    [[nodiscard]] std::string formatGreetingWithInferredTitle(
        const NameAnalysis& analysis) const noexcept {
        
        std::string greeting = "Good day, ";
        
        // Infer appropriate title (simplified heuristic)
        if (analysis.has_academic_credential) {
            greeting += "Dr. ";
        } else {
            // Use gender-neutral formal address
            greeting += "Mx. ";
        }
        
        greeting += analysis.last_name;
        greeting += ".";
        
        return greeting;
    }

    /**
     * @brief Enhanced simple greeting for formal context
     */
    [[nodiscard]] std::string formatEnhancedSimpleGreeting(
        const PersonName& person) const noexcept {
        
        return "Good day, " + person.value() + ".";
    }

    /**
     * @brief Release-mode simple formal formatting
     */
    [[nodiscard]] std::string formatFormalGreetingSimple(
        const PersonName& person) const noexcept {
        
        const auto& name = person.value();
        
        // Quick title detection and formatting
        for (const auto& title : FORMAL_TITLES) {
            if (name.starts_with(title)) {
                return "Good day, " + name + ".";
            }
        }
        
        // Simple formal greeting
        return "Good day, " + name + ".";
    }

    // Validation and utility methods
    [[nodiscard]] bool containsInformalCharacters(std::string_view name) const noexcept {
        // Characters that are inappropriate in formal contexts
        constexpr std::string_view informal_chars = "!@#$%^&*()+={}[]|\\:;\"'<>?,/~`";
        
        return std::any_of(name.begin(), name.end(), [&](char c) {
            return informal_chars.find(c) != std::string_view::npos;
        });
    }

    [[nodiscard]] std::optional<GreetingError> validateTitleFormatting(
        std::string_view name) const noexcept {
        
        // Check for proper title formatting
        for (const auto& title : FORMAL_TITLES) {
            if (name.starts_with(title)) {
                // Ensure proper spacing after title
                if (name.length() > title.length() && name[title.length()] != ' ') {
                    return GreetingError::InvalidName;
                }
            }
        }
        
        return std::nullopt;
    }

    [[nodiscard]] bool isFormalTitle(std::string_view token) const noexcept {
        return std::find(FORMAL_TITLES.begin(), FORMAL_TITLES.end(), token) != FORMAL_TITLES.end();
    }

    [[nodiscard]] bool isAcademicCredential(std::string_view token) const noexcept {
        return std::find(ACADEMIC_TITLES.begin(), ACADEMIC_TITLES.end(), token) != ACADEMIC_TITLES.end();
    }

    [[nodiscard]] std::vector<std::string_view> tokenizeName(
        std::string_view name) const noexcept {
        
        std::vector<std::string_view> tokens;
        std::size_t start = 0;
        
        while (start < name.length()) {
            // Skip whitespace
            while (start < name.length() && std::isspace(name[start])) {
                ++start;
            }
            
            if (start >= name.length()) break;
            
            // Find end of token
            std::size_t end = start;
            while (end < name.length() && !std::isspace(name[end])) {
                ++end;
            }
            
            tokens.emplace_back(name.substr(start, end - start));
            start = end;
        }
        
        return tokens;
    }
    
    // Configuration and debug utilities
    void validateFormalCapabilities() const noexcept {
        // Debug-mode validation of formal greeting capabilities
        if constexpr (greeting::config::is_debug_build()) {
            if (!validateTitleDatabase()) {
                // In a real implementation, this might log to stderr
                // For now, just continue silently
            }
        }
    }

    void initializeDebugContext() const noexcept {
        if constexpr (greeting::config::is_debug_build()) {
            // Initialize debug context for formal strategy
            // Could include logging, metrics initialization, etc.
        }
    }

    [[nodiscard]] bool validateTitleDatabase() const noexcept {
        // Validate that title database is properly initialized
        return !FORMAL_TITLES.empty() && !ACADEMIC_TITLES.empty();
    }

    [[nodiscard]] bool validateFormattingCapabilities() const noexcept {
        // Validate that string formatting capabilities are available
        return true; // Basic implementation - could be enhanced
    }

    [[nodiscard]] std::string getCurrentFormalTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    // Error handling utilities
    [[nodiscard]] Result<GreetingMessage> propagateFormalValidationError(
        GreetingError error, const PersonName& person) const noexcept {
        // For now, just propagate the error
        // Future enhancement: add formal context information in debug builds
        return Expected<GreetingMessage, GreetingError>{error};
    }

    [[nodiscard]] Result<GreetingMessage> propagateNameAnalysisError(
        GreetingError error, const PersonName& person) const noexcept {
        // For now, just propagate the error
        return Expected<GreetingMessage, GreetingError>{error};
    }

    [[nodiscard]] Result<GreetingMessage> propagateFormattingError(
        GreetingError error, const PersonName& person) const noexcept {
        // For now, just propagate the error
        return Expected<GreetingMessage, GreetingError>{error};
    }

    [[nodiscard]] Result<GreetingMessage> createFormalExceptionError(
        const std::string& what, const PersonName& person) const noexcept {
        // Convert exception to appropriate error
        return Expected<GreetingMessage, GreetingError>{GreetingError::InvalidState};
    }
};

// Factory function
[[nodiscard]] std::unique_ptr<IGreetingStrategy> createFormalStrategy() noexcept {
    try {
        return std::make_unique<FormalGreetingStrategy>();
    } catch (...) {
        return nullptr;
    }
}

} // namespace greeting::strategy
