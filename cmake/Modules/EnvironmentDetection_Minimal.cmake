# cmake/Modules/EnvironmentDetection_Minimal.cmake
#
# Module to detect available build and analysis tools in the environment.
# Sets cache variables that can be used by other CMake scripts or presets.
# This is a MINIMAL version focusing on essential compiler feature checks.

include_guard(GLOBAL)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)

function(detect_build_environment_minimal)
    message(STATUS "--- Minimal Build Environment Capability Detection ---")

    # CMake Version Check
    if(CMAKE_VERSION VERSION_LESS "3.16.0")
        message(WARNING "CMake version is ${CMAKE_VERSION}, which is less than the recommended 3.16.0. Some features might not be available or work as expected.")
    else()
        message(STATUS "CMake version: ${CMAKE_VERSION} (>= 3.16.0, OK)")
    endif()

    # Compiler ID and Version (already available as CMAKE_CXX_COMPILER_ID, CMAKE_CXX_COMPILER_VERSION)
    message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
    message(STATUS "Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

    set(BUILD_ENV_COMPILER_IS_GCC OFF CACHE BOOL "GCC Compiler Detected" FORCE)
    set(BUILD_ENV_COMPILER_IS_CLANG OFF CACHE BOOL "Clang Compiler Detected" FORCE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(BUILD_ENV_COMPILER_IS_GCC ON CACHE BOOL "GCC Compiler Detected" FORCE)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(BUILD_ENV_COMPILER_IS_CLANG ON CACHE BOOL "Clang Compiler Detected" FORCE)
    endif()

    # Compiler Capability Detection (C++ Standards)
    message(STATUS "Detecting supported C++ standards for ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}...")

    check_cxx_compiler_flag("-std=c++17" COMPILER_SUPPORTS_CXX17)
    check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_CXX20)
    check_cxx_compiler_flag("-std=c++23" COMPILER_SUPPORTS_CXX23)

    set(DETECTED_CXX_STANDARD "17") # Default to 17
    if(COMPILER_SUPPORTS_CXX23)
        set(DETECTED_CXX_STANDARD "23")
        message(STATUS "Compiler supports C++23.")
    elseif(COMPILER_SUPPORTS_CXX20)
        set(DETECTED_CXX_STANDARD "20")
        message(STATUS "Compiler supports C++20.")
    elseif(COMPILER_SUPPORTS_CXX17)
        message(STATUS "Compiler supports C++17.")
    else()
        message(WARNING "Compiler does not appear to support C++17 with -std=c++17. Project might use C++17 anyway; build could fail.")
    endif()

    set(BUILD_ENV_CXX_STANDARD ${DETECTED_CXX_STANDARD} CACHE STRING "Highest C++ standard supported by compiler and intended for project" FORCE)
    message(STATUS "Project will target C++ Standard: ${BUILD_ENV_CXX_STANDARD} (based on compiler capabilities).")

    # Specific C++ Feature Detection (using check_cxx_source_compiles)
    # These checks will use the CMAKE_CXX_STANDARD set by the project, or an explicit flag if needed.

    set(CMAKE_REQUIRED_QUIET ${CMAKE_QUIET}) # Suppress verbose output from these checks

    message(STATUS "Checking for C++20 Concepts support (using C++${BUILD_ENV_CXX_STANDARD})...")
    set(CXX_CONCEPTS_CODE "
        #include <concepts>
        #include <type_traits>
        template<typename T>
        concept Integral = std::is_integral_v<T>;
        int main() { Integral auto i = 5; return i-5; }")
    check_cxx_source_compiles("${CXX_CONCEPTS_CODE}" BUILD_ENV_HAS_CONCEPTS_FLAG_UNAWARE)

    if(NOT BUILD_ENV_HAS_CONCEPTS_FLAG_UNAWARE AND (BUILD_ENV_CXX_STANDARD STREQUAL "20" OR BUILD_ENV_CXX_STANDARD STREQUAL "23"))
        message(STATUS "Retrying C++20 Concepts check with -std=c++${BUILD_ENV_CXX_STANDARD}...")
        check_cxx_source_compiles("${CXX_CONCEPTS_CODE}" BUILD_ENV_HAS_CONCEPTS CMAKE_FLAGS "-std=c++${BUILD_ENV_CXX_STANDARD}")
    else()
        set(BUILD_ENV_HAS_CONCEPTS ${BUILD_ENV_HAS_CONCEPTS_FLAG_UNAWARE})
    endif()

    if(BUILD_ENV_HAS_CONCEPTS)
        message(STATUS "C++20 Concepts: Supported")
    else()
        message(STATUS "C++20 Concepts: Not supported (even with explicit -std=c++${BUILD_ENV_CXX_STANDARD} if tried)")
    endif()
    set(BUILD_ENV_HAS_CONCEPTS ${BUILD_ENV_HAS_CONCEPTS} CACHE BOOL "Compiler supports C++20 Concepts" FORCE)


    message(STATUS "Checking for C++20 Coroutines support (using C++${BUILD_ENV_CXX_STANDARD})...")
    set(CXX_COROUTINES_CODE "
        #include <coroutine>
        struct task {
            struct promise_type {
                task get_return_object() { return {}; }
                std::suspend_never initial_suspend() { return {}; }
                std::suspend_never final_suspend() noexcept { return {}; }
                void return_void() {}
                void unhandled_exception() {}
            };
        };
        int main() { task t; return 0; }")
    check_cxx_source_compiles("${CXX_COROUTINES_CODE}" BUILD_ENV_HAS_COROUTINES_FLAG_UNAWARE)

    if(NOT BUILD_ENV_HAS_COROUTINES_FLAG_UNAWARE AND (BUILD_ENV_CXX_STANDARD STREQUAL "20" OR BUILD_ENV_CXX_STANDARD STREQUAL "23"))
        message(STATUS "Retrying C++20 Coroutines check with -std=c++${BUILD_ENV_CXX_STANDARD}...")
        check_cxx_source_compiles("${CXX_COROUTINES_CODE}" BUILD_ENV_HAS_COROUTINES CMAKE_FLAGS "-std=c++${BUILD_ENV_CXX_STANDARD}")
    else()
        set(BUILD_ENV_HAS_COROUTINES ${BUILD_ENV_HAS_COROUTINES_FLAG_UNAWARE})
    endif()

    if(BUILD_ENV_HAS_COROUTINES)
        message(STATUS "C++20 Coroutines: Supported (basic check)")
    else()
        message(STATUS "C++20 Coroutines: Not supported (basic check, even with explicit -std=c++${BUILD_ENV_CXX_STANDARD} if tried)")
    endif()
    set(BUILD_ENV_HAS_COROUTINES ${BUILD_ENV_HAS_COROUTINES} CACHE BOOL "Compiler supports C++20 Coroutines" FORCE)

    # Compiler-Specific Optimization Detection (for GCC)
    if(BUILD_ENV_COMPILER_IS_GCC)
        message(STATUS "Detecting GCC-specific optimization flags...")
        check_cxx_compiler_flag("-march=native" GCC_HAS_MARCH_NATIVE)
        set(BUILD_ENV_GCC_HAS_MARCH_NATIVE ${GCC_HAS_MARCH_NATIVE} CACHE BOOL "GCC supports -march=native" FORCE)
        if(GCC_HAS_MARCH_NATIVE) message(STATUS "GCC: -march=native supported"); else() message(STATUS "GCC: -march=native not supported"); endif()

        check_cxx_compiler_flag("-flto" GCC_HAS_LTO)
        set(BUILD_ENV_GCC_HAS_LTO ${GCC_HAS_LTO} CACHE BOOL "GCC supports -flto (Link Time Optimization)" FORCE)
        if(GCC_HAS_LTO) message(STATUS "GCC: -flto supported"); else() message(STATUS "GCC: -flto not supported"); endif()

        check_cxx_compiler_flag("-fdiagnostics-color=always" GCC_HAS_COLOR_DIAG)
        set(BUILD_ENV_GCC_HAS_COLOR_DIAG ${GCC_HAS_COLOR_DIAG} CACHE BOOL "GCC supports -fdiagnostics-color=always" FORCE)
        if(GCC_HAS_COLOR_DIAG) message(STATUS "GCC: -fdiagnostics-color=always supported"); else() message(STATUS "GCC: -fdiagnostics-color=always not supported"); endif()

        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "10.0")
            check_cxx_compiler_flag("-fconcepts-diagnostics-depth=3" GCC_HAS_CONCEPTS_DIAG_DEPTH) # Requires C++20
            set(BUILD_ENV_GCC_HAS_CONCEPTS_DIAG_DEPTH ${GCC_HAS_CONCEPTS_DIAG_DEPTH} CACHE BOOL "GCC supports -fconcepts-diagnostics-depth=3" FORCE)
            if(GCC_HAS_CONCEPTS_DIAG_DEPTH) message(STATUS "GCC: -fconcepts-diagnostics-depth=3 supported"); else() message(STATUS "GCC: -fconcepts-diagnostics-depth=3 not supported"); endif()
        else()
             set(BUILD_ENV_GCC_HAS_CONCEPTS_DIAG_DEPTH OFF CACHE BOOL "GCC supports -fconcepts-diagnostics-depth=3 (Compiler < 10.0)" FORCE)
             message(STATUS "GCC: -fconcepts-diagnostics-depth=3 not checked (Compiler < 10.0)")
        endif()
    else()
        set(BUILD_ENV_GCC_HAS_MARCH_NATIVE OFF CACHE BOOL "GCC supports -march=native" FORCE)
        set(BUILD_ENV_GCC_HAS_LTO OFF CACHE BOOL "GCC supports -flto" FORCE)
        set(BUILD_ENV_GCC_HAS_COLOR_DIAG OFF CACHE BOOL "GCC supports -fdiagnostics-color=always" FORCE)
        set(BUILD_ENV_GCC_HAS_CONCEPTS_DIAG_DEPTH OFF CACHE BOOL "GCC supports -fconcepts-diagnostics-depth=3" FORCE)
    endif()

    message(STATUS "--- End of Minimal Capability Detection ---")
endfunction()
