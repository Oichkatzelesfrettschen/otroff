project(roff_project) # Sub-project

# Pure C17 sources - Complete ROFF implementation
add_library(roff_c_core OBJECT
    roff3.c            # Text processing (C17)
    roff4.c            # Output formatting (C17)
    roff5.c            # Hyphenation engine (C17)
    roff7.c            # Digram tables (C17)
    roff8.c            # Global variables and data segment (C17)
    stubs.c            # I/O and utility functions (C17)
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

# Pure C17 executable
add_executable(troff_exe main.c)
set_target_properties(troff_exe PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)
target_link_libraries(troff_exe PRIVATE
    $<TARGET_OBJECTS:roff_c_core>
    otroff_common
    os_abstraction
)
