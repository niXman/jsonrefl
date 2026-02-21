cmake_minimum_required(VERSION 3.5)

project(jsonrefl-test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable AddressSanitizer")
option(ENABLE_UBSAN "Enable UndefinedBehaviourSanitizer")

if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
endif()

if(ENABLE_UBSAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")

message("CMDLINE: ${CMAKE_CXX_FLAGS}")

include_directories(../include)

add_executable(
    ${PROJECT_NAME}
    main.cpp
    main.hpp
    #
    ../include/jsonrefl/jsonrefl.hpp
)

target_link_libraries(
    ${PROJECT_NAME}
)
