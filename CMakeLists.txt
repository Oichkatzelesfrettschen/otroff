cmake_minimum_required(VERSION 3.5)
    project(otroff C)

        set(CMAKE_C_STANDARD 90)
            set(CMAKE_C_STANDARD_REQUIRED ON)

                if (WIN32)
                    set(OS_WINDOWS TRUE)
                        elseif(APPLE)
                            set(OS_MACOS TRUE)
                                elseif(UNIX)
                                    set(OS_UNIX TRUE)
                                        endif()

                                            add_library(os_abstraction) if (OS_WINDOWS)
                                                target_sources(os_abstraction PRIVATE src / os / os_windows.c) else()
                                                    target_sources(os_abstraction PRIVATE src / os / os_unix.c)
                                                        endif()

                                                            target_include_directories(os_abstraction PUBLIC src / os)

                                                                option(BUILD_ROFF "Build the roff text formatter" OFF)
                                                                    option(BUILD_CROFF "Build the croff formatter" OFF)
                                                                        option(BUILD_NEQN "Build the neqn preprocessor" OFF)

                                                                            if (BUILD_ROFF)
                                                                                add_subdirectory(roff)
                                                                                    endif() if (BUILD_CROFF)
                                                                                        add_subdirectory(croff)
                                                                                            endif() if (BUILD_NEQN)
                                                                                                add_subdirectory(neqn)
                                                                                                    endif()
