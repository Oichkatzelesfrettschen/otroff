project(roff_project) # Sub-project

# Pure C sources - compiled as C17
add_library(roff_c_core OBJECT
    roff3.c            # Text processing (C17)
    roff4.c            # Output formatting (C17)
    roff5.c            # Hyphenation engine (C17)
)
set_target_properties(roff_c_core PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF   # Enforce standard C, no GNU extensions
)
target_link_libraries(roff_c_core PRIVATE
    otroff_common
    os_abstraction
)

# C++ sources - compiled as C++17
add_library(roff_cpp_core_obj OBJECT
    # roff2.cpp          # Temporarily excluded due to C++23 experimental code compilation errors
    # roff3.cpp, roff4.cpp, roff5.cpp moved to roff_c_core
    roff7.cpp          # Digram tables
    roff8.cpp          # Global definitions
    hyphen_utils.cpp
    runtime.cpp
    sse_memops.cpp
    time_utils.cpp
)
# Object libraries typically use PRIVATE for linking build-time dependencies
target_link_libraries(roff_cpp_core_obj PRIVATE
    otroff_common
    os_abstraction
)

add_executable(troff_exe main.cpp) # Uses the new roff/main.cpp
target_link_libraries(troff_exe PRIVATE
    $<TARGET_OBJECTS:roff_c_core>
    $<TARGET_OBJECTS:roff_cpp_core_obj>
)
