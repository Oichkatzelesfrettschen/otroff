cmake_minimum_required(VERSION 3.16)

# Main project
project(otroff VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(GNUInstallDirs) # For CMAKE_INSTALL_INCLUDEDIR etc.

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose build type: Debug Release RelWithDebInfo MinSizeRel" FORCE)
endif()

# Require modern C++ for everything built
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
