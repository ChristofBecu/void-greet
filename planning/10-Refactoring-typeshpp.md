# Refactoring `types.hpp` for Improved Structure and Maintainability

## 🧐 **Current Structure Analysis**

Based on my examination, types.hpp currently contains:

1. **Error Enumerations** (~200 lines)
   - `ErrorSeverity` enum (7 lines)
   - `ErrorDomain` enum (10 lines)
   - `GreetingError` enum (35+ error codes, ~60 lines)
   - Error info lookup functions (~130 lines)

2. **Expected<T,E> Implementation** (~200 lines)
   - Result type alias
   - Custom Expected wrapper class

3. **Compile-time Validation** (~210 lines)
   - `consteval_validation` namespace
   - Multiple validation functions using `if consteval`

4. **Strong Type Classes** (~300 lines)
   - `PersonName` class (~150 lines)
   - `GreetingMessage` class (~150 lines)

5. **Factory Functions** (~20 lines)
   - `makePersonName` and `makeGreetingMessage`

## 📋 **Refactoring Plan: Breaking Down types.hpp**

### **Phase 1: Error System Separation**

*Target: ~300 lines → multiple files under 100 lines*

#### **1.1 Error Enumerations**

```cpp
// include/greeting/error_codes.hpp (~60 lines)
- ErrorSeverity enum
- ErrorDomain enum  
- GreetingError enum
```

#### **1.2 Error Information System**

```cpp
// include/greeting/error_info.hpp (~80 lines)
- ErrorInfo struct
- Error lookup functions
- toString functions
```

#### **1.3 Error Categories**

```cpp
// include/greeting/error_categories.hpp (~70 lines)
- GreetingErrorCategory class
- std::error_code integration
- Helper functions
```

### **Phase 2: Expected<T,E> Separation**

*Target: ~200 lines → single focused file*

#### **2.1 Expected Wrapper**

```cpp
// include/greeting/expected.hpp (~200 lines)
- Result type alias
- Expected<T,E> implementation
- Monadic operations (future)
```

### **Phase 3: Validation System Separation**

*Target: ~210 lines → multiple focused files*

#### **3.1 Validation Interface**

```cpp
// include/greeting/validation.hpp (~40 lines)
- Validation function declarations
- Common validation types
- Interface definitions
```

#### **3.2 Compile-time Validation**

```cpp
// include/greeting/consteval_validation.hpp (~170 lines)
- consteval_validation namespace
- if consteval implementations
- Compile-time optimized functions
```

### **Phase 4: Strong Types Separation**

*Target: ~300 lines → two focused files*

#### **4.1 PersonName Type**

```cpp
// include/greeting/person_name.hpp (~150 lines)
- PersonName class
- PersonName validation
- PersonName factory function
```

#### **4.2 GreetingMessage Type**

```cpp
// include/greeting/greeting_message.hpp (~150 lines)
- GreetingMessage class  
- GreetingMessage validation
- GreetingMessage factory function
```

### **Phase 5: Integration Header**

*Target: Main types.hpp becomes a lightweight coordinator*

#### **5.1 Main Types Header**

```cpp
// include/greeting/types.hpp (~30 lines)
- Include all component headers
- Namespace aliases (if needed)
- Main convenience exports
```

## 🏗️ **Detailed Refactoring Implementation Plan**

### **Step 1: Create Error System Files**

1. **Create `include/greeting/error_codes.hpp`**
   - Move all enum definitions
   - Keep focused on declarations only
   - ~60 lines total

2. **Create `include/greeting/error_info.hpp`**
   - Move ErrorInfo struct and lookup functions
   - Implement toString functions
   - ~80 lines total

3. **Create `include/greeting/error_categories.hpp`**
   - Move std::error_code integration
   - Create error category classes
   - ~70 lines total

### **Step 2: Extract Expected Implementation**

4. **Create `include/greeting/expected.hpp`**
   - Move entire Expected<T,E> implementation
   - Keep as single focused file
   - Add forward declarations as needed
   - ~200 lines total

### **Step 3: Separate Validation Logic**

5. **Create `include/greeting/validation.hpp`**
   - Common validation interfaces
   - Function declarations
   - ~40 lines total

6. **Create `include/greeting/consteval_validation.hpp`**
   - Move entire consteval_validation namespace
   - All if consteval implementations
   - ~170 lines total

### **Step 4: Split Strong Types**

7. **Create `include/greeting/person_name.hpp`**
   - Move PersonName class entirely
   - Include its validation and factory
   - ~150 lines total

8. **Create `include/greeting/greeting_message.hpp`**
   - Move GreetingMessage class entirely  
   - Include its validation and factory
   - ~150 lines total

### **Step 5: Update Main Header**

9. **Refactor types.hpp**
   - Include all new headers
   - Provide convenience exports
   - Keep under 50 lines total

## ⚡ **Implementation Strategy**

### **Dependencies & Order**

1. Error system files (independent)
2. Expected implementation (depends on errors)
3. Validation system (depends on errors)
4. Strong types (depend on validation, expected, errors)
5. Main types header (includes all)

### **Testing Strategy**

- Update test includes incrementally
- Ensure no functionality changes
- Verify all 857 assertions still pass
- Update CMakeLists.txt if needed

### **Benefits of This Refactoring**

✅ **Follows Clean Code Principles:**

- Each file < 300 LOC (most < 200 LOC)
- Single responsibility per file
- Clear, focused interfaces

✅ **Improves Maintainability:**

- Easier to locate specific functionality
- Faster compilation of individual components
- Better separation of concerns

✅ **Enables Future Growth:**

- Easy to add new error types
- Simple to extend validation logic
- Clear extension points for new features

Would you like me to proceed with implementing this refactoring plan? I'll start with Step 1 (Error System Files) and work through each step systematically, ensuring that tests continue to pass at each stage.
