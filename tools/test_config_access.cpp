#include "config/build_config.hpp"
#include <iostream>

int main() {
    constexpr bool debug = greeting::config::is_debug_build();
    std::cout << "Debug: " << debug << std::endl;
    return 0;
}
