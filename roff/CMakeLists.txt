# Build the roff formatter
file(GLOB ROFF_SOURCES *.c *.cpp)
add_executable(roff ${ROFF_SOURCES})
# Include headers from the roff directory
target_include_directories(roff PRIVATE ${CMAKE_SOURCE_DIR}/roff)
# Link against the OS abstraction layer
target_link_libraries(roff PRIVATE os_abstraction)
