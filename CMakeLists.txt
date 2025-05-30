cmake_minimum_required(VERSION 3.5)

# Main project
project(otroff C CXX)

# Enforce ANSI C standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

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
    target_sources(os_abstraction PRIVATE src/os/os_windows.c)
else()
    target_sources(os_abstraction PRIVATE src/os/os_unix.c)
endif()

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

# Enable warnings and optimization
target_compile_options(troff PRIVATE -Wall -O2)
