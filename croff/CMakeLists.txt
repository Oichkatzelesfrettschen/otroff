# Build the croff formatter
file(GLOB CROFF_SOURCES *.c)
add_executable(croff ${CROFF_SOURCES})
# Include headers from the croff directory
target_include_directories(croff PRIVATE ${CMAKE_SOURCE_DIR}/croff)
# Link against the OS abstraction layer
target_link_libraries(croff PRIVATE os_abstraction)
