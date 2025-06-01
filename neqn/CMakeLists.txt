# Build the neqn preprocessor
find_package(BISON)
if(NOT BISON_FOUND)
    message(FATAL_ERROR "Bison not found, cannot process ne.g for neqn target.")
endif()

set(NEQN_YACC_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/ne.g)
set(NEQN_YACC_OUTPUT_C ${CMAKE_CURRENT_BINARY_DIR}/y.tab.c)
set(NEQN_YACC_OUTPUT_H ${CMAKE_CURRENT_BINARY_DIR}/y.tab.h)

add_custom_command(
    OUTPUT ${NEQN_YACC_OUTPUT_C} ${NEQN_YACC_OUTPUT_H}
    COMMAND ${BISON_EXECUTABLE} -d -o ${NEQN_YACC_OUTPUT_C} ${NEQN_YACC_INPUT}
    DEPENDS ${NEQN_YACC_INPUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running Bison on ne.g to generate y.tab.c and y.tab.h"
)

file(GLOB NEQN_SOURCES_USER *.c *.cpp) # User-provided sources
add_executable(neqn ${NEQN_SOURCES_USER} ${NEQN_YACC_OUTPUT_C})
set_source_files_properties(${NEQN_SOURCES_USER} ${NEQN_YACC_OUTPUT_C} PROPERTIES LANGUAGE CXX)

# Explicitly set C++ standard for this target
set_target_properties(neqn PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Include headers from the neqn directory AND the binary directory for generated headers
target_include_directories(neqn PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
# Link against the OS abstraction layer
target_link_libraries(neqn PRIVATE os_abstraction)
