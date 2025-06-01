project(croff_project)

add_library(croff_lib STATIC "") # Sources for croff_lib will be added later
# For now, ensure it links to common settings and os_abstraction if needed
target_link_libraries(croff_lib INTERFACE otroff_common os_abstraction)

add_executable(croff_exe n1.cpp) # Changed from main.cpp to n1.cpp
target_link_libraries(croff_exe PRIVATE croff_lib)
