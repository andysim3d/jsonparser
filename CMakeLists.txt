cmake_minimum_required (VERSION 2.6)
project (adjson C)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall -Werror -DDEBUG")
endif()

add_library(adjson adjson.c)
add_executable(adjson_test test.c)
target_link_libraries(adjson_test adjson)
