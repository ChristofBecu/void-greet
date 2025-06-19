#include <iostream>
#include <version>

int main() {
    std::cout << "=== C++23 Feature Support Test ===\n";
    std::cout << "Compiler: GCC " << __VERSION__ << "\n";
    std::cout << "C++ Standard: " << __cplusplus << "\n\n";
    
    #ifdef __cpp_deducing_this
    std::cout << "✅ Deducing this: " << __cpp_deducing_this << "\n";
    #else
    std::cout << "❌ Deducing this: Not supported\n";
    #endif
    
    #ifdef __cpp_if_consteval
    std::cout << "✅ if consteval: " << __cpp_if_consteval << "\n";
    #else
    std::cout << "❌ if consteval: Not supported\n";
    #endif
    
    #ifdef __cpp_modules
    std::cout << "✅ Modules: " << __cpp_modules << "\n";
    #else
    std::cout << "❌ Modules: Not supported\n";
    #endif
    
    #ifdef __cpp_multidimensional_subscript
    std::cout << "✅ Multidimensional subscript: " << __cpp_multidimensional_subscript << "\n";
    #else
    std::cout << "❌ Multidimensional subscript: Not supported\n";
    #endif
    
    #ifdef __cpp_lib_expected
    std::cout << "✅ std::expected: " << __cpp_lib_expected << "\n";
    #else
    std::cout << "❌ std::expected: Not supported\n";
    #endif
    
    return 0;
}
