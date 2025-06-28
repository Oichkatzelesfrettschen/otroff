# cmake/Modules/EnvironmentDetection.cmake
#
# Module to detect available build and analysis tools in the environment.
# Sets cache variables that can be used by other CMake scripts or presets.
# This is the FULL version.

include_guard(GLOBAL)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(CMakeParseArguments) # For parsing arguments in helper functions

# --- Helper function to set cache variables consistently ---
function(_set_build_env_var var_name value description)
    set(${var_name} ${value} CACHE INTERNAL "${description}")
    # Using INTERNAL to avoid polluting CMake GUI unless explicitly shown,
    # but they are effectively global for the build script.
endfunction()

# --- CMake Version Validation ---
function(validate_cmake_version)
    message(STATUS "Validating CMake version...")
    if(CMAKE_VERSION VERSION_LESS "3.16.0") # Minimum required for some features used
        message(WARNING "CMake version is ${CMAKE_VERSION}. Recommended version is 3.16.0 or higher. Some features might not be available.")
    else()
        message(STATUS "CMake version: ${CMAKE_VERSION} (>= 3.16.0, OK)")
    endif()
endfunction()

# --- Compiler Capability Detection ---
function(detect_compiler_capabilities)
    message(STATUS "Detecting compiler capabilities...")
    _set_build_env_var(BUILD_ENV_COMPILER_ID "${CMAKE_CXX_COMPILER_ID}" "Detected C++ Compiler ID")
    _set_build_env_var(BUILD_ENV_COMPILER_VERSION "${CMAKE_CXX_COMPILER_VERSION}" "Detected C++ Compiler Version")
    message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

    set(BUILD_ENV_COMPILER_IS_GCC OFF)
    set(BUILD_ENV_COMPILER_IS_CLANG OFF)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(BUILD_ENV_COMPILER_IS_GCC ON)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(BUILD_ENV_COMPILER_IS_CLANG ON)
    endif()
    _set_build_env_var(BUILD_ENV_COMPILER_IS_GCC ${BUILD_ENV_COMPILER_IS_GCC} "GCC Compiler Detected")
    _set_build_env_var(BUILD_ENV_COMPILER_IS_CLANG ${BUILD_ENV_COMPILER_IS_CLANG} "Clang Compiler Detected")

    message(STATUS "Detecting C++ standards support...")
    check_cxx_compiler_flag("-std=c++17" COMPILER_SUPPORTS_CXX17_FLAG)
    check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_CXX20_FLAG)
    check_cxx_compiler_flag("-std=c++23" COMPILER_SUPPORTS_CXX23_FLAG)

    _set_build_env_var(BUILD_ENV_SUPPORTS_CXX17 ${COMPILER_SUPPORTS_CXX17_FLAG} "Compiler supports -std=c++17 flag")
    _set_build_env_var(BUILD_ENV_SUPPORTS_CXX20 ${COMPILER_SUPPORTS_CXX20_FLAG} "Compiler supports -std=c++20 flag")
    _set_build_env_var(BUILD_ENV_SUPPORTS_CXX23 ${COMPILER_SUPPORTS_CXX23_FLAG} "Compiler supports -std=c++23 flag")

    set(DETECTED_STANDARD "17") # Default
    if(COMPILER_SUPPORTS_CXX23_FLAG)
        set(DETECTED_STANDARD "23")
    elseif(COMPILER_SUPPORTS_CXX20_FLAG)
        set(DETECTED_STANDARD "20")
    endif()
    _set_build_env_var(BUILD_ENV_CXX_STANDARD_MAX_SUPPORTED_FLAG ${DETECTED_STANDARD} "Highest C++ standard flag supported by compiler")
    # BUILD_ENV_CXX_STANDARD will be set by root CMakeLists based on this and project choice

    set(CMAKE_REQUIRED_QUIET ${CMAKE_QUIET})

    set(CXX_CONCEPTS_CODE "#include <concepts>\n#include <type_traits>\ntemplate<typename T> concept Integral = std::is_integral_v<T>;\nint main() { Integral auto i = 5; return i-5; }")
    check_cxx_source_compiles("${CXX_CONCEPTS_CODE}" BUILD_ENV_HAS_CONCEPTS_FEATURE CMAKE_FLAGS "-std=c++${DETECTED_STANDARD}")
    _set_build_env_var(BUILD_ENV_HAS_CONCEPTS ${BUILD_ENV_HAS_CONCEPTS_FEATURE} "Compiler supports C++20 Concepts with -std=c++${DETECTED_STANDARD}")
    if(BUILD_ENV_HAS_CONCEPTS_FEATURE) message(STATUS "C++20 Concepts: Supported"); else() message(STATUS "C++20 Concepts: Not supported (with -std=c++${DETECTED_STANDARD})"); endif()

    set(CXX_COROUTINES_CODE "#include <coroutine>\nstruct task { struct promise_type { task get_return_object() { return {}; } std::suspend_never initial_suspend() { return {}; } std::suspend_never final_suspend() noexcept { return {}; } void return_void() {} void unhandled_exception() {} }; };\nint main() { task t; return 0; }")
    check_cxx_source_compiles("${CXX_COROUTINES_CODE}" BUILD_ENV_HAS_COROUTINES_FEATURE CMAKE_FLAGS "-std=c++${DETECTED_STANDARD}")
    _set_build_env_var(BUILD_ENV_HAS_COROUTINES ${BUILD_ENV_HAS_COROUTINES_FEATURE} "Compiler supports C++20 Coroutines with -std=c++${DETECTED_STANDARD}")
    if(BUILD_ENV_HAS_COROUTINES_FEATURE) message(STATUS "C++20 Coroutines: Supported"); else() message(STATUS "C++20 Coroutines: Not supported (with -std=c++${DETECTED_STANDARD})"); endif()

    set(CXX_RANGES_CODE "#include <ranges>\n#include <vector>\nint main() { std::vector<int> v = {1,2,3}; auto r = v | std::views::take(2); return 0; }")
    check_cxx_source_compiles("${CXX_RANGES_CODE}" BUILD_ENV_HAS_RANGES_FEATURE CMAKE_FLAGS "-std=c++${DETECTED_STANDARD}")
    _set_build_env_var(BUILD_ENV_HAS_RANGES ${BUILD_ENV_HAS_RANGES_FEATURE} "Compiler supports C++20 Ranges with -std=c++${DETECTED_STANDARD}")
    if(BUILD_ENV_HAS_RANGES_FEATURE) message(STATUS "C++20 Ranges: Supported"); else() message(STATUS "C++20 Ranges: Not supported (with -std=c++${DETECTED_STANDARD})"); endif()

    set(CXX_MODULES_CODE "#if !defined(__cpp_modules) || __cpp_modules < 201907L\n#error \"C++20 Modules not supported or version too low\"\n#endif\nint main() { return 0; }")
    check_cxx_source_compiles("${CXX_MODULES_CODE}" BUILD_ENV_HAS_MODULES_FEATURE CMAKE_FLAGS "-std=c++${DETECTED_STANDARD}")
    _set_build_env_var(BUILD_ENV_HAS_MODULES ${BUILD_ENV_HAS_MODULES_FEATURE} "Compiler supports C++20 Modules (via __cpp_modules) with -std=c++${DETECTED_STANDARD}")
    if(BUILD_ENV_HAS_MODULES_FEATURE) message(STATUS "C++20 Modules: Supported (via __cpp_modules)"); else() message(STATUS "C++20 Modules: Not supported (via __cpp_modules, with -std=c++${DETECTED_STANDARD})"); endif()

    set(CXX_SPACESHIP_CODE "#include <compare>\nint main() { int x=0, y=1; auto cmp = x <=> y; return (cmp < 0) ? 0 : 1; }")
    check_cxx_source_compiles("${CXX_SPACESHIP_CODE}" BUILD_ENV_HAS_SPACESHIP_FEATURE CMAKE_FLAGS "-std=c++${DETECTED_STANDARD}")
    _set_build_env_var(BUILD_ENV_HAS_SPACESHIP ${BUILD_ENV_HAS_SPACESHIP_FEATURE} "Compiler supports C++20 Three-Way Comparison with -std=c++${DETECTED_STANDARD}")
    if(BUILD_ENV_HAS_SPACESHIP_FEATURE) message(STATUS "C++20 Three-Way Comparison: Supported"); else() message(STATUS "C++20 Three-Way Comparison: Not supported (with -std=c++${DETECTED_STANDARD})"); endif()
endfunction()

function(detect_compiler_optimizations)
    message(STATUS "Detecting compiler optimization flags...")
    if(BUILD_ENV_COMPILER_IS_GCC)
        message(STATUS "Performing GCC-specific flag detection...")
        check_cxx_compiler_flag("-march=native" GCC_HAS_MARCH_NATIVE)
        check_cxx_compiler_flag("-flto" GCC_HAS_LTO)
        check_cxx_compiler_flag("-fdiagnostics-color=always" GCC_HAS_COLOR_DIAG)
        check_cxx_compiler_flag("-ftime-trace" GCC_HAS_FTIME_TRACE)
        check_cxx_compiler_flag("-fsanitize=cfi" GCC_HAS_FSANITIZE_CFI)

        _set_build_env_var(BUILD_ENV_GCC_HAS_MARCH_NATIVE ${GCC_HAS_MARCH_NATIVE} "GCC supports -march=native")
        _set_build_env_var(BUILD_ENV_GCC_HAS_LTO ${GCC_HAS_LTO} "GCC supports -flto")
        _set_build_env_var(BUILD_ENV_GCC_HAS_COLOR_DIAG ${GCC_HAS_COLOR_DIAG} "GCC supports -fdiagnostics-color=always")
        _set_build_env_var(BUILD_ENV_GCC_HAS_FTIME_TRACE ${GCC_HAS_FTIME_TRACE} "GCC supports -ftime-trace")
        _set_build_env_var(BUILD_ENV_GCC_HAS_FSANITIZE_CFI ${GCC_HAS_FSANITIZE_CFI} "GCC supports -fsanitize=cfi")
        if(GCC_HAS_MARCH_NATIVE) message(STATUS "GCC: -march=native supported"); endif()
        if(GCC_HAS_LTO) message(STATUS "GCC: -flto supported"); endif()
        if(GCC_HAS_COLOR_DIAG) message(STATUS "GCC: -fdiagnostics-color=always supported"); endif()
        if(GCC_HAS_FTIME_TRACE) message(STATUS "GCC: -ftime-trace supported"); endif()
        if(GCC_HAS_FSANITIZE_CFI) message(STATUS "GCC: -fsanitize=cfi supported"); endif()
    elseif(BUILD_ENV_COMPILER_IS_CLANG)
        message(STATUS "Performing Clang-specific flag detection...")
        check_cxx_compiler_flag("-march=native" CLANG_HAS_MARCH_NATIVE)
        check_cxx_compiler_flag("-flto=thin" CLANG_HAS_LTO_THIN)
        check_cxx_compiler_flag("-fcolor-diagnostics" CLANG_HAS_COLOR_DIAG)
        check_cxx_compiler_flag("-ftime-trace" CLANG_HAS_FTIME_TRACE)
        check_cxx_compiler_flag("-fsanitize=cfi" CLANG_HAS_FSANITIZE_CFI)

        _set_build_env_var(BUILD_ENV_CLANG_HAS_MARCH_NATIVE ${CLANG_HAS_MARCH_NATIVE} "Clang supports -march=native")
        _set_build_env_var(BUILD_ENV_CLANG_HAS_LTO_THIN ${CLANG_HAS_LTO_THIN} "Clang supports -flto=thin")
        _set_build_env_var(BUILD_ENV_CLANG_HAS_COLOR_DIAG ${CLANG_HAS_COLOR_DIAG} "Clang supports -fcolor-diagnostics")
        _set_build_env_var(BUILD_ENV_CLANG_HAS_FTIME_TRACE ${CLANG_HAS_FTIME_TRACE} "Clang supports -ftime-trace")
        _set_build_env_var(BUILD_ENV_CLANG_HAS_FSANITIZE_CFI ${CLANG_HAS_FSANITIZE_CFI} "Clang supports -fsanitize=cfi")
        if(CLANG_HAS_MARCH_NATIVE) message(STATUS "Clang: -march=native supported"); endif()
        if(CLANG_HAS_LTO_THIN) message(STATUS "Clang: -flto=thin supported"); endif()
        if(CLANG_HAS_COLOR_DIAG) message(STATUS "Clang: -fcolor-diagnostics supported"); endif()
        if(CLANG_HAS_FTIME_TRACE) message(STATUS "Clang: -ftime-trace supported"); endif()
        if(CLANG_HAS_FSANITIZE_CFI) message(STATUS "Clang: -fsanitize=cfi supported"); endif()
    else()
        message(STATUS "No GCC or Clang specific optimization flags detected/checked.")
    endif()
endfunction()

function(detect_build_tools)
    message(STATUS "Detecting build tools...")
    find_program(CCACHE_EXECUTABLE ccache)
    if(CCACHE_EXECUTABLE) _set_build_env_var(BUILD_ENV_HAS_CCACHE ON "ccache Available"); message(STATUS "ccache: ${CCACHE_EXECUTABLE}"); else() _set_build_env_var(BUILD_ENV_HAS_CCACHE OFF "ccache Available"); message(STATUS "ccache: Not found (based on env scan)"); endif()
    mark_as_advanced(CCACHE_EXECUTABLE)

    find_program(NINJA_EXECUTABLE ninja)
    if(NINJA_EXECUTABLE) _set_build_env_var(BUILD_ENV_HAS_NINJA ON "Ninja Available"); message(STATUS "Ninja: ${NINJA_EXECUTABLE}"); else() _set_build_env_var(BUILD_ENV_HAS_NINJA OFF "Ninja Available"); message(STATUS "Ninja: Not found (based on env scan)"); endif()
    mark_as_advanced(NINJA_EXECUTABLE)

    find_program(CLANG_TIDY_EXECUTABLE clang-tidy)
    if(CLANG_TIDY_EXECUTABLE) _set_build_env_var(BUILD_ENV_HAS_CLANG_TIDY ON "clang-tidy Available"); message(STATUS "clang-tidy: ${CLANG_TIDY_EXECUTABLE}"); else() _set_build_env_var(BUILD_ENV_HAS_CLANG_TIDY OFF "clang-tidy Available"); message(STATUS "clang-tidy: Not found (based on env scan)"); endif()
    mark_as_advanced(CLANG_TIDY_EXECUTABLE)

    find_program(LCOV_EXECUTABLE lcov)
    if(LCOV_EXECUTABLE) _set_build_env_var(BUILD_ENV_HAS_LCOV ON "LCOV Available"); message(STATUS "LCOV: ${LCOV_EXECUTABLE}"); else() _set_build_env_var(BUILD_ENV_HAS_LCOV OFF "LCOV Available"); message(STATUS "LCOV: Not found (based on env scan)"); endif()
    mark_as_advanced(LCOV_EXECUTABLE)

    find_program(GCOVR_EXECUTABLE gcovr)
    if(GCOVR_EXECUTABLE) _set_build_env_var(BUILD_ENV_HAS_GCOVR ON "gcovr Available"); message(STATUS "gcovr: ${GCOVR_EXECUTABLE}"); else() _set_build_env_var(BUILD_ENV_HAS_GCOVR OFF "gcovr Available"); message(STATUS "gcovr: Not found (based on env scan)"); endif()
    mark_as_advanced(GCOVR_EXECUTABLE)

    find_program(DOXYGEN_EXECUTABLE doxygen)
    if(DOXYGEN_EXECUTABLE) _set_build_env_var(BUILD_ENV_HAS_DOXYGEN ON "Doxygen Available"); message(STATUS "Doxygen: ${DOXYGEN_EXECUTABLE}"); else() _set_build_env_var(BUILD_ENV_HAS_DOXYGEN OFF "Doxygen Available"); message(STATUS "Doxygen: Not found (based on env scan)"); endif()
    mark_as_advanced(DOXYGEN_EXECUTABLE)

    _set_build_env_var(BUILD_ENV_HAS_PYTHON_AST ON "Python3 with AST module available (assumed from prior scan)")
    message(STATUS "Python3 with AST: Assumed available (based on prior scan).")
endfunction()

function(synthesize_build_configuration)
    message(STATUS "Synthesizing build configuration recommendations...")
    if(BUILD_ENV_HAS_CCACHE AND BUILD_ENV_HAS_NINJA)
        message(STATUS "Recommendation: Use a Ninja-based preset for potentially faster builds with ccache.")
    elseif(BUILD_ENV_HAS_CCACHE)
        message(STATUS "Recommendation: ccache is available and can speed up rebuilds.")
    endif()
    if(BUILD_ENV_COMPILER_IS_CLANG AND BUILD_ENV_HAS_CLANG_TIDY)
        message(STATUS "Recommendation: Enable clang-tidy checks for Clang builds.")
    endif()
    if(BUILD_ENV_COMPILER_IS_GCC AND (BUILD_ENV_HAS_LCOV OR BUILD_ENV_HAS_GCOVR))
         message(STATUS "Recommendation: Code coverage can be generated for GCC builds.")
    endif()
endfunction()

function(generate_capability_report)
    message(STATUS "Generating build environment capability report...")
    set(REPORT_FILE_TXT "${CMAKE_BINARY_DIR}/BuildEnvironmentReport.txt")
    set(REPORT_FILE_JSON "${CMAKE_BINARY_DIR}/BuildEnvironmentReport.json")

    # Get current date/time for the report
    string(TIMESTAMP GEN_TIME "%Y-%m-%d %H:%M:%S UTC" UTC)

    set(REPORT_CONTENT "Build Environment Capability Report (otroff)\n")
    string(APPEND REPORT_CONTENT "Generated: ${GEN_TIME}\n\n")
    string(APPEND REPORT_CONTENT "CMake Version: ${CMAKE_VERSION}\n")
    string(APPEND REPORT_CONTENT "Compiler ID: ${BUILD_ENV_COMPILER_ID}\n")
    string(APPEND REPORT_CONTENT "Compiler Version: ${BUILD_ENV_COMPILER_VERSION}\n")
    string(APPEND REPORT_CONTENT "Target C++ Standard (Project): ${CMAKE_CXX_STANDARD}\n") # What the project aims for
    string(APPEND REPORT_CONTENT "Max C++ Standard Flag Supported (Compiler): ${BUILD_ENV_CXX_STANDARD_MAX_SUPPORTED_FLAG}\n\n")

    string(APPEND REPORT_CONTENT "Detected Capabilities (Boolean Flags):\n")
    get_cmake_property(_variableNames CACHE_VARIABLES)
    foreach(_variableName ${_variableNames})
        if(_variableName MATCHES "^BUILD_ENV_")
            get_property(_type CACHE_VARIABLE ${_variableName} PROPERTY TYPE)
            if(_type STREQUAL "BOOL")
                string(APPEND REPORT_CONTENT "  ${_variableName}: ${${_variableName}}\n")
            elseif(_type STREQUAL "STRING")
                 string(APPEND REPORT_CONTENT "  ${_variableName}: \"${${_variableName}}\"\n")
            endif()
        endif()
    endforeach()
    file(WRITE ${REPORT_FILE_TXT} "${REPORT_CONTENT}")
    message(STATUS "Build environment text report generated: ${REPORT_FILE_TXT}")

    # JSON Report
    set(JSON_CONTENT "{\n")
    string(APPEND JSON_CONTENT "  \"report_generated_utc\": \"${GEN_TIME}\",\n")
    string(APPEND JSON_CONTENT "  \"cmake_version\": \"${CMAKE_VERSION}\",\n")
    string(APPEND JSON_CONTENT "  \"compiler_id\": \"${BUILD_ENV_COMPILER_ID}\",\n")
    string(APPEND JSON_CONTENT "  \"compiler_version\": \"${BUILD_ENV_COMPILER_VERSION}\",\n")
    string(APPEND JSON_CONTENT "  \"cxx_standard_project_target\": \"${CMAKE_CXX_STANDARD}\",\n")
    string(APPEND JSON_CONTENT "  \"cxx_standard_compiler_max_flag\": \"${BUILD_ENV_CXX_STANDARD_MAX_SUPPORTED_FLAG}\",\n")
    string(APPEND JSON_CONTENT "  \"capabilities\": {\n")
    set(first_cap TRUE)
    foreach(_variableName ${_variableNames})
        if(_variableName MATCHES "^BUILD_ENV_")
            if(NOT first_cap) string(APPEND JSON_CONTENT ",\n") endif()
            set(first_cap FALSE)
            string(REPLACE "BUILD_ENV_" "" _keyName ${_variableName})
            string(TOLOWER ${_keyName} _keyName) # Consistent JSON keys
            get_property(_type CACHE_VARIABLE ${_variableName} PROPERTY TYPE)
            if(_type STREQUAL "BOOL")
                 string(APPEND JSON_CONTENT "    \"${_keyName}\": ${${_variableName}}")
            elseif(_type STREQUAL "STRING")
                 string(APPEND JSON_CONTENT "    \"${_keyName}\": \"${${_variableName}}\"")
            else()
                 string(APPEND JSON_CONTENT "    \"${_keyName}\": \"${${_variableName}}\"") # Default to string
            endif()
        endif()
    endforeach()
    string(APPEND JSON_CONTENT "\n  }\n}\n")
    file(WRITE ${REPORT_FILE_JSON} "${JSON_CONTENT}")
    message(STATUS "Build environment JSON report generated: ${REPORT_FILE_JSON}")
endfunction()

function(detect_build_environment)
    if(DEFINED BUILD_ENVIRONMENT_DETECTED_FULL)
        message(STATUS "Full build environment already detected. Skipping redetection.")
        return()
    endif()

    message(STATUS "--- Full Build Environment Capability Detection ---")
    validate_cmake_version()
    detect_compiler_capabilities()
    detect_compiler_optimizations()
    detect_build_tools()
    synthesize_build_configuration()
    generate_capability_report()

    _set_build_env_var(BUILD_ENVIRONMENT_DETECTED_FULL ON "Flag to indicate full detection has run")
    message(STATUS "--- End of Full Capability Detection ---")
endfunction()

function(require_capability capability_var)
    # ARGN contains additional arguments (the message parts)
    if(NOT DEFINED ${capability_var} OR NOT ${${capability_var}})
        message(FATAL_ERROR "Required capability ${capability_var} is not available. ${ARGN}")
    endif()
endfunction()

function(optional_capability result_var capability_var)
    if(DEFINED ${capability_var} AND ${${capability_var}})
        set(${result_var} TRUE PARENT_SCOPE)
    else()
        set(${result_var} FALSE PARENT_SCOPE)
    endif()
endfunction()

function(get_build_env_var result_var variable_name default_value)
    if(DEFINED ${variable_name})
        set(${result_var} "${${variable_name}}" PARENT_SCOPE)
    else()
        set(${result_var} "${default_value}" PARENT_SCOPE)
    endif()
endfunction()

if(DEFINED ENV{CI})
    message(STATUS "CI Environment Detected (ENV{CI} is set). Applying CI-specific overrides.")
    # Example: Disable ccache in CI if it's problematic, or set specific paths
    # _set_build_env_var(BUILD_ENV_HAS_CCACHE OFF "ccache forcibly disabled in CI")
    # _set_build_env_var(BUILD_ENV_CI_MODE ON "CI Mode Active")
endif()

if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross-compilation detected. Tool detection may be affected.")
    # Example: Tool detection might need hints from toolchain file
    # _set_build_env_var(BUILD_ENV_CROSSCOMPILING ON "Cross-compilation active")
endif()
