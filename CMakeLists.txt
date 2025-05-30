cmake_minimum_required(VERSION 3.10)
    project(otroff C)

        set(CMAKE_C_STANDARD 90)
            set(CMAKE_C_STANDARD_REQUIRED ON)

                file(GLOB ROFF_SRC CONFIGURE_DEPENDS "roff/*.c")
                    add_executable(troff ${ROFF_SRC})
                        target_compile_options(troff PRIVATE - Wall - O2)
