cmake_minimum_required(VERSION 3.15)
project(lab2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab2
        main.cpp
        tree/parser.cpp
        tree/treedot.cpp
        tree/ast.hpp
        tree/parser.hpp
        tree/treedot.hpp
)

target_include_directories(lab2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
