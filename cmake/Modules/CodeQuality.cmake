# cmake/Modules/CodeQuality.cmake
#
# Module for enabling code quality tools like sanitizers and coverage.
# These functions/macros are typically called from the root CMakeLists.txt
# or per-target based on cache variables set by CMake presets.

include_guard(GLOBAL)
include(CheckCXXCompilerFlag) # For checking compiler flag support

# Macro to enable sanitizers for a target
macro(enable_sanitizers target_name)
    if(NOT TARGET ${target_name})
        message(WARNING "Target ${target_name} not found. Cannot enable sanitizers.")
        return()
    endif()

    # AddressSanitizer (ASan)
    if(ENABLE_ASAN)
        if(MSVC)
            # MSVC /fsanitize=address (Visual Studio 2019 v16.4+)
            target_compile_options(${target_name} PRIVATE $<$<CONFIG:Debug>:/fsanitize=address>)
            message(STATUS "ASan enabled for MSVC target: ${target_name} (Debug only)")
        else()
            check_cxx_compiler_flag("-fsanitize=address" HAS_ASAN_FLAG)
            if(HAS_ASAN_FLAG)
                target_compile_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls>)
                target_link_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
                message(STATUS "ASan enabled for GCC/Clang target: ${target_name} (Debug only)")
            else()
                message(WARNING "ASan requested for ${target_name} but -fsanitize=address not supported by compiler.")
            endif()
        endif()
    endif()

    # ThreadSanitizer (TSan) - Mutually exclusive with ASan for GCC/Clang
    if(ENABLE_TSAN)
        if(ENABLE_ASAN AND NOT MSVC)
            message(WARNING "TSan requested for ${target_name}, but ASan is also enabled. ASan takes precedence on GCC/Clang. TSAN will NOT be enabled.")
        elseif(MSVC)
            message(WARNING "TSan is not typically supported on MSVC. Skipping for ${target_name}.")
        else()
            check_cxx_compiler_flag("-fsanitize=thread" HAS_TSAN_FLAG)
            if(HAS_TSAN_FLAG)
                target_compile_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-fsanitize=thread>)
                target_link_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-fsanitize=thread>)
                message(STATUS "TSan enabled for GCC/Clang target: ${target_name} (Debug only)")
            else()
                message(WARNING "TSan requested for ${target_name} but -fsanitize=thread not supported by compiler.")
            endif()
        endif()
    endif()

    # UndefinedBehaviorSanitizer (UBSan)
    if(ENABLE_UBSAN)
        if(MSVC)
             message(WARNING "UBSan (-fsanitize=undefined) is not directly supported on MSVC via this flag. Skipping for ${target_name}.")
        else()
            check_cxx_compiler_flag("-fsanitize=undefined" HAS_UBSAN_FLAG)
            if(HAS_UBSAN_FLAG)
                target_compile_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-fsanitize=undefined>)
                target_link_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-fsanitize=undefined>)
                message(STATUS "UBSan enabled for GCC/Clang target: ${target_name} (Debug only)")
            else()
                message(WARNING "UBSan requested for ${target_name} but -fsanitize=undefined not supported by compiler.")
            endif()
        endif()
    endif()
endmacro()

# Function to enable code coverage for a target
function(enable_coverage target_name)
    if(NOT TARGET ${target_name})
        message(WARNING "Target ${target_name} not found. Cannot enable coverage.")
        return()
    endif()

    if(ENABLE_COVERAGE)
        # Check for gcov compatibility (GCC)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            # Ensure BUILD_ENV_HAS_LCOV or BUILD_ENV_HAS_GCOVR from EnvironmentDetection.cmake can be checked here if desired
            # For now, just add flags if GCC. Report generation tools are separate.
            message(STATUS "Enabling GCC coverage flags for target: ${target_name} (Debug only)")
            target_compile_options(${target_name} PRIVATE $<$<CONFIG:Debug>:--coverage -fprofile-arcs -ftest-coverage -O0 -g>)
            target_link_options(${target_name} PRIVATE $<$<CONFIG:Debug>:--coverage>)
        # Check for llvm-cov compatibility (Clang) - Clang not available based on discovery
        # elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND BUILD_ENV_HAS_LLVM_COV) # Need BUILD_ENV_HAS_LLVM_COV
        #     message(STATUS "Enabling Clang coverage flags for target: ${target_name} (Debug only)")
        #     target_compile_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-fprofile-instr-generate -fcoverage-mapping -O0 -g>)
        #     target_link_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-fprofile-instr-generate>)
        else()
            message(WARNING "Coverage requested for ${target_name}, but compiler (${CMAKE_CXX_COMPILER_ID}) is not GNU (or Clang with llvm-cov was not configured).")
        endif()
    endif()
endfunction()

# Global target application helper
function(apply_code_quality_to_all_targets)
    get_property(ALL_PROJECT_TARGETS DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY BUILDSYSTEM_TARGETS)
    if(NOT ALL_PROJECT_TARGETS)
        # Try global property if BUILDSYSTEM_TARGETS is not populated at this point (older CMake or specific invocation)
        get_property(ALL_PROJECT_TARGETS GLOBAL PROPERTY BUILDSYSTEM_TARGETS)
    endif()

    message(STATUS "Available targets for code quality application: ${ALL_PROJECT_TARGETS}")

    foreach(current_target ${ALL_PROJECT_TARGETS})
        # Check if the target is a valid one to apply settings to (executable or library)
        # And not an INTERFACE library or a custom target like 'format'.
        get_target_property(target_type ${current_target} TYPE)
        if(target_type STREQUAL "EXECUTABLE" OR
           target_type STREQUAL "SHARED_LIBRARY" OR
           target_type STREQUAL "STATIC_LIBRARY" OR
           target_type STREQUAL "OBJECT_LIBRARY")

            message(STATUS "Applying code quality settings to target: ${current_target} (Type: ${target_type})")
            enable_sanitizers(${current_target})
            enable_coverage(${current_target})
        else()
            # message(STATUS "Skipping code quality for target: ${current_target} (Type: ${target_type})")
        endif()
    endforeach()
endfunction()
