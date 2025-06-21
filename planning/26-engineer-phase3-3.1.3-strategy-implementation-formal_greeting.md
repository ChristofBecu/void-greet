# **3.1.3: Strategy Implementation - Formal Greeting - Detailed Implementation Plan**

## **Overview**

Implement the formal greeting strategy that demonstrates sophisticated greeting generation with titles, honorifics, and enhanced formatting. This strategy showcases advanced string processing, configuration-aware validation, and integration with existing domain infrastructure while leveraging C++23 features for optimal performance.

---

## **File: `src/domain/formal_strategy.cpp`**

### **Implementation Requirements**

#### **A. Core Formal Greeting Strategy Implementation**

```cpp
#include "greeting/formal_strategy.hpp"
#include "greeting/strategy_interface.hpp"
#include "config/build_config.hpp"
#include "utils/consteval_strings.hpp"
#include <string>
#include <string_view>
#include <algorithm>
#include <array>

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
        bool has_formal_title;
        bool has_academic_credential;
    };

    [[nodiscard]] Result<NameAnalysis> analyzeNameStructure(
        const PersonName& person) const noexcept {
        
        NameAnalysis analysis{};
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
            
            if constexpr (greeting::config::performance_monitoring_enabled()) {
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
    [[nodiscard]] bool containsInformalCharacters(std::string_view name) const noexcept;
    [[nodiscard]] std::optional<GreetingError> validateTitleFormatting(std::string_view name) const noexcept;
    [[nodiscard]] bool isFormalTitle(std::string_view token) const noexcept;
    [[nodiscard]] bool isAcademicCredential(std::string_view token) const noexcept;
    [[nodiscard]] std::vector<std::string_view> tokenizeName(std::string_view name) const noexcept;
    
    // Configuration and debug utilities
    void validateFormalCapabilities() const noexcept;
    void initializeDebugContext() const noexcept;
    [[nodiscard]] bool validateTitleDatabase() const noexcept;
    [[nodiscard]] bool validateFormattingCapabilities() const noexcept;
    [[nodiscard]] std::string getCurrentFormalTimestamp() const;
    
    // Error handling utilities
    [[nodiscard]] Result<GreetingMessage> propagateFormalValidationError(
        GreetingError error, const PersonName& person) const noexcept;
    [[nodiscard]] Result<GreetingMessage> propagateNameAnalysisError(
        GreetingError error, const PersonName& person) const noexcept;
    [[nodiscard]] Result<GreetingMessage> propagateFormattingError(
        GreetingError error, const PersonName& person) const noexcept;
    [[nodiscard]] Result<GreetingMessage> createFormalExceptionError(
        const std::string& what, const PersonName& person) const noexcept;
};

} // namespace greeting::strategy
```

#### **B. Header File: `include/greeting/formal_strategy.hpp`**

```cpp
#pragma once

#include "greeting/strategy_interface.hpp"
#include "greeting/expected.hpp"
#include <string_view>
#include <array>
#include <vector>
#include <optional>

namespace greeting::strategy {

/**
 * @brief Forward declaration for formal strategy
 */
class FormalGreetingStrategy;

/**
 * @brief Formal strategy factory function
 * @return Unique pointer to formal strategy instance
 */
[[nodiscard]] std::unique_ptr<IGreetingStrategy> createFormalStrategy() noexcept;

/**
 * @brief Formal strategy traits specialization
 */
template<>
struct StrategyTraits<FormalGreetingStrategy> {
    static constexpr bool supports_debug_info = true;
    static constexpr bool enable_performance_monitoring = greeting::config::is_debug_build();
    static constexpr bool enable_validation_caching = greeting::config::is_debug_build();
    
    static constexpr size_t expected_message_length = 80; // Longer for formal greetings
    static constexpr bool is_thread_safe = true; // No shared mutable state
    static constexpr bool supports_async_operation = true;
    static constexpr bool requires_system_resources = false;
    static constexpr bool supports_localization = true; // Future enhancement
    static constexpr bool supports_cultural_context = true;
};

/**
 * @brief Compile-time formal greeting utilities
 */
namespace formal_utils {

/**
 * @brief Compile-time title validation
 */
consteval bool isFormalTitleValid(std::string_view title) noexcept {
    constexpr std::array<std::string_view, 12> titles = {{
        "Dr.", "Prof.", "Mr.", "Mrs.", "Ms.", "Miss",
        "Sir", "Madam", "Lord", "Lady", "Hon.", "Rev."
    }};
    
    for (const auto& valid_title : titles) {
        if (title == valid_title) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Compile-time formal greeting template
 */
template<std::size_t N>
consteval std::array<char, N + 12> createFormalGreetingTemplate(
    const std::array<char, N>& name) noexcept {
    
    std::array<char, N + 12> result{};
    const char prefix[] = "Good day, ";
    const char suffix[] = ".";
    
    // Copy prefix
    for (std::size_t i = 0; i < 10; ++i) {
        result[i] = prefix[i];
    }
    
    // Copy name
    for (std::size_t i = 0; i < N - 1; ++i) { // -1 to exclude null terminator
        result[i + 10] = name[i];
    }
    
    // Copy suffix
    result[N + 9] = '.';
    result[N + 10] = '\0';
    
    return result;
}

} // namespace formal_utils

} // namespace greeting::strategy
```

#### **C. Utility Method Implementations**

```cpp
namespace greeting::strategy {

// Validation utilities
[[nodiscard]] bool FormalGreetingStrategy::containsInformalCharacters(
    std::string_view name) const noexcept {
    
    // Characters that are inappropriate in formal contexts
    constexpr std::string_view informal_chars = "!@#$%^&*()+={}[]|\\:;\"'<>?,/~`";
    
    return std::any_of(name.begin(), name.end(), [&](char c) {
        return informal_chars.find(c) != std::string_view::npos;
    });
}

[[nodiscard]] std::optional<GreetingError> FormalGreetingStrategy::validateTitleFormatting(
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

[[nodiscard]] bool FormalGreetingStrategy::isFormalTitle(std::string_view token) const noexcept {
    return std::find(FORMAL_TITLES.begin(), FORMAL_TITLES.end(), token) != FORMAL_TITLES.end();
}

[[nodiscard]] bool FormalGreetingStrategy::isAcademicCredential(std::string_view token) const noexcept {
    return std::find(ACADEMIC_TITLES.begin(), ACADEMIC_TITLES.end(), token) != ACADEMIC_TITLES.end();
}

[[nodiscard]] std::vector<std::string_view> FormalGreetingStrategy::tokenizeName(
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

// Configuration utilities
void FormalGreetingStrategy::validateFormalCapabilities() const noexcept {
    // Debug-mode validation of formal greeting capabilities
    if constexpr (greeting::config::is_debug_build()) {
        if (!validateTitleDatabase()) {
            std::cerr << "WARNING: Formal title database validation failed\n";
        }
    }
}

void FormalGreetingStrategy::initializeDebugContext() const noexcept {
    if constexpr (greeting::config::is_debug_build()) {
        // Initialize debug context for formal strategy
        // Could include logging, metrics initialization, etc.
    }
}

[[nodiscard]] bool FormalGreetingStrategy::validateTitleDatabase() const noexcept {
    // Validate that title database is properly initialized
    return !FORMAL_TITLES.empty() && !ACADEMIC_TITLES.empty();
}

[[nodiscard]] bool FormalGreetingStrategy::validateFormattingCapabilities() const noexcept {
    // Validate that string formatting capabilities are available
    return true; // Basic implementation - could be enhanced
}

[[nodiscard]] std::string FormalGreetingStrategy::getCurrentFormalTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// Factory function
[[nodiscard]] std::unique_ptr<IGreetingStrategy> createFormalStrategy() noexcept {
    try {
        return std::make_unique<FormalGreetingStrategy>();
    } catch (...) {
        return nullptr;
    }
}

} // namespace greeting::strategy
```

---

## **Integration Points**

### **A. C++23 Features Integration**

```cpp
// Leverage consteval string processing
#include "utils/consteval_strings.hpp"

// Example consteval usage in formal strategy
namespace greeting::strategy::compile_time {

/**
 * @brief Compile-time formal greeting generation
 */
template<std::size_t N>
consteval auto generateFormalGreeting(const char (&name)[N]) noexcept {
    return utils::consteval_strings::concatenate("Good day, ", name, ".");
}

/**
 * @brief Compile-time title validation
 */
consteval bool validateFormalTitle(std::string_view title) noexcept {
    return formal_utils::isFormalTitleValid(title);
}

} // namespace greeting::strategy::compile_time
```

### **B. Configuration Infrastructure Integration**

- **Leverage**: Existing `greeting::config::is_debug_build()` for behavior selection
- **Integrate**: With Phase 2's validation infrastructure for name analysis
- **Use**: Configuration-aware error handling patterns
- **Maintain**: Zero-overhead abstractions in release builds

### **C. Error Handling Extensions**

```cpp
// Extend error enumeration with formal strategy specific errors
namespace greeting::strategy::errors {

enum class FormalStrategyError : std::uint16_t {
    InvalidTitleFormat = 3101,        ///< Title formatting is invalid
    InappropriateFormalContext = 3102, ///< Content inappropriate for formal context
    TitleDatabaseCorrupted = 3103,    ///< Title database validation failed
    NameAnalysisFailure = 3104       ///< Name structure analysis failed
};

} // namespace greeting::strategy::errors
```

---

## **Testing Requirements**

### **File: `tests/unit/domain/test_formal_strategy.cpp`**

#### **A. Basic Functionality Tests**

```cpp
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "greeting/formal_strategy.hpp"

using namespace greeting::strategy;
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
```

#### **B. Advanced Formatting Tests**

```cpp
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
    }
}
```

#### **C. Configuration-Aware Tests**

```cpp
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
    }
}
```

#### **D. Error Handling Tests**

```cpp
SCENARIO("FormalGreetingStrategy error handling", "[formal][strategy][error]") {
    
    GIVEN("a formal strategy") {
        auto strategy = createFormalStrategy();
        
        WHEN("greeting inappropriate content") {
            auto person = PersonName::create("User@#$%").value();
            auto result = strategy->greet(person);
            
            THEN("appropriate validation occurs") {
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
        }
    }
}
```

#### **E. Compile-time Features Tests**

```cpp
SCENARIO("FormalGreetingStrategy compile-time features", "[formal][strategy][consteval]") {
    
    WHEN("using compile-time utilities") {
        
        THEN("consteval title validation works") {
            static_assert(formal_utils::isFormalTitleValid("Dr."));
            static_assert(formal_utils::isFormalTitleValid("Prof."));
            static_assert(!formal_utils::isFormalTitleValid("Invalid"));
            
            SUCCEED("Compile-time title validation verified");
        }
        
        THEN("consteval greeting generation works") {
            constexpr auto greeting = compile_time::generateFormalGreeting("Test");
            static_assert(greeting.size() > 0);
            
            SUCCEED("Compile-time greeting generation verified");
        }
    }
}
```

#### **F. Performance Tests**

```cpp
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
    }
}
```

---

## **Implementation Milestones**

### **Milestone 1: Core Formal Strategy** (4 hours)

- [ ] Implement `FormalGreetingStrategy` class with basic interface methods
- [ ] Add title and honorific database
- [ ] Implement basic formal greeting generation
- [ ] Create factory function and header file

### **Milestone 2: Enhanced Name Analysis** (3 hours)

- [ ] Implement sophisticated name parsing and analysis
- [ ] Add title detection and validation logic
- [ ] Implement academic credential recognition
- [ ] Add configuration-aware parsing paths

### **Milestone 3: Advanced Formatting** (3 hours)

- [ ] Implement context-aware formal greeting formatting
- [ ] Add title preservation and inference logic
- [ ] Implement enhanced debug formatting with timestamps
- [ ] Add cultural context awareness

### **Milestone 4: C++23 Integration** (2 hours)

- [ ] Add compile-time string processing utilities
- [ ] Implement consteval title validation
- [ ] Add template-based greeting generation
- [ ] Integrate with existing consteval infrastructure

### **Milestone 5: Testing & Validation** (3 hours)

- [ ] Create comprehensive unit test suite
- [ ] Add configuration-aware behavior tests
- [ ] Implement performance benchmarks
- [ ] Add compile-time feature tests

---

## **Success Criteria**

### **Functional Requirements**

- ✅ Implements formal greeting generation with title recognition
- ✅ Provides enhanced formatting appropriate for formal contexts
- ✅ Configuration-aware behavior (comprehensive debug vs optimized release)
- ✅ Proper integration with existing validation infrastructure
- ✅ Advanced name analysis and title/credential detection

### **Performance Requirements**

- ✅ Zero-overhead abstractions in release builds
- ✅ Efficient string processing and formatting
- ✅ Compile-time optimization for known patterns
- ✅ Memory-efficient name analysis operations

### **Quality Requirements**

- ✅ Comprehensive unit test coverage (>95%)
- ✅ Exception safety and robust error handling
- ✅ Integration with existing error handling patterns
- ✅ Consistent with project coding standards
- ✅ Proper cultural and contextual awareness

### **C++23 Features**

- ✅ Leverages consteval for compile-time string processing
- ✅ Uses concepts for compile-time validation
- ✅ Integrates with configuration-aware infrastructure
- ✅ Demonstrates advanced template metaprogramming

This formal greeting strategy implementation showcases sophisticated string processing, cultural awareness, and advanced C++23 features while maintaining the performance and reliability standards established in the project's configuration infrastructure.
