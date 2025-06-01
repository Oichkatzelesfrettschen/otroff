project(neqn_project)

# Gather all .cpp files, then exclude main and test files
file(GLOB NEQN_ALL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

set(NEQN_MAIN_SOURCE "neqn.cpp")
set(NEQN_TEST_SOURCES
    "test_neqn.cpp"
    "test_ne2.cpp"
    # Add any other neqn test files here if they exist and match the glob
)

list(REMOVE_ITEM NEQN_ALL_SOURCES ${NEQN_MAIN_SOURCE})
foreach(test_file ${NEQN_TEST_SOURCES})
    list(REMOVE_ITEM NEQN_ALL_SOURCES ${test_file})
endforeach()

set(NEQN_CORE_SOURCES ${NEQN_ALL_SOURCES})

add_library(neqn_core_obj OBJECT ${NEQN_CORE_SOURCES})
# Object libraries typically use PRIVATE for linking build-time dependencies
target_link_libraries(neqn_core_obj PRIVATE
    otroff_common
    # os_abstraction is not directly linked here, assuming neqn core doesn't need it.
    # It can be added if specific files in neqn_core_obj require direct os_abstraction functions.
)

add_executable(neqn_exe ${NEQN_MAIN_SOURCE})
target_link_libraries(neqn_exe PRIVATE $<TARGET_OBJECTS:neqn_core_obj>)

# Basic regression test for neqn
# Create a dummy input file for neqn
set(NEQN_BASIC_TEST_INPUT_CONTENT "x sup 2")
set(NEQN_BASIC_TEST_INPUT_FILE "\${CMAKE_CURRENT_BINARY_DIR}/neqn_basic_input.txt")
file(WRITE \${NEQN_BASIC_TEST_INPUT_FILE} \${NEQN_BASIC_TEST_INPUT_CONTENT})

add_test(
    NAME NeqnRunsTest
    COMMAND $<TARGET_FILE:neqn_exe> < \${NEQN_BASIC_TEST_INPUT_FILE}
    WORKING_DIRECTORY \${CMAKE_CURRENT_BINARY_DIR}
)
# This test primarily checks if neqn_exe runs and exits with 0 when processing basic input.
# More complex output checking (e.g., diffing output against a golden file) can be added later.

add_library(neqn_lib STATIC "") # Sources for neqn_lib will be added later
# For now, ensure it links to common settings
target_link_libraries(neqn_lib INTERFACE otroff_common)

add_executable(neqn_exe neqn.cpp) # Changed from main.cpp to neqn.cpp
target_link_libraries(neqn_exe PRIVATE neqn_lib)
