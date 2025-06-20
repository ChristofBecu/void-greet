#include <iostream>
#include <string>
#include <cctype>

int main() {
    std::string test_string = "Hello\x00World";
    
    std::cout << "Test string length: " << test_string.length() << std::endl;
    std::cout << "String contents: ";
    for (size_t i = 0; i < test_string.length(); ++i) {
        char c = test_string[i];
        if (c == '\0') {
            std::cout << "\\0";
        } else if (std::isprint(static_cast<unsigned char>(c))) {
            std::cout << c;
        } else {
            std::cout << "\\x" << std::hex << static_cast<int>(static_cast<unsigned char>(c));
        }
    }
    std::cout << std::endl;
    
    // Test character by character
    for (size_t i = 0; i < test_string.length(); ++i) {
        char c = test_string[i];
        bool is_printable = std::isprint(static_cast<unsigned char>(c));
        bool is_space = std::isspace(static_cast<unsigned char>(c));
        std::cout << "Character " << i << ": '" << (is_printable ? c : '?') << "' "
                  << "(ASCII " << static_cast<int>(static_cast<unsigned char>(c)) << ") "
                  << "printable=" << is_printable << " space=" << is_space << std::endl;
    }
    
    return 0;
}
