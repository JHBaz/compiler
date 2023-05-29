cmake_minimum_required(VERSION 3.0.0)
project(COMPILER_2)

set(SOURCE_FILES 
        main.c
        error.c
        file_io.c
        parser.c
        environment.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

