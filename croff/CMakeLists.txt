file(GLOB CROFF_SOURCES *.c)
    add_executable(croff ${CROFF_SOURCES})
        target_include_directories(croff PRIVATE ${CMAKE_SOURCE_DIR} / croff)
            target_link_libraries(croff PRIVATE os_abstraction)
