cmake_minimum_required(VERSION 3.28)
project(LeptJson C)

set(CMAKE_C_STANDARD 11)

add_library(leptjson leptjson.c)
add_executable(LeptJson_test test.c)
target_link_libraries(LeptJson_test leptjson)
