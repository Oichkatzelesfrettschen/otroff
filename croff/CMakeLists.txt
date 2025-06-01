project(croff_project)

# Gather all .cpp files, then exclude main and test files
file(GLOB_RECURSE CROFF_ALL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

set(CROFF_MAIN_SOURCE "n1.cpp")
set(CROFF_TEST_SOURCES
    "test_hytab.cpp"
    "test_n1.cpp"
    "test_n2.cpp"
    "test_n3.cpp"
    "test_n4.cpp"
    "test_n8.cpp"
)

list(REMOVE_ITEM CROFF_ALL_SOURCES ${CROFF_MAIN_SOURCE})
foreach(test_file ${CROFF_TEST_SOURCES})
    list(REMOVE_ITEM CROFF_ALL_SOURCES ${test_file})
endforeach()

set(CROFF_CORE_SOURCES ${CROFF_ALL_SOURCES})

add_library(croff_core_obj OBJECT ${CROFF_CORE_SOURCES})
# Object libraries typically use PRIVATE for linking build-time dependencies
target_link_libraries(croff_core_obj PRIVATE
    otroff_common
    os_abstraction # Assuming core croff logic might need OS functions
)

add_executable(croff_exe ${CROFF_MAIN_SOURCE})
target_link_libraries(croff_exe PRIVATE $<TARGET_OBJECTS:croff_core_obj>)
