#pragma once

/**
 * @file types.hpp
 * @brief Integration header for greeting types and utilities
 * 
 * This header provides a single point of access to all greeting-related types,
 * error handling utilities, validation functions, and strong type wrappers.
 * It includes all modular components that were refactored from the original
 * monolithic types.hpp file.
 * 
 * Usage:
 *   #include "greeting/types.hpp"
 *   
 * This will make available:
 *   - Error handling: GreetingError, ErrorSeverity, ErrorDomain, error categories
 *   - Expected wrapper: Expected<T, E> for error handling
 *   - Validation utilities: name and message validation functions
 *   - Strong types: PersonName, GreetingMessage
 *   - Consteval validation: compile-time validation support
 */

// Core error system
#include "error_codes.hpp"
#include "error_info.hpp"
#include "error_categories.hpp"

// Error handling wrapper
#include "expected.hpp"

// Validation utilities
#include "validation.hpp"
#include "consteval_validation.hpp"

// Strong type wrappers
#include "person_name.hpp"
#include "greeting_message.hpp"
