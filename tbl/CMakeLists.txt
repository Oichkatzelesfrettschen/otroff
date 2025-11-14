project(tbl_project)

# Gather all .cpp files, then exclude main and test files
file(GLOB TBL_ALL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

set(TBL_MAIN_SOURCE "t1.cpp")
# No specific test files are known for tbl in its directory from previous listings,
# but if any "*.cpp" test files existed, they would be listed here.
set(TBL_TEST_SOURCES
    # "test_example.cpp"
)

list(REMOVE_ITEM TBL_ALL_SOURCES ${TBL_MAIN_SOURCE})
foreach(test_file ${TBL_TEST_SOURCES})
    list(REMOVE_ITEM TBL_ALL_SOURCES ${test_file})
endforeach()

set(TBL_CORE_SOURCES ${TBL_ALL_SOURCES})

add_library(tbl_core_obj OBJECT ${TBL_CORE_SOURCES})
# Object libraries typically use PRIVATE for linking build-time dependencies
target_link_libraries(tbl_core_obj PRIVATE
    otroff_common
    # os_abstraction is not directly linked here, assuming tbl core doesn't need it.
)

add_executable(tbl_exe ${TBL_MAIN_SOURCE}) # Ensure this uses t1.cpp
target_link_libraries(tbl_exe PRIVATE $<TARGET_OBJECTS:tbl_core_obj>)
