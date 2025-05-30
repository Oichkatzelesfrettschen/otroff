file(GLOB ROFF_SOURCES *.c)
    add_executable(roff ${ROFF_SOURCES})
        target_include_directories(roff PRIVATE ${CMAKE_SOURCE_DIR} / roff)
            target_link_libraries(roff PRIVATE os_abstraction)
