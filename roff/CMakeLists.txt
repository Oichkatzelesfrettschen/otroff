# Build the roff formatter
file(GLOB ROFF_SOURCES_ALL *.c *.cpp)
list(REMOVE_ITEM ROFF_SOURCES_ALL "${CMAKE_CURRENT_SOURCE_DIR}/roff1_cxx23.cpp")
add_executable(roff ${ROFF_SOURCES_ALL})
set_source_files_properties(${ROFF_SOURCES_ALL} PROPERTIES LANGUAGE CXX)

# Explicitly set C++ standard for this target
set_target_properties(roff PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Include headers from the roff directory
target_include_directories(roff PRIVATE ${CMAKE_SOURCE_DIR}/roff)
# Link against the OS abstraction layer
target_link_libraries(roff PRIVATE os_abstraction)
