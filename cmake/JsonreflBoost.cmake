
option(JSONREFL_USE_BOOST_CHARCONV
    "If ON when CMAKE_CXX_STANDARD is 14: define JSONREFL_USE_BOOST_CHARCONV and link Boost.Charconv. \
If OFF (default), jsonrefl uses strtod/strtoll in the header (set LC_NUMERIC=C for correct JSON floats)."
    OFF
)

set(JSONREFL_BOOST_INCLUDE_DIR "" CACHE PATH
    "Directory that contains the boost/ header tree (i.e. passes -I here so <boost/...> resolves).")
set(JSONREFL_BOOST_LIBRARY_DIR "" CACHE PATH
    "Directory with built Boost.Charconv library (e.g. stage/lib). Used only when JSONREFL_USE_BOOST_CHARCONV is ON and C++ standard < 17.")

function(jsonrefl_apply_boost target_name)
    if(JSONREFL_BOOST_INCLUDE_DIR)
        target_include_directories(${target_name} PRIVATE "${JSONREFL_BOOST_INCLUDE_DIR}")
    endif()
    if(JSONREFL_BOOST_LIBRARY_DIR)
        target_link_directories(${target_name} PRIVATE "${JSONREFL_BOOST_LIBRARY_DIR}")
    endif()
    if(JSONREFL_USE_BOOST_CHARCONV AND CMAKE_CXX_STANDARD VERSION_LESS 17)
        target_compile_definitions(${target_name} PRIVATE JSONREFL_USE_BOOST_CHARCONV)
        find_library(
            _jsonrefl_boost_charconv_lib
            NAMES boost_charconv libboost_charconv
            NAMES_PER_DIR
            PATHS "${JSONREFL_BOOST_LIBRARY_DIR}"
            PATH_SUFFIXES ""
            NO_DEFAULT_PATH
        )
        if(NOT _jsonrefl_boost_charconv_lib OR NOT EXISTS "${_jsonrefl_boost_charconv_lib}")
            set(_jsonrefl_boost_charconv_lib "")
            file(GLOB _jsonrefl_boost_charconv_glob
                "${JSONREFL_BOOST_LIBRARY_DIR}/*boost_charconv*.lib"
                "${JSONREFL_BOOST_LIBRARY_DIR}/*boost_charconv*.a"
                "${JSONREFL_BOOST_LIBRARY_DIR}/*boost_charconv*.so"
                "${JSONREFL_BOOST_LIBRARY_DIR}/*boost_charconv*.so.*"
            )
            if(_jsonrefl_boost_charconv_glob)
                set(_jsonrefl_boost_charconv_lib "")
                foreach(_cand IN LISTS _jsonrefl_boost_charconv_glob)
                    if(_cand MATCHES "[.]dll[.]a$")
                        continue()
                    endif()
                    set(_jsonrefl_boost_charconv_lib "${_cand}")
                    break()
                endforeach()
                if(NOT _jsonrefl_boost_charconv_lib)
                    list(SORT _jsonrefl_boost_charconv_glob)
                    list(GET _jsonrefl_boost_charconv_glob 0 _jsonrefl_boost_charconv_lib)
                endif()
            endif()
        endif()
        if(_jsonrefl_boost_charconv_lib AND EXISTS "${_jsonrefl_boost_charconv_lib}")
            target_link_libraries(${target_name} PRIVATE "${_jsonrefl_boost_charconv_lib}")
            if(_jsonrefl_boost_charconv_lib MATCHES "[.]a$"
               AND (
                   (UNIX AND NOT APPLE)
                   OR (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
               )
            )
                target_link_libraries(${target_name} PRIVATE quadmath)
            endif()
        else()
            message(FATAL_ERROR
                "JSONREFL_USE_BOOST_CHARCONV is ON but Boost.Charconv was not found under JSONREFL_BOOST_LIBRARY_DIR "
                "(looked for *boost_charconv*.{lib,a,so}): ${JSONREFL_BOOST_LIBRARY_DIR}")
        endif()
    endif()
endfunction()
