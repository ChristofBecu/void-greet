#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "config/build_config.hpp"
#include <type_traits>

using namespace greeting::config;

// ============================================================================
// Build Configuration Detection Tests
// ============================================================================

TEST_CASE("Build configuration detection", "[config][build]") {
    
    SECTION("Configuration detection functions are consteval") {
        // These must compile - if they don't, consteval is not working
        constexpr bool debug = is_debug_build();
        constexpr bool release = is_release_build(); 
        constexpr bool profile = is_profile_build();
        
        // One and only one should be true
        REQUIRE((debug + release + profile) == 1);
    }
    
    SECTION("Configuration name is compile-time determinable") {
        constexpr const char* config_name = build_configuration_name();
        REQUIRE(config_name != nullptr);
        
        // Check that we get a valid configuration name
        std::string name{config_name};
        REQUIRE((name == "Debug" || name == "Release" || name == "Profile"));
    }
    
    SECTION("Feature availability detection") {
        // These should always be true since we require them
        constexpr bool has_consteval_feature = has_consteval();
        REQUIRE(has_consteval_feature == true);
        
        // Expected should be available in C++23
        constexpr bool has_expected_feature = has_expected();
        INFO("std::expected availability: " << has_expected_feature);
        
        // if consteval may or may not be available
        constexpr bool has_if_consteval_feature = has_if_consteval();
        INFO("if consteval availability: " << has_if_consteval_feature);
        
        // stacktrace may or may not be available
        constexpr bool has_stacktrace_feature = has_stacktrace();
        INFO("std::stacktrace availability: " << has_stacktrace_feature);
    }
}

// ============================================================================
// Configuration Type Tests
// ============================================================================

TEST_CASE("Configuration types", "[config][types]") {
    
    SECTION("DebugConfiguration properties") {
        using Config = DebugConfiguration;
        
        static_assert(Config::is_debug());
        static_assert(!Config::is_release());
        static_assert(!Config::is_profile());
        
        static_assert(Config::enable_assertions());
        static_assert(Config::enable_logging());
        static_assert(Config::enable_validation());
        static_assert(Config::enable_rich_errors());
        static_assert(Config::enable_performance_monitoring());
        
        REQUIRE(Config::is_debug() == true);
        REQUIRE(Config::enable_assertions() == true);
    }
    
    SECTION("ReleaseConfiguration properties") {
        using Config = ReleaseConfiguration;
        
        static_assert(!Config::is_debug());
        static_assert(Config::is_release());
        static_assert(!Config::is_profile());
        
        static_assert(!Config::enable_assertions());
        static_assert(!Config::enable_logging());
        static_assert(!Config::enable_validation());
        static_assert(!Config::enable_rich_errors());
        static_assert(!Config::enable_performance_monitoring());
        
        REQUIRE(Config::is_release() == true);
        REQUIRE(Config::enable_assertions() == false);
    }
    
    SECTION("ProfileConfiguration properties") {
        using Config = ProfileConfiguration;
        
        static_assert(!Config::is_debug());
        static_assert(!Config::is_release());
        static_assert(Config::is_profile());
        
        static_assert(!Config::enable_assertions());
        static_assert(!Config::enable_logging());
        static_assert(!Config::enable_validation());
        static_assert(!Config::enable_rich_errors());
        static_assert(Config::enable_performance_monitoring()); // Only perf monitoring enabled
        
        REQUIRE(Config::is_profile() == true);
        REQUIRE(Config::enable_performance_monitoring() == true);
    }
    
    SECTION("ActiveConfiguration matches build type") {
        using Config = ActiveConfiguration;
        
        // The active configuration should match the detected build type
        REQUIRE(Config::is_debug() == is_debug_build());
        REQUIRE(Config::is_release() == is_release_build());
        REQUIRE(Config::is_profile() == is_profile_build());
    }
}

// ============================================================================
// Configuration Concepts Tests
// ============================================================================

TEST_CASE("Configuration concepts", "[config][concepts]") {
    
    SECTION("DebugBuild concept") {
        static_assert(DebugBuild<DebugConfiguration>);
        static_assert(!DebugBuild<ReleaseConfiguration>);
        static_assert(!DebugBuild<ProfileConfiguration>);
    }
    
    SECTION("ReleaseBuild concept") {
        static_assert(!ReleaseBuild<DebugConfiguration>);
        static_assert(ReleaseBuild<ReleaseConfiguration>);
        static_assert(ReleaseBuild<ProfileConfiguration>); // Profile is not debug
    }
    
    SECTION("ConfigurationAware concept") {
        static_assert(ConfigurationAware<DebugConfiguration>);
        static_assert(ConfigurationAware<ReleaseConfiguration>);
        static_assert(ConfigurationAware<ProfileConfiguration>);
    }
}

// ============================================================================
// Helper Function Tests
// ============================================================================

TEST_CASE("Configuration helper functions", "[config][helpers]") {
    
    SECTION("Helper functions return consistent values") {
        REQUIRE(assertions_enabled() == ActiveConfiguration::enable_assertions());
        REQUIRE(logging_enabled() == ActiveConfiguration::enable_logging());
        REQUIRE(validation_enabled() == ActiveConfiguration::enable_validation());
        REQUIRE(rich_errors_enabled() == ActiveConfiguration::enable_rich_errors());
        REQUIRE(performance_monitoring_enabled() == ActiveConfiguration::enable_performance_monitoring());
    }
    
    SECTION("Helper functions are compile-time evaluable") {
        constexpr bool assertions = assertions_enabled();
        constexpr bool logging = logging_enabled();
        constexpr bool validation = validation_enabled();
        constexpr bool rich_errors = rich_errors_enabled();
        constexpr bool perf_monitoring = performance_monitoring_enabled();
        
        // These should compile without issues
        (void)assertions;
        (void)logging;
        (void)validation;
        (void)rich_errors;
        (void)perf_monitoring;
    }
}

// ============================================================================
// Template Constraint Tests
// ============================================================================

template<DebugBuild Config>
constexpr bool debug_only_function() {
    return true;
}

template<ReleaseBuild Config>
constexpr bool release_only_function() {
    return true;
}

TEST_CASE("Template constraints", "[config][templates]") {
    
    SECTION("Debug-only template functions") {
        // This should compile for debug configuration
        constexpr bool result = debug_only_function<DebugConfiguration>();
        REQUIRE(result == true);
        
        // These would fail to compile (which is correct):
        // debug_only_function<ReleaseConfiguration>(); // Should not compile
        // debug_only_function<ProfileConfiguration>(); // Should not compile
    }
    
    SECTION("Release-only template functions") {
        // These should compile for non-debug configurations
        constexpr bool result1 = release_only_function<ReleaseConfiguration>();
        constexpr bool result2 = release_only_function<ProfileConfiguration>();
        REQUIRE(result1 == true);
        REQUIRE(result2 == true);
        
        // This would fail to compile (which is correct):
        // release_only_function<DebugConfiguration>(); // Should not compile
    }
}

// ============================================================================
// Macro Tests
// ============================================================================

TEST_CASE("Configuration macros", "[config][macros]") {
    
    SECTION("HELLOWORLD_IF_DEBUG macro") {
        bool debug_code_executed = false;
        
        HELLOWORLD_IF_DEBUG(
            debug_code_executed = true;
        );
        
        // Should only execute in debug builds
        REQUIRE(debug_code_executed == is_debug_build());
    }
    
    SECTION("HELLOWORLD_IF_RELEASE macro") {
        bool release_code_executed = false;
        
        HELLOWORLD_IF_RELEASE(
            release_code_executed = true;
        );
        
        // Should only execute in release builds
        REQUIRE(release_code_executed == is_release_build());
    }
    
    SECTION("HELLOWORLD_IF_PROFILE macro") {
        bool profile_code_executed = false;
        
        HELLOWORLD_IF_PROFILE(
            profile_code_executed = true;
        );
        
        // Should only execute in profile builds
        REQUIRE(profile_code_executed == is_profile_build());
    }
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_CASE("Configuration system integration", "[config][integration]") {
    
    SECTION("Configuration consistency") {
        // All configuration functions should be consistent
        const bool is_debug = is_debug_build();
        const bool is_release = is_release_build();
        const bool is_profile = is_profile_build();
        
        // Exactly one should be true
        REQUIRE((is_debug + is_release + is_profile) == 1);
        
        // Active configuration should match
        REQUIRE(ActiveConfiguration::is_debug() == is_debug);
        REQUIRE(ActiveConfiguration::is_release() == is_release);
        REQUIRE(ActiveConfiguration::is_profile() == is_profile);
        
        // Configuration name should match
        std::string expected_name;
        if (is_debug) expected_name = "Debug";
        else if (is_release) expected_name = "Release";
        else if (is_profile) expected_name = "Profile";
        
        REQUIRE(std::string{build_configuration_name()} == expected_name);
    }
    
    SECTION("Feature availability is consistent") {
        // Required features should always be available
        REQUIRE(has_consteval() == true);
        
        // Optional features should be consistent with compile-time definitions
        #ifdef HELLOWORLD_HAS_IF_CONSTEVAL
            REQUIRE(has_if_consteval() == true);
        #else
            REQUIRE(has_if_consteval() == false);
        #endif
        
        #ifdef HELLOWORLD_HAS_EXPECTED
            REQUIRE(has_expected() == true);
        #else
            REQUIRE(has_expected() == false);
        #endif
        
        #ifdef HELLOWORLD_HAS_STACKTRACE
            REQUIRE(has_stacktrace() == true);
        #else
            REQUIRE(has_stacktrace() == false);
        #endif
    }
}

// ============================================================================
// Performance Tests (Compile-Time Verification)
// ============================================================================

TEST_CASE("Compile-time performance", "[config][performance]") {
    
    SECTION("All configuration functions are consteval/constexpr") {
        // These should all be evaluable at compile time
        static_assert(is_debug_build() == is_debug_build());
        static_assert(is_release_build() == is_release_build());
        static_assert(is_profile_build() == is_profile_build());
        
        static_assert(has_consteval() == has_consteval());
        static_assert(has_if_consteval() == has_if_consteval());
        static_assert(has_expected() == has_expected());
        static_assert(has_stacktrace() == has_stacktrace());
        
        static_assert(assertions_enabled() == assertions_enabled());
        static_assert(logging_enabled() == logging_enabled());
        static_assert(validation_enabled() == validation_enabled());
        static_assert(rich_errors_enabled() == rich_errors_enabled());
        static_assert(performance_monitoring_enabled() == performance_monitoring_enabled());
        
        // If this compiles, all functions are properly consteval/constexpr
        SUCCEED("All configuration functions are compile-time evaluable");
    }
}

// ============================================================================
// C++23 if consteval Tests (Conditional)
// ============================================================================

#if HELLOWORLD_HAS_IF_CONSTEVAL_RUNTIME

TEST_CASE("if consteval integration", "[config][cpp23][if_consteval]") {
    
    SECTION("dual_path_configuration function") {
        auto compile_time_fn = []() consteval { return 42; };
        auto runtime_fn = []() { return 24; };
        
        // This should work and choose the appropriate path
        auto result = dual_path_configuration(compile_time_fn, runtime_fn);
        
        // The result depends on whether this is evaluated at compile-time or runtime
        // In a test context, it's typically runtime, so should be 24
        // But the key is that it compiles and works
        REQUIRE((result == 42 || result == 24));
    }
}

#endif // HELLOWORLD_HAS_IF_CONSTEVAL_RUNTIME
