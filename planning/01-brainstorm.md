# Clean Code Implementation of "Hello World" in C++

Overview

The initial "Hello World" program in C++ can be refactored to follow clean code principles,
including abstraction, interfaces, and mapping. In this implementation, we will apply these
principles to create a more maintainable, flexible, and scalable program.

Code

Greeter Interface

(cpp)(cpp)
// greeter.h
#ifndef GREETER_H
#define GREETER_H

#include <string>

class Greeter {
public:
    virtual ~Greeter() = default;
    virtual std::string greet(const std::string& name) = 0;
};

#endif  // GREETER_H


Console Greeter Implementation

(cpp)(cpp)
// console_greeter.h
#ifndef CONSOLE_GREETER_H
#define CONSOLE_GREETER_H

#include "greeter.h"
#include <iostream>

class ConsoleGreeter : public Greeter {
public:
    std::string greet(const std::string& name) override;
};

#endif  // CONSOLE_GREETER_H


(cpp)(cpp)
// console_greeter.cpp
#include "console_greeter.h"

std::string ConsoleGreeter::greet(const std::string& name) {
    return "Hello, " + name + "!";
}


Greeter Factory

(cpp)(cpp)
// greeter_factory.h
#ifndef GREETER_FACTORY_H
#define GREETER_FACTORY_H

#include "greeter.h"

class GreeterFactory {
public:
    static Greeter* createGreeter();
};

#endif  // GREETER_FACTORY_H


(cpp)(cpp)
// greeter_factory.cpp
#include "greeter_factory.h"
#include "console_greeter.h"

Greeter* GreeterFactory::createGreeter() {
    return new ConsoleGreeter();
}


Mapping

(cpp)(cpp)
// greeting_mapper.h
#ifndef GREETING_MAPPER_H
#define GREETING_MAPPER_H

#include "greeter.h"
#include <string>
#include <unordered_map>

class GreetingMapper {
public:
    std::string mapGreeting(const std::string& name);
    void addMapping(const std::string& name, const std::string& greeting);
private:
    std::unordered_map<std::string, std::string> mappings_;
};

#endif  // GREETING_MAPPER_H


(cpp)(cpp)
// greeting_mapper.cpp
#include "greeting_mapper.h"

std::string GreetingMapper::mapGreeting(const std::string& name) {
    auto it = mappings_.find(name);
    if (it != mappings_.end()) {
        return it->second;
    }
    Greeter* greeter = GreeterFactory::createGreeter();
    std::string greeting = greeter->greet(name);
    delete greeter;
    return greeting;
}

void GreetingMapper::addMapping(const std::string& name, const std::string& greeting) {
    mappings_[name] = greeting;
}


Main Function

(cpp)(cpp)
// main.cpp
#include "greeting_mapper.h"
#include <iostream>

int main() {
    GreetingMapper mapper;
    mapper.addMapping("John", "Hello, John!");
    std::cout << mapper.mapGreeting("John") << std::endl;
    std::cout << mapper.mapGreeting("Jane") << std::endl;
    return 0;
}


Explanation

• We define a Greeter interface with a pure virtual greet() function.
• We create a ConsoleGreeter class that implements the Greeter interface.
• We introduce a GreeterFactory class that creates instances of Greeter.
• We create a GreetingMapper class that maps names to greetings.
• In the main() function, we use the GreetingMapper to map names to greetings.


Abstraction

• The Greeter interface abstracts the greeting behavior.
• The GreetingMapper class abstracts the mapping of names to greetings.


Interfaces

• The Greeter interface defines the contract for greeting behavior.
• The GreetingMapper class defines the contract for mapping names to greetings.


Mapping

• The GreetingMapper class uses an unordered map to store mappings of names to greetings.


Advantages

• Abstraction: The Greeter interface and GreetingMapper class abstract the greeting behavior
and mapping of names to greetings.
principles to create a more maintainable, flexible, and scalable program.

Code

Greeter Interface

(cpp)(cpp)
// greeter.h
#ifndef GREETER_H
#define GREETER_H

#include <string>

class Greeter {
public:
    virtual ~Greeter() = default;
    virtual std::string greet(const std::string& name) = 0;
};

#endif  // GREETER_H


Console Greeter Implementation

(cpp)(cpp)
// console_greeter.h
#ifndef CONSOLE_GREETER_H
#define CONSOLE_GREETER_H

#include "greeter.h"
#include <iostream>

class ConsoleGreeter : public Greeter {
public:
    std::string greet(const std::string& name) override;
};

#endif  // CONSOLE_GREETER_H


(cpp)(cpp)
// console_greeter.cpp
#include "console_greeter.h"

std::string ConsoleGreeter::greet(const std::string& name) {
    return "Hello, " + name + "!";
}


Greeter Factory

(cpp)(cpp)
// greeter_factory.h
#ifndef GREETER_FACTORY_H
#define GREETER_FACTORY_H

#include "greeter.h"

class GreeterFactory {
public:
    static Greeter* createGreeter();
};

#endif  // GREETER_FACTORY_H


(cpp)(cpp)
// greeter_factory.cpp
#include "greeter_factory.h"
#include "console_greeter.h"

Greeter* GreeterFactory::createGreeter() {
    return new ConsoleGreeter();
}


Mapping

(cpp)(cpp)
// greeting_mapper.h
#ifndef GREETING_MAPPER_H
#define GREETING_MAPPER_H

#include "greeter.h"
#include <string>
#include <unordered_map>

class GreetingMapper {
public:
    std::string mapGreeting(const std::string& name);
    void addMapping(const std::string& name, const std::string& greeting);
private:
    std::unordered_map<std::string, std::string> mappings_;
};

#endif  // GREETING_MAPPER_H


(cpp)(cpp)
// greeting_mapper.cpp
#include "greeting_mapper.h"

std::string GreetingMapper::mapGreeting(const std::string& name) {
    auto it = mappings_.find(name);
    if (it != mappings_.end()) {
        return it->second;
    }
    Greeter* greeter = GreeterFactory::createGreeter();
    std::string greeting = greeter->greet(name);
    delete greeter;
    return greeting;
}

void GreetingMapper::addMapping(const std::string& name, const std::string& greeting) {
    mappings_[name] = greeting;
}


Main Function

(cpp)(cpp)
// main.cpp
#include "greeting_mapper.h"
#include <iostream>

int main() {
    GreetingMapper mapper;
    mapper.addMapping("John", "Hello, John!");
    std::cout << mapper.mapGreeting("John") << std::endl;
    std::cout << mapper.mapGreeting("Jane") << std::endl;
    return 0;
}


Explanation

• We define a Greeter interface with a pure virtual greet() function.
• We create a ConsoleGreeter class that implements the Greeter interface.
• We introduce a GreeterFactory class that creates instances of Greeter.
• We create a GreetingMapper class that maps names to greetings.
• In the main() function, we use the GreetingMapper to map names to greetings.


Abstraction

• The Greeter interface abstracts the greeting behavior.
• The GreetingMapper class abstracts the mapping of names to greetings.


Interfaces

• The Greeter interface defines the contract for greeting behavior.
• The GreetingMapper class defines the contract for mapping names to greetings.


Mapping

• The GreetingMapper class uses an unordered map to store mappings of names to greetings.


Advantages

• Abstraction: The Greeter interface and GreetingMapper class abstract the greeting behavior
and mapping of names to greetings.
• Polymorphism: The ConsoleGreeter class is a concrete implementation of the Greeter interface.
• Factory Pattern: The GreeterFactory class encapsulates the creation of Greeter instances.
• Mapping: The GreetingMapper class maps names to greetings.


Best Practices

• Separate Concerns: Each class has a single responsibility.
• Interface-Based Programming: The Greeter interface and GreetingMapper class define the
contracts.
• Dependency Injection: The main() function depends on the GreetingMapper to map names to
greetings.


By following clean code principles and using abstraction, interfaces, and mapping, we have
refactored the "Hello World" program to be more maintainable, flexible, and scalable.

Key Points

• The program uses an interface (Greeter) to define the greeting behavior.
• The program uses a factory (GreeterFactory) to create instances of Greeter.
• The program uses a mapper (GreetingMapper) to map names to greetings.
• The program separates concerns into different classes.


Example Use Cases

• Learning clean code principles and design patterns.
• Demonstrating abstraction, interfaces, and polymorphism.
• Creating maintainable and scalable software.


Advice

• Use interfaces to define contracts and abstract behavior.
• Use factories to encapsulate object creation.
• Apply dependency injection to reduce coupling.
• Separate concerns into different classes.


By applying these principles, you can write clean, maintainable, and scalable code in C++.

API Documentation

Greeter Interface

Methods

• virtual std::string greet(const std::string& name) = 0: Returns a greeting message for the
given name.


GreeterFactory Class

Methods

• static Greeter* createGreeter(): Creates a Greeter instance.


GreetingMapper Class

Methods

• std::string mapGreeting(const std::string& name): Returns a greeting message for the given
name.
• void addMapping(const std::string& name, const std::string& greeting): Adds a mapping of a
name to a greeting.


Further Improvements

• Use smart pointers (e.g., std::unique_ptr) to manage memory.
• Consider using a dependency injection framework.
• Add error handling and logging mechanisms.


Conclusion

In this implementation, we have demonstrated how to apply clean code principles to the "Hello
World" program in C++. By using abstraction, interfaces, and mapping, we have created a more
maintainable, flexible, and scalable program.