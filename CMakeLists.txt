cmake_minimum_required(VERSION 3.16)

# Main project
project(otroff VERSION 1.0.0 LANGUAGES CXX)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
include(CheckCXXCompilerFlag) # Ensure this is available for CodeQuality.cmake if it uses it
include(CheckCXXSourceCompiles) # Ensure this is available for CodeQuality.cmake if it uses it (or for Minimal Detection)
include(CodeQuality)

# Option to use minimal environment detection for debugging/stability
option(USE_MINIMAL_ENV_DETECTION "Use minimal environment detection module" ON) # Default to ON for now
message(STATUS "USE_MINIMAL_ENV_DETECTION: ${USE_MINIMAL_ENV_DETECTION}")

if(USE_MINIMAL_ENV_DETECTION)
    message(STATUS "Using Minimal Environment Detection Module.")
    include(EnvironmentDetection_Minimal)
    if(COMMAND detect_build_environment_minimal)
        detect_build_environment_minimal() # Call the function from the minimal module
    else()
        message(FATAL_ERROR "EnvironmentDetection_Minimal.cmake did not provide detect_build_environment_minimal().")
    endif()
else()
    message(STATUS "Using Full Environment Detection Module.")
    include(EnvironmentDetection) # This will be for the full version
    if(COMMAND detect_build_environment)
        detect_build_environment() # Call the function from the full module
    else()
        message(FATAL_ERROR "EnvironmentDetection.cmake did not provide detect_build_environment(). Check if the file exists or if the function name is correct.")
    endif()
endif()

# Graceful fallback if detection function wasn't called or didn't set key variables
if(NOT DEFINED BUILD_ENV_CXX_STANDARD)
    message(WARNING "BUILD_ENV_CXX_STANDARD not set by environment detection. Defaulting to C++17.")
    set(BUILD_ENV_CXX_STANDARD "17")
endif()
if(NOT DEFINED BUILD_ENV_HAS_CCACHE) # Ensure this is defined for ccache logic later
    set(BUILD_ENV_HAS_CCACHE FALSE)
    # CCACHE_EXECUTABLE would also not be defined, so ccache logic will skip
endif()
if(NOT DEFINED BUILD_ENV_COMPILER_IS_GCC) # For compiler-specific flags
    set(BUILD_ENV_COMPILER_IS_GCC FALSE)
endif()
# Add similar fallbacks for other BUILD_ENV_HAS_* variables if they are critical before use

# Options for code quality tools (can be controlled by presets)
option(ENABLE_ASAN "Enable AddressSanitizer (Debug builds only)" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer (Debug builds only)" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer (Debug builds only)" OFF)
option(ENABLE_COVERAGE "Enable Code Coverage (Debug builds, GCC/Clang only)" OFF)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(GNUInstallDirs) # For CMAKE_INSTALL_INCLUDEDIR etc.

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose build type: Debug Release RelWithDebInfo MinSizeRel" FORCE)
endif()

# Require modern C++ for everything built
# Set C++ standard based on detection (defaulting to 17 if detection module doesn't run or variable is empty)
if(NOT DEFINED BUILD_ENV_CXX_STANDARD OR BUILD_ENV_CXX_STANDARD STREQUAL "")
    message(WARNING "BUILD_ENV_CXX_STANDARD not set by EnvironmentDetection. Defaulting CMAKE_CXX_STANDARD to 17.")
    set(CMAKE_CXX_STANDARD 17)
else()
    message(STATUS "Setting CMAKE_CXX_STANDARD to ${BUILD_ENV_CXX_STANDARD} (from EnvironmentDetection)")
    set(CMAKE_CXX_STANDARD ${BUILD_ENV_CXX_STANDARD})
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler-specific flags based on detection
if(BUILD_ENV_COMPILER_IS_GCC) # This variable is set in EnvironmentDetection.cmake
    message(STATUS "Adding GCC specific compile options.")
    if(BUILD_ENV_GCC_HAS_COLOR_DIAG)
        add_compile_options(-fdiagnostics-color=always)
    endif()
    if(BUILD_ENV_GCC_HAS_CONCEPTS_DIAG_DEPTH) # Set by EnvDetection if GCC >= 10 and flag supported
        add_compile_options(-fconcepts-diagnostics-depth=3)
    endif()
    # Potentially add -march=native and -flto for release builds later,
    # or manage them via CMAKE_CXX_FLAGS_<CONFIG> in presets/CodeQuality.cmake
    # For now, focus on diagnostic flags here.
endif()
# Add similar block for Clang if BUILD_ENV_COMPILER_IS_CLANG and specific Clang flags were detected

# Enable clang-tidy if available (typically with Clang compiler)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  find_program(CLANG_TIDY_EXE clang-tidy)
  if(CLANG_TIDY_EXE)
    set(CMAKE_CXX_CLANG_TIDY
        "${CLANG_TIDY_EXE}"
        -extra-arg=-Wno-unknown-warning-option # Suppress warning about unknown checks if .clang-tidy has them
        -extra-arg=-Wno-ignored-optimization-argument # Common in complex builds
    )
    message(STATUS "clang-tidy found and enabled: ${CLANG_TIDY_EXE}")
  else()
    message(WARNING "clang-tidy not found. CXX_CLANG_TIDY checks will be skipped.")
  endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  # Placeholder for potential GCC static analysis if desired in future (e.g. cppcheck)
  # For now, just note that clang-tidy is typically for Clang.
  # Some projects try to run clang-tidy with gcc-compiled code using compile_commands.json,
  # but direct integration is simpler with Clang.
endif()

# CCache integration (uses BUILD_ENV_HAS_CCACHE from EnvironmentDetection.cmake)
if(BUILD_ENV_HAS_CCACHE)
    # find_program(CCACHE_PROGRAM ccache REQUIRED) # BUILD_ENV_HAS_CCACHE implies it was found
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE}) # CCACHE_EXECUTABLE is set by find_program in EnvDetection
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE})

    # Configure ccache for CI (example)
    if(DEFINED ENV{CI})
        set(ENV{CCACHE_DIR} "${CMAKE_BINARY_DIR}/.ccache")
        set(ENV{CCACHE_COMPRESS} "1")
        set(ENV{CCACHE_COMPRESSLEVEL} "6")
        set(ENV{CCACHE_MAXSIZE} "500M")
        message(STATUS "ccache: Configured for CI environment.")
    endif()
    message(STATUS "Build acceleration: ccache enabled via CMAKE_CXX_COMPILER_LAUNCHER.")
else()
    message(STATUS "Build acceleration: ccache not found or not enabled by environment detection.")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set default visibility to hidden for supporting compilers (GCC/Clang)
# This means symbols are not exported unless explicitly marked.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
endif()
# Note: CMAKE_VISIBILITY_PRESET and CMAKE_CXX_VISIBILITY_PRESET require CMake 3.3+
# but -fvisibility flags are more direct for GCC/Clang.
# If we need to support MSVC for export/import, that's a separate setup.

# Common settings library
add_library(otroff_common INTERFACE)
target_compile_features(otroff_common INTERFACE cxx_std_17)
# Add common warning flags.
target_compile_options(otroff_common INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic>
    # Add MSVC specific flags if needed, e.g. $<$<CXX_COMPILER_ID:MSVC>:/W4>
)
target_include_directories(otroff_common INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  # Convention for project's public headers
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>        # For installed public headers
)
target_compile_definitions(otroff_common INTERFACE
    $<$<CONFIG:DEBUG>:OTROFF_DEBUG>
    # Add other general preprocessor definitions if needed
)

# Find all relevant source and header files for formatting
file(GLOB_RECURSE ALL_CXX_SOURCES
    RELATIVE "${CMAKE_SOURCE_DIR}"
    "*.cpp" "*.hpp" "*.c" "*.h"
)
# Exclude files in .cache or build directories if the glob picks them up
# For instance, if build directory is always ${CMAKE_SOURCE_DIR}/build
list(FILTER ALL_CXX_SOURCES EXCLUDE REGEX "^build/")
list(FILTER ALL_CXX_SOURCES EXCLUDE REGEX "^\\.cache/")
# Add more specific excludes if necessary, e.g., for third-party code

if(ALL_CXX_SOURCES)
    add_custom_target(format
        COMMAND clang-format -i -style=file ${ALL_CXX_SOURCES}
        COMMENT "Formatting C/C++ source files with clang-format..."
    )
else()
    add_custom_target(format COMMENT "No C/C++ source files found to format.")
endif()

# OS Abstraction Library
add_library(os_abstraction) # Could be STATIC or SHARED if it grows, defaults to STATIC if sources are added.
                            # For now, it will be an INTERFACE library if it's header-only,
                            # or a regular library if it has .cpp files.


# Common settings library
add_library(otroff_common INTERFACE)
target_compile_features(otroff_common INTERFACE cxx_std_17)
# Add common warning flags.
target_compile_options(otroff_common INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic>
    # Add MSVC specific flags if needed, e.g. $<$<CXX_COMPILER_ID:MSVC>:/W4>
)
target_include_directories(otroff_common INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  # Convention for project's public headers
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>        # For installed public headers
)
target_compile_definitions(otroff_common INTERFACE
    $<$<CONFIG:DEBUG>:OTROFF_DEBUG>
    # Add other general preprocessor definitions if needed
)

# OS Abstraction Library
add_library(os_abstraction) # Could be STATIC or SHARED if it grows, defaults to STATIC if sources are added.
                            # For now, it will be an INTERFACE library if it's header-only,
                            # or a regular library if it has .cpp files.

# Correctly set sources based on OS
if(WIN32)
    target_sources(os_abstraction PRIVATE src/os/os_windows.cpp)
    # Add any Windows-specific definitions or links here if necessary
elseif(UNIX AND NOT APPLE) # For Linux
    target_sources(os_abstraction PRIVATE src/os/os_unix.cpp)
    # Add any Linux-specific definitions or links here if necessary
elseif(APPLE)
    target_sources(os_abstraction PRIVATE src/os/os_unix.cpp) # Assuming os_unix.cpp is suitable for macOS
    # Add any macOS-specific definitions or links here if necessary
else()
    message(WARNING "OS not fully supported by os_abstraction, defaulting to os_unix.cpp")
    target_sources(os_abstraction PRIVATE src/os/os_unix.cpp)
endif()

target_include_directories(os_abstraction
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/os
)

# Ensure os_abstraction uses the common settings from otroff_common
target_link_libraries(os_abstraction INTERFACE otroff_common)

# Options to control building components
option(BUILD_ROFF "Build the roff component" ON)
option(BUILD_CROFF "Build the croff component" ON)
option(BUILD_NEQN "Build the neqn component" ON)
option(BUILD_TBL "Build the tbl component" ON)

# Add subdirectories for components based on options
if(BUILD_ROFF)
    add_subdirectory(roff)
endif()

if(BUILD_CROFF)
    add_subdirectory(croff)
endif()

if(BUILD_NEQN)
    add_subdirectory(neqn)
endif()

if(BUILD_TBL)
    add_subdirectory(tbl)
endif()

# Install executables
# Note: These targets are defined in subdirectories.
# We need to ensure they are globally known or install them from here using their target names.
# This assumes the executable targets are named consistently (e.g., <component>_exe).

# Example for troff_exe (legacy roff) - this assumes roff/CMakeLists.txt defines troff_exe
# A more robust way might be to install from the subdirectory's CMakeLists.txt,
# but for now, we'll add placeholder install rules here if the targets are global.
# If targets are not global, this part will need to be in subdirectory CMake files.
# For this subtask, assume we are adding rules for known executable target names.

if(TARGET troff_exe) # Check if target exists
    install(TARGETS troff_exe DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
if(TARGET croff_exe)
    install(TARGETS croff_exe DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
if(TARGET neqn_exe)
    install(TARGETS neqn_exe DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
if(TARGET tbl_exe)
    install(TARGETS tbl_exe DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# Placeholder for modern roff1.cpp executable, if it gets its own target name
# if(TARGET roff_modern_exe)
#   install(TARGETS roff_modern_exe DESTINATION ${CMAKE_INSTALL_BINDIR})
# endif()

# Apply code quality settings (sanitizers, coverage) to all defined targets
# This is deferred to run after all targets in the project have been defined.
if(COMMAND apply_code_quality_to_all_targets)
    cmake_language(DEFER CALL apply_code_quality_to_all_targets)
    message(STATUS "Code quality settings (sanitizers/coverage) will be applied to targets if enabled by presets/options.")
else()
    message(WARNING "apply_code_quality_to_all_targets function not found (from CodeQuality.cmake).")
endif()

# Commenting out old sections as requested for now.
# These will be refactored and added back.

# # Detect the operating system
# if(WIN32)
#     set(OS_WINDOWS TRUE)
# elseif(APPLE)
#     set(OS_MACOS TRUE)
# elseif(UNIX)
#     set(OS_UNIX TRUE)
# endif()
#
# # Build the OS abstraction library
# add_library(os_abstraction)
# if(OS_WINDOWS)
#     target_sources(os_abstraction PRIVATE src/os/os_windows.cpp)
# else()
#     target_sources(os_abstraction PRIVATE src/os/os_unix.cpp)
# endif()
#
# set_source_files_properties(
#     src/os/os_windows.cpp
#     src/os/os_unix.cpp
#     PROPERTIES LANGUAGE CXX
# )
#
# target_include_directories(os_abstraction PUBLIC src/os)
#
# # Optional components
# # Option to build the roff text formatter (currently has C++17 compilation issues)
# option(BUILD_ROFF "Build the roff text formatter" OFF)
# option(BUILD_CROFF "Build the croff formatter" OFF)
# option(BUILD_NEQN "Build the neqn preprocessor" OFF)
#
# # Add optional subdirectories
# if(BUILD_ROFF)
#     add_subdirectory(roff)
# endif()
#
# if(BUILD_CROFF)
#     add_subdirectory(croff)
# endif()
#
# if(BUILD_NEQN)
#     add_subdirectory(neqn)
# endif()
#
# # Build troff executable from roff sources
# file(GLOB ROFF_SRC CONFIGURE_DEPENDS "roff/*.c" "roff/*.cpp")
# add_executable(troff ${ROFF_SRC})
# set_source_files_properties(${ROFF_SRC} PROPERTIES LANGUAGE CXX)
#
# # Enable warnings and optimization
# target_compile_options(troff PRIVATE -Wall -O2)
