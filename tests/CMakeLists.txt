cmake_minimum_required(VERSION 3.8)

project(jsonrefl-test LANGUAGES CXX)

set(JSONREFL_CXX_STANDARD "17" CACHE STRING "C++ standard (14: default strtod fallback; enable JSONREFL_USE_BOOST_CHARCONV for Boost.Charconv).")
set_property(CACHE JSONREFL_CXX_STANDARD PROPERTY STRINGS "14" "17")
set(CMAKE_CXX_STANDARD ${JSONREFL_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable AddressSanitizer")
option(ENABLE_UBSAN "Enable UndefinedBehaviourSanitizer")

if( MSVC )
    add_compile_options(/W4 /permissive-)
    if( NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
        add_compile_options(/Zc:preprocessor)
    endif()
    add_compile_definitions(NOMINMAX)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    if( WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
        # GCC 15 + MinGW: -Wnonnull + libstdc++ inlining gives false positives through jsonrefl::parser.
        add_compile_options(-Wno-nonnull)
    endif()
endif()

if( ENABLE_ASAN )
    if( MSVC )
        add_compile_options(/fsanitize=address /Zi)
        if( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
            # lld-link does not accept /fsanitize=address (treats it as a path). Mirror MSVC
            # link.exe by pulling in the same ASan import libraries; see Microsoft Learn
            # "AddressSanitizer runtime" (/MD and /MT thunk names).
            if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
                set(_jsonrefl_asan_arch "x86_64")
            else()
                set(_jsonrefl_asan_arch "i386")
            endif()
            if( CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "MultiThreaded" OR CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "MultiThreadedDebug" )
                set(_jsonrefl_asan_thunk "clang_rt.asan_static_runtime_thunk-${_jsonrefl_asan_arch}.lib")
            else()
                set(_jsonrefl_asan_thunk "clang_rt.asan_dynamic_runtime_thunk-${_jsonrefl_asan_arch}.lib")
            endif()
            add_link_options(
                /INCREMENTAL:NO
                /DEBUG
                "/DEFAULTLIB:clang_rt.asan_dynamic-${_jsonrefl_asan_arch}.lib"
                "/DEFAULTLIB:${_jsonrefl_asan_thunk}"
            )
        else()
            add_link_options(/fsanitize=address /INCREMENTAL:NO /DEBUG)
        endif()
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    endif()
endif()

if( ENABLE_UBSAN )
    if( NOT MSVC )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    endif()
endif()

message("CMDLINE: ${CMAKE_CXX_FLAGS}")

include_directories(../include)

add_executable(
    ${PROJECT_NAME}
    main.cpp
    main.hpp
    #
    ../include/jsonrefl/jsonrefl.hpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake/JsonreflBoost.cmake")
jsonrefl_apply_boost(${PROJECT_NAME})

target_link_libraries(
    ${PROJECT_NAME}
)

if( WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    target_link_options(
        ${PROJECT_NAME}
        PRIVATE
            -static-libgcc
            -static-libstdc++
            -Wl,-Bstatic
            -lwinpthread
            -Wl,-Bdynamic
    )
endif()
