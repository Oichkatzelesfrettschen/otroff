project(tbl_project)

add_library(tbl_lib STATIC "")
target_link_libraries(tbl_lib INTERFACE otroff_common) # Assuming no direct os_abstraction needed

add_executable(tbl_exe main.cpp)
target_link_libraries(tbl_exe PRIVATE tbl_lib)
