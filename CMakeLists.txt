cmake_minimum_required(VERSION 3.5)

# Main project
project(otroff CXX)

# Require modern C++ for everything built
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect the operating system
if(WIN32)
    set(OS_WINDOWS TRUE)
elseif(APPLE)
    set(OS_MACOS TRUE)
elseif(UNIX)
    set(OS_UNIX TRUE)
endif()

# Build the OS abstraction library
add_library(os_abstraction)
if(OS_WINDOWS)
    target_sources(os_abstraction PRIVATE src/os/os_windows.cpp)
else()
    target_sources(os_abstraction PRIVATE src/os/os_unix.cpp)
endif()

set_source_files_properties(
    src/os/os_windows.cpp
    src/os/os_unix.cpp
    PROPERTIES LANGUAGE CXX
)

target_include_directories(os_abstraction PUBLIC src/os)

# Optional components
option(BUILD_ROFF "Build the roff text formatter" OFF)
option(BUILD_CROFF "Build the croff formatter" OFF)
option(BUILD_NEQN "Build the neqn preprocessor" OFF)

# Add optional subdirectories
if(BUILD_ROFF)
    add_subdirectory(roff)
endif()

if(BUILD_CROFF)
    add_subdirectory(croff)
endif()

if(BUILD_NEQN)
    add_subdirectory(neqn)
endif()

# Build troff executable from roff sources
file(GLOB ROFF_SRC CONFIGURE_DEPENDS "roff/*.c" "roff/*.cpp")
add_executable(troff ${ROFF_SRC})
set_source_files_properties(${ROFF_SRC} PROPERTIES LANGUAGE CXX)

# Enable warnings and optimization
target_compile_options(troff PRIVATE -Wall -O2)
