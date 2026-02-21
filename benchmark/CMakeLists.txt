cmake_minimum_required(VERSION 3.5)

project(jsonrefl-bench LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(../include)
add_executable(
    ${PROJECT_NAME}
    main.cpp
    #
    ../include/jsonrefl/jsonrefl.hpp
)
