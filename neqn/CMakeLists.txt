# Build the neqn preprocessor
file(GLOB NEQN_SOURCES *.c)
add_executable(neqn ${NEQN_SOURCES})
# Include headers from the neqn directory
target_include_directories(neqn PRIVATE ${CMAKE_SOURCE_DIR}/neqn)
# Link against the OS abstraction layer
target_link_libraries(neqn PRIVATE os_abstraction)
