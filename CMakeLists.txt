cmake_minimum_required(VERSION 3.15)
project(lab2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(regexlib STATIC
            tree/parser.cpp
            tree/treedot.cpp)

target_include_directories(regexlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tree)

add_executable(lab2 main.cpp)
target_link_libraries(lab2 PRIVATE regexlib)
