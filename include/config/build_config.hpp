#pragma once

// ============================================================================
// C++23 Build Configuration System - Core Interface
// 
// This header provides the main configuration interface for compile-time
// build configuration decisions using C++23 features.
// ============================================================================

#include <version>

// Check for required C++23 features
#ifndef __cpp_consteval
    #error "consteval support required for build configuration system"
#endif

#ifndef __cpp_concepts  
    #error "concepts support required for build configuration system"
#endif

// Optional C++23 features
#ifdef __cpp_if_consteval
    #define HELLOWORLD_HAS_IF_CONSTEVAL_RUNTIME 1
#else
    #define HELLOWORLD_HAS_IF_CONSTEVAL_RUNTIME 0
#endif

#ifdef __cpp_lib_stacktrace
    #include <stacktrace>
    #define HELLOWORLD_HAS_STACKTRACE_RUNTIME 1
#else
    #define HELLOWORLD_HAS_STACKTRACE_RUNTIME 0
#endif

namespace greeting::config {

// ============================================================================
// Build Configuration Detection
// ============================================================================

/**
 * @brief Compile-time detection of debug build configuration
 * @return true if debug build, false if release build
 */
consteval bool is_debug_build() noexcept {
    #ifdef DEBUG
        return true;
    #elif defined(HELLOWORLD_DEBUG_BUILD)
        return true;
    #else
        return false;
    #endif
}

/**
 * @brief Compile-time detection of profile build configuration
 * @return true if profile build (RelWithDebInfo)
 */
consteval bool is_profile_build() noexcept {
    #ifdef HELLOWORLD_PROFILE_BUILD
        return true;
    #elif defined(RELWITHDEBINFO)
        return true;
    #else
        return false;
    #endif
}

/**
 * @brief Compile-time detection of release build configuration
 * @return true if release build, false if debug or profile build
 */
consteval bool is_release_build() noexcept {
    // Release is true only if neither debug nor profile
    #ifdef DEBUG
        return false;
    #elif defined(HELLOWORLD_DEBUG_BUILD)
        return false;
    #elif defined(HELLOWORLD_PROFILE_BUILD)
        return false;
    #elif defined(RELWITHDEBINFO)
        return false;
    #else
        return true;
    #endif
}

// ============================================================================
// Feature Availability Detection  
// ============================================================================

/**
 * @brief Compile-time check for consteval availability
 */
consteval bool has_consteval() noexcept {
    #ifdef HELLOWORLD_HAS_CONSTEVAL
        return true;
    #else
        return false;
    #endif
}

/**
 * @brief Compile-time check for if consteval availability
 */
consteval bool has_if_consteval() noexcept {
    #ifdef HELLOWORLD_HAS_IF_CONSTEVAL
        return true;
    #else
        return false;
    #endif
}

/**
 * @brief Compile-time check for std::expected availability
 */
consteval bool has_expected() noexcept {
    #ifdef HELLOWORLD_HAS_EXPECTED
        return true;
    #else
        return false;
    #endif
}

/**
 * @brief Compile-time check for std::stacktrace availability
 */
consteval bool has_stacktrace() noexcept {
    #ifdef HELLOWORLD_HAS_STACKTRACE
        return true;
    #else
        return false;
    #endif
}

// ============================================================================
// Configuration Concepts
// ============================================================================

/**
 * @brief Concept to identify debug build configurations
 */
template<typename T>
concept DebugBuild = requires {
    { T::is_debug() } -> std::same_as<bool>;
    requires T::is_debug() == true;
};

/**
 * @brief Concept to identify release build configurations  
 */
template<typename T>
concept ReleaseBuild = requires {
    { T::is_debug() } -> std::same_as<bool>;
    requires T::is_debug() == false;
};

/**
 * @brief Concept for types that support configuration awareness
 */
template<typename T>
concept ConfigurationAware = requires {
    typename T::configuration_type;
    { T::build_configuration() } -> std::same_as<typename T::configuration_type>;
};

// ============================================================================
// Build Configuration Types
// ============================================================================

/**
 * @brief Debug build configuration type
 */
struct DebugConfiguration {
    static consteval bool is_debug() noexcept { return true; }
    static consteval bool is_release() noexcept { return false; }
    static consteval bool is_profile() noexcept { return false; }
    
    // Debug-specific configuration flags
    static consteval bool enable_assertions() noexcept { return true; }
    static consteval bool enable_logging() noexcept { return true; }
    static consteval bool enable_validation() noexcept { return true; }
    static consteval bool enable_rich_errors() noexcept { return true; }
    static consteval bool enable_performance_monitoring() noexcept { return true; }
    
    using configuration_type = DebugConfiguration;
    static consteval configuration_type build_configuration() noexcept {
        return {};
    }
};

/**
 * @brief Release build configuration type
 */
struct ReleaseConfiguration {
    static consteval bool is_debug() noexcept { return false; }
    static consteval bool is_release() noexcept { return true; }
    static consteval bool is_profile() noexcept { return false; }
    
    // Release-specific configuration flags
    static consteval bool enable_assertions() noexcept { return false; }
    static consteval bool enable_logging() noexcept { return false; }
    static consteval bool enable_validation() noexcept { return false; }
    static consteval bool enable_rich_errors() noexcept { return false; }
    static consteval bool enable_performance_monitoring() noexcept { return false; }
    
    using configuration_type = ReleaseConfiguration;
    static consteval configuration_type build_configuration() noexcept {
        return {};
    }
};

/**
 * @brief Profile build configuration type
 */
struct ProfileConfiguration {
    static consteval bool is_debug() noexcept { return false; }
    static consteval bool is_release() noexcept { return false; }
    static consteval bool is_profile() noexcept { return true; }
    
    // Profile-specific configuration flags (debug symbols + release performance)
    static consteval bool enable_assertions() noexcept { return false; }
    static consteval bool enable_logging() noexcept { return false; }
    static consteval bool enable_validation() noexcept { return false; }
    static consteval bool enable_rich_errors() noexcept { return false; }
    static consteval bool enable_performance_monitoring() noexcept { return true; }
    
    using configuration_type = ProfileConfiguration;
    static consteval configuration_type build_configuration() noexcept {
        return {};
    }
};

// ============================================================================
// Active Configuration Detection
// ============================================================================

/**
 * @brief Get the active build configuration type
 */
using ActiveConfiguration = 
    #ifdef HELLOWORLD_DEBUG_BUILD
        DebugConfiguration;
    #elif defined(HELLOWORLD_PROFILE_BUILD) || defined(RELWITHDEBINFO)
        ProfileConfiguration;
    #else
        ReleaseConfiguration;
    #endif

/**
 * @brief Global configuration instance  
 */
inline constexpr ActiveConfiguration active_config{};

// ============================================================================
// Configuration Helper Functions
// ============================================================================

/**
 * @brief Get current build configuration as string (compile-time)
 */
consteval const char* build_configuration_name() noexcept {
    if constexpr (is_debug_build()) {
        return "Debug";
    } else if constexpr (is_profile_build()) {
        return "Profile";  
    } else {
        return "Release";
    }
}

/**
 * @brief Check if assertions should be enabled
 */
consteval bool assertions_enabled() noexcept {
    return ActiveConfiguration::enable_assertions();
}

/**
 * @brief Check if logging should be enabled
 */
consteval bool logging_enabled() noexcept {
    return ActiveConfiguration::enable_logging();
}

/**
 * @brief Check if validation should be enabled
 */
consteval bool validation_enabled() noexcept {
    return ActiveConfiguration::enable_validation();
}

/**
 * @brief Check if rich error context should be enabled
 */
consteval bool rich_errors_enabled() noexcept {
    return ActiveConfiguration::enable_rich_errors();
}

/**
 * @brief Check if performance monitoring should be enabled
 */
consteval bool performance_monitoring_enabled() noexcept {
    return ActiveConfiguration::enable_performance_monitoring();
}

// ============================================================================
// C++23 if consteval Integration
// ============================================================================

#if HELLOWORLD_HAS_IF_CONSTEVAL_RUNTIME

/**
 * @brief Dual-path configuration function using if consteval
 * 
 * This function demonstrates the C++23 if consteval feature for 
 * compile-time vs runtime path selection based on evaluation context.
 * 
 * @tparam CompileTimeFn Callable for compile-time execution
 * @tparam RuntimeFn Callable for runtime execution  
 * @param compile_time_fn Function to call during compile-time evaluation
 * @param runtime_fn Function to call during runtime evaluation
 * @return Result from appropriate function based on evaluation context
 */
template<typename CompileTimeFn, typename RuntimeFn>
constexpr auto dual_path_configuration(CompileTimeFn&& compile_time_fn, RuntimeFn&& runtime_fn) {
    if consteval {
        // Compile-time path: minimal, fast execution
        return compile_time_fn();
    } else {
        // Runtime path: comprehensive functionality  
        return runtime_fn();
    }
}

#endif // HELLOWORLD_HAS_IF_CONSTEVAL_RUNTIME

} // namespace greeting::config

// ============================================================================
// Configuration Macros for Convenience
// ============================================================================

/**
 * @brief Conditional compilation macro for debug builds
 */
#define HELLOWORLD_IF_DEBUG(code) \
    if constexpr (::greeting::config::is_debug_build()) { code }

/**
 * @brief Conditional compilation macro for release builds
 */
#define HELLOWORLD_IF_RELEASE(code) \
    if constexpr (::greeting::config::is_release_build()) { code }

/**
 * @brief Conditional compilation macro for profile builds
 */
#define HELLOWORLD_IF_PROFILE(code) \
    if constexpr (::greeting::config::is_profile_build()) { code }

/**
 * @brief Assertion macro that compiles away in release builds
 */
#define HELLOWORLD_ASSERT(condition) \
    HELLOWORLD_IF_DEBUG( \
        do { \
            if (!(condition)) { \
                /* In a full implementation, this would integrate with error handling */ \
                throw std::logic_error("Assertion failed: " #condition); \
            } \
        } while(false) \
    )

/**
 * @brief Debug logging macro that compiles away in release builds
 */
#define HELLOWORLD_DEBUG_LOG(message) \
    HELLOWORLD_IF_DEBUG( \
        /* In a full implementation, this would integrate with logging system */ \
        std::cerr << "[DEBUG] " << message << std::endl \
    )
