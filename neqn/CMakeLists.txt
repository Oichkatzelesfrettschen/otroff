project(neqn_project)

add_library(neqn_lib STATIC "") # Sources for neqn_lib will be added later
# For now, ensure it links to common settings
target_link_libraries(neqn_lib INTERFACE otroff_common)

add_executable(neqn_exe neqn.cpp) # Changed from main.cpp to neqn.cpp
target_link_libraries(neqn_exe PRIVATE neqn_lib)
