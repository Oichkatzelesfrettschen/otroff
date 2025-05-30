file(GLOB NEQN_SOURCES *.c)
    add_executable(neqn ${NEQN_SOURCES})
        target_include_directories(neqn PRIVATE ${CMAKE_SOURCE_DIR} / neqn)
            target_link_libraries(neqn PRIVATE os_abstraction)
