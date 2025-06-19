// Enhanced feature detection (save as cpp23_feature_test.cpp)
#include <version>
#include <iostream>
#include <iomanip>

struct FeatureTest {
    const char* name;
    bool supported;
    long value;
};

int main() {
    std::cout << "=== C++23 Feature Support Matrix ===\n";
    std::cout << "Compiler: " << __VERSION__ << "\n";
    std::cout << "C++ Standard: " << __cplusplus << "\n\n";
    
    FeatureTest features[] = {
        // Core language features
        {"Modules", 
         #ifdef __cpp_modules
         true, __cpp_modules
         #else
         false, 0
         #endif
        },
        {"Deducing this", 
         #ifdef __cpp_deducing_this
         true, __cpp_deducing_this
         #else
         false, 0
         #endif
        },
        {"if consteval", 
         #ifdef __cpp_if_consteval
         true, __cpp_if_consteval
         #else
         false, 0
         #endif
        },
        {"Multidimensional subscript", 
         #ifdef __cpp_multidimensional_subscript
         true, __cpp_multidimensional_subscript
         #else
         false, 0
         #endif
        },
        // Library features
        {"std::expected", 
         #ifdef __cpp_lib_expected
         true, __cpp_lib_expected
         #else
         false, 0
         #endif
        },
        {"std::generator", 
         #ifdef __cpp_lib_generator
         true, __cpp_lib_generator
         #else
         false, 0
         #endif
        },
        {"std::stacktrace", 
         #ifdef __cpp_lib_stacktrace
         true, __cpp_lib_stacktrace
         #else
         false, 0
         #endif
        }
    };
    
    for (const auto& feature : features) {
        std::cout << std::left << std::setw(30) << feature.name 
                  << (feature.supported ? "✅ " : "❌ ");
        if (feature.supported) {
            std::cout << feature.value;
        }
        std::cout << "\n";
    }
    
    return 0;
}