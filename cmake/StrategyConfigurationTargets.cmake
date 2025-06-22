# ============================================================================
# Strategy Configuration Targets
# 
# CMake configuration for strategy pattern integration with the build
# configuration system.
# ============================================================================

# ============================================================================
# Strategy Configuration Interface Libraries
# ============================================================================

function(setup_strategy_configuration_targets)
    message(STATUS "Setting up strategy configuration targets...")
    
    # Create configuration-aware strategy interface library
    add_library(greeting_strategy_config_interface INTERFACE)
    target_include_directories(greeting_strategy_config_interface INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(greeting_strategy_config_interface INTERFACE
        greeting_build_config
        greeting_strategy_interface
        greeting_validation
    )
    
    # Create strategy configuration manager library
    add_library(greeting_strategy_config_manager STATIC
        src/domain/strategy_configuration_manager.cpp
    )
    target_link_libraries(greeting_strategy_config_manager PUBLIC
        greeting_strategy_config_interface
        greeting_domain
        greeting_strategy_console
        greeting_strategy_formal
        greeting_strategy_casual
    )
    
    # Create strategy factory configuration library
    add_library(greeting_strategy_factory_config INTERFACE)
    target_include_directories(greeting_strategy_factory_config INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(greeting_strategy_factory_config INTERFACE
        greeting_strategy_config_interface
    )
    
    # Create strategy registry application library
    add_library(greeting_strategy_registry_config STATIC
        src/application/config_aware_strategy_registry.cpp
    )
    target_link_libraries(greeting_strategy_registry_config PUBLIC
        greeting_strategy_factory_config
        greeting_strategy_config_manager
    )
    
    # Create unified strategy configuration library
    add_library(greeting_strategy_config INTERFACE)
    target_link_libraries(greeting_strategy_config INTERFACE
        greeting_strategy_config_interface
        greeting_strategy_config_manager
        greeting_strategy_factory_config
        greeting_strategy_registry_config
    )
    
    message(STATUS "✅ Strategy configuration targets created successfully")
    
endfunction()

# ============================================================================
# Strategy Configuration Application
# ============================================================================

function(apply_strategy_configuration TARGET_NAME)
    if(NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR "Target ${TARGET_NAME} does not exist")
    endif()
    
    # Link strategy configuration libraries
    target_link_libraries(${TARGET_NAME} PRIVATE
        greeting_strategy_config
    )
    
    # Apply configuration-specific optimizations
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${TARGET_NAME} PRIVATE
            HELLOWORLD_STRATEGY_DEBUG_ENABLED=1
            HELLOWORLD_STRATEGY_DIAGNOSTICS_ENABLED=1
        )
        message(STATUS "Applied debug strategy configuration to ${TARGET_NAME}")
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        target_compile_definitions(${TARGET_NAME} PRIVATE
            HELLOWORLD_STRATEGY_PROFILE_ENABLED=1
            HELLOWORLD_STRATEGY_MONITORING_ENABLED=1
        )
        message(STATUS "Applied profile strategy configuration to ${TARGET_NAME}")
    else()
        target_compile_definitions(${TARGET_NAME} PRIVATE
            HELLOWORLD_STRATEGY_RELEASE_ENABLED=1
            HELLOWORLD_STRATEGY_OPTIMIZATION_ENABLED=1
        )
        message(STATUS "Applied release strategy configuration to ${TARGET_NAME}")
    endif()
    
endfunction()

# ============================================================================
# Strategy Target Configuration
# ============================================================================

function(configure_strategy_targets)
    message(STATUS "Configuring strategy targets with build configuration...")
    
    # Configure console strategy target
    if(TARGET greeting_strategy_console)
        apply_strategy_configuration(greeting_strategy_console)
    endif()
    
    # Configure formal strategy target
    if(TARGET greeting_strategy_formal)
        apply_strategy_configuration(greeting_strategy_formal)
    endif()
    
    # Configure casual strategy target
    if(TARGET greeting_strategy_casual)
        apply_strategy_configuration(greeting_strategy_casual)
    endif()
    
    # Configure strategy context target
    if(TARGET greeting_strategy_context)
        apply_strategy_configuration(greeting_strategy_context)
    endif()
    
    # Configure main application targets
    if(TARGET hello_world)
        apply_strategy_configuration(hello_world)
    endif()
    
    if(TARGET hello_simple)
        apply_strategy_configuration(hello_simple)
    endif()
    
    # Configure strategy demos
    if(TARGET demo_console_strategy)
        apply_strategy_configuration(demo_console_strategy)
    endif()
    
    if(TARGET demo_formal_strategy)
        apply_strategy_configuration(demo_formal_strategy)
    endif()
    
    if(TARGET demo_casual_strategy)
        apply_strategy_configuration(demo_casual_strategy)
    endif()
    
    if(TARGET demo_strategy_context)
        apply_strategy_configuration(demo_strategy_context)
    endif()
    
    message(STATUS "✅ Strategy targets configured successfully")
    
endfunction()

# ============================================================================
# Strategy Configuration Validation
# ============================================================================

function(validate_strategy_configuration)
    message(STATUS "Validating strategy configuration...")
    
    # Check that required configuration targets exist
    set(required_config_targets
        greeting_build_config
        greeting_debug_config
        greeting_release_config
        greeting_validation
    )
    
    foreach(target ${required_config_targets})
        if(NOT TARGET ${target})
            message(FATAL_ERROR "Required configuration target ${target} not found")
        endif()
    endforeach()
    
    # Check that strategy targets exist
    set(required_strategy_targets
        greeting_strategy_interface
        greeting_strategy_console
        greeting_strategy_formal
        greeting_strategy_casual
    )
    
    foreach(target ${required_strategy_targets})
        if(NOT TARGET ${target})
            message(FATAL_ERROR "Required strategy target ${target} not found")
        endif()
    endforeach()
    
    message(STATUS "✅ Strategy configuration validation completed")
    
endfunction()

# ============================================================================
# Strategy Configuration Setup
# ============================================================================

# Main function to set up all strategy configuration
function(setup_complete_strategy_configuration)
    validate_strategy_configuration()
    setup_strategy_configuration_targets()
    configure_strategy_targets()
endfunction()
