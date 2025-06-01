project(roff_project) # Sub-project

add_library(roff_core_obj OBJECT
    # roff2.cpp          # Temporarily excluded due to C++23 experimental code compilation errors
    roff3.cpp
    roff4.cpp
    roff5.cpp          # Hyphenation engine
    roff7.cpp          # Digram tables
    roff8.cpp          # Global definitions
    hyphen_utils.cpp
    runtime.cpp
    sse_memops.cpp
    time_utils.cpp
)
# Object libraries typically use PRIVATE for linking build-time dependencies
target_link_libraries(roff_core_obj PRIVATE
    otroff_common
    os_abstraction
)

add_executable(troff_exe main.cpp) # Uses the new roff/main.cpp
target_link_libraries(troff_exe PRIVATE $<TARGET_OBJECTS:roff_core_obj>)
