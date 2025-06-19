#include "../include/greeting/types.hpp"
#include <iostream>
#include <cassert>

int main() {
    using namespace greeting;
    
    // Test PersonName construction and basic operations
    PersonName name1 = makePersonName("Alice");
    PersonName name2{std::string{"Bob"}};
    PersonName name3 = makePersonName("Charlie");
    
    std::cout << "Testing PersonName:\n";
    std::cout << "name1: " << name1.view() << " (length: " << name1.length() << ")\n";
    std::cout << "name2: " << name2.value() << " (empty: " << std::boolalpha << name2.empty() << ")\n";
    std::cout << "name3: " << name3.view() << "\n";
    
    // Test GreetingMessage construction and basic operations
    GreetingMessage msg1 = makeGreetingMessage("Hello, World!");
    GreetingMessage msg2{std::string{"Good morning!"}};
    GreetingMessage msg3 = makeGreetingMessage("Good evening!");
    
    std::cout << "\nTesting GreetingMessage:\n";
    std::cout << "msg1: " << msg1.view() << " (length: " << msg1.length() << ")\n";
    std::cout << "msg2: " << msg2.value() << " (empty: " << std::boolalpha << msg2.empty() << ")\n";
    std::cout << "msg3: " << msg3.view() << "\n";
    
    // Test comparison operators
    PersonName alice1 = makePersonName("Alice");
    PersonName alice2 = makePersonName("Alice");
    PersonName bob = makePersonName("Bob");
    
    std::cout << "\nTesting comparisons:\n";
    std::cout << "alice1 == alice2: " << std::boolalpha << (alice1 == alice2) << "\n";
    std::cout << "alice1 != bob: " << std::boolalpha << (alice1 != bob) << "\n";
    std::cout << "alice1 < bob: " << std::boolalpha << (alice1 < bob) << "\n";
    
    // Basic assertions for correctness
    assert(alice1 == alice2);
    assert(alice1 != bob);
    assert(alice1 < bob);  // "Alice" < "Bob" alphabetically
    assert(!name1.empty());
    assert(name1.length() == 5);
    
    std::cout << "\n✅ All tests passed! Strong type aliases work correctly.\n";
    
    return 0;
}
