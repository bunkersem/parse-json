cmake_minimum_required (VERSION 2.6)
project (parse-json)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall -Wextra -pedantic -fprofile-arcs -ftest-coverage")

add_executable (parse-json test-main.c json-parser helpers.c)
