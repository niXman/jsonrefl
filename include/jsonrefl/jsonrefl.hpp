
#pragma once

// Copyright 2024-2026 niXman, github.com/nixman/jsonrefl
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <array>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <string>
#include <utility>
#include <ostream>
#include <type_traits>
#include <tuple>
#include <system_error>
#include <limits>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/*************************************************************************************************/

#define __JSONREFL_STRINGIFY_I(x) #x
#define __JSONREFL_STRINGIFY(x) __JSONREFL_STRINGIFY_I(x)

#define JSONREFL_VERSION_MAJOR 1
#define JSONREFL_VERSION_MINOR 0
#define JSONREFL_VERSION_BUGFIX 0

// JSONREFL_VERSION % 100 is the patch level
// JSONREFL_VERSION / 100 % 1000 is the minor version
// JSONREFL_VERSION / 100000 is the major version
#define JSONREFL_VERSION \
    (JSONREFL_VERSION_MAJOR * 100000 \
   + JSONREFL_VERSION_MINOR * 100 \
   + JSONREFL_VERSION_BUGFIX)

#define JSONREFL_VERSION_GET_MAJOR(x)  ((x) / 100000)
#define JSONREFL_VERSION_GET_MINOR(x)  (((x) / 100) % 1000)
#define JSONREFL_VERSION_GET_BUGFIX(x) ((x) % 100)

#define JSONREFL_VERSION_STRING \
    __JSONREFL_STRINGIFY(JSONREFL_VERSION_MAJOR) \
    "." __JSONREFL_STRINGIFY(JSONREFL_VERSION_MINOR) \
    "." __JSONREFL_STRINGIFY(JSONREFL_VERSION_BUGFIX)

/*************************************************************************************************/

#if defined(_MSVC_LANG)
#  define JSONREFL_CXX _MSVC_LANG
#else
#  define JSONREFL_CXX __cplusplus
#endif

#if JSONREFL_CXX >= 201703L
#  define JSONREFL_INLINE_VAR inline
#  include <string_view>
#  include <optional>
#  include <charconv>
#else
#  define JSONREFL_INLINE_VAR
#  include <boost/optional.hpp>
#  include <boost/utility/string_view.hpp>
#  if defined(JSONREFL_USE_BOOST_CHARCONV)
#    include <boost/charconv.hpp>
#  else
#    include <cerrno>
#    include <climits>
#    include <limits>
#  endif
#endif

/*************************************************************************************************/

#define __JSONREFL_CAT_I(a, b) a ## b
#define __JSONREFL_CAT(a, b) __JSONREFL_CAT_I(a, b)

#define __JSONREFL_EXPAND(...) __VA_ARGS__

#define __JSONREFL_INVOKE(m, args) m args

#define __JSONREFL_ARG_N( \
    _1 , _2, _3, _4, _5, _6, _7, _8, _9,_10, \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
    _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
    _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
    _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
    _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
    _61,_62,_63,N,...) N

#define __JSONREFL_REVERSE_SEQ_N() \
    63,62,61,60,                   \
    59,58,57,56,55,54,53,52,51,50, \
    49,48,47,46,45,44,43,42,41,40, \
    39,38,37,36,35,34,33,32,31,30, \
    29,28,27,26,25,24,23,22,21,20, \
    19,18,17,16,15,14,13,12,11,10, \
    9,8,7,6,5,4,3,2,1,0

#define __JSONREFL_NARG_IMPL2(tuple) __JSONREFL_ARG_N tuple

#define __JSONREFL_NARG(...) \
    __JSONREFL_EXPAND( \
        __JSONREFL_EXPAND( \
            __JSONREFL_NARG_IMPL2( \
                (__VA_ARGS__, __JSONREFL_REVERSE_SEQ_N()) \
            ) \
        ) \
    )

#define __JSONREFL_ENUM_ARGS_0(macro, data)
#define __JSONREFL_ENUM_ARGS_1(macro, data, _0) \
    macro(0, data, _0)
#define __JSONREFL_ENUM_ARGS_2(macro, data, _0, _1) \
    __JSONREFL_ENUM_ARGS_1(macro, data, _0) macro(1, data, _1)
#define __JSONREFL_ENUM_ARGS_3(macro, data, _0, _1, _2) \
    __JSONREFL_ENUM_ARGS_2(macro, data, _0, _1) macro(2, data, _2)
#define __JSONREFL_ENUM_ARGS_4(macro, data, _0, _1, _2, _3) \
    __JSONREFL_ENUM_ARGS_3(macro, data, _0, _1, _2) macro(3, data, _3)
#define __JSONREFL_ENUM_ARGS_5(macro, data, _0, _1, _2, _3, _4) \
    __JSONREFL_ENUM_ARGS_4(macro, data, _0, _1, _2, _3) macro(4, data, _4)
#define __JSONREFL_ENUM_ARGS_6(macro, data, _0, _1, _2, _3, _4, _5) \
    __JSONREFL_ENUM_ARGS_5(macro, data, _0, _1, _2, _3, _4) macro(5, data, _5)
#define __JSONREFL_ENUM_ARGS_7(macro, data, _0, _1, _2, _3, _4, _5, _6) \
    __JSONREFL_ENUM_ARGS_6(macro, data, _0, _1, _2, _3, _4, _5) macro(6, data, _6)
#define __JSONREFL_ENUM_ARGS_8(macro, data, _0, _1, _2, _3, _4, _5, _6, _7) \
    __JSONREFL_ENUM_ARGS_7(macro, data, _0, _1, _2, _3, _4, _5, _6) macro(7, data, _7)
#define __JSONREFL_ENUM_ARGS_9(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8) \
    __JSONREFL_ENUM_ARGS_8(macro, data, _0, _1, _2, _3, _4, _5, _6, _7) macro(8, data, _8)
#define __JSONREFL_ENUM_ARGS_10(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9) \
    __JSONREFL_ENUM_ARGS_9(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8) macro(9, data, _9)
#define __JSONREFL_ENUM_ARGS_11(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
    __JSONREFL_ENUM_ARGS_10(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9) macro(10, data, _10)
#define __JSONREFL_ENUM_ARGS_12(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) \
    __JSONREFL_ENUM_ARGS_11(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) macro(11, data, _11)
#define __JSONREFL_ENUM_ARGS_13(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
    __JSONREFL_ENUM_ARGS_12(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) macro(12, data, _12)
#define __JSONREFL_ENUM_ARGS_14(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) \
    __JSONREFL_ENUM_ARGS_13(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) macro(13, data, _13)
#define __JSONREFL_ENUM_ARGS_15(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) \
    __JSONREFL_ENUM_ARGS_14(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) macro(14, data, _14)
#define __JSONREFL_ENUM_ARGS_16(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) \
    __JSONREFL_ENUM_ARGS_15(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) macro(15, data, _15)
#define __JSONREFL_ENUM_ARGS_17(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) \
    __JSONREFL_ENUM_ARGS_16(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) macro(16, data, _16)
#define __JSONREFL_ENUM_ARGS_18(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) \
    __JSONREFL_ENUM_ARGS_17(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) macro(17, data, _17)
#define __JSONREFL_ENUM_ARGS_19(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) \
    __JSONREFL_ENUM_ARGS_18(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) macro(18, data, _18)
#define __JSONREFL_ENUM_ARGS_20(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) \
    __JSONREFL_ENUM_ARGS_19(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) macro(19, data, _19)
#define __JSONREFL_ENUM_ARGS_21(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) \
    __JSONREFL_ENUM_ARGS_20(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) macro(20, data, _20)
#define __JSONREFL_ENUM_ARGS_22(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21) \
    __JSONREFL_ENUM_ARGS_21(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) macro(21, data, _21)
#define __JSONREFL_ENUM_ARGS_23(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22) \
    __JSONREFL_ENUM_ARGS_22(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21) macro(22, data, _22)
#define __JSONREFL_ENUM_ARGS_24(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23) \
    __JSONREFL_ENUM_ARGS_23(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22) macro(23, data, _23)
#define __JSONREFL_ENUM_ARGS_25(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24) \
    __JSONREFL_ENUM_ARGS_24(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23) macro(24, data, _24)
#define __JSONREFL_ENUM_ARGS_26(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25) \
    __JSONREFL_ENUM_ARGS_25(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24) macro(25, data, _25)
#define __JSONREFL_ENUM_ARGS_27(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26) \
    __JSONREFL_ENUM_ARGS_26(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25) macro(26, data, _26)
#define __JSONREFL_ENUM_ARGS_28(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27) \
    __JSONREFL_ENUM_ARGS_27(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26) macro(27, data, _27)
#define __JSONREFL_ENUM_ARGS_29(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28) \
    __JSONREFL_ENUM_ARGS_28(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27) macro(28, data, _28)
#define __JSONREFL_ENUM_ARGS_30(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29) \
    __JSONREFL_ENUM_ARGS_29(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28) macro(29, data, _29)
#define __JSONREFL_ENUM_ARGS_31(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30) \
    __JSONREFL_ENUM_ARGS_30(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29) macro(30, data, _30)
#define __JSONREFL_ENUM_ARGS_32(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31) \
    __JSONREFL_ENUM_ARGS_31(macro, data, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30) macro(31, data, _31)

#define __JSONREFL_ENUM_ARGS_APPLY_I(op, macro, data, ...) op(macro, data, __VA_ARGS__)

#define __JSONREFL_ENUM_ARGS_CAT1(macro, data, n, tup) \
    __JSONREFL_ENUM_ARGS_CAT2(macro, data, n, tup)

#define __JSONREFL_ENUM_ARGS_CAT2(macro, data, n, tup) \
    __JSONREFL_ENUM_ARGS_APPLY_I( \
         __JSONREFL_CAT(__JSONREFL_ENUM_ARGS_, n) \
        ,macro \
        ,data \
        ,__JSONREFL_EXPAND tup \
    )

#define __JSONREFL_ENUM_ARGS_INNER(macro, data, tuple) \
    __JSONREFL_ENUM_ARGS_CAT1( \
         macro \
        ,data \
        ,__JSONREFL_EXPAND(__JSONREFL_INVOKE(__JSONREFL_NARG, tuple)) \
        ,tuple \
    )

#define __JSONREFL_ENUM_ARGS(macro, data, ...) \
    __JSONREFL_EXPAND(__JSONREFL_ENUM_ARGS_INNER(macro, data, (__VA_ARGS__)))

/*************************************************************************************************/

#define __JSONREFL_OBJECT_MEMBER(index, data, elem) \
    ,::jsonrefl::object_member(#elem, data elem)

#define JSONREFL_METADATA(_type, ...)                                            \
    JSONREFL_INLINE_VAR                                                          \
    constexpr auto __jsonrefl_meta_##_type = ::jsonrefl::object_holder(          \
        ::jsonrefl::string_view_t{#_type, sizeof(#_type) - 1u}                   \
        __JSONREFL_ENUM_ARGS(__JSONREFL_OBJECT_MEMBER, &_type::, __VA_ARGS__)    \
    );                                                                           \
    constexpr const auto* __jsonrefl_adl_meta(_type*) noexcept {                 \
        return &__jsonrefl_meta_##_type;                                         \
    }

/*************************************************************************************************/

#define __JSONREFL_PAIR_SECOND(type, name) \
    name

#define __JSONREFL_STRUCT_FIELD_I(type, name) \
    type name;

#define __JSONREFL_STRUCT_FIELD(index, data, elem) \
    __JSONREFL_STRUCT_FIELD_I elem

#define __JSONREFL_STRUCT_META_I(data, name) \
    ::jsonrefl::object_member(__JSONREFL_STRINGIFY(name), data name)

#define __JSONREFL_STRUCT_META(index, data, elem) \
    ,__JSONREFL_STRUCT_META_I(data, __JSONREFL_PAIR_SECOND elem)

#define JSONREFL_STRUCT(_type, ...)                                              \
    struct _type {                                                               \
        __JSONREFL_ENUM_ARGS(__JSONREFL_STRUCT_FIELD, ~, __VA_ARGS__)            \
    };                                                                           \
    JSONREFL_INLINE_VAR                                                          \
    constexpr auto __jsonrefl_meta_##_type = ::jsonrefl::object_holder(          \
        ::jsonrefl::string_view_t{#_type, sizeof(#_type) - 1u}                   \
        __JSONREFL_ENUM_ARGS(__JSONREFL_STRUCT_META, &_type::, __VA_ARGS__)      \
    );                                                                           \
    constexpr const auto* __jsonrefl_adl_meta(_type*) noexcept {                 \
        return &__jsonrefl_meta_##_type;                                         \
    }

/*************************************************************************************************/

#define __JSONREFL_DOC_PAIR_NAME(name, doc) name
#define __JSONREFL_DOC_PAIR_DOC(name, doc)  doc

#define __JSONREFL_OBJECT_MEMBER_DOC(index, data, elem)                  \
    ,::jsonrefl::object_member(                                          \
         __JSONREFL_STRINGIFY(__JSONREFL_DOC_PAIR_NAME elem)             \
        ,__JSONREFL_DOC_PAIR_DOC elem                                    \
        ,data __JSONREFL_DOC_PAIR_NAME elem                              \
    )

#define JSONREFL_METADATA_DOC(_type, ...)                                          \
    JSONREFL_INLINE_VAR                                                            \
    constexpr auto __jsonrefl_meta_##_type = ::jsonrefl::object_holder(            \
        ::jsonrefl::string_view_t{#_type, sizeof(#_type) - 1u}                     \
        __JSONREFL_ENUM_ARGS(__JSONREFL_OBJECT_MEMBER_DOC, &_type::, __VA_ARGS__)  \
    );                                                                             \
    constexpr const auto* __jsonrefl_adl_meta(_type*) noexcept {                   \
        return &__jsonrefl_meta_##_type;                                           \
    }

/*************************************************************************************************/

#define __JSONREFL_DOC_TRIP_NAME(type, name, doc) name
#define __JSONREFL_DOC_TRIP_DOC(type, name, doc)  doc

#define __JSONREFL_STRUCT_FIELD_DOC_I(type, name, doc) \
    type name;

#define __JSONREFL_STRUCT_FIELD_DOC(index, data, elem) \
    __JSONREFL_STRUCT_FIELD_DOC_I elem

#define __JSONREFL_STRUCT_META_DOC(index, data, elem)                    \
    ,::jsonrefl::object_member(                                          \
         __JSONREFL_STRINGIFY(__JSONREFL_DOC_TRIP_NAME elem)             \
        ,__JSONREFL_DOC_TRIP_DOC elem                                    \
        ,data __JSONREFL_DOC_TRIP_NAME elem                              \
    )

#define JSONREFL_STRUCT_DOC(_type, ...)                                            \
    struct _type {                                                                 \
        __JSONREFL_ENUM_ARGS(__JSONREFL_STRUCT_FIELD_DOC, ~, __VA_ARGS__)          \
    };                                                                             \
    JSONREFL_INLINE_VAR                                                            \
    constexpr auto __jsonrefl_meta_##_type = ::jsonrefl::object_holder(            \
        ::jsonrefl::string_view_t{#_type, sizeof(#_type) - 1u}                     \
        __JSONREFL_ENUM_ARGS(__JSONREFL_STRUCT_META_DOC, &_type::, __VA_ARGS__)    \
    );                                                                             \
    constexpr const auto* __jsonrefl_adl_meta(_type*) noexcept {                   \
        return &__jsonrefl_meta_##_type;                                           \
    }

/*************************************************************************************************/

namespace jsonrefl {

#if JSONREFL_CXX >= 201703L
using string_view_t = std::string_view;
template<typename T> using optional_t = std::optional<T>;
template<bool B>
using bool_constant = std::bool_constant<B>;
#else
using string_view_t = boost::string_view;
template<typename T> using optional_t = boost::optional<T>;
template<bool B>
using bool_constant = std::integral_constant<bool, B>;
#endif

/*************************************************************************************************/

namespace details {
    constexpr std::nullptr_t __jsonrefl_adl_meta(...) noexcept { return nullptr; }

    template<typename T>
    constexpr auto __jsonrefl_resolve_meta() noexcept {
        return __jsonrefl_adl_meta(static_cast<T *>(nullptr));
    }
} // ns details

/*************************************************************************************************/

template<typename T>
struct has_metadata: bool_constant<
    !std::is_null_pointer<decltype(details::__jsonrefl_resolve_meta<T>())>::value
> {};

template<typename T>
constexpr decltype(auto) metadata() noexcept {
    return *details::__jsonrefl_resolve_meta<T>();
}

enum class value_kind: std::uint8_t {
     null = 0
    ,string = 1
    ,number = 2
    ,boolean = 3
};

enum class serialize_flags: std::uint8_t {
     none     = 0
    ,pretty   = 1
    ,comments = 2
};

constexpr serialize_flags operator|(serialize_flags a, serialize_flags b) noexcept {
    return static_cast<serialize_flags>(
        static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b)
    );
}

constexpr serialize_flags operator&(serialize_flags a, serialize_flags b) noexcept {
    return static_cast<serialize_flags>(
        static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b)
    );
}

/*************************************************************************************************/

namespace details {

constexpr bool pretty_set(serialize_flags fl) noexcept
{ return static_cast<std::uint8_t>(fl) & static_cast<std::uint8_t>(serialize_flags::pretty); }

constexpr bool comments_set(serialize_flags fl) noexcept
{ return static_cast<std::uint8_t>(fl) & static_cast<std::uint8_t>(serialize_flags::comments); }

enum { k_charconv_buf = 512 };

#if JSONREFL_CXX < 201703L
template<typename Map, typename Key>
std::pair<typename Map::iterator, bool> map_try_emplace(Map& m, Key&& key) {
    auto it = m.find(key);
    if ( it != m.end() ) { return {it, false}; }
    it = m.emplace(std::forward<Key>(key), typename Map::mapped_type{}).first;
    return {it, true};
}
#endif

template<typename Map, typename Key, typename Value>
void map_insert_or_assign(Map &m, Key &&key, Value &&val) {
    auto it = m.find(key);
    if ( it != m.end() ) {
        it->second = std::forward<Value>(val);
    } else {
        m.emplace(std::forward<Key>(key), std::forward<Value>(val));
    }
}

#if JSONREFL_CXX >= 201703L
template<typename Map, typename Key>
auto map_try_emplace(Map& m, Key&& key)
    -> decltype(m.try_emplace(std::forward<Key>(key)))
{ return m.try_emplace(std::forward<Key>(key)); }

using chars_format = std::chars_format;
using from_chars_result = std::from_chars_result;
using to_chars_result = std::to_chars_result;

template<typename ...Args>
inline from_chars_result from_chars(Args&& ...args)
{ return std::from_chars(std::forward<Args>(args)...); }

template<typename ...Args>
inline to_chars_result to_chars(Args&& ...args)
{ return std::to_chars(std::forward<Args>(args)...); }

#elif defined(JSONREFL_USE_BOOST_CHARCONV)

using boost::charconv::chars_format;
using boost::charconv::from_chars_result;
using boost::charconv::to_chars_result;

template<typename ...Args>
inline from_chars_result from_chars(Args&& ...args)
{ return boost::charconv::from_chars(std::forward<Args>(args)...); }

template<typename ...Args>
inline to_chars_result to_chars(Args&& ...args)
{ return boost::charconv::to_chars(std::forward<Args>(args)...); }

#else

/*************************************************************************************************/
// C++14: strtod/strtoll — LC_NUMERIC must be "C" for correct JSON floats with '.'
/*************************************************************************************************/

enum class chars_format {
     scientific = 1
    ,fixed = 2
    ,hex = 4
    ,general = scientific | fixed
};

struct from_chars_result {
    const char *ptr{};
    std::errc ec{std::errc::invalid_argument};
};

struct to_chars_result {
    char *ptr{};
    std::errc ec{};
};

inline bool charconv_copy_nt(
     const char *first
    ,const char *last
    ,char *buf
) noexcept {
    const auto n = static_cast<std::size_t>(last - first);
    if ( n >= k_charconv_buf ) { return false; }
    std::memcpy(buf, first, n);
    buf[n] = '\0';
    return true;
}

inline from_chars_result from_chars_float_g(
     const char *first
    ,const char *last
    ,double &value
    ,chars_format
) noexcept {
    from_chars_result res{first, std::errc::invalid_argument};
    if ( first >= last ) { return res; }
    char buf[k_charconv_buf];
    if ( !charconv_copy_nt(first, last, buf) ) { return res; }
    errno = 0;
    char *endp = nullptr;
    const double parsed = std::strtod(buf, &endp);
    if ( endp == buf ) { return res; }
    // glibc sets ERANGE on subnormal underflow even when the rounded value is non-zero and valid.
    value = parsed;
    res.ptr = first + static_cast<std::size_t>(endp - buf);
    res.ec = std::errc{};
    return res;
}

inline from_chars_result from_chars_float_g(
     const char *first
    ,const char *last
    ,float &value
    ,chars_format
) noexcept {
    from_chars_result res{first, std::errc::invalid_argument};
    if ( first >= last ) { return res; }
    char buf[k_charconv_buf];
    if ( !charconv_copy_nt(first, last, buf) ) { return res; }
    errno = 0;
    char *endp = nullptr;
    const float parsed = ::strtof(buf, &endp);
    if ( endp == buf ) { return res; }
    value = parsed;
    res.ptr = first + static_cast<std::size_t>(endp - buf);
    res.ec = std::errc{};
    return res;
}

template<typename T>
inline typename std::enable_if<
     std::is_integral<T>::value
     && !std::is_same<T, bool>::value
    ,from_chars_result
>::type
from_chars_i(const char *first, const char *last, T &value, int base) noexcept {
    from_chars_result res{first, std::errc::invalid_argument};
    if ( first >= last || (base != 10 && base != 16) ) { return res; }
    char buf[k_charconv_buf];
    if ( !charconv_copy_nt(first, last, buf) ) { return res; }
    errno = 0;
    char *endp = nullptr;
    if ( std::is_unsigned<T>::value ) {
        unsigned long long u = ::strtoull(buf, &endp, base);
        if ( endp == buf ) { return res; }
        if ( errno == ERANGE || u > std::numeric_limits<T>::max() ) {
            res.ptr = first + static_cast<std::size_t>(endp - buf);
            res.ec = std::errc::result_out_of_range;
            return res;
        }
        value = static_cast<T>(u);
    } else {
        long long s = ::strtoll(buf, &endp, base);
        if ( endp == buf ) { return res; }
        if ( errno == ERANGE
            || s > static_cast<long long>(std::numeric_limits<T>::max())
            || s < static_cast<long long>(std::numeric_limits<T>::min()) )
        {
            res.ptr = first + static_cast<std::size_t>(endp - buf);
            res.ec = std::errc::result_out_of_range;
            return res;
        }
        value = static_cast<T>(s);
    }
    res.ptr = first + static_cast<std::size_t>(endp - buf);
    res.ec = std::errc{};
    return res;
}

template<typename T>
inline typename std::enable_if<
     std::is_integral<T>::value
     && !std::is_same<T, bool>::value
    ,to_chars_result
>::type
to_chars_i(char *first, char *last, T value) noexcept {
    to_chars_result res{first, std::errc::value_too_large};
    if ( first >= last ) { return res; }
    const auto cap = static_cast<std::size_t>(last - first);
    char tmp[k_charconv_buf];
    const int n = std::is_signed<T>::value
        ? ::snprintf(tmp, sizeof(tmp), "%lld", static_cast<long long>(value))
        : ::snprintf(tmp, sizeof(tmp), "%llu", static_cast<unsigned long long>(value))
    ;
    if ( n < 0 || static_cast<std::size_t>(n) > cap ) { return res; }
    std::memcpy(first, tmp, static_cast<std::size_t>(n));
    res.ptr = first + static_cast<std::size_t>(n);
    res.ec = std::errc{};
    return res;
}

inline to_chars_result to_chars_d(char *first, char *last, double value, chars_format, int prec) noexcept {
    to_chars_result res{first, std::errc::value_too_large};
    if ( first >= last || prec < 0 ) { return res; }
    const auto cap = static_cast<std::size_t>(last - first);
    char tmp[k_charconv_buf];
    const int n = ::snprintf(tmp, sizeof(tmp), "%.*f", prec, value);
    if ( n < 0 || static_cast<std::size_t>(n) > cap ) { return res; }
    std::memcpy(first, tmp, static_cast<std::size_t>(n));
    res.ptr = first + static_cast<std::size_t>(n);
    res.ec = std::errc{};
    return res;
}

template<typename T>
inline typename std::enable_if<
     std::is_integral<T>::value
     && !std::is_same<T, bool>::value
    ,from_chars_result
>::type
from_chars(const char *first, const char *last, T &value, int base) noexcept
{ return from_chars_i(first, last, value, base); }

inline from_chars_result from_chars(
     const char *first
    ,const char *last
    ,double &value
    ,chars_format fmt
) noexcept
{ return from_chars_float_g(first, last, value, fmt); }

inline from_chars_result from_chars(
     const char *first
    ,const char *last
    ,float &value
    ,chars_format fmt
) noexcept
{ return from_chars_float_g(first, last, value, fmt); }

template<typename T>
inline typename std::enable_if<
     std::is_integral<T>::value
     && !std::is_same<T, bool>::value
    ,to_chars_result
>::type
to_chars(char *first, char *last, T value) noexcept
{ return to_chars_i(first, last, value); }

inline to_chars_result to_chars(
     char *first
    ,char *last
    ,double value
    ,chars_format fmt
    ,int prec
) noexcept
{ return to_chars_d(first, last, value, fmt, prec); }

#endif

inline bool is_hex_json_number(string_view_t str) noexcept
{ return str.length() > 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X'); }

inline bool parse_sv_fully_consumed(
     const char *
    ,const char *last
    ,const char *ptr
) noexcept
{ return ptr == last; }

inline optional_t<bool> parse_bool_sv(string_view_t str) noexcept {
    return (str == "true" || str == "1")
        ? optional_t<bool>{true}
        : (str == "false" || str == "0")
            ? optional_t<bool>{false}
            : optional_t<bool>{}
    ;
}

template<typename T>
inline typename std::enable_if<
     std::is_integral<T>::value
     && !std::is_same<T, bool>::value
    ,optional_t<T>
>::type
parse_integral_sv(string_view_t str) noexcept {
    const int base = is_hex_json_number(str) ? 16 : 10;
    T v{};
    const auto res = details::from_chars(str.data(), str.data() + str.size(), v, base);
    if ( res.ec != std::errc{} ) { return optional_t<T>{}; }
    if ( !parse_sv_fully_consumed(str.data(), str.data() + str.size(), res.ptr) ) {
        return optional_t<T>{};
    }

    return optional_t<T>{v};
}

template<typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, optional_t<T>>::type
parse_floating_sv(string_view_t str) noexcept {
    if ( str == "NaN" )       { return optional_t<T>{std::numeric_limits<T>::quiet_NaN()}; }
    if ( str == "Infinity" )  { return optional_t<T>{std::numeric_limits<T>::infinity()}; }
    if ( str == "-Infinity" ) { return optional_t<T>{-std::numeric_limits<T>::infinity()}; }
    T v{};
    const auto flags = is_hex_json_number(str)
        ? chars_format::hex
        : chars_format::general
    ;
    const auto res = details::from_chars(str.data(), str.data() + str.size(), v, flags);
    if ( res.ec != std::errc{} ) { return optional_t<T>{}; }
    if ( !parse_sv_fully_consumed(str.data(), str.data() + str.size(), res.ptr) ) {
        return optional_t<T>{};
    }

    return optional_t<T>{v};
}

} // ns details

class value_t {
    string_view_t m_sv{};
    value_kind m_kind{value_kind::null};

public:
    value_t() = default;

    void assign(string_view_t sv, value_kind kind) noexcept {
        m_sv = sv;
        m_kind = kind;
    }

    const char* data() const noexcept { return m_sv.data(); }
    std::size_t size() const noexcept { return m_sv.size(); }
    bool empty() const noexcept { return m_sv.empty(); }
    value_kind kind() const noexcept { return m_kind; }

    explicit operator string_view_t() const noexcept { return m_sv; }

    optional_t<bool> to_bool() const noexcept
    { return details::parse_bool_sv(m_sv); }

    optional_t<std::int8_t> to_int8() const noexcept
    { return details::parse_integral_sv<std::int8_t>(m_sv); }

    optional_t<std::uint8_t> to_uint8() const noexcept
    { return details::parse_integral_sv<std::uint8_t>(m_sv); }

    optional_t<std::int16_t> to_int16() const noexcept
    { return details::parse_integral_sv<std::int16_t>(m_sv); }

    optional_t<std::uint16_t> to_uint16() const noexcept
    { return details::parse_integral_sv<std::uint16_t>(m_sv); }

    optional_t<std::int32_t> to_int32() const noexcept
    { return details::parse_integral_sv<std::int32_t>(m_sv); }

    optional_t<std::uint32_t> to_uint32() const noexcept
    { return details::parse_integral_sv<std::uint32_t>(m_sv); }

    optional_t<std::int64_t> to_int64() const noexcept
    { return details::parse_integral_sv<std::int64_t>(m_sv); }

    optional_t<std::uint64_t> to_uint64() const noexcept
    { return details::parse_integral_sv<std::uint64_t>(m_sv); }

    optional_t<float> to_float() const noexcept
    { return details::parse_floating_sv<float>(m_sv); }

    optional_t<double> to_double() const noexcept
    { return details::parse_floating_sv<double>(m_sv); }

    optional_t<std::string> to_string() const {
        if ( m_kind == value_kind::null && m_sv.empty() ) { return optional_t<std::string>{}; }

        return optional_t<std::string>{std::string(m_sv.data(), m_sv.size())};
    }

    template<typename T>
    typename std::enable_if<std::is_same<T, bool>::value, optional_t<T>>::type
    to() const noexcept
    { return to_bool(); }

    template<typename T>
    typename std::enable_if<
         std::is_integral<T>::value
         && !std::is_same<T, bool>::value
        ,optional_t<T>
    >::type
    to() const noexcept
    { return details::parse_integral_sv<T>(m_sv); }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, optional_t<T>>::type
    to() const noexcept
    { return details::parse_floating_sv<T>(m_sv); }

    template<typename T>
    typename std::enable_if<std::is_enum<T>::value, optional_t<T>>::type
    to() const noexcept {
        const auto raw = details::parse_integral_sv<std::underlying_type_t<T>>(m_sv);
        return raw
            ? optional_t<T>{static_cast<T>(*raw)}
            : optional_t<T>{}
        ;
    }
};

/*************************************************************************************************/
// calculate the required maximum stack size

namespace details {

constexpr std::size_t jsonrefl_max() noexcept
{ return 0; }

template<typename T>
constexpr T jsonrefl_max(T v)
{ return v; }

template<typename T>
constexpr T jsonrefl_max(T l, T r)
{ return (l > r) ? l : r; }

template<typename T, typename... Ts>
constexpr auto jsonrefl_max(T a0, T a1, Ts... ts)
{ return jsonrefl_max(jsonrefl_max(a0, a1), ts...); }

template<typename F, typename Tuple, std::size_t ...I>
auto tuple_apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
    -> decltype(std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...))
{ return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...); }

template<typename F, typename Tuple>
auto tuple_apply(F&& f, Tuple&& t)
    -> decltype(tuple_apply_impl(std::forward<F>(f), std::forward<Tuple>(t),
        std::make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>{}))
{
    return tuple_apply_impl(std::forward<F>(f), std::forward<Tuple>(t),
        std::make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>{});
}

template<typename F, typename Tuple, std::size_t ...I>
void tuple_for_each_impl(F&& f, Tuple&& t, std::index_sequence<I...>) {
    using expander = int[];
    (void)expander{0, ((void)std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))), 0)...};
}

template<typename F, typename Tuple>
void tuple_for_each(F&& f, Tuple&& t) {
    tuple_for_each_impl(std::forward<F>(f), std::forward<Tuple>(t),
        std::make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>{});
}

/*************************************************************************************************/


template<typename It, typename T>
constexpr It upper_bound(It first, It last, const T &value) noexcept {
    auto count = last - first;
    while ( count > 0 ) {
        auto step = count / 2;
        auto mid = first + step;
        !(value < *mid)
            ? (first = mid + 1, count -= step + 1)
            : (count = step)
        ;
    }

    return first;
}

/*************************************************************************************************/

constexpr std::uint32_t fnv1a(const char *p, std::size_t n) noexcept {
    auto seed = 0x811c9dc5u;
    for ( std::size_t i = 0; i < n; ++i ) {
        seed = (seed ^ static_cast<std::uint32_t>(static_cast<unsigned char>(p[i]))) * 0x01000193u;
    }

    return seed;
}

constexpr std::uint32_t fnv1a(string_view_t s) noexcept {
    return fnv1a(s.data(), s.size());
}

/*************************************************************************************************/

template<typename>
struct is_string_like_t: std::false_type {};

template<>
struct is_string_like_t<std::string>: std::true_type {};

template<>
struct is_string_like_t<string_view_t>: std::true_type {};

template<typename>
struct is_array_type: std::false_type {};

template<typename T, typename A>
struct is_array_type<std::vector<T, A>>: std::true_type {};

template<typename T, typename A>
struct is_array_type<std::list<T, A>>: std::true_type {};

template<typename>
struct is_object_type : std::false_type {};

template<typename K, typename V, typename A>
struct is_object_type<std::map<K, V, A>>: std::true_type {};

template<typename K, typename V, typename A>
struct is_object_type<std::unordered_map<K, V, A>>: std::true_type {};

template<typename T>
struct is_object_or_array: bool_constant<
    (is_object_type<T>::value || is_array_type<T>::value)
> {};

template<typename>
struct is_optional_type : std::false_type {};

template<typename T>
struct is_optional_type<optional_t<T>> : std::true_type {};

template<typename T>
struct unwrap_optional { using type = T; };

template<typename T>
struct unwrap_optional<optional_t<T>> { using type = T; };

template<typename T>
using unwrap_optional_t = typename unwrap_optional<T>::type;

static constexpr std::size_t k_indent_step_len = 3u;

inline std::uint8_t esc_extra(unsigned char c) noexcept {
    static constexpr std::uint8_t data[256] = {
        5,5,5,5,5,5,5,5,1,1,1,5,1,1,5,5,
        5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
        0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };

    return data[c];
}

inline char esc_replacement(unsigned char c) noexcept {
    static constexpr char data[256] = {
        0,0,0,0,0,0,0,0,'b','t','n',0,'f','r',0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,'"',0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,'\\',0,0,0,
    };

    return data[c];
}

inline std::size_t float_chars_count(double v) noexcept {
    static constexpr double pow10[] = {
         1e0,   1e1,   1e2,   1e3,   1e4,   1e5,   1e6,   1e7,   1e8,   1e9
        ,1e10,  1e11,  1e12,  1e13,  1e14,  1e15,  1e16,  1e17,  1e18,  1e19
        ,1e20,  1e21,  1e22,  1e23,  1e24,  1e25,  1e26,  1e27,  1e28,  1e29
        ,1e30,  1e31,  1e32,  1e33,  1e34,  1e35,  1e36,  1e37,  1e38,  1e39
        ,1e40,  1e41,  1e42,  1e43,  1e44,  1e45,  1e46,  1e47,  1e48,  1e49
        ,1e50,  1e51,  1e52,  1e53,  1e54,  1e55,  1e56,  1e57,  1e58,  1e59
        ,1e60,  1e61,  1e62,  1e63,  1e64,  1e65,  1e66,  1e67,  1e68,  1e69
        ,1e70,  1e71,  1e72,  1e73,  1e74,  1e75,  1e76,  1e77,  1e78,  1e79
        ,1e80,  1e81,  1e82,  1e83,  1e84,  1e85,  1e86,  1e87,  1e88,  1e89
        ,1e90,  1e91,  1e92,  1e93,  1e94,  1e95,  1e96,  1e97,  1e98,  1e99
        ,1e100, 1e101, 1e102, 1e103, 1e104, 1e105, 1e106, 1e107, 1e108, 1e109
        ,1e110, 1e111, 1e112, 1e113, 1e114, 1e115, 1e116, 1e117, 1e118, 1e119
        ,1e120, 1e121, 1e122, 1e123, 1e124, 1e125, 1e126, 1e127, 1e128, 1e129
        ,1e130, 1e131, 1e132, 1e133, 1e134, 1e135, 1e136, 1e137, 1e138, 1e139
        ,1e140, 1e141, 1e142, 1e143, 1e144, 1e145, 1e146, 1e147, 1e148, 1e149
        ,1e150, 1e151, 1e152, 1e153, 1e154, 1e155, 1e156, 1e157, 1e158, 1e159
        ,1e160, 1e161, 1e162, 1e163, 1e164, 1e165, 1e166, 1e167, 1e168, 1e169
        ,1e170, 1e171, 1e172, 1e173, 1e174, 1e175, 1e176, 1e177, 1e178, 1e179
        ,1e180, 1e181, 1e182, 1e183, 1e184, 1e185, 1e186, 1e187, 1e188, 1e189
        ,1e190, 1e191, 1e192, 1e193, 1e194, 1e195, 1e196, 1e197, 1e198, 1e199
        ,1e200, 1e201, 1e202, 1e203, 1e204, 1e205, 1e206, 1e207, 1e208, 1e209
        ,1e210, 1e211, 1e212, 1e213, 1e214, 1e215, 1e216, 1e217, 1e218, 1e219
        ,1e220, 1e221, 1e222, 1e223, 1e224, 1e225, 1e226, 1e227, 1e228, 1e229
        ,1e230, 1e231, 1e232, 1e233, 1e234, 1e235, 1e236, 1e237, 1e238, 1e239
        ,1e240, 1e241, 1e242, 1e243, 1e244, 1e245, 1e246, 1e247, 1e248, 1e249
        ,1e250, 1e251, 1e252, 1e253, 1e254, 1e255, 1e256, 1e257, 1e258, 1e259
        ,1e260, 1e261, 1e262, 1e263, 1e264, 1e265, 1e266, 1e267, 1e268, 1e269
        ,1e270, 1e271, 1e272, 1e273, 1e274, 1e275, 1e276, 1e277, 1e278, 1e279
        ,1e280, 1e281, 1e282, 1e283, 1e284, 1e285, 1e286, 1e287, 1e288, 1e289
        ,1e290, 1e291, 1e292, 1e293, 1e294, 1e295, 1e296, 1e297, 1e298, 1e299
        ,1e300, 1e301, 1e302, 1e303, 1e304, 1e305, 1e306, 1e307, 1e308
    };
    if ( v != v ) { return 4; }  // NaN → "null"
    const auto n = (v < 0.0) ? 1 : 0;
    const auto av = v < 0.0 ? -v : v;
    if ( av * 0.0 != 0.0 ) { return 4; }  // ±Inf → "null"
    const auto it = upper_bound(pow10, pow10 + sizeof(pow10)/sizeof(pow10[0]), av);
    auto int_digits = static_cast<std::size_t>(it - pow10);
    if ( int_digits == 0 ) { int_digits = 1; }

    return n + int_digits + 7;
}

template<typename U>
std::size_t required_bytes(const U &v, serialize_flags fl, std::size_t indent) noexcept;

inline std::size_t required_bytes_value_t(const value_t &v) noexcept {
    switch ( v.kind() ) {
        case value_kind::null:
            return 4u;

        case value_kind::string: {
            auto n = 2u;
            for ( char c : static_cast<string_view_t>(v) ) {
                n += 1u + esc_extra(static_cast<unsigned char>(c));
            }

            return n;
        }
        case value_kind::number:
        case value_kind::boolean:
            return v.size();

        default:
            return 4u;
    }
}

inline char* write_value_t_json(char *ptr, const value_t &v) noexcept {
    switch ( v.kind() ) {
        case value_kind::null: {
            std::memcpy(ptr, "null", 4);

            return ptr + 4;
        }
        case value_kind::number:
        case value_kind::boolean: {
            std::memcpy(ptr, v.data(), v.size());

            return ptr + v.size();
        }
        case value_kind::string: {
            static constexpr char kHex[] = "0123456789abcdef";
            *ptr++ = '"';
            const auto sv = static_cast<string_view_t>(v);
            const char *src = sv.data();
            const char *const end = src + sv.size();
            while ( src != end ) {
                const char *run = src;
                while ( run != end && !esc_extra(static_cast<unsigned char>(*run)) ) { ++run; }
                if ( run != src ) {
                    std::memcpy(ptr, src, static_cast<std::size_t>(run - src));
                    ptr += run - src;
                }
                if ( run != end ) {
                    const auto uc = static_cast<unsigned char>(*run);
                    if ( esc_replacement(uc) ) {
                        *ptr++ = '\\'; *ptr++ = esc_replacement(uc);
                    } else {
                        ptr[0]='\\'; ptr[1]='u'; ptr[2]='0'; ptr[3]='0';
                        ptr[4]=kHex[uc>>4]; ptr[5]=kHex[uc&0xFu]; ptr+=6;
                    }
                    ++run;
                }
                src = run;
            }
            *ptr++ = '"';

            return ptr;
        }
        default: {
            assert(false && "unexpected value_kind");
            std::memcpy(ptr, "null", 4);

            return ptr + 4;
        }
    }
}

template<typename U>
char* write_json(char *ptr, const U &v, serialize_flags fl, std::size_t indent) noexcept;


template<typename U, typename = void>
struct required_bytes_j {
    static std::size_t go(const U &, serialize_flags, std::size_t) noexcept
    { return 4; }
};

template<typename U>
struct required_bytes_j<
     U
    ,typename std::enable_if<has_metadata<U>::value>::type>
{
    static std::size_t go(const U &v, serialize_flags fl, std::size_t indent) noexcept
    { return metadata<U>().required_bytes(&v, fl, indent); }
};

template<typename U>
struct required_bytes_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && std::is_same<U, value_t>::value
    >::type
> {
    static std::size_t go(const value_t &v, serialize_flags, std::size_t) noexcept
    { return required_bytes_value_t(v); }
};

template<typename U>
struct required_bytes_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && !std::is_same<U, value_t>::value
        && is_string_like_t<U>::value
    >::type
> {
    static std::size_t go(const U &v, serialize_flags, std::size_t) noexcept {
        auto n = 2u;
        for ( char c : v ) {
            n += 1 + esc_extra(static_cast<unsigned char>(c));
        }
        return n;
    }
};

template<typename U>
struct required_bytes_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && !is_string_like_t<U>::value
        && std::is_same<U, bool>::value
    >::type
> {
    static std::size_t go(const U &v, serialize_flags, std::size_t) noexcept
    { return v ? 4u : 5u; }
};

template<typename U>
struct required_bytes_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && !is_string_like_t<U>::value
        && !std::is_same<U, bool>::value
        && std::is_integral<U>::value
    >::type
> {
    static std::size_t go(const U &v, serialize_flags, std::size_t) noexcept {
        if ( v == 0 ) { return 1; }
        static constexpr std::uint64_t kPow10[] = {
             1ull
            ,10ull
            ,100ull
            ,1000ull
            ,10000ull
            ,100000ull
            ,1000000ull
            ,10000000ull
            ,100000000ull
            ,1000000000ull
            ,10000000000ull
            ,100000000000ull
            ,1000000000000ull
            ,10000000000000ull
            ,100000000000000ull
            ,1000000000000000ull
            ,10000000000000000ull
            ,100000000000000000ull
            ,1000000000000000000ull
            ,10000000000000000000ull
        };
        const auto n_sign = static_cast<std::size_t>(std::is_signed<U>::value && v < 0);
        typename std::make_unsigned_t<U> uv;
        if ( std::is_signed<U>::value && v < 0 ) {
            uv = static_cast<typename std::make_unsigned_t<U>>(0)
                - static_cast<typename std::make_unsigned_t<U>>(v);
        } else {
            uv = static_cast<typename std::make_unsigned_t<U>>(v);
        }
        const auto *it = upper_bound(
             std::begin(kPow10)
            ,std::end(kPow10)
            ,static_cast<std::uint64_t>(uv)
        );
        return n_sign + static_cast<std::size_t>(it - std::begin(kPow10));
    }
};

template<typename U>
struct required_bytes_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && !is_string_like_t<U>::value
        && !std::is_same<U, bool>::value
        && !std::is_integral<U>::value
        && std::is_enum<U>::value
    >::type
> {
    static std::size_t go(const U &v, serialize_flags fl, std::size_t indent) noexcept
    { return required_bytes(static_cast<std::underlying_type_t<U>>(v), fl, indent); }
};

template<typename U>
struct required_bytes_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && !is_string_like_t<U>::value
        && !std::is_same<U, bool>::value
        && !std::is_integral<U>::value
        && !std::is_enum<U>::value
        && std::is_floating_point<U>::value
    >::type
> {
    static std::size_t go(const U &v, serialize_flags, std::size_t) noexcept
    { return float_chars_count(static_cast<double>(v)); }
};

template<typename U>
struct required_bytes_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && is_array_type<U>::value
    >::type
> {
    static std::size_t go(const U &v, serialize_flags fl, std::size_t indent) noexcept {
        if ( v.empty() ) { return 2; }
        const bool pretty = pretty_set(fl);
        const auto child = pretty ? indent + k_indent_step_len : std::size_t{0};
        std::size_t n = 1;
        if ( pretty ) { n += 1; }
        bool first = true;
        for ( const auto &elem : v ) {
            if ( !first ) {
                n += 1;
                if ( pretty ) { n += 1; }
            }
            if ( pretty ) { n += child; }
            n += required_bytes(elem, fl, child);
            first = false;
        }
        if ( pretty ) { n += 1; n += indent; }
        n += 1;
        return n;
    }
};

template<typename U>
struct required_bytes_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && !is_array_type<U>::value
        && is_object_type<U>::value
    >::type
> {
    static std::size_t go(const U &v, serialize_flags fl, std::size_t indent) noexcept {
        if ( v.empty() ) { return 2; }
        const bool pretty = pretty_set(fl);
        const auto child = pretty ? indent + k_indent_step_len : std::size_t{0};
        std::size_t n = 1;
        if ( pretty ) { n += 1; }
        bool first = true;
        for ( const auto &pr : v ) {
            const auto &k = pr.first;
            const auto &val = pr.second;
            if ( !first ) {
                n += 1;
                if ( pretty ) { n += 1; }
            }
            if ( pretty ) { n += child; }
            n += required_bytes(k, fl, child);
            n += pretty ? 2 : 1;
            n += required_bytes(val, fl, child);
            first = false;
        }
        if ( pretty ) { n += 1; n += indent; }
        n += 1;
        return n;
    }
};

template<typename U>
struct required_bytes_j<
     U
    ,typename std::enable_if<is_optional_type<U>::value>::type
> {
    static std::size_t go(const U &v, serialize_flags fl, std::size_t indent) noexcept
    { return v.has_value() ? required_bytes(*v, fl, indent) : 4; }
};

template<typename U>
std::size_t required_bytes(const U &v, serialize_flags fl, std::size_t indent) noexcept
{ return required_bytes_j<U>::go(v, fl, indent); }



template<typename U, typename = void>
struct write_json_j {
    static char* go(char *ptr, const U &, serialize_flags, std::size_t) noexcept
    { std::memcpy(ptr, "null", 4); return ptr + 4; }
};

template<typename U>
struct write_json_j<U, typename std::enable_if<has_metadata<U>::value>::type> {
    static char* go(char *ptr, const U &v, serialize_flags fl, std::size_t indent) noexcept
    { return metadata<U>().to_buffer(&v, ptr, fl, indent); }
};

template<typename U>
struct write_json_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && std::is_same<U, value_t>::value
    >::type
> {
    static char* go(char *ptr, const value_t &v, serialize_flags, std::size_t) noexcept
    { return write_value_t_json(ptr, v); }
};

template<typename U>
struct write_json_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && !std::is_same<U, value_t>::value
        && is_string_like_t<U>::value
    >::type
> {
    static char* go(char *ptr, const U &v, serialize_flags, std::size_t) noexcept {
        static constexpr char kHex[] = "0123456789abcdef";
        *ptr++ = '"';
        const char *src = v.data();
        const char *const end = src + v.size();
        while ( src != end ) {
            const char *run = src;
            while ( run != end && !esc_extra(static_cast<unsigned char>(*run)) ) { ++run; }
            if ( run != src ) {
                std::memcpy(ptr, src, static_cast<std::size_t>(run - src));
                ptr += run - src;
            }
            if ( run != end ) {
                const auto uc = static_cast<unsigned char>(*run);
                if ( esc_replacement(uc) ) {
                    *ptr++ = '\\';
                    *ptr++ = esc_replacement(uc);
                } else {
                    // \u00XX for C0 control characters
                    ptr[0] = '\\'; ptr[1] = 'u'; ptr[2] = '0'; ptr[3] = '0';
                    ptr[4] = kHex[uc >> 4]; ptr[5] = kHex[uc & 0xFu];
                    ptr += 6;
                }
                ++run;
            }
            src = run;
        }
        *ptr++ = '"';

        return ptr;
    }
};

template<typename U>
struct write_json_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && !is_string_like_t<U>::value
        && std::is_same<U, bool>::value
    >::type
> {
    static char* go(char *ptr, const U &v, serialize_flags, std::size_t) noexcept {
        const unsigned len = v ? 4u : 5u;
        std::memcpy(ptr, v ? "true" : "false", len);
        return ptr + len;
    }
};

template<typename U>
struct write_json_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && !is_string_like_t<U>::value
        && !std::is_same<U, bool>::value
        && std::is_integral<U>::value
    >::type
> {
    static char* go(char *ptr, const U &v, serialize_flags, std::size_t) noexcept {
        auto res = details::to_chars(ptr, ptr + 24, v);
        if ( res.ec != std::errc{} ) { return ptr; }
        return res.ptr;
    }
};

template<typename U>
struct write_json_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && !is_string_like_t<U>::value
        && !std::is_same<U, bool>::value
        && !std::is_integral<U>::value
        && std::is_enum<U>::value
    >::type
> {
    static char* go(char *ptr, const U &v, serialize_flags fl, std::size_t indent) noexcept
    { return write_json(ptr, static_cast<std::underlying_type_t<U>>(v), fl, indent); }
};

template<typename U>
struct write_json_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && !is_string_like_t<U>::value
        && !std::is_same<U, bool>::value
        && !std::is_integral<U>::value
        && !std::is_enum<U>::value
        && std::is_floating_point<U>::value
    >::type
> {
    static char* go(char *ptr, const U &v, serialize_flags, std::size_t) noexcept {
        const auto dv = static_cast<double>(v);
        if ( dv != dv || dv * 0.0 != 0.0 ) {
            std::memcpy(ptr, "null", 4);
            return ptr + 4;
        }
        char tmp[k_charconv_buf];
        const auto res = details::to_chars(tmp, tmp + sizeof(tmp), dv, chars_format::fixed, 6);
        if ( res.ec != std::errc{} ) { return ptr; }
        const auto n = static_cast<std::size_t>(res.ptr - tmp);
        std::memcpy(ptr, tmp, n);
        return ptr + n;
    }
};

template<typename U>
struct write_json_j<
    U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && is_array_type<U>::value
    >::type
> {
    static char* go(char *ptr, const U &v, serialize_flags fl, std::size_t indent) noexcept {
        if ( v.empty() ) { std::memcpy(ptr, "[]", 2); return ptr + 2; }
        const bool pretty = pretty_set(fl);
        const auto child = pretty ? indent + k_indent_step_len : std::size_t{0};
        *ptr++ = '[';
        if ( pretty ) { *ptr++ = '\n'; }
        bool first = true;
        for ( const auto &elem : v ) {
            if ( !first ) {
                *ptr++ = ',';
                if ( pretty ) { *ptr++ = '\n'; }
            }
            if ( pretty ) { std::memset(ptr, ' ', child); ptr += child; }
            ptr = write_json(ptr, elem, fl, child);
            first = false;
        }
        if ( pretty ) {
            *ptr++ = '\n';
            std::memset(ptr, ' ', indent); ptr += indent;
        }
        *ptr++ = ']';
        return ptr;
    }
};

template<typename U>
struct write_json_j<
     U
    ,typename std::enable_if<
        !has_metadata<U>::value
        && !is_array_type<U>::value
        && is_object_type<U>::value
    >::type
> {
    static char* go(char *ptr, const U &v, serialize_flags fl, std::size_t indent) noexcept {
        if ( v.empty() ) { std::memcpy(ptr, "{}", 2); return ptr + 2; }
        const bool pretty = pretty_set(fl);
        const auto child = pretty ? indent + k_indent_step_len : std::size_t{0};
        *ptr++ = '{';
        if ( pretty ) { *ptr++ = '\n'; }
        bool first = true;
        for ( const auto &pr : v ) {
            const auto &k = pr.first;
            const auto &val = pr.second;
            if ( !first ) {
                *ptr++ = ',';
                if ( pretty ) { *ptr++ = '\n'; }
            }
            if ( pretty ) { std::memset(ptr, ' ', child); ptr += child; }
            ptr = write_json(ptr, k, fl, child);
            pretty ? (*ptr++ = ':', *ptr++ = ' ') : (*ptr++ = ':');
            ptr = write_json(ptr, val, fl, child);
            first = false;
        }
        if ( pretty ) {
            *ptr++ = '\n';
            std::memset(ptr, ' ', indent); ptr += indent;
        }
        *ptr++ = '}';
        return ptr;
    }
};

template<typename U>
struct write_json_j<
     U
    ,typename std::enable_if<is_optional_type<U>::value>::type
> {
    static char* go(char *ptr, const U &v, serialize_flags fl, std::size_t indent) noexcept {
        return v.has_value()
            ? write_json(ptr, *v, fl, indent)
            : (std::memcpy(ptr, "null", 4), ptr + 4)
        ;
    }
};

template<typename U>
char* write_json(char *ptr, const U &v, serialize_flags fl, std::size_t indent) noexcept
{ return write_json_j<U>::go(ptr, v, fl, indent); }


struct chunked_writer {
    using flush_fn_t = bool(*)(void *ctx, const void *data, std::size_t size);

    char *const buf;
    const std::size_t capacity;
    std::size_t pos;
    flush_fn_t flush_fn;
    void *ctx;
    bool ok;

    chunked_writer(char *b, std::size_t cap, flush_fn_t fn, void *c) noexcept
        :buf{b}
        ,capacity{cap}
        ,pos{0}
        ,flush_fn{fn}
        ,ctx{c}
        ,ok{true}
    {}

    bool put(char c) noexcept {
        if ( pos == capacity && !flush() ) { return false; }
        buf[pos++] = c;

        return true;
    }

    bool write(const char *data, std::size_t len) noexcept {
        while ( ok && len > 0 ) {
            const auto avail = capacity - pos;
            const auto n = len < avail ? len : avail;
            std::memcpy(buf + pos, data, n);
            pos += n;
            data += n;
            len -= n;
            if ( pos == capacity && !flush() ) { return false; }
        }

        return ok;
    }

    bool fill(char c, std::size_t count) noexcept {
        while ( ok && count > 0 ) {
            const auto avail = capacity - pos;
            const auto n = count < avail ? count : avail;
            std::memset(buf + pos, c, n);
            pos += n;
            count -= n;
            if ( pos == capacity && !flush() ) { return false; }
        }

        return ok;
    }

    bool write_token(
         const char *data
        ,std::size_t len
        ,const char *tail = nullptr
        ,std::size_t tail_len = 0
    ) noexcept {
        const auto total = len + tail_len;
        if ( capacity - pos < total ) {
            if ( !flush() ) { return false; }
        }
        if ( total > capacity ) {
            if ( !write(data, len) ) { return false; }

            return tail_len ? write(tail, tail_len) : true;
        }
        std::memcpy(buf + pos, data, len);
        if ( tail_len ) { std::memcpy(buf + pos + len, tail, tail_len); }
        pos += total;

        return true;
    }

    bool write_quoted_token(
         string_view_t sv
        ,const char *tail = nullptr
        ,std::size_t tail_len = 0
    ) noexcept {
        static constexpr char kHex[] = "0123456789abcdef";
        auto total = 2u + tail_len;
        for ( auto c : sv )
            total += 1u + esc_extra(static_cast<unsigned char>(c));
        if ( capacity - pos < total ) {
            if ( !flush() ) { return false; }
        }
        if ( capacity - pos >= total ) {
            buf[pos++] = '"';
            for ( auto c : sv ) {
                const auto uc = static_cast<unsigned char>(c);
                if ( !esc_extra(uc) ) {
                    buf[pos++] = c;
                } else if ( esc_replacement(uc) ) {
                    buf[pos++] = '\\'; buf[pos++] = esc_replacement(uc);
                } else {
                    buf[pos++] = '\\'; buf[pos++] = 'u';
                    buf[pos++] = '0';  buf[pos++] = '0';
                    buf[pos++] = kHex[uc >> 4]; buf[pos++] = kHex[uc & 0xFu];
                }
            }
            buf[pos++] = '"';
            if ( tail_len ) { std::memcpy(buf + pos, tail, tail_len); pos += tail_len; }

            return true;
        }
        if ( !put('"') ) { return false; }
        const char *src = sv.data();
        const char *const end = src + sv.size();
        while ( src != end ) {
            const char *run = src;
            while ( run != end && !esc_extra(static_cast<unsigned char>(*run)) )
                ++run;
            if ( run != src ) {
                if ( !write(src, static_cast<std::size_t>(run - src)) ) { return false; }
            }
            if ( run != end ) {
                const auto uc = static_cast<unsigned char>(*run);
                if ( esc_replacement(uc) ) {
                    const char esc[2] = {'\\', esc_replacement(uc)};
                    if ( !write(esc, 2) ) { return false; }
                } else {
                    const char esc[6] = {
                         '\\', 'u', '0', '0'
                        ,kHex[uc >> 4], kHex[uc & 0xFu]
                    };
                    if ( !write(esc, 6) ) { return false; }
                }
                ++run;
            }
            src = run;
        }
        if ( !put('"') ) { return false; }

        return tail_len ? write(tail, tail_len) : true;
    }

    template<bool Pretty>
    bool write_field_key(string_view_t name, std::size_t indent) noexcept {
        const auto ind = Pretty ? indent : std::size_t{0};
        const auto total = ind + 1 + name.size() + (Pretty ? 3 : 2);
        if ( capacity - pos < total && !flush() ) { return false; }
        if ( capacity - pos >= total ) {
            if ( Pretty ) { std::memset(buf + pos, ' ', indent); pos += indent; }
            buf[pos++] = '"';
            std::memcpy(buf + pos, name.data(), name.size());
            pos += name.size();
            pos += Pretty
                ? (std::memcpy(buf + pos, "\": ", 3), 3)
                : (std::memcpy(buf + pos, "\":", 2), 2)
            ;

            return true;
        }
        if ( Pretty ) { if ( !fill(' ', indent) ) { return false; } }
        if ( !put('"') ) { return false; }
        if ( !write(name.data(), name.size()) ) { return false; }
        return Pretty ? write("\": ", 3) : write("\":", 2);
    }

    char* reserve(std::size_t n) noexcept {
        if ( capacity - pos < n ) {
            if ( !flush() ) { return nullptr; }
        }

        return buf + pos;
    }
    void advance(std::size_t n) noexcept { pos += n; }

    bool flush() noexcept {
        if ( pos > 0 ) {
            ok = flush_fn(ctx, buf, pos);
            pos = 0;
        }

        return ok;
    }
};

inline bool stream_value_t_json(
     chunked_writer &w
    ,const value_t &v
    ,const char *tail
    ,std::size_t tail_len
) noexcept {
    switch ( v.kind() ) {
        case value_kind::null:
            return w.write_token("null", 4, tail, tail_len);

        case value_kind::number:
        case value_kind::boolean:
            return w.write_token(v.data(), v.size(), tail, tail_len);

        case value_kind::string:
            return w.write_quoted_token(static_cast<string_view_t>(v), tail, tail_len);

        default: {
            assert(false && "unexpected value_kind");
            return false;
        }
    }
}

template<bool Pretty, typename U>
std::enable_if_t<has_metadata<U>::value, bool>
stream_json(
     chunked_writer &w
    ,const U &v
    ,std::size_t indent
    ,const char *tail
    ,std::size_t tail_len
    ,serialize_flags fl) noexcept
{ return metadata<U>().to_stream(&v, w, fl, indent, tail, tail_len); }

template<bool Pretty, typename U>
std::enable_if_t<!has_metadata<U>::value && std::is_same<U, value_t>::value, bool>
stream_json(
     chunked_writer &w
    ,const value_t &v
    ,std::size_t
    ,const char *tail
    ,std::size_t tail_len
    ,serialize_flags) noexcept
{ return stream_value_t_json(w, v, tail, tail_len); }

template<bool Pretty, typename U>
std::enable_if_t<
    !has_metadata<U>::value
    && !std::is_same<U, value_t>::value
    && is_string_like_t<U>::value
    ,bool
>
stream_json(
     chunked_writer &w
    ,const U &v
    ,std::size_t
    ,const char *tail
    ,std::size_t tail_len
    ,serialize_flags) noexcept
{ return w.write_quoted_token(v, tail, tail_len); }

template<bool Pretty, typename U>
std::enable_if_t<
    !has_metadata<U>::value
    && !is_string_like_t<U>::value
    && std::is_same<U, bool>::value
    ,bool
>
stream_json(
     chunked_writer &w
    ,const U &v
    ,std::size_t
    ,const char *tail
    ,std::size_t tail_len
    ,serialize_flags) noexcept
{
    return v
        ? w.write_token("true", 4, tail, tail_len)
        : w.write_token("false", 5, tail, tail_len)
    ;
}

template<bool Pretty, typename U>
std::enable_if_t<
    !has_metadata<U>::value
    && !is_string_like_t<U>::value
    && !std::is_same<U, bool>::value
    && std::is_integral<U>::value
    ,bool
>
stream_json(
     chunked_writer &w
    ,const U &v
    ,std::size_t
    ,const char *tail
    ,std::size_t tail_len
    ,serialize_flags) noexcept
{
    char tmp[32];
    const auto res = details::to_chars(tmp, tmp + sizeof(tmp), v);
    if ( res.ec != std::errc{} ) { return false; }
    return w.write_token(tmp, static_cast<std::size_t>(res.ptr - tmp), tail, tail_len);
}

template<bool Pretty, typename U>
std::enable_if_t<
     !has_metadata<U>::value
     && !is_string_like_t<U>::value
     && !std::is_same<U, bool>::value
     && !std::is_integral<U>::value
     && std::is_enum<U>::value
    ,bool
>
stream_json(
     chunked_writer &w
    ,const U &v
    ,std::size_t
    ,const char *tail
    ,std::size_t tail_len
    ,serialize_flags fl) noexcept
{
    return stream_json<Pretty>(
         w
        ,static_cast<std::underlying_type_t<U>>(v)
        ,std::size_t{0}
        ,tail
        ,tail_len
        ,fl
    );
}

template<bool Pretty, typename U>
std::enable_if_t<
     !has_metadata<U>::value
     && !is_string_like_t<U>::value
     && !std::is_same<U, bool>::value
     && !std::is_integral<U>::value
     && !std::is_enum<U>::value
     && std::is_floating_point<U>::value
    ,bool
>
stream_json(
     chunked_writer &w
    ,const U &v
    ,std::size_t
    ,const char *tail
    ,std::size_t tail_len
    ,serialize_flags) noexcept
{
    char tmp[k_charconv_buf];
    const auto res = details::to_chars(tmp, tmp + sizeof(tmp), static_cast<double>(v), chars_format::fixed, 6);
    if ( res.ec != std::errc{} ) { return false; }
    return w.write_token(tmp, static_cast<std::size_t>(res.ptr - tmp), tail, tail_len);
}

template<bool Pretty, typename U>
std::enable_if_t<!has_metadata<U>::value && is_array_type<U>::value, bool>
stream_json(
     chunked_writer &w
    ,const U &v
    ,std::size_t indent
    ,const char *tail
    ,std::size_t tail_len
    ,serialize_flags fl) noexcept
{
    if ( v.empty() ) { return w.write_token("[]", 2, tail, tail_len); }
    if ( !Pretty ) {
        if ( !w.put('[') ) { return false; }
        const auto n = v.size();
        std::size_t idx = 0;
        for ( const auto &elem : v ) {
            ++idx;
            if ( idx == n ) {
                char tail_buf[16];
                const auto need = 1u + tail_len;
                if ( need <= sizeof(tail_buf) ) {
                    tail_buf[0] = ']';
                    if ( tail_len ) { std::memcpy(tail_buf + 1, tail, tail_len); }

                    return stream_json<false>(w, elem, 0, tail_buf, need, fl);
                }
                // deep-nesting fallback: no atomic guarantee
                if ( !stream_json<false>(w, elem, 0, nullptr, 0, fl) ) { return false; }
                if ( !w.put(']') ) { return false; }
                return tail_len ? w.write(tail, tail_len) : true;
            }
            const char comma = ',';
            if ( !stream_json<false>(w, elem, 0, &comma, 1, fl) ) { return false; }
        }
        return true; // unreachable when n > 0
    }
    const auto child = indent + k_indent_step_len;
    if ( !w.put('[') ) { return false; }
    if ( !w.put('\n') ) { return false; }
    bool first = true;
    for ( const auto &elem : v ) {
        if ( !first ) {
            if ( !w.put(',') ) { return false; }
            if ( !w.put('\n') ) { return false; }
        }
        if ( !w.fill(' ', child) ) { return false; }
        if ( !stream_json<true>(w, elem, child, nullptr, 0, fl) ) { return false; }
        first = false;
    }
    if ( !w.put('\n') ) { return false; }
    if ( !w.fill(' ', indent) ) { return false; }
    if ( !w.put(']') ) { return false; }
    return tail_len ? w.write(tail, tail_len) : true;
}

template<bool Pretty, typename U>
std::enable_if_t<
     !has_metadata<U>::value
     && !is_array_type<U>::value
     && is_object_type<U>::value
    ,bool
>
stream_json(
     chunked_writer &w
    ,const U &v
    ,std::size_t indent
    ,const char *tail
    ,std::size_t tail_len
    ,serialize_flags fl) noexcept
{
    if ( v.empty() ) { return w.write_token("{}", 2, tail, tail_len); }
    if ( !Pretty ) {
        if ( !w.put('{') ) { return false; }
        const auto n = v.size();
        std::size_t idx = 0;
        for ( const auto &pr : v ) {
            const auto &k = pr.first;
            const auto &val = pr.second;
            ++idx;
            if ( !w.write_quoted_token(k, ":", 1) ) { return false; }
            if ( idx == n ) {
                char tail_buf[16];
                const auto need = 1u + tail_len;
                if ( need <= sizeof(tail_buf) ) {
                    tail_buf[0] = '}';
                    if ( tail_len ) { std::memcpy(tail_buf + 1, tail, tail_len); }
                    if ( !stream_json<false>(w, val, 0, tail_buf, need, fl) ) { return false; }
                    return true;
                }
                // deep-nesting fallback
                if ( !stream_json<false>(w, val, 0, nullptr, 0, fl) ) { return false; }
                if ( !w.put('}') ) { return false; }
                return tail_len ? w.write(tail, tail_len) : true;
            }
            const char comma = ',';
            if ( !stream_json<false>(w, val, 0, &comma, 1, fl) ) { return false; }
        }
        return true; // unreachable when n > 0
    }
    const auto child = indent + k_indent_step_len;
    if ( !w.put('{') ) { return false; }
    if ( !w.put('\n') ) { return false; }
    bool first = true;
    for ( const auto &pr : v ) {
        const auto &k = pr.first;
        const auto &val = pr.second;
        if ( !first ) {
            if ( !w.put(',') ) { return false; }
            if ( !w.put('\n') ) { return false; }
        }
        if ( !w.fill(' ', child) ) { return false; }
        if ( !w.write_quoted_token(k) ) { return false; }
        if ( !w.write(": ", 2) ) { return false; }
        if ( !stream_json<true>(w, val, child, nullptr, 0, fl) ) { return false; }
        first = false;
    }
    if ( !w.put('\n') ) { return false; }
    if ( !w.fill(' ', indent) ) { return false; }
    if ( !w.put('}') ) { return false; }
    return tail_len ? w.write(tail, tail_len) : true;
}

template<bool Pretty, typename U>
std::enable_if_t<
     !has_metadata<U>::value
     && !is_array_type<U>::value
     && !is_object_type<U>::value
     && is_optional_type<U>::value
    ,bool
>
stream_json(
     chunked_writer &w
    ,const U &v
    ,std::size_t indent
    ,const char *tail
    ,std::size_t tail_len
    ,serialize_flags fl) noexcept
{
    return v.has_value()
        ? stream_json<Pretty>(w, *v, indent, tail, tail_len, fl)
        : w.write_token("null", 4, tail, tail_len)
    ;
}

template<bool Pretty, typename U>
std::enable_if_t<
     !has_metadata<U>::value
     && !is_array_type<U>::value
     && !is_object_type<U>::value
     && !is_optional_type<U>::value
     && !is_string_like_t<U>::value
     && !std::is_same<U, bool>::value
     && !std::is_integral<U>::value
     && !std::is_enum<U>::value
     && !std::is_floating_point<U>::value
    ,bool
>
stream_json(
     chunked_writer &w
    ,const U &
    ,std::size_t
    ,const char *tail
    ,std::size_t tail_len
    ,serialize_flags) noexcept
{ return w.write_token("null", 4, tail, tail_len); }

/*************************************************************************************************/

struct object_holder_base;

template<typename T> class root_setter_t;
template<typename T> const root_setter_t<T>* root_setter_ptr() noexcept;

struct setter_base {
private:
    enum class type_kind: std::uint8_t {
         unknown     = 0
        ,object      = 1
        ,array       = 2
        ,sint        = 3
        ,uint        = 4
        ,fp          = 5
        ,string      = 6
        ,string_view = 7
        ,value_t     = 8
    };

    type_kind m_kind;
    bool      m_has_metadata;

protected:
    template<typename U>
    static constexpr typename std::enable_if<std::is_enum<U>::value, type_kind>::type
    member_kind_enum() noexcept {
        using ut = std::underlying_type_t<U>;
        return std::is_unsigned<ut>::value ? type_kind::uint : type_kind::sint;
    }

    template<typename U>
    static constexpr typename std::enable_if<!std::is_enum<U>::value, type_kind>::type
    member_kind_enum() noexcept
    { return type_kind::unknown; }

    template<typename U>
    static constexpr auto deduce_member_kind() noexcept {
        return is_object_type<U>::value
            ? type_kind::object
            : is_array_type<U>::value
                ? type_kind::array
                : std::is_same<U, string_view_t>::value
                    ? type_kind::string_view
                    : std::is_same<U, jsonrefl::value_t>::value
                        ? type_kind::value_t
                        : is_string_like_t<U>::value
                            ? type_kind::string
                            : std::is_floating_point<U>::value
                                ? type_kind::fp
                                : std::is_enum<U>::value
                                    ? member_kind_enum<U>()
                                    : std::is_unsigned<U>::value
                                        ? type_kind::uint
                                        : std::is_integral<U>::value
                                            ? type_kind::sint
                                            : type_kind::unknown
        ;
    }

    template<typename U>
    static constexpr auto deduce_root_kind() noexcept {
        return is_object_type<U>::value
            ? type_kind::object
            : is_array_type<U>::value
                ? type_kind::array
                : std::is_same<U, jsonrefl::value_t>::value
                    ? type_kind::value_t
                    : type_kind::unknown
        ;
    }

public:
    constexpr setter_base(type_kind kind = type_kind::unknown, bool has_meta = false) noexcept
        :m_kind{kind}
        ,m_has_metadata{has_meta}
    {}

    bool is_object()      const noexcept { return m_kind == type_kind::object; }
    bool is_array()       const noexcept { return m_kind == type_kind::array; }
    bool is_int()         const noexcept { return m_kind == type_kind::sint || m_kind == type_kind::uint; }
    bool is_uint()        const noexcept { return m_kind == type_kind::uint; }
    bool is_double()      const noexcept { return m_kind == type_kind::fp; }
    bool is_string()      const noexcept { return m_kind == type_kind::string || m_kind == type_kind::string_view; }
    bool is_string_view() const noexcept { return m_kind == type_kind::string_view; }
    bool is_value_t()     const noexcept { return m_kind == type_kind::value_t; }
    bool has_metadata()   const noexcept { return m_has_metadata; }

    virtual bool element_is_string_view()   const noexcept { return false; }
    virtual bool element_is_value_t()       const noexcept { return false; }
    virtual bool map_key_is_string_view()   const noexcept { return false; }
    virtual bool map_value_is_string_view() const noexcept { return false; }
    virtual bool map_value_is_value_t()     const noexcept { return false; }

    char type_id() const noexcept {
        constexpr char map[] = "-OAIUFSVL";
        return map[static_cast<std::uint8_t>(m_kind)];
    }

    virtual const object_holder_base* get_metadata() const noexcept = 0;
    virtual const object_holder_base* get_element_metadata() const noexcept { return nullptr; }

    /** element of outer JSON array is itself an array/container without object metadata (`[[…]]`). */
    virtual const setter_base* get_nested_array_root_setter() const noexcept { return nullptr; }

    /** JSON object value is a nested map without reflection metadata (`{"o":{"k":…}}`). */
    virtual const setter_base* get_nested_map_root_setter() const noexcept { return nullptr; }

    /** JSON object value under a map key is itself an array (`{"k":[…]}`). */
    virtual const setter_base* get_map_value_array_root_setter() const noexcept { return nullptr; }

    virtual void* member_address(void *obj) const noexcept = 0;
    virtual const void* member_address(const void *obj) const noexcept = 0;

    virtual void* prepare(void *obj) const = 0;
    virtual void commit(void *obj, void *elem) const noexcept = 0;
    virtual void set(void *obj, string_view_t) const = 0;
    virtual void set(void *obj, string_view_t key, string_view_t val) const = 0;

    virtual bool set_leaf(void *obj, string_view_t sv, value_kind kind) const {
        (void)kind;
        set(obj, sv);
        return true;
    }

    virtual bool set_map_leaf(
         void *obj
        ,string_view_t key
        ,string_view_t val
        ,value_kind kind
    ) const {
        (void)kind;
        set(obj, key, val);
        return true;
    }

    virtual void set_null_leaf(void *obj) const { (void)obj; }

    virtual void set_map_null_leaf(void *obj, string_view_t key) const {
        (void)obj;
        (void)key;
    }

    virtual void* prepare_map_value(void * /*obj*/, string_view_t /*key*/) const { return nullptr; }
    virtual const object_holder_base* get_map_value_metadata() const noexcept { return nullptr; }
    virtual std::size_t value_required_bytes(const void *obj, serialize_flags fl, std::size_t indent) const noexcept = 0;
    virtual char* value_to_buffer(const void *obj, char *ptr, serialize_flags fl, std::size_t indent) const noexcept = 0;
    virtual bool value_to_stream(
         const void *obj
        ,chunked_writer &w
        ,serialize_flags fl
        ,std::size_t indent
        ,const char *tail = nullptr
        ,std::size_t tail_len = 0
    ) const noexcept = 0;
};

/*************************************************************************************************/

template<typename C, typename T>
class setter_t final: public setter_base {
    T C::*m_ptr;

    template<typename U>
    static bool leaf_assign(U &v, string_view_t sv, value_kind kind) {
        return leaf_assign_impl(
             v
            ,sv
            ,kind
            ,bool_constant<std::is_same<U, jsonrefl::value_t>::value>{}
        );
    }

    static bool leaf_assign_impl(
         jsonrefl::value_t &v
        ,string_view_t sv
        ,value_kind kind
        ,std::true_type
    ) noexcept
    { v.assign(sv, kind); return true; }

    template<typename U>
    static bool leaf_assign_impl(U &v, string_view_t sv, value_kind, std::false_type)
    { return set_impl(v, sv); }

    // bool
    template<typename U, std::enable_if_t<std::is_same<U, bool>::value, bool> = true>
    static bool set_impl(U &v, string_view_t str) noexcept {
        if ( str == "true"  || str == "1" ) { v = true;  return true; }
        if ( str == "false" || str == "0" ) { v = false; return true; }
        return false;
    }
    // integrals (excluding bool)
    template<typename U, std::enable_if_t<std::is_integral<U>::value && !std::is_same<U, bool>::value, bool> = true>
    static bool set_impl(U &v, string_view_t str) noexcept {
        const auto parsed = details::parse_integral_sv<U>(str);
        if ( parsed ) { v = *parsed; return true; }
        return false;
    }
    template<typename U, std::enable_if_t<std::is_enum<U>::value, bool> = true>
    static bool set_impl(U &v, string_view_t str) noexcept {
        using ut = std::underlying_type_t<U>;
        const auto parsed = details::parse_integral_sv<ut>(str);
        if ( parsed ) { v = static_cast<U>(*parsed); return true; }
        return false;
    }
    // floating point
    template<typename U, std::enable_if_t<std::is_floating_point<U>::value, bool> = true>
    static bool set_impl(U &v, string_view_t str) noexcept {
        const auto parsed = details::parse_floating_sv<U>(str);
        if ( parsed ) { v = *parsed; return true; }
        return false;
    }
    template<typename U, std::enable_if_t<std::is_same<U, jsonrefl::value_t>::value, bool> = true>
    static bool set_impl(U &v, string_view_t str) noexcept
    { v.assign(str, value_kind::string); return true; }
    template<typename U, std::enable_if_t<std::is_same<U, string_view_t>::value, bool> = true>
    static bool set_impl(U &v, string_view_t str) noexcept { v = str; return true; }
    // string
    template<typename U, std::enable_if_t<std::is_same<U, std::string>::value, bool> = true>
    static bool set_impl(U &v, string_view_t str) { v.assign(str.data(), str.size()); return true; }
    // vector of primitives / strings (objects with metadata are handled via prepare)
    template<typename V>
    static bool set_impl_vector_elem(std::vector<V> &v, string_view_t str, std::false_type) {
        v.emplace_back();
        return set_impl(v.back(), str);
    }
    template<typename V>
    static bool set_impl_vector_elem(std::vector<V> &, string_view_t, std::true_type) noexcept
    { return true; }

    template<typename V>
    static bool set_impl(std::vector<V> &v, string_view_t str) {
        return set_impl_vector_elem(
            v, str, bool_constant<jsonrefl::has_metadata<V>::value>{}
        );
    }
    // list of primitives / strings (objects with metadata are handled via prepare)
    template<typename V>
    static bool set_impl_list_elem(std::list<V> &v, string_view_t str, std::false_type)
    { v.emplace_back(); return set_impl(v.back(), str); }

    template<typename V>
    static bool set_impl_list_elem(std::list<V> &, string_view_t, std::true_type) noexcept
    { return true; }

    template<typename V>
    static bool set_impl(std::list<V> &v, string_view_t str)
    { return set_impl_list_elem(v, str, bool_constant<jsonrefl::has_metadata<V>::value>{}); }
    // map — populated via set, not set_impl
    template<typename K, typename V>
    static bool set_impl(std::map<K, V> & /*v*/, string_view_t /*str*/) noexcept
    { return true; }
    // unordered_map — populated via set, not set_impl
    template<typename K, typename V>
    static bool set_impl(std::unordered_map<K, V> & /*v*/, string_view_t /*str*/) noexcept
    { return true; }
    // optional — emplace and delegate
    template<typename V>
    static bool set_impl(optional_t<V> &v, string_view_t str)
    { if ( !v ) { v.emplace(); } return set_impl(*v, str); }

    // for object (maps) — not used directly; maps are populated via set
    template<typename U, std::enable_if_t<is_object_type<U>::value, bool> = true>
    static void* obj_prepare(U & /*u*/, void * /*obj*/) noexcept { return nullptr; }
    template<typename U, std::enable_if_t<is_object_type<U>::value, bool> = true>
    static void obj_commit(U & /*u*/, void * /*obj*/, void * /*elem*/) noexcept {}

    // for array
    template<typename U, std::enable_if_t<is_array_type<U>::value, bool> = true>
    static void* arr_prepare(U &u, void * /*arr*/)
    { u.emplace_back(); return arr_elem_ptr(u.back()); }
    template<typename U, std::enable_if_t<is_array_type<U>::value, bool> = true>
    static void arr_commit(U & /*u*/, void * /*arr*/, void * /*elem*/) noexcept {}

    template<typename U, std::enable_if_t<!is_object_or_array<U>::value, bool> = true>
    static void* arr_prepare(U &, void */*arr*/) noexcept { return nullptr; }
    template<typename U, std::enable_if_t<!is_object_or_array<U>::value, bool> = true>
    static void arr_commit(U &, void */*arr*/, void */*elem*/) noexcept {}

    using inner_type = unwrap_optional_t<T>;

    template<typename U>
    static typename std::enable_if<is_optional_type<U>::value, unwrap_optional_t<U>&>::type
    ensure_ref_impl(U &v)
    { if ( !v ) { v.emplace(); } return *v; }
    template<typename U>
    static typename std::enable_if<!is_optional_type<U>::value, U&>::type
    ensure_ref_impl(U &v)
    { return v; }

    static auto& ensure_ref(T &v)
    { return ensure_ref_impl(v); }

    template<typename M>
    static typename std::enable_if<is_optional_type<M>::value, void*>::type
    map_value_slot_ptr(M &slot) noexcept
    { if ( !slot ) { slot.emplace(); } return std::addressof(*slot); }
    template<typename M>
    static typename std::enable_if<!is_optional_type<M>::value, void*>::type
    map_value_slot_ptr(M &slot) noexcept
    { return std::addressof(slot); }

    template<typename Elem>
    static typename std::enable_if<is_optional_type<Elem>::value, void*>::type
    arr_elem_ptr(Elem &e) noexcept
    { e.emplace(); return std::addressof(*e); }
    template<typename Elem>
    static typename std::enable_if<!is_optional_type<Elem>::value, void*>::type
    arr_elem_ptr(Elem &e) noexcept
    { return std::addressof(e); }

    template<typename I>
    static typename std::enable_if<is_object_type<I>::value, const object_holder_base*>::type
    map_value_meta_for() noexcept {
        using val_t = unwrap_optional_t<typename I::mapped_type>;
        return map_value_meta_slot_for<I>(bool_constant<jsonrefl::has_metadata<val_t>::value>{});
    }

    template<typename I>
    static typename std::enable_if<is_object_type<I>::value, const object_holder_base*>::type
    map_value_meta_slot_for(std::true_type) noexcept {
        using val_t = unwrap_optional_t<typename I::mapped_type>;
        return std::addressof(metadata<val_t>());
    }

    template<typename I>
    static typename std::enable_if<is_object_type<I>::value, const object_holder_base*>::type
    map_value_meta_slot_for(std::false_type) noexcept
    { return nullptr; }

    template<typename I>
    static typename std::enable_if<!is_object_type<I>::value, const object_holder_base*>::type
    map_value_meta_for() noexcept { return nullptr; }

    template<typename I>
    typename std::enable_if<is_object_type<I>::value, void*>::type
    prepare_map_value_for(void *obj, string_view_t key) const {
        using val_t = unwrap_optional_t<typename I::mapped_type>;
        return prepare_map_value_slot_for<I>(
             bool_constant<jsonrefl::has_metadata<val_t>::value>{}
            ,obj
            ,key
        );
    }

    template<typename I>
    void* prepare_map_value_slot_for(std::true_type, void *obj, string_view_t key) const {
        auto &m = ensure_ref(static_cast<C *>(obj)->*m_ptr);
        typename I::key_type k;
        set_impl(k, key);
        const auto em = details::map_try_emplace(m, std::move(k));
        auto it = em.first;
        return map_value_slot_ptr(it->second);
    }

    template<typename I>
    void* prepare_map_nested_map_embind(void *obj, string_view_t key) const {
        auto &m = ensure_ref(static_cast<C *>(obj)->*m_ptr);
        typename I::key_type k;
        set_impl(k, key);
        const auto em = details::map_try_emplace(m, std::move(k));
        auto it = em.first;
        return map_value_slot_ptr(it->second);
    }

    template<typename I>
    void* prepare_map_nested_map_slot_if(std::true_type, void *obj, string_view_t key) const
    { return prepare_map_nested_map_embind<I>(obj, key); }

    template<typename I>
    void* prepare_map_nested_map_slot_if(std::false_type, void *, string_view_t) const noexcept
    { return nullptr; }

    template<typename I>
    void* prepare_map_value_slot_for(std::false_type, void *obj, string_view_t key) const {
        using val_t = unwrap_optional_t<typename I::mapped_type>;
        using nested_map_tag = bool_constant<
            !jsonrefl::has_metadata<val_t>::value && is_object_type<val_t>::value
        >;
        using nested_arr_tag = bool_constant<
            !jsonrefl::has_metadata<val_t>::value && is_array_type<val_t>::value
        >;
        return nested_arr_tag::value
            ? prepare_map_nested_map_embind<I>(obj, key)
            : prepare_map_nested_map_slot_if<I>(nested_map_tag{}, obj, key)
        ;
    }

    template<typename I>
    typename std::enable_if<!is_object_type<I>::value, void*>::type
    prepare_map_value_for(void *, string_view_t) const { return nullptr; }

    template<typename I>
    static typename std::enable_if<is_object_type<I>::value, bool>::type
    map_key_is_sv_for() noexcept
    { return std::is_same<typename I::key_type, string_view_t>::value; }

    template<typename I>
    static typename std::enable_if<!is_object_type<I>::value, bool>::type
    map_key_is_sv_for() noexcept { return false; }

    template<typename I>
    static typename std::enable_if<is_object_type<I>::value, bool>::type
    map_val_is_sv_for() noexcept {
        using vt = unwrap_optional_t<typename I::mapped_type>;
        return std::is_same<vt, string_view_t>::value;
    }

    template<typename I>
    static typename std::enable_if<is_object_type<I>::value, bool>::type
    map_val_is_value_t_for() noexcept {
        using vt = unwrap_optional_t<typename I::mapped_type>;
        return std::is_same<vt, jsonrefl::value_t>::value;
    }

    template<typename I>
    static typename std::enable_if<!is_object_type<I>::value, bool>::type
    map_val_is_value_t_for() noexcept { return false; }

    template<typename I>
    static typename std::enable_if<!is_object_type<I>::value, bool>::type
    map_val_is_sv_for() noexcept { return false; }

    template<typename I>
    typename std::enable_if<is_object_type<I>::value>::type
    set_kv_for(void *obj, string_view_t key, string_view_t val) const {
        using val_t = unwrap_optional_t<typename I::mapped_type>;
        set_kv_slot_for<I>(
             bool_constant<jsonrefl::has_metadata<val_t>::value>{}
            ,obj
            ,key
            ,val
        );
    }

    template<typename K>
    static typename std::enable_if<std::is_same<K, std::string>::value, K>::type
    make_key(string_view_t sv) { return K{sv.data(), sv.size()}; }

    template<typename K>
    static typename std::enable_if<!std::is_same<K, std::string>::value, K>::type
    make_key(string_view_t sv) {
        K k{};
        set_impl(k, sv);
        return k;
    }

    template<typename I>
    void set_kv_slot_for(std::false_type, void *obj, string_view_t key, string_view_t val) const {
        auto &m = ensure_ref(static_cast<C *>(obj)->*m_ptr);
        typename I::mapped_type v{};
        set_impl(v, val);
        details::map_insert_or_assign(m, make_key<typename I::key_type>(key), std::move(v));
    }

    template<typename I>
    bool set_kv_leaf_slot_for(
         std::false_type
        ,void *obj
        ,string_view_t key
        ,string_view_t val
        ,value_kind kind
    ) const {
        auto &m = ensure_ref(static_cast<C *>(obj)->*m_ptr);
        typename I::mapped_type v{};
        if ( !leaf_assign(v, val, kind) ) { return false; }
        details::map_insert_or_assign(m, make_key<typename I::key_type>(key), std::move(v));
        return true;
    }

    template<typename I>
    bool set_kv_leaf_slot_for(
         std::true_type
        ,void *
        ,string_view_t
        ,string_view_t
        ,value_kind
    ) const noexcept { return true; }

    template<typename I>
    typename std::enable_if<is_object_type<I>::value, bool>::type
    set_kv_leaf_for(void *obj, string_view_t key, string_view_t val, value_kind kind) const {
        using val_t = unwrap_optional_t<typename I::mapped_type>;
        return set_kv_leaf_slot_for<I>(
             bool_constant<jsonrefl::has_metadata<val_t>::value>{}
            ,obj
            ,key
            ,val
            ,kind
        );
    }

    template<typename I>
    typename std::enable_if<!is_object_type<I>::value, bool>::type
    set_kv_leaf_for(void *, string_view_t, string_view_t, value_kind) const noexcept
    { return true; }

    template<typename I>
    typename std::enable_if<
        is_object_type<I>::value
        && std::is_same<unwrap_optional_t<typename I::mapped_type>, jsonrefl::value_t>::value
    >::type
    set_map_null_for(void *obj, string_view_t key) const {
        auto &m = ensure_ref(static_cast<C *>(obj)->*m_ptr);
        typename I::key_type k{};
        set_impl(k, key);
        details::map_insert_or_assign(m, std::move(k), jsonrefl::value_t{});
    }

    template<typename I>
    typename std::enable_if<
        is_object_type<I>::value
        && !std::is_same<unwrap_optional_t<typename I::mapped_type>, jsonrefl::value_t>::value
    >::type
    set_map_null_for(void *, string_view_t) const noexcept {}

    template<typename I>
    typename std::enable_if<!is_object_type<I>::value>::type
    set_map_null_for(void *, string_view_t) const noexcept {}

    template<typename I>
    void set_kv_slot_for(std::true_type, void *, string_view_t, string_view_t) const noexcept {}

    template<typename I>
    typename std::enable_if<!is_object_type<I>::value>::type
    set_kv_for(void *, string_view_t, string_view_t) const noexcept {}

    const object_holder_base* get_element_meta_array(std::true_type) const noexcept {
        using raw_elem = unwrap_optional_t<typename inner_type::value_type>;
        return get_element_holder_for_elem(bool_constant<jsonrefl::has_metadata<raw_elem>::value>{});
    }

    const object_holder_base* get_element_holder_for_elem(std::true_type) const noexcept {
        using raw_elem = unwrap_optional_t<typename inner_type::value_type>;
        return std::addressof(metadata<raw_elem>());
    }

    const object_holder_base* get_element_holder_for_elem(std::false_type) const noexcept
    { return nullptr; }

    const object_holder_base* get_element_meta_array(std::false_type) const noexcept
    { return nullptr; }

    bool element_is_sv_array(std::true_type) const noexcept {
        using elem = unwrap_optional_t<typename inner_type::value_type>;
        return std::is_same<elem, string_view_t>::value;
    }

    bool element_is_value_t_array(std::true_type) const noexcept {
        using elem = unwrap_optional_t<typename inner_type::value_type>;
        return std::is_same<elem, jsonrefl::value_t>::value;
    }

    bool element_is_value_t_array(std::false_type) const noexcept
    { return false; }

    bool element_is_sv_array(std::false_type) const noexcept
    { return false; }

    const object_holder_base* get_inner_metadata_holder(std::true_type) const noexcept
    { return std::addressof(metadata<inner_type>()); }

    const object_holder_base* get_inner_metadata_holder(std::false_type) const noexcept
    { return nullptr; }

public:
    constexpr setter_t(T C::*ptr) noexcept
        :setter_base{setter_base::template deduce_member_kind<inner_type>(), jsonrefl::has_metadata<inner_type>::value}
        ,m_ptr{ptr}
    {}

    const object_holder_base* get_metadata() const noexcept override
    { return get_inner_metadata_holder(bool_constant<jsonrefl::has_metadata<inner_type>::value>{}); }

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
    const object_holder_base* get_element_metadata() const noexcept override
    { return get_element_meta_array(bool_constant<is_array_type<inner_type>::value>{}); }

    const object_holder_base* get_map_value_metadata() const noexcept override
    { return map_value_meta_for<inner_type>(); }

    void* prepare_map_value(void *obj, string_view_t key) const override
    { return prepare_map_value_for<inner_type>(obj, key); }

#ifdef _MSC_VER
#pragma warning(pop)
#endif

    bool element_is_string_view() const noexcept override
    { return element_is_sv_array(bool_constant<is_array_type<inner_type>::value>{}); }

    bool element_is_value_t() const noexcept override
    { return element_is_value_t_array(bool_constant<is_array_type<inner_type>::value>{}); }

    bool map_key_is_string_view() const noexcept override
    { return map_key_is_sv_for<inner_type>(); }

    bool map_value_is_string_view() const noexcept override
    { return map_val_is_sv_for<inner_type>(); }

    bool map_value_is_value_t() const noexcept override
    { return map_val_is_value_t_for<inner_type>(); }

    const setter_base* get_nested_array_root_setter() const noexcept override
    { return nested_array_root_from_member(bool_constant<is_array_type<inner_type>::value>{}); }

    static const setter_base* nested_array_root_from_member(std::false_type) noexcept
    { return nullptr; }

    static const setter_base* nested_array_root_from_member(std::true_type) noexcept {
        using elem = unwrap_optional_t<typename inner_type::value_type>;
        using nested_elem_tag = bool_constant<
            is_array_type<elem>::value && !::jsonrefl::has_metadata<elem>::value
        >;
        return nested_array_root_from_elem_type(nested_elem_tag{});
    }

    static const setter_base* nested_array_root_from_elem_type(std::false_type) noexcept
    { return nullptr; }

    static const setter_base* nested_array_root_from_elem_type(std::true_type) noexcept {
        using elem = unwrap_optional_t<typename inner_type::value_type>;
        return root_setter_ptr<elem>();
    }

    const setter_base* get_map_value_array_root_setter() const noexcept override
    { return map_value_arr_root_for(bool_constant<is_object_type<inner_type>::value>{}); }

    static const setter_base* map_value_arr_root_for(std::false_type) noexcept { return nullptr; }

    static const setter_base* map_value_arr_root_for(std::true_type) noexcept {
        using val_t = unwrap_optional_t<typename inner_type::mapped_type>;
        return map_value_arr_root_if(bool_constant<is_array_type<val_t>::value>{});
    }

    static const setter_base* map_value_arr_root_if(std::false_type) noexcept { return nullptr; }

    static const setter_base* map_value_arr_root_if(std::true_type) noexcept {
        using val_t = unwrap_optional_t<typename inner_type::mapped_type>;
        return root_setter_ptr<val_t>();
    }

    static const setter_base* nested_map_root_mapped_if(std::false_type) noexcept { return nullptr; }

    static const setter_base* nested_map_root_mapped_if(std::true_type) noexcept {
        using val_t = unwrap_optional_t<typename inner_type::mapped_type>;
        return root_setter_ptr<val_t>();
    }

    static const setter_base* nested_map_root_member_if(std::false_type) noexcept { return nullptr; }

    static const setter_base* nested_map_root_member_if(std::true_type) noexcept {
        using val_t = unwrap_optional_t<typename inner_type::mapped_type>;
        using nested_map_tag = bool_constant<
            !jsonrefl::has_metadata<val_t>::value && is_object_type<val_t>::value
        >;
        return nested_map_root_mapped_if(nested_map_tag{});
    }

    const setter_base* get_nested_map_root_setter() const noexcept override
    { return nested_map_root_member_if(bool_constant<is_object_type<inner_type>::value>{}); }

    void* member_address_dispatch(void *obj, std::true_type) const noexcept {
        auto &opt = static_cast<C *>(obj)->*m_ptr;
        if ( !opt ) { opt.emplace(); }
        return std::addressof(*opt);
    }

    void* member_address_dispatch(void *obj, std::false_type) const noexcept
    { return std::addressof(static_cast<C *>(obj)->*m_ptr); }

    void* member_address(void *obj) const noexcept override
    { return member_address_dispatch(obj, bool_constant<is_optional_type<T>::value>{}); }

    const void* member_address_const_dispatch(const void *obj, std::true_type) const noexcept {
        const auto &opt = static_cast<const C *>(obj)->*m_ptr;
        return opt.has_value() ? std::addressof(*opt) : nullptr;
    }

    const void* member_address_const_dispatch(const void *obj, std::false_type) const noexcept
    { return std::addressof(static_cast<const C *>(obj)->*m_ptr); }

    const void* member_address(const void *obj) const noexcept override {
        return member_address_const_dispatch(
             obj
            ,bool_constant<is_optional_type<T>::value>{}
        );
    }

    void* prepare(void *obj) const override {
        assert(is_array() || is_object());
        auto &inner = ensure_ref(static_cast<C *>(obj)->*m_ptr);
        return prepare_inner_dispatch(
             inner
            ,obj
            ,bool_constant<is_object_type<inner_type>::value>{}
        );
    }

    template<typename U>
    static void* prepare_inner_dispatch(U &inner, void *obj, std::true_type)
    { return obj_prepare(inner, obj); }

    template<typename U>
    static void* prepare_inner_dispatch(U &inner, void *obj, std::false_type)
    { return arr_prepare(inner, obj); }

    template<typename U>
    static void commit_inner_dispatch(U &inner, void *obj, void *elem, std::true_type) noexcept
    { obj_commit(inner, obj, elem); }

    template<typename U>
    static void commit_inner_dispatch(U &inner, void *obj, void *elem, std::false_type) noexcept
    { arr_commit(inner, obj, elem); }

    void commit_field_dispatch(C *co, void *obj, void *elem, std::true_type) const noexcept
    { obj_commit(co->*m_ptr, obj, elem); }

    void commit_field_dispatch(C *co, void *obj, void *elem, std::false_type) const noexcept
    { arr_commit(co->*m_ptr, obj, elem); }

    void commit_top(C *co, void *obj, void *elem, std::true_type) const noexcept {
        auto &inner = *(co->*m_ptr);
        commit_inner_dispatch(
             inner
            ,obj
            ,elem
            ,bool_constant<is_object_type<inner_type>::value>{}
        );
    }
    void commit_top(C *co, void *obj, void *elem, std::false_type) const noexcept {
        commit_field_dispatch(
             co
            ,obj
            ,elem
            ,bool_constant<is_object_type<T>::value>{}
        );
    }

    void commit(void *obj, void *elem) const noexcept override {
        assert(is_array() || is_object());
        commit_top(
             static_cast<C *>(obj)
            ,obj
            ,elem
            ,bool_constant<is_optional_type<T>::value>{}
        );
    }

    template<typename I>
    typename std::enable_if<!jsonrefl::has_metadata<I>::value>::type
    set_member_from_string(void *obj, string_view_t str) const
    { set_impl(static_cast<C *>(obj)->*m_ptr, str); }

    template<typename I>
    typename std::enable_if<jsonrefl::has_metadata<I>::value>::type
    set_member_from_string(void *, string_view_t) const noexcept {}

    void set(void *obj, string_view_t str) const override
    { set_member_from_string<inner_type>(obj, str); }

    void set(void *obj, string_view_t key, string_view_t val) const override
    { set_kv_for<inner_type>(obj, key, val); }

    bool set_leaf(void *obj, string_view_t sv, value_kind kind) const override
    { return set_leaf_member<inner_type>(obj, sv, kind); }

    template<typename I>
    typename std::enable_if<!jsonrefl::has_metadata<I>::value, bool>::type
    set_leaf_member(void *obj, string_view_t sv, value_kind kind) const
    { return leaf_assign(static_cast<C *>(obj)->*m_ptr, sv, kind); }

    template<typename I>
    typename std::enable_if<jsonrefl::has_metadata<I>::value, bool>::type
    set_leaf_member(void *, string_view_t, value_kind) const noexcept { return true; }

    bool set_map_leaf(void *obj,string_view_t key,string_view_t val,value_kind kind) const override
    { return set_kv_leaf_for<inner_type>(obj, key, val, kind); }

    void set_null_leaf(void *obj) const override {
        set_null_leaf_impl(
             obj
            ,bool_constant<std::is_same<inner_type, jsonrefl::value_t>::value>{}
        );
    }

    void set_null_leaf_impl(void *obj, std::true_type) const
    { (static_cast<C *>(obj)->*m_ptr).assign({}, value_kind::null); }

    void set_null_leaf_impl(void *, std::false_type) const noexcept {}

    void set_map_null_leaf(void *obj, string_view_t key) const override
    { set_map_null_for<inner_type>(obj, key); }

    std::size_t value_required_bytes(
         const void *obj
        ,serialize_flags fl
        ,std::size_t indent) const noexcept override
    { return required_bytes(static_cast<const C *>(obj)->*m_ptr, fl, indent); }

    char* value_to_buffer(
         const void *obj
        ,char *ptr
        ,serialize_flags fl
        ,std::size_t indent) const noexcept override
    { return write_json(ptr, static_cast<const C *>(obj)->*m_ptr, fl, indent); }

    bool value_to_stream(
         const void *obj
        ,chunked_writer &w
        ,serialize_flags fl
        ,std::size_t indent
        ,const char *tail
        ,std::size_t tail_len) const noexcept override;
};

template<typename T>
class root_setter_t final: public setter_base {
    template<typename U>
    static bool leaf_assign(U &v, string_view_t sv, value_kind kind) {
        return leaf_assign_impl(
             v
            ,sv
            ,kind
            ,bool_constant<std::is_same<U, jsonrefl::value_t>::value>{}
        );
    }

    static bool leaf_assign_impl(
         jsonrefl::value_t &v
        ,string_view_t sv
        ,value_kind kind
        ,std::true_type
    ) noexcept
    { v.assign(sv, kind); return true; }

    template<typename U>
    static bool leaf_assign_impl(U &v, string_view_t sv, value_kind, std::false_type)
    { return conv(v, sv); }

    template<typename U, typename std::enable_if<std::is_integral<U>::value, int>::type = 0>
    static bool conv(U &v, string_view_t str) noexcept {
        const auto parsed = details::parse_integral_sv<U>(str);
        if ( parsed ) { v = *parsed; return true; }
        return false;
    }

    template<typename U, typename std::enable_if<std::is_enum<U>::value, int>::type = 0>
    static bool conv(U &v, string_view_t str) noexcept {
        using ut = std::underlying_type_t<U>;
        const auto parsed = details::parse_integral_sv<ut>(str);
        if ( parsed ) { v = static_cast<U>(*parsed); return true; }
        return false;
    }

    template<typename U, typename std::enable_if<std::is_floating_point<U>::value, int>::type = 0>
    static bool conv(U &v, string_view_t str) noexcept {
        const auto parsed = details::parse_floating_sv<U>(str);
        if ( parsed ) { v = *parsed; return true; }
        return false;
    }

    template<typename U, typename std::enable_if<std::is_same<U, jsonrefl::value_t>::value, int>::type = 0>
    static bool conv(U &v, string_view_t str) noexcept
    { v.assign(str, value_kind::string); return true; }

    template<typename U, typename std::enable_if<std::is_same<U, string_view_t>::value, int>::type = 0>
    static bool conv(U &v, string_view_t str) noexcept { v = str; return true; }

    template<typename U, typename std::enable_if<std::is_same<U, std::string>::value, int>::type = 0>
    static bool conv(U &v, string_view_t str) { v.assign(str.data(), str.size()); return true; }

    template<typename M>
    static typename std::enable_if<is_optional_type<M>::value, void*>::type
    root_map_slot_ptr(M &slot) noexcept
    { if ( !slot ) slot.emplace(); return std::addressof(*slot); }

    template<typename M>
    static typename std::enable_if<!is_optional_type<M>::value, void*>::type
    root_map_slot_ptr(M &slot) noexcept
    { return std::addressof(slot); }

    template<typename I>
    static typename std::enable_if<is_object_type<I>::value, const object_holder_base*>::type
    root_map_value_meta() noexcept {
        using val_t = unwrap_optional_t<typename I::mapped_type>;
        return root_map_value_meta_slot<I>(bool_constant<jsonrefl::has_metadata<val_t>::value>{});
    }

    template<typename I>
    static typename std::enable_if<is_object_type<I>::value, const object_holder_base*>::type
    root_map_value_meta_slot(std::true_type) noexcept {
        using val_t = unwrap_optional_t<typename I::mapped_type>;
        return std::addressof(metadata<val_t>());
    }

    template<typename I>
    static typename std::enable_if<is_object_type<I>::value, const object_holder_base*>::type
    root_map_value_meta_slot(std::false_type) noexcept { return nullptr; }

    template<typename I>
    static typename std::enable_if<!is_object_type<I>::value, const object_holder_base*>::type
    root_map_value_meta() noexcept { return nullptr; }

    template<typename I>
    typename std::enable_if<is_object_type<I>::value, void*>::type
    root_prepare_map_value(void *obj, string_view_t key) const {
        using val_t = unwrap_optional_t<typename I::mapped_type>;
        return root_prepare_map_value_slot<I>(
             bool_constant<jsonrefl::has_metadata<val_t>::value>{}
            ,obj
            ,key
        );
    }

    template<typename I>
    void* root_prepare_map_value_slot(std::true_type, void *obj, string_view_t key) const {
        auto &m = *static_cast<I*>(obj);
        typename I::key_type k;
        conv(k, key);
        const auto em = details::map_try_emplace(m, std::move(k));
        auto it = em.first;
        return root_map_slot_ptr(it->second);
    }

    template<typename I>
    void* root_nested_map_embind_slot(void *obj, string_view_t key) const {
        auto &m = *static_cast<I*>(obj);
        typename I::key_type k;
        conv(k, key);
        const auto em = details::map_try_emplace(m, std::move(k));
        auto it = em.first;
        return root_map_slot_ptr(it->second);
    }

    template<typename I>
    void* root_prep_nested_map_dispatch(std::true_type, void *obj, string_view_t key) const
    { return root_nested_map_embind_slot<I>(obj, key); }

    template<typename I>
    void* root_prep_nested_map_dispatch(std::false_type, void *, string_view_t) const noexcept
    { return nullptr; }

    template<typename I>
    void* root_prepare_map_value_slot(std::false_type, void *obj, string_view_t key) const {
        using val_t = unwrap_optional_t<typename I::mapped_type>;
        using nested_map_tag = bool_constant<
            !jsonrefl::has_metadata<val_t>::value && is_object_type<val_t>::value
        >;
        return root_prep_nested_map_dispatch<I>(nested_map_tag{}, obj, key);
    }

    template<typename I>
    typename std::enable_if<!is_object_type<I>::value, void*>::type
    root_prepare_map_value(void *, string_view_t) const { return nullptr; }

    template<typename I>
    static typename std::enable_if<is_object_type<I>::value, bool>::type
    root_map_key_is_sv() noexcept
    { return std::is_same<typename I::key_type, string_view_t>::value; }

    template<typename I>
    static typename std::enable_if<!is_object_type<I>::value, bool>::type
    root_map_key_is_sv() noexcept { return false; }

    template<typename I>
    static typename std::enable_if<is_object_type<I>::value, bool>::type
    root_map_val_is_sv() noexcept {
        using vt = unwrap_optional_t<typename I::mapped_type>;
        return std::is_same<vt, string_view_t>::value;
    }

    template<typename I>
    static typename std::enable_if<is_object_type<I>::value, bool>::type
    root_map_val_is_value_t() noexcept {
        using vt = unwrap_optional_t<typename I::mapped_type>;
        return std::is_same<vt, jsonrefl::value_t>::value;
    }

    template<typename I>
    static typename std::enable_if<!is_object_type<I>::value, bool>::type
    root_map_val_is_value_t() noexcept { return false; }

    template<typename I>
    static typename std::enable_if<!is_object_type<I>::value, bool>::type
    root_map_val_is_sv() noexcept { return false; }

    template<typename I>
    typename std::enable_if<is_object_type<I>::value>::type
    root_set_kv(void *obj, string_view_t key, string_view_t val) const {
        using val_t = unwrap_optional_t<typename I::mapped_type>;
        root_set_kv_slot<I>(
             bool_constant<jsonrefl::has_metadata<val_t>::value>{}
            ,obj
            ,key
            ,val
        );
    }

    template<typename I>
    typename std::enable_if<
        (!jsonrefl::has_metadata<typename I::mapped_type>::value)
        && !is_object_type<typename I::mapped_type>::value
    >::type
    root_set_kv_slot(std::false_type, void *obj, string_view_t key, string_view_t val) const {
        auto &m = *static_cast<I*>(obj);
        typename I::key_type k{};
        conv(k, key);
        typename I::mapped_type v{};
        conv(v, val);
        details::map_insert_or_assign(m, std::move(k), std::move(v));
    }

    template<typename I>
    typename std::enable_if<
        (!jsonrefl::has_metadata<typename I::mapped_type>::value)
        && is_object_type<typename I::mapped_type>::value
    >::type
    root_set_kv_slot(std::false_type, void *, string_view_t, string_view_t) const noexcept {}

    template<typename I>
    void root_set_kv_slot(std::true_type, void *, string_view_t, string_view_t) const noexcept {}

    template<typename I>
    typename std::enable_if<!is_object_type<I>::value>::type
    root_set_kv(void *, string_view_t, string_view_t) const noexcept {}

    template<typename I>
    typename std::enable_if<is_array_type<I>::value>::type
    root_set_one(void *obj, string_view_t str) const {
        using elem = unwrap_optional_t<typename I::value_type>;
        using has_meta_tag = bool_constant<::jsonrefl::has_metadata<elem>::value>;
        using nested_arr_tag = bool_constant<
            !::jsonrefl::has_metadata<elem>::value && is_array_type<elem>::value
        >;
        root_set_one_slot<I>(has_meta_tag{}, nested_arr_tag{}, obj, str);
    }

    template<typename I>
    void root_set_one_slot(
         std::true_type
        ,std::false_type
        ,void *
        ,string_view_t) const noexcept
    {}

    template<typename I>
    void root_set_one_slot(
         std::false_type
        ,std::true_type
        ,void *
        ,string_view_t) const noexcept
    {}

    template<typename I>
    void root_set_one_slot(
         std::false_type
        ,std::false_type
        ,void *obj
        ,string_view_t str) const {
        auto &v = *static_cast<I*>(obj);
        v.emplace_back();
        conv(v.back(), str);
    }

    template<typename I>
    typename std::enable_if<!is_array_type<I>::value>::type
    root_set_one(void *, string_view_t) const noexcept {}

    const object_holder_base* root_get_element_meta(std::true_type) const noexcept {
        using raw_elem = unwrap_optional_t<typename T::value_type>;
        return root_elem_meta_ptr(bool_constant<jsonrefl::has_metadata<raw_elem>::value>{});
    }

    const object_holder_base* root_elem_meta_ptr(std::true_type) const noexcept {
        using raw_elem = unwrap_optional_t<typename T::value_type>;
        return std::addressof(metadata<raw_elem>());
    }

    const object_holder_base* root_elem_meta_ptr(std::false_type) const noexcept
    { return nullptr; }

    const object_holder_base* root_get_element_meta(std::false_type) const noexcept
    { return nullptr; }

    bool root_elem_is_sv(std::true_type) const noexcept {
        using elem = unwrap_optional_t<typename T::value_type>;
        return std::is_same<elem, string_view_t>::value;
    }

    bool root_elem_is_value_t(std::true_type) const noexcept {
        using elem = unwrap_optional_t<typename T::value_type>;
        return std::is_same<elem, jsonrefl::value_t>::value;
    }

    bool root_elem_is_value_t(std::false_type) const noexcept
    { return false; }

    bool root_elem_is_sv(std::false_type) const noexcept
    { return false; }

    void* root_prepare_arr(std::true_type, void *obj) const {
        using raw_elem = unwrap_optional_t<typename T::value_type>;
        using has_meta_tag = bool_constant<::jsonrefl::has_metadata<raw_elem>::value>;
        using nested_arr_tag = bool_constant<
            !::jsonrefl::has_metadata<raw_elem>::value && is_array_type<raw_elem>::value
        >;
        return root_prepare_arr_slot(obj, has_meta_tag{}, nested_arr_tag{});
    }

    void* root_prepare_arr_slot(void *obj, std::true_type, std::false_type) const {
        auto &v = *static_cast<T*>(obj);
        v.emplace_back();
        return std::addressof(v.back());
    }

    void* root_prepare_arr_slot(void *obj, std::false_type, std::true_type) const {
        auto &v = *static_cast<T*>(obj);
        v.emplace_back();
        return std::addressof(v.back());
    }

    void* root_prepare_arr_slot(void *, std::false_type, std::false_type) const noexcept
    { return nullptr; }

    void* root_prepare_arr(std::false_type, void *) const noexcept
    { return nullptr; }

    template<typename I>
    typename std::enable_if<is_array_type<I>::value, bool>::type
    root_set_one_leaf(void *obj, string_view_t str, value_kind kind) const {
        using elem = unwrap_optional_t<typename I::value_type>;
        using has_meta_tag = bool_constant<::jsonrefl::has_metadata<elem>::value>;
        using nested_arr_tag = bool_constant<
            !::jsonrefl::has_metadata<elem>::value && is_array_type<elem>::value
        >;
        return root_set_one_leaf_slot<I>(has_meta_tag{}, nested_arr_tag{}, obj, str, kind);
    }

    template<typename I>
    bool root_set_one_leaf_slot(
         std::true_type
        ,std::false_type
        ,void *
        ,string_view_t
        ,value_kind) const noexcept
    { return true; }

    template<typename I>
    bool root_set_one_leaf_slot(
         std::false_type
        ,std::true_type
        ,void *
        ,string_view_t
        ,value_kind) const noexcept
    { return true; }

    template<typename I>
    bool root_set_one_leaf_slot(
         std::false_type
        ,std::false_type
        ,void *obj
        ,string_view_t str
        ,value_kind kind) const
    {
        auto &v = *static_cast<I*>(obj);
        v.emplace_back();
        return leaf_assign(v.back(), str, kind);
    }

    template<typename I>
    typename std::enable_if<!is_array_type<I>::value && !is_object_type<I>::value, bool>::type
    root_set_one_leaf(void *obj, string_view_t str, value_kind kind) const
    { return leaf_assign(*static_cast<I*>(obj), str, kind); }

    template<typename I>
    typename std::enable_if<is_object_type<I>::value, bool>::type
    root_set_kv_leaf(void *obj, string_view_t key, string_view_t val, value_kind kind) const {
        using val_t = unwrap_optional_t<typename I::mapped_type>;
        return root_set_kv_leaf_slot<I>(
             bool_constant<jsonrefl::has_metadata<val_t>::value>{}
            ,obj
            ,key
            ,val
            ,kind
        );
    }

    template<typename I>
    typename std::enable_if<
         (!jsonrefl::has_metadata<typename I::mapped_type>::value)
         && is_object_type<typename I::mapped_type>::value
        ,bool
    >::type
    root_set_kv_leaf_slot(std::false_type, void *, string_view_t, string_view_t, value_kind) const noexcept
    { return true; }

    // Direct construction from string_view for string keys avoids default-init + assign.
    template<typename K>
    static typename std::enable_if<std::is_same<K, std::string>::value, K>::type
    make_key(string_view_t sv) { return K{sv.data(), sv.size()}; }

    template<typename K>
    static typename std::enable_if<!std::is_same<K, std::string>::value, K>::type
    make_key(string_view_t sv) {
        K k{};
        conv(k, sv);
        return k;
    }

    template<typename I>
    typename std::enable_if<
         (!jsonrefl::has_metadata<typename I::mapped_type>::value)
         && !is_object_type<typename I::mapped_type>::value
        ,bool
    >::type
    root_set_kv_leaf_slot(std::false_type, void *obj, string_view_t key, string_view_t val, value_kind kind) const {
        auto &m = *static_cast<I*>(obj);
        typename I::mapped_type v{};
        if ( !leaf_assign(v, val, kind) ) { return false; }
        details::map_insert_or_assign(m, make_key<typename I::key_type>(key), std::move(v));
        return true;
    }

    template<typename I>
    bool root_set_kv_leaf_slot(std::true_type, void *, string_view_t, string_view_t, value_kind) const noexcept
    { return true; }

    template<typename I>
    typename std::enable_if<!is_object_type<I>::value, bool>::type
    root_set_kv_leaf(void *, string_view_t, string_view_t, value_kind) const noexcept
    { return true; }

    template<typename I>
    typename std::enable_if<
        is_object_type<I>::value
        && std::is_same<unwrap_optional_t<typename I::mapped_type>, jsonrefl::value_t>::value
    >::type
    root_set_map_null(void *obj, string_view_t key) const {
        auto &m = *static_cast<I*>(obj);
        typename I::key_type k{};
        conv(k, key);
        m.emplace(std::move(k), jsonrefl::value_t{});
    }

    template<typename I>
    typename std::enable_if<
        is_object_type<I>::value
        && !std::is_same<unwrap_optional_t<typename I::mapped_type>, jsonrefl::value_t>::value
    >::type
    root_set_map_null(void *, string_view_t) const noexcept {}

    template<typename I>
    typename std::enable_if<!is_object_type<I>::value>::type
    root_set_map_null(void *, string_view_t) const noexcept {}

    template<typename I>
    typename std::enable_if<is_array_type<I>::value, bool>::type
    root_set_leaf(void *obj, string_view_t sv, value_kind kind) const
    { return root_set_one_leaf<I>(obj, sv, kind); }

    template<typename I>
    typename std::enable_if<!is_array_type<I>::value && !is_object_type<I>::value, bool>::type
    root_set_leaf(void *obj, string_view_t sv, value_kind kind) const
    { return leaf_assign(*static_cast<I*>(obj), sv, kind); }

    template<typename I>
    typename std::enable_if<is_object_type<I>::value, bool>::type
    root_set_leaf(void *, string_view_t, value_kind) const noexcept { return true; }

    template<typename I>
    void root_set_null_scalar(void *obj, std::true_type) const
    { static_cast<I *>(obj)->assign({}, value_kind::null); }

    template<typename I>
    void root_set_null_scalar(void *, std::false_type) const noexcept {}

public:
    constexpr root_setter_t() noexcept
        :setter_base{setter_base::template deduce_root_kind<T>(), false}
    {}

    const object_holder_base* get_metadata() const noexcept override { return nullptr; }

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
    const object_holder_base* get_element_metadata() const noexcept override
    { return root_get_element_meta(bool_constant<is_array_type<T>::value>{}); }

    const object_holder_base* get_map_value_metadata() const noexcept override
    { return root_map_value_meta<T>(); }

    void* prepare_map_value(void *obj, string_view_t key) const override
    { return root_prepare_map_value<T>(obj, key); }

#ifdef _MSC_VER
#pragma warning(pop)
#endif

    const setter_base* get_nested_array_root_setter() const noexcept override
    { return root_nested_array_from_t(bool_constant<is_array_type<T>::value>{}); }

    static const setter_base* root_nested_array_from_t(std::false_type) noexcept
    { return nullptr; }

    static const setter_base* root_nested_array_from_t(std::true_type) noexcept {
        using elem = unwrap_optional_t<typename T::value_type>;
        using nested_elem_tag = bool_constant<
            is_array_type<elem>::value && !::jsonrefl::has_metadata<elem>::value
        >;
        return root_nested_elem_slot(nested_elem_tag{});
    }

    static const setter_base* root_nested_elem_slot(std::false_type) noexcept
    { return nullptr; }

    static const setter_base* root_nested_elem_slot(std::true_type) noexcept {
        using elem = unwrap_optional_t<typename T::value_type>;
        return root_setter_ptr<elem>();
    }

    static const setter_base* root_nested_map_from_mapped_flag(std::false_type) noexcept { return nullptr; }

    static const setter_base* root_nested_map_from_mapped_flag(std::true_type) noexcept {
        using val_t = unwrap_optional_t<typename T::mapped_type>;
        return root_setter_ptr<val_t>();
    }

    static const setter_base* root_nested_map_from_tslot(std::false_type) noexcept { return nullptr; }

    static const setter_base* root_nested_map_from_tslot(std::true_type) noexcept {
        using val_t = unwrap_optional_t<typename T::mapped_type>;
        using nested_map_tag = bool_constant<
            !jsonrefl::has_metadata<val_t>::value && is_object_type<val_t>::value
        >;
        return root_nested_map_from_mapped_flag(nested_map_tag{});
    }

    const setter_base* get_nested_map_root_setter() const noexcept override
    { return root_nested_map_from_tslot(bool_constant<is_object_type<T>::value>{}); }

    bool element_is_string_view() const noexcept override
    { return root_elem_is_sv(bool_constant<is_array_type<T>::value>{}); }

    bool element_is_value_t() const noexcept override
    { return root_elem_is_value_t(bool_constant<is_array_type<T>::value>{}); }

    bool map_key_is_string_view() const noexcept override
    { return root_map_key_is_sv<T>(); }

    bool map_value_is_string_view() const noexcept override
    { return root_map_val_is_sv<T>(); }

    bool map_value_is_value_t() const noexcept override
    { return root_map_val_is_value_t<T>(); }

    void* member_address(void *obj) const noexcept override { return obj; }
    const void* member_address(const void *obj) const noexcept override { return const_cast<void*>(obj); }

    void* prepare(void *obj) const override
    { return root_prepare_arr(bool_constant<is_array_type<T>::value>{}, obj); }

    void commit(void *, void *) const noexcept override {}

    void set(void *obj, string_view_t str) const override
    { root_set_one<T>(obj, str); }

    void set(void *obj, string_view_t key, string_view_t val) const override
    { root_set_kv<T>(obj, key, val); }

    bool set_leaf(void *obj, string_view_t sv, value_kind kind) const override
    { return root_set_leaf<T>(obj, sv, kind); }

    bool set_map_leaf(
         void *obj
        ,string_view_t key
        ,string_view_t val
        ,value_kind kind
    ) const override
    { return root_set_kv_leaf<T>(obj, key, val, kind); }

    void set_null_leaf(void *obj) const override {
        root_set_null_scalar<T>(
             obj
            ,bool_constant<std::is_same<T, jsonrefl::value_t>::value>{}
        );
    }

    void set_map_null_leaf(void *obj, string_view_t key) const override
    { root_set_map_null<T>(obj, key); }

    std::size_t value_required_bytes(const void *, serialize_flags, std::size_t) const noexcept override
    { return 4; }

    char* value_to_buffer(const void *, char *ptr, serialize_flags, std::size_t) const noexcept override
    { std::memcpy(ptr, "null", 4); return ptr + 4; }

    bool value_to_stream(
         const void *
        ,chunked_writer &w
        ,serialize_flags
        ,std::size_t
        ,const char *tail
        ,std::size_t tail_len) const noexcept override
    { return w.write_token("null", 4, tail, tail_len); }
};

template<typename T>
const root_setter_t<T>* root_setter_ptr() noexcept {
    static const root_setter_t<T> instance;
    return &instance;
}

/*************************************************************************************************/

constexpr std::size_t next_pow2(std::size_t v) noexcept {
    std::size_t p = 1;
    while ( p < v ) { p <<= 1; }
    return p;
}

template<std::size_t M>
constexpr std::size_t phf_slot(std::uint32_t hash, std::uint32_t mult, std::uint32_t seed) noexcept {
    static_assert((M & (M - 1)) == 0, "M must be a power of two");
    auto x = static_cast<std::uint32_t>(hash * mult + seed);
    x ^= x >> 16;
    x *= 0x85ebca6bu;
    x ^= x >> 13;
    return static_cast<std::size_t>(x & static_cast<std::uint32_t>(M - 1));
}

struct index_elem_type {
    std::uint32_t hash{};
    const setter_base *setter{};
};

template<std::size_t N>
using index_array_type= std::array<index_elem_type, N>;

template<std::size_t N, std::size_t M>
struct phf_index {
    std::uint32_t mult{1};
    std::uint32_t seed{0};
    std::array<index_elem_type, M> table{};

    constexpr const setter_base* get(std::uint32_t hash) const noexcept {
        const auto idx = phf_slot<M>(hash, mult, seed);
        const auto &elem = table[idx];
        if ( elem.setter == nullptr || elem.hash != hash ) { return nullptr; }
        return elem.setter;
    }
};

// compile-time hard-mode sentinel.
// the function is intentionally NOT `constexpr`, so calling it from inside
// a constexpr context (where the PHF is built) is *itself* a non-constant
// operation - which makes the program ill-formed.
inline void phf_perfect_search_failed() noexcept {
    static const char *msg =
        "jsonrefl PHF: perfect hash function search exhausted - "
        "schema too pathological for current PHF heuristics. "
        "bump phf_table_size or rename conflicting fields."
    ;
    std::fprintf(stderr, "%s\n", msg);
    std::abort();
}

inline void linear_index_collision_detected() noexcept {
    static const char *msg =
        "jsonrefl linear index: two JSONREFL_METADATA fields produce the "
        "same FNV-1a hash. rename one of the conflicting fields."
    ;
    std::fprintf(stderr, "%s\n", msg);
    std::abort();
}

template<std::size_t N, std::size_t I, std::size_t J>
constexpr typename std::enable_if<(J < N), void>::type
check_linear_collision_pair_row(const index_array_type<N> &src) noexcept {
    if ( std::get<I>(src).hash == std::get<J>(src).hash ) {
        linear_index_collision_detected();
    }
    check_linear_collision_pair_row<N, I, J + 1u>(src);
}

template<std::size_t N, std::size_t I, std::size_t J>
constexpr typename std::enable_if<(J >= N), void>::type
check_linear_collision_pair_row(const index_array_type<N> &) noexcept
{}

template<std::size_t N, std::size_t I>
constexpr typename std::enable_if<(I + 1u < N), void>::type
check_linear_collision_all_rows(const index_array_type<N> &src) noexcept {
    check_linear_collision_pair_row<N, I, I + 1u>(src);
    check_linear_collision_all_rows<N, I + 1u>(src);
}

template<std::size_t N, std::size_t I>
constexpr typename std::enable_if<(I + 1u >= N), void>::type
check_linear_collision_all_rows(const index_array_type<N> &) noexcept {}

template<std::size_t N>
constexpr typename std::enable_if<(N > 1u), void>::type
check_linear_src_collisions(const index_array_type<N> &src) noexcept
{ check_linear_collision_all_rows<N, 0u>(src); }

template<std::size_t N>
constexpr typename std::enable_if<(N <= 1u), void>::type
check_linear_src_collisions(const index_array_type<N> &) noexcept {}

template<std::size_t N, std::size_t M>
constexpr phf_index<N, M> make_phf_index(const index_array_type<N> &src) noexcept {
    phf_index<N, M> out{};
    if ( N == 0 ) { return out; }
    std::uint32_t seed_hint = 0;
    for ( const auto &elem : src ) { seed_hint ^= elem.hash; }
    for ( std::uint32_t mult = 1; mult < 256; mult += 2 ) {
        for ( std::uint32_t si = 0; si < 4096; ++si ) {
            const std::uint32_t seed = (si + seed_hint) & 4095u;
            std::array<bool, M> probe{};
            bool ok = true;
            for ( const auto &elem: src ) {
                const auto idx = phf_slot<M>(elem.hash, mult, seed);
                if ( probe[idx] ) { ok = false; break; }
                probe[idx] = true;
            }
            if ( !ok ) { continue; }

            out.mult = mult;
            out.seed = seed;
            for ( const auto &elem: src ) {
                out.table[phf_slot<M>(elem.hash, mult, seed)] = {elem.hash, elem.setter};
            }

            return out;
        }
    }
    phf_perfect_search_failed();

    return out;
}

template<std::size_t M, std::size_t ...Ids, typename ...TupleElems>
constexpr phf_index<sizeof...(TupleElems), M> make_phf_index(
     std::index_sequence<Ids...>
    ,const std::tuple<TupleElems...> &tuple) noexcept
{
    constexpr auto N = sizeof...(TupleElems);
    return make_phf_index<N, M>(index_array_type<N>{{
        {
             std::get<Ids>(tuple).hash
            ,static_cast<const setter_base *>(&(std::get<Ids>(tuple).member))
        }...
    }});
}

template<std::size_t N>
struct linear_index {
    std::array<std::uint32_t,       N> hashes{};
    std::array<const setter_base *, N> setters{};

    constexpr const setter_base* get(std::uint32_t hash) const noexcept {
        for ( std::size_t i = 0; i < N; ++i ) {
            if ( hashes[i] == hash ) { return setters[i]; }
        }
        return nullptr;
    }
};

template<std::size_t N, std::size_t ...I>
constexpr linear_index<N> make_linear_index_from_src(
    const index_array_type<N> &src
    ,std::index_sequence<I...>) noexcept
{
    check_linear_src_collisions(src);
    return linear_index<N>{
        std::array<std::uint32_t, N>{{std::get<I>(src).hash...}}
        ,std::array<const setter_base *, N>{{std::get<I>(src).setter...}}
    };
}

template<std::size_t N>
constexpr linear_index<N> make_linear_index(const index_array_type<N> &src) noexcept
{ return make_linear_index_from_src(src, std::make_index_sequence<N>{}); }

template<std::size_t ...Ids, typename ...TupleElems>
constexpr linear_index<sizeof...(TupleElems)> make_linear_index(
     std::index_sequence<Ids...>
    ,const std::tuple<TupleElems...> &tuple) noexcept
{
    constexpr auto N = sizeof...(TupleElems);
    return make_linear_index<N>(index_array_type<N>{{
        {
             std::get<Ids>(tuple).hash
            ,static_cast<const setter_base *>(&(std::get<Ids>(tuple).member))
        }...
    }});
}

/*************************************************************************************************/

struct object_holder_base {
    using get_fn_t = const setter_base* (*)(const void *ctx, std::uint32_t hash);

private:
    const void *m_ctx;
    get_fn_t m_get;
    std::size_t m_size;

public:
    constexpr object_holder_base(const void *ctx, get_fn_t get, std::size_t size)
        :m_ctx{ctx}
        ,m_get{get}
        ,m_size{size}
    {}

    constexpr auto size() const noexcept { return m_size; }
    constexpr auto empty() const noexcept { return size() == 0; }

    const setter_base* get(string_view_t key) const noexcept {
        if ( key.empty() ) { return nullptr; }
        return m_get(m_ctx, fnv1a(key.data(), key.size()));
    }
};

/*************************************************************************************************/

template<typename ...Types>
class object_holder_t final: public object_holder_base {
    static constexpr auto k_n = sizeof...(Types);
    static constexpr auto k_linear_threshold = std::size_t{16};
    static constexpr bool k_use_linear = k_n <= k_linear_threshold;

    static constexpr auto phf_table_size = next_pow2((k_n == 0) ? 1u : k_n * 4u);
    using index_type = std::conditional_t<
         k_use_linear
        ,linear_index<k_n>
        ,phf_index<k_n, phf_table_size>
    >;

    const string_view_t m_name;
    const std::tuple<Types...> m_tuple;
    const index_type m_index;
    const std::size_t m_max_name_len;

    static const setter_base* get_by_hash_thunk(const void *ctx, std::uint32_t hash) noexcept
    { return static_cast<const object_holder_t *>(ctx)->m_index.get(hash); }

    template<std::size_t ...Ids>
    static constexpr index_type build_index_dispatch(
         std::index_sequence<Ids...> seq
        ,const std::tuple<Types...> &t
        ,std::true_type) noexcept
    { return make_linear_index(seq, t); }

    template<std::size_t ...Ids>
    static constexpr index_type build_index_dispatch(
         std::index_sequence<Ids...> seq
        ,const std::tuple<Types...> &t
        ,std::false_type) noexcept
    { return make_phf_index<phf_table_size>(seq, t); }

    template<std::size_t ...Ids>
    static constexpr index_type build_index(
         std::index_sequence<Ids...> seq
        ,const std::tuple<Types...> &t) noexcept
    { return build_index_dispatch(seq, t, bool_constant<k_use_linear>{}); }

    template<typename PrintFn>
    static void dump_index(const linear_index<k_n> &idx, PrintFn &&print_entry) {
        for ( std::size_t i = 0; i < k_n; ++i ) print_entry(idx.hashes[i], idx.setters[i]);
    }

    template<typename PrintFn>
    static void dump_index(const phf_index<k_n, phf_table_size> &idx, PrintFn &&print_entry) {
        for ( std::size_t i = 0; i < phf_table_size; ++i ) {
            if ( idx.table[i].setter == nullptr ) { continue; }
            const auto &it = idx.table[i];
            print_entry(it.hash, it.setter);
        }
    }

public:
    template<typename ...Args>
    constexpr object_holder_t(string_view_t name, Args && ...args)
        :object_holder_base{this, get_by_hash_thunk, sizeof...(Types)}
        ,m_name{name}
        ,m_tuple{std::forward<Args>(args)...}
        ,m_index{build_index(std::index_sequence_for<Types...>{}, m_tuple)}
        ,m_max_name_len{details::jsonrefl_max(args.name.length()...)}
    {}

    constexpr auto name() const noexcept { return m_name; }
    static constexpr bool uses_minimal_index() noexcept { return k_use_linear; }
    constexpr const index_type& index() const noexcept { return m_index; }

    std::ostream& dump(std::ostream &os) const {
        // print tuple (not sorted)
        os << "-- hash --  -- name --  -- address -- type --" << std::endl;
        tuple_apply(
            [&os, this](const auto & ...elems){
                static const auto print_cb = [this](std::ostream &os, const auto &elem){
                    static const auto name_min_len = sizeof("-- name --") - 1;
                    static const auto spaces = string_view_t{"                                "}; // 32

                    os << "0x";

                    auto width = os.width(8);
                    auto fillc = os.fill('0');
                    os << std::hex << elem.hash << " : " << elem.name;

                    os.width(width);
                    os.fill(fillc);

                    const auto target_width = (m_max_name_len > name_min_len) ? m_max_name_len : name_min_len;
                    const auto align_for = target_width - elem.name.size();
                    assert(align_for <= spaces.size());

                    os.write(spaces.data(), align_for);
                    os
                        << " : " << std::addressof(elem.member)
                        << " : " << elem.member.type_id()
                        << std::endl
                    ;
                };
                using expander = int[];
                (void)expander{0, ((void)print_cb(os, elems), 0)...};
            }
            ,m_tuple
        );

        // print index
        os << "-- hash --  -- address --" << std::endl;
        auto flags = os.flags();
        const auto print_entry = [&os](std::uint32_t hash, const setter_base *setter) {
            os << "0x";

            auto width = os.width(8);
            auto fillc = os.fill('0');
            os << std::hex << hash;

            os.width(width);
            os.fill(fillc);
            os << ": " << setter << std::endl;
        };
        dump_index(m_index, print_entry);
        os.flags(flags);

        return os;
    }

    std::size_t required_bytes(const void *obj, serialize_flags fl, std::size_t indent) const noexcept {
        const bool pretty = pretty_set(fl);
        const bool comments = pretty && comments_set(fl);
        std::size_t n = 1;
        const auto child = pretty ? indent + k_indent_step_len : std::size_t{0};
        if ( pretty && sizeof...(Types) > 0 ) { n += 1; }
        bool first = true;
        tuple_apply(
            [&n, &first, obj, fl, pretty, comments, child](const auto & ...elems) {
                const auto count = [&n, &first, obj, fl, pretty, comments, child](const auto &elem) {
                    if ( !first ) {
                        n += 1;
                        if ( pretty ) { n += 1; }
                    }
                    if ( comments && !elem.doc.empty() ) {
                        n += child + 3 + elem.doc.size() + 1;
                    }
                    if ( pretty ) { n += child; }
                    n += 1 + elem.name.size() + (pretty ? 3 : 2);
                    n += elem.member.value_required_bytes(obj, fl, child);
                    first = false;
                };
                using expander = int[];
                (void)expander{0, ((void)count(elems), 0)...};
            }
            ,m_tuple
        );
        if ( pretty && !first ) {
            n += 1;
            n += indent;
        }
        n += 1;

        return n;
    }

    char* to_buffer(const void *obj, char *ptr, serialize_flags fl, std::size_t indent) const noexcept {
        const bool pretty = pretty_set(fl);
        const bool comments = pretty && comments_set(fl);
        const auto child = pretty ? indent + k_indent_step_len : std::size_t{0};
        *ptr++ = '{';
        if ( pretty && sizeof...(Types) > 0 ) { *ptr++ = '\n'; }
        bool first = true;
        tuple_apply(
            [&ptr, &first, obj, fl, pretty, comments, child](const auto & ...elems) {
                const auto write = [&ptr, &first, obj, fl, pretty, comments, child](const auto &elem) {
                    if ( !first ) {
                        *ptr++ = ',';
                        if ( pretty ) { *ptr++ = '\n'; }
                    }
                    if ( comments && !elem.doc.empty() ) {
                        std::memset(ptr, ' ', child); ptr += child;
                        *ptr++ = '/'; *ptr++ = '/'; *ptr++ = ' ';
                        std::memcpy(ptr, elem.doc.data(), elem.doc.size());
                        ptr += elem.doc.size();
                        *ptr++ = '\n';
                    }
                    if ( pretty ) { std::memset(ptr, ' ', child); ptr += child; }
                    *ptr++ = '"';
                    std::memcpy(ptr, elem.name.data(), elem.name.size());
                    ptr += elem.name.size();
                    ptr += pretty
                        ? (std::memcpy(ptr, "\": ", 3), 3)
                        : (std::memcpy(ptr, "\":", 2), 2)
                    ;
                    ptr = elem.member.value_to_buffer(obj, ptr, fl, child);
                    first = false;
                };
                using expander = int[];
                (void)expander{0, ((void)write(elems), 0)...};
            }
            ,m_tuple
        );
        if ( pretty && !first ) {
            *ptr++ = '\n';
            std::memset(ptr, ' ', indent); ptr += indent;
        }
        *ptr++ = '}';

        return ptr;
    }

    bool to_stream(
         const void *obj
        ,chunked_writer &w
        ,serialize_flags fl
        ,std::size_t indent
        ,const char *tail = nullptr
        ,std::size_t tail_len = 0) const noexcept
    {
        return pretty_set(fl)
            ? to_stream_impl<true >(obj, w, fl, indent, tail, tail_len)
            : to_stream_impl<false>(obj, w, fl, indent, tail, tail_len)
        ;
    }

    template<bool Pretty>
    bool to_stream_impl(
             const void *obj
            ,chunked_writer &w
            ,serialize_flags fl
            ,std::size_t indent
            ,const char *tail
            ,std::size_t tail_len) const noexcept
    {
        const auto child = Pretty ? indent + k_indent_step_len : std::size_t{0};
        if ( k_n == 0 ) { return w.write_token("{}", 2, tail, tail_len); }
        if ( !Pretty ) {
            if ( !w.put('{') ) { return false; }
            std::size_t idx = 0;
            bool ok = true;
            tuple_apply(
                [&](const auto & ...elems) {
                    const auto write_one = [&](const auto &elem) {
                        if ( !ok ) { return; }
                        ++idx;
                        if ( !w.template write_field_key<false>(elem.name, 0) ) { ok = false; return; }
                        if ( idx == k_n ) {
                            char tail_buf[16];
                            const auto need = 1u + tail_len;
                            if ( need <= sizeof(tail_buf) ) {
                                tail_buf[0] = '}';
                                if ( tail_len ) { std::memcpy(tail_buf + 1, tail, tail_len); }
                                if ( !elem.member.value_to_stream(obj, w, fl, 0, tail_buf, need) ) { ok = false; }
                            } else {
                                if ( !elem.member.value_to_stream(obj, w, fl, 0, nullptr, 0) ) { ok = false; return; }
                                if ( !w.put('}') ) { ok = false; return; }
                                if ( tail_len ) { ok = w.write(tail, tail_len); }
                            }
                        } else {
                            const char comma = ',';
                            if ( !elem.member.value_to_stream(obj, w, fl, 0, &comma, 1) ) { ok = false; }
                        }
                    };
                    using expander = int[];
                    (void)expander{0, ((void)write_one(elems), 0)...};
                }
                ,m_tuple
            );
            return ok;
        } else {
            const bool comments = comments_set(fl);
            if ( !w.put('{') ) { return false; }
            if ( k_n > 0 && !w.put('\n') ) { return false; }
            bool first = true;
            bool ok = true;
            tuple_apply(
                [&](const auto & ...elems) {
                    const auto write_one = [&](const auto &elem) {
                        if ( !ok ) { return; }
                        if ( !first ) {
                            if ( !w.put(',') ) { ok = false; return; }
                            if ( !w.put('\n') ) { ok = false; return; }
                        }
                        if ( comments && !elem.doc.empty() ) {
                            if ( !w.fill(' ', child) ) { ok = false; return; }
                            if ( !w.write("// ", 3) ) { ok = false; return; }
                            if ( !w.write(elem.doc.data(), elem.doc.size()) ) { ok = false; return; }
                            if ( !w.put('\n') ) { ok = false; return; }
                        }
                        if ( !w.template write_field_key<true>(elem.name, child) ) { ok = false; return; }
                        if ( !elem.member.value_to_stream(obj, w, fl, child, nullptr, 0) ) { ok = false; return; }
                        first = false;
                    };
                    using expander = int[];
                    (void)expander{0, ((void)write_one(elems), 0)...};
                }
                ,m_tuple
            );
            if ( !ok ) { return false; }
            if ( !first ) {
                if ( !w.put('\n') ) { return false; }
                if ( !w.fill(' ', indent) ) { return false; }
            }
            if ( !w.put('}') ) { return false; }
            return tail_len ? w.write(tail, tail_len) : true;
        }
    }
};

} // ns details

template<typename T, typename C>
struct object_member_t {
    std::uint32_t hash;
    string_view_t name;
    string_view_t doc;
    details::setter_t<C, T> member;
};

namespace details {

template<typename>
struct member_type_of;

template<typename T, typename C>
struct member_type_of<object_member_t<T, C>> { using type = T; };

template<typename ...Types>
constexpr std::size_t max_member_depth(const object_holder_t<Types...> &) noexcept;

template<typename T, typename = void>
struct stack_depth_holder
{ static constexpr std::size_t value = 0u; };

template<typename T>
struct stack_depth_holder<
     T
    ,typename std::enable_if<has_metadata<T>::value>::type
> { static constexpr std::size_t value = 1u + max_member_depth(metadata<T>()); };

template<typename T>
struct stack_depth_holder<
     T
    ,typename std::enable_if<
        !has_metadata<T>::value && details::is_array_type<T>::value
    >::type
> { static constexpr std::size_t value = 1u + stack_depth_holder<typename T::value_type>::value; };

template<typename T>
struct stack_depth_holder<
     T
    ,typename std::enable_if<
        !has_metadata<T>::value
        && !details::is_array_type<T>::value
        && details::is_object_type<T>::value
    >::type
> { static constexpr std::size_t value = 1u + stack_depth_holder<typename T::mapped_type>::value; };

template<typename T>
struct stack_depth_holder<
     T
    ,typename std::enable_if<
        !has_metadata<T>::value
        && !details::is_array_type<T>::value
        && !details::is_object_type<T>::value
        && details::is_optional_type<T>::value
    >::type
> { static constexpr std::size_t value = stack_depth_holder<details::unwrap_optional_t<T>>::value; };

} // ns details

template<typename T>
constexpr std::size_t stack_depth() noexcept
{ return details::stack_depth_holder<T>::value; }

namespace details {

template<typename ...Types>
constexpr std::size_t max_member_depth(const object_holder_t<Types...> &) noexcept {
    if ( sizeof...(Types) == 0 ) { return 0u; }
    return jsonrefl_max(::jsonrefl::stack_depth<typename member_type_of<Types>::type>()...);
}

} // ns details

/*************************************************************************************************/

template<std::size_t N, typename T, typename C>
constexpr auto object_member(const char (&str)[N], T C::*mptr) noexcept
{ return object_member_t<T, C>{details::fnv1a(str, N - 1u), {str, N - 1u}, {}, mptr}; }

template<std::size_t N, std::size_t D, typename T, typename C>
constexpr auto object_member(const char (&str)[N], const char (&doc)[D], T C::*mptr) noexcept
{ return object_member_t<T, C>{details::fnv1a(str, N - 1u), {str, N - 1u}, {doc, D - 1u}, mptr}; }

// Used as:
// auto o = object_holder(
//      "type name"
//     ,object_member("member0 name", &type::member0)
//     ,object_member("member1 name", &type::member1)
// );
//
// so, `o` is the final storage for all members metadata for `type name`
template<typename ...Members>
constexpr auto object_holder(string_view_t name, Members && ...members) noexcept
{ return details::object_holder_t<Members...>{name, std::forward<Members>(members)...}; }

/*************************************************************************************************/

namespace details {

template<typename C, typename T>
bool setter_t<C, T>::value_to_stream(
     const void *obj
    ,chunked_writer &w
    ,serialize_flags fl
    ,std::size_t indent
    ,const char *tail
    ,std::size_t tail_len) const noexcept
{
    return pretty_set(fl)
        ? stream_json<true>(w, static_cast<const C *>(obj)->*m_ptr, indent, tail, tail_len, fl)
        : stream_json<false>(w, static_cast<const C *>(obj)->*m_ptr, indent, tail, tail_len, fl)
    ;
}

} // ns details

/*************************************************************************************************/

namespace details {

#if JSONREFL_CXX >= 201703L
constexpr
#else
inline
#endif
std::array<bool, 256> make_ws_table() noexcept {
    std::array<bool, 256> t{};
    t[static_cast<unsigned char>(' ')] = true;
    t[static_cast<unsigned char>('\t')] = true;
    t[static_cast<unsigned char>('\n')] = true;
    t[static_cast<unsigned char>('\r')] = true;
    return t;
}

inline bool is_ws(char c) noexcept {
#if JSONREFL_CXX >= 201703L
    static constexpr auto k_ws = make_ws_table();
#else
    static const auto k_ws = make_ws_table();
#endif
    return k_ws[static_cast<unsigned char>(c)];
}

#if JSONREFL_CXX >= 201703L
constexpr
#else
inline
#endif
std::array<bool, 256> make_num_table() noexcept {
    std::array<bool, 256> t{};
    t['0']=t['1']=t['2']=t['3']=t['4']=true;
    t['5']=t['6']=t['7']=t['8']=t['9']=true;
    t[static_cast<unsigned char>('.')]=true;
    t[static_cast<unsigned char>('-')]=true;
    t[static_cast<unsigned char>('+')]=true;
    t[static_cast<unsigned char>('e')]=true;
    t[static_cast<unsigned char>('E')]=true;
    return t;
}

inline bool is_num_char(char c) noexcept {
#if JSONREFL_CXX >= 201703L
    static constexpr auto k_num = make_num_table();
#else
    static const auto k_num = make_num_table();
#endif
    return k_num[static_cast<unsigned char>(c)];
}

inline bool skip_line_comment(const char *&p, const char *end) noexcept {
    while ( p < end && *p != '\n' ) ++p;
    if ( p < end ) { ++p; return true; }
    return false;
}


inline bool hex4(const char *p, std::uint32_t &v) noexcept {
    v = 0;
    for ( auto i = 0u; i < 4u; ++i ) {
        v <<= 4;
        const auto c = static_cast<unsigned char>(p[i]);
        if ( c >= '0' && c <= '9' ) { v += c - '0'; }
        else if ( c >= 'a' && c <= 'f' ) { v += 10u + (c - 'a'); }
        else if ( c >= 'A' && c <= 'F' ) { v += 10u + (c - 'A'); }
        else { v = 0; return false; }
    }
    return true;
}

inline void encode_utf8(std::uint32_t cp, char *&dst) noexcept {
    if ( cp < 0x80u ) {
        *dst++ = static_cast<char>(cp);
    } else if ( cp < 0x800u ) {
        *dst++ = static_cast<char>(0xC0u | (cp >> 6));
        *dst++ = static_cast<char>(0x80u | (cp & 0x3Fu));
    } else if ( cp < 0x10000u ) {
        *dst++ = static_cast<char>(0xE0u | (cp >> 12));
        *dst++ = static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu));
        *dst++ = static_cast<char>(0x80u | (cp & 0x3Fu));
    } else {
        *dst++ = static_cast<char>(0xF0u | (cp >> 18));
        *dst++ = static_cast<char>(0x80u | ((cp >> 12) & 0x3Fu));
        *dst++ = static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu));
        *dst++ = static_cast<char>(0x80u | (cp & 0x3Fu));
    }
}

inline bool decode_escape_into(const char *&p, const char *end, char *&dst, bool allow_lone_surrogates = false, bool allow_invalid_escape = false) noexcept {
    char e = *p++;
    switch ( e ) {
        case '"':  *dst++ = '"';  return true;
        case '\\': *dst++ = '\\'; return true;
        case '/':  *dst++ = '/';  return true;
        case 'b':  *dst++ = '\b'; return true;
        case 'f':  *dst++ = '\f'; return true;
        case 'n':  *dst++ = '\n'; return true;
        case 'r':  *dst++ = '\r'; return true;
        case 't':  *dst++ = '\t'; return true;
        case 'u': {
            if ( end - p < 4 ) { return false; }
            std::uint32_t cp;
            if ( !hex4(p, cp) ) { return false; }
            p += 4;
            if ( cp >= 0xD800u && cp <= 0xDBFFu ) {
                std::uint32_t lo = 0;
                if ( (end - p) >= 6 && p[0] == '\\' && p[1] == 'u'
                    && hex4(p + 2, lo) && lo >= 0xDC00u && lo <= 0xDFFFu )
                {
                    cp = 0x10000u + ((cp - 0xD800u) << 10) + (lo - 0xDC00u);
                    p += 6;
                } else if ( allow_lone_surrogates ) {
                    encode_utf8(cp, dst);
                    return true;
                } else {
                    return false;
                }
            } else if ( cp >= 0xDC00u && cp <= 0xDFFFu ) {
                if ( !allow_lone_surrogates ) { return false; }
                encode_utf8(cp, dst);
                return true;
            }
            encode_utf8(cp, dst);
            return true;
        }
        default:
            if ( !allow_invalid_escape ) { return false; }
            *dst++ = '\\';
            *dst++ = e;
            return true;
    }
}

inline std::size_t unescape_range(char *data, std::size_t len, bool allow_lone_surrogates = false, bool allow_invalid_escape = false) noexcept {
    char *dst = data;
    const char *src = data;
    const char *end = data + len;
    while ( src < end ) {
        if ( *src != '\\' ) { *dst++ = *src++; continue; }
        ++src;
        if ( !decode_escape_into(src, end, dst, allow_lone_surrogates, allow_invalid_escape) ) { return std::size_t(-1); }
    }
    return static_cast<std::size_t>(dst - data);
}

inline std::size_t unescape_range(const char *src, std::size_t len, char *dst, bool allow_lone_surrogates = false, bool allow_invalid_escape = false) noexcept {
    const char *end = src + len;
    char *out = dst;
    while ( src < end ) {
        if ( *src != '\\' ) { *out++ = *src++; continue; }
        ++src;
        if ( !decode_escape_into(src, end, out, allow_lone_surrogates, allow_invalid_escape) ) { return std::size_t(-1); }
    }
    return static_cast<std::size_t>(out - dst);
}

inline bool unescape_inplace(std::string &s) {
    if ( s.empty() ) { return true; }
    const auto new_len = unescape_range(&s[0], s.size());
    if ( new_len == std::size_t(-1) ) { return false; }
    s.resize(new_len);
    return true;
}

template<typename F>
inline constexpr bool has_flag(F fl, F f) noexcept {
    using U = typename std::underlying_type<F>::type;
    return static_cast<bool>(static_cast<U>(fl) & static_cast<U>(f));
}

} // ns details

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

enum class flags: std::uint8_t {
     none                   = 0
    ,skip_unknown_keys      = 1u<<0
    ,allow_comments         = 1u<<1
    ,allow_infinity_and_nan = 1u<<2
    ,allow_invalid_utf16    = 1u<<3
    ,allow_invalid_utf8     = 1u<<4
    ,allow_invalid_escapes  = 1u<<5
};

constexpr flags operator|(flags a, flags b) noexcept {
    return static_cast<flags>(
        static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b)
    );
}
constexpr flags operator&(flags a, flags b) noexcept {
    return static_cast<flags>(
        static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b)
    );
}

enum class state: std::uint8_t {
     ok
    ,incomplete       // need more bytes; safe to feed another chunk
    ,invalid          // unrecoverable: malformed JSON or schema mismatch
    ,no_buffer        // escape decoding required but no `accum` was bound in the parser ctor
    ,sv_cross_chunk   // a string_view-typed value/key spans a chunk boundary; not representable
    ,unknown_key      // object key not in metadata (use flags::skip_unknown_keys to ignore)
    ,record_end       // document N complete; next document begins at the updated ptr/remaining
};

/*************************************************************************************************/

struct cursor {
    cursor() noexcept
        :m_st{state::ok}
        ,m_next{nullptr}
        ,m_remaining{0}
    {}

    state status() const noexcept { return m_st; }
    std::size_t remaining() const noexcept { return m_remaining; }

private:
    template<typename, std::size_t>
    friend class parser;

    state       m_st;
    const char *m_next;
    std::size_t m_remaining;
};

/*************************************************************************************************/

template<typename C, std::size_t MaxNesting = stack_depth<C>() + 8>
class parser {
    static_assert(MaxNesting >= 2, "MaxNesting must be at least 2");
    static_assert(
         has_metadata<C>::value || details::is_object_or_array<C>::value
        ,"not has_metadata, nor is_object_or_array"
    );

    enum class json_state: uint8_t {
         value
        ,value_or_arr_end
        ,key_or_obj_end
        ,colon
        ,comma_or_obj_end
        ,comma_or_arr_end
        ,in_string
        ,in_escape
        ,in_unicode
        ,in_number
        ,in_literal
        ,done
        ,in_comment_slash
        ,in_line_comment
    };
    enum class json_context: uint8_t {
         object
        ,array
    };

    enum class skv_phase: uint8_t {
         obj_expect_key_or_close
        ,obj_expect_colon
        ,obj_expect_value
        ,obj_expect_comma_or_close
        ,arr_expect_value_or_close
        ,arr_expect_comma_or_close
    };

    struct stack_elem {
        const details::object_holder_base *holder;
        const details::setter_base *arr_setter;
        const details::setter_base *map_setter;
        void *addr;
    };

    C *m_cls;
    std::string *m_accum;
    const char *m_key_inline_ptr;
    std::size_t m_key_off;
    std::size_t m_key_len;
    std::size_t m_value_off;
    const char *m_seg_start;
    std::size_t m_str_len;
    std::size_t m_uni_remain;
    const char *m_lit_str;
    std::size_t m_lit_pos;
    std::size_t m_lit_len;
    const char *m_skv_lit_src;
    std::size_t m_skv_lit_i;
    std::size_t m_skv_lit_n;
    std::array<stack_elem, MaxNesting> m_stack;
    int m_stack_idx;
    int m_ctx_depth;
    int m_skv_sp;
    json_state m_js;
    json_state m_comment_ret_js;
    bool m_key_in_accum;
    bool m_key_has_esc;
    bool m_has_esc;
    bool m_is_key;
    bool m_accumulating;
    bool m_skip_next_value;
    bool m_skv_num;
    std::uint8_t m_skv_str_mode;
    std::uint8_t m_skv_u4_remain;
    std::uint8_t m_skv_after_str;
    std::uint8_t m_skv_comment;
    std::uint8_t m_utf8_remain;
    bool         m_allow_lone_surrogates;
    bool         m_allow_invalid_escape;
    flags        m_flags;
    json_context m_ctx_buf[MaxNesting];
    std::array<std::uint8_t, MaxNesting> m_skv_kind;
    std::array<skv_phase, MaxNesting> m_skv_ph;

    bool stack_push(stack_elem elem) noexcept {
        if ( m_stack_idx + 1 >= int(m_stack.size()) ) { return false; }
        m_stack[++m_stack_idx] = elem;
        return true;
    }

    void stack_pop() noexcept {
        if ( m_stack_idx >= 0 ) { m_stack[m_stack_idx--] = stack_elem{}; }
    }

    stack_elem stack_top() const noexcept {
        return m_stack_idx >= 0 ? m_stack[m_stack_idx] : stack_elem{};
    }

    bool ctx_push(json_context c) noexcept {
        if ( static_cast<std::size_t>(m_ctx_depth + 1) >= MaxNesting ) { return false; }
        m_ctx_buf[++m_ctx_depth] = c;
        return true;
    }
    void ctx_pop() noexcept { --m_ctx_depth; }
    json_context ctx_top() const noexcept { return m_ctx_buf[m_ctx_depth]; }

    void enter_object_root_push(std::true_type)
    { (void)stack_push({std::addressof(metadata<C>()), nullptr, nullptr, m_cls}); }

    void enter_object_root_push(std::false_type) {
        if ( details::is_object_type<C>::value )
        { (void)stack_push({nullptr, nullptr, details::root_setter_ptr<C>(), m_cls}); }
    }

    void enter_array_root_push(std::true_type)
    { (void)stack_push({std::addressof(metadata<C>()), nullptr, nullptr, m_cls}); }

    void enter_array_root_push(std::false_type) {
        if ( details::is_array_type<C>::value )
        { (void)stack_push({nullptr, details::root_setter_ptr<C>(), nullptr, m_cls}); }
    }

    void reset_key() noexcept {
        m_key_inline_ptr = nullptr;
        m_key_off = 0;
        m_key_len = 0;
        m_key_in_accum = false;
        m_key_has_esc = false;
    }

    string_view_t current_key(const std::string *accum) const noexcept {
        if ( m_key_len == 0 ) { return {}; }
        return m_key_in_accum
            ? string_view_t{accum->data() + m_key_off, m_key_len}
            : string_view_t{m_key_inline_ptr, m_key_len}
        ;
    }

    void after_value() noexcept {
        m_js = (m_ctx_depth < 0)
            ? json_state::done
            : (ctx_top() == json_context::object)
                ? json_state::comma_or_obj_end
                : json_state::comma_or_arr_end
        ;
    }

    bool current_string_target_is_sv(const std::string *accum) const noexcept {
        if ( m_stack_idx < 0 ) { return false; }
        const auto top = stack_top();
        if ( m_is_key ) { return top.map_setter && top.map_setter->map_key_is_string_view(); }
        if ( top.holder ) {
            const auto key = current_key(accum);
            if ( key.empty() ) { return false; }
            const auto *setter = top.holder->get(key);
            return setter && (setter->is_string_view() || setter->is_value_t());
        }
        if ( top.map_setter ) {
            return top.map_setter->map_value_is_string_view()
                || top.map_setter->map_value_is_value_t();
        }
        if ( top.arr_setter ) {
            return top.arr_setter->element_is_string_view()
                || top.arr_setter->element_is_value_t();
        }
        return false;
    }

    state sv_or_incomplete(const std::string *accum) const noexcept
    { return current_string_target_is_sv(accum) ? state::sv_cross_chunk : state::incomplete; }

    bool flush_segment(std::string *accum, const char *seg_end) {
        if ( !accum ) { return false; }
        if ( !m_accumulating ) {
            const bool is_key_token = (m_js == json_state::in_string) && m_is_key;
            if ( is_key_token ) {
                accum->assign(m_seg_start, seg_end);
                m_value_off = 0;
            } else if ( m_key_in_accum ) {
                m_value_off = accum->size();
                accum->append(m_seg_start, seg_end);
            } else if ( m_key_len > 0 ) {
                accum->assign(m_key_inline_ptr, m_key_len);
                m_key_in_accum = true;
                m_key_off = 0;
                m_value_off = accum->size();
                accum->append(m_seg_start, seg_end);
            } else {
                accum->assign(m_seg_start, seg_end);
                m_value_off = 0;
            }
            m_accumulating = true;
        } else {
            accum->append(m_seg_start, seg_end);
        }
        return true;
    }

    state decode_key(std::string *accum) {
        if ( !m_key_has_esc || m_key_len == 0 ) { return state::ok; }
        if ( m_key_in_accum ) {
            const auto new_len = details::unescape_range(&(*accum)[m_key_off], m_key_len, m_allow_lone_surrogates, m_allow_invalid_escape);
            if ( new_len == std::size_t(-1) ) { return state::invalid; }
            m_key_len = new_len;
        } else {
            if ( !accum ) { return state::no_buffer; }
            const auto off = accum->size();
            accum->resize(off + m_key_len);
            const auto new_len = details::unescape_range(m_key_inline_ptr, m_key_len, &(*accum)[off], m_allow_lone_surrogates, m_allow_invalid_escape);
            if ( new_len == std::size_t(-1) ) { accum->resize(off); return state::invalid; }
            accum->resize(off + new_len);
            m_key_in_accum = true;
            m_key_off = off;
            m_key_len = new_len;
        }
        m_key_has_esc = false;
        return state::ok;
    }

    state decode_value(std::string *accum, string_view_t &val) {
        if ( m_accumulating ) {
            const auto new_len = details::unescape_range(
                &(*accum)[m_value_off], accum->size() - m_value_off, m_allow_lone_surrogates, m_allow_invalid_escape
            );
            if ( new_len == std::size_t(-1) ) { return state::invalid; }
            accum->resize(m_value_off + new_len);
            val = string_view_t{accum->data() + m_value_off, new_len};
            return state::ok;
        }
        if ( !accum ) { return state::no_buffer; }
        const auto off = accum->size();
        accum->resize(off + val.size());
        const auto new_len = details::unescape_range(val.data(), val.size(), &(*accum)[off], m_allow_lone_surrogates, m_allow_invalid_escape);
        if ( new_len == std::size_t(-1) ) { accum->resize(off); return state::invalid; }
        accum->resize(off + new_len);
        val = string_view_t{accum->data() + off, new_len};
        return state::ok;
    }

    void skv_reset() noexcept {
        m_skv_sp = -1;
        m_skv_str_mode = 0;
        m_skv_u4_remain = 0;
        m_skv_lit_src = nullptr;
        m_skv_lit_i = 0;
        m_skv_lit_n = 0;
        m_skv_num = false;
        m_skv_after_str = 0;
    }

    state skv_push_obj_frame() noexcept {
        if ( static_cast<std::size_t>(m_skv_sp + 1) >= MaxNesting ) { return state::invalid; }
        ++m_skv_sp;
        m_skv_kind[m_skv_sp] = 0;
        m_skv_ph[m_skv_sp] = skv_phase::obj_expect_key_or_close;
        return state::ok;
    }

    state skv_push_arr_frame() noexcept {
        if ( static_cast<std::size_t>(m_skv_sp + 1) >= MaxNesting ) { return state::invalid; }
        ++m_skv_sp;
        m_skv_kind[m_skv_sp] = 1;
        m_skv_ph[m_skv_sp] = skv_phase::arr_expect_value_or_close;
        return state::ok;
    }

    void skv_schedule_literal(const char *lit, std::size_t n, std::size_t start_pos) noexcept {
        m_skv_lit_src = lit;
        m_skv_lit_n = n;
        m_skv_lit_i = start_pos;
    }

    state skv_consume_literal(const char *&p, const char *end) {
        while ( m_skv_lit_i < m_skv_lit_n && p < end ) {
            if ( *p++ != m_skv_lit_src[m_skv_lit_i] ) { return state::invalid; }
            ++m_skv_lit_i;
        }
        if ( m_skv_lit_i < m_skv_lit_n ) { return state::incomplete; }
        m_skv_lit_n = 0;
        m_skv_lit_src = nullptr;
        return state::ok;
    }

    bool skv_pop_container() noexcept {
        if ( m_skv_sp < 0 ) { return true; }
        --m_skv_sp;
        if ( m_skv_sp < 0 ) { return true; }
        m_skv_ph[m_skv_sp] = (m_skv_kind[m_skv_sp] == 0)
            ? skv_phase::obj_expect_comma_or_close
            : skv_phase::arr_expect_comma_or_close
        ;
        return false;
    }

    void skv_apply_after_str_code(std::uint8_t code) {
        if ( code == 4 ) { return; }
        if ( m_skv_sp < 0 ) { return; }
        if ( code == 1 ) {
            m_skv_ph[m_skv_sp] = skv_phase::obj_expect_colon;
            return;
        }
        if ( code == 2 ) {
            m_skv_ph[m_skv_sp] = skv_phase::obj_expect_comma_or_close;
            return;
        }
        if ( code == 3 ) {
            m_skv_ph[m_skv_sp] = skv_phase::arr_expect_comma_or_close;
        }
    }

    state skv_string_advance(const char *&p, const char *end) {
        for ( ;; ) {
            if ( m_skv_str_mode == 1 ) {
                while ( p < end ) {
                    const auto n = static_cast<std::size_t>(end - p);
                    const auto *q = static_cast<const char *>(::memchr(p, '"',  n));
                    const auto *r = static_cast<const char *>(::memchr(p, '\\', n));
                    if ( !q && !r ) { p = end; break; }
                    if ( !r || (q && q < r) ) {
                        p = q + 1;
                        m_skv_str_mode = 0;
                        return state::ok;
                    }
                    p = r + 1;
                    m_skv_str_mode = 2;
                    break;
                }
                if ( m_skv_str_mode == 1 ) { return state::incomplete; }
            }
            if ( m_skv_str_mode == 2 ) {
                if ( p >= end ) { return state::incomplete; }
                const char e = *p++;
                e == 'u'
                    ? (m_skv_str_mode = 3, m_skv_u4_remain = 4)
                    : (m_skv_str_mode = 1)
                ;
                continue;
            }
            if ( m_skv_str_mode == 3 ) {
                while ( p < end && m_skv_u4_remain > 0 ) {
                    ++p;
                    --m_skv_u4_remain;
                }
                if ( m_skv_u4_remain > 0 ) { return state::incomplete; }
                if ( (end - p) >= 6 && p[0] == '\\' && p[1] == 'u' ) {
                    std::uint32_t lo = 0;
                    if ( details::hex4(p + 2, lo) && lo >= 0xDC00u && lo <= 0xDFFFu ) {
                        p += 6;
                    }
                }
                m_skv_str_mode = 1;
                continue;
            }
            break;
        }
        return state::ok;
    }

    state skv_read_value_token(const char *&p, const char *end, std::uint8_t str_code, flags fl) {
        const bool allow_cmt_local = details::has_flag(fl, flags::allow_comments);
        for ( ;; ) {
            while ( p < end && details::is_ws(*p) ) ++p;
            if ( p >= end ) { return state::incomplete; }
            if ( !allow_cmt_local || *p != '/' ) { break; }
            if ( p + 1 >= end ) { m_skv_comment = 1; ++p; return state::incomplete; }
            if ( *(p+1) != '/' ) { break; }
            p += 2;
            if ( details::skip_line_comment(p, end) ) { continue; }
            m_skv_comment = 2; return state::incomplete;
        }
        const char c = *p;
        if ( c == '"' ) {
            ++p;
            m_skv_str_mode = 1;
            m_skv_after_str = str_code;
            return state::ok;
        }
        if ( c == '{' ) {
            ++p;
            return skv_push_obj_frame();
        }
        if ( c == '[' ) {
            ++p;
            return skv_push_arr_frame();
        }
        if ( c == 't' ) {
            ++p;
            skv_schedule_literal("true", 4, 1);
            return state::ok;
        }
        if ( c == 'f' ) {
            ++p;
            skv_schedule_literal("false", 5, 1);
            return state::ok;
        }
        if ( c == 'n' ) {
            ++p;
            skv_schedule_literal("null", 4, 1);
            return state::ok;
        }
        if ( details::has_flag(fl, flags::allow_infinity_and_nan) ) {
            if ( c == 'N' ) { ++p; skv_schedule_literal("NaN", 3, 1); return state::ok; }
            if ( c == 'I' ) { ++p; skv_schedule_literal("Infinity", 8, 1); return state::ok; }
            if ( c == '-' && p + 1 < end && *(p+1) == 'I' ) {
                ++p; skv_schedule_literal("-Infinity", 9, 1); return state::ok;
            }
        }
        if ( c == '-' || (c >= '0' && c <= '9') ) {
            m_skv_num = true;
            return state::ok;
        }
        return state::invalid;
    }

    template<bool InSource>
    state skip_discard_json_value(const char *&p, const char *end, std::string *accum, flags fl) {
        (void)InSource;
        (void)accum;
        static constexpr std::uint8_t skv_str_obj_key = 1;
        static constexpr std::uint8_t skv_str_obj_val = 2;
        static constexpr std::uint8_t skv_str_arr_val = 3;
        static constexpr std::uint8_t skv_str_root = 4;
        const bool allow_cmt = details::has_flag(fl, flags::allow_comments);
        const bool allow_inf_nan = details::has_flag(fl, flags::allow_infinity_and_nan);

        auto skip_ws = [&]() -> bool {
            for ( ;; ) {
                while ( p < end && details::is_ws(*p) ) ++p;
                if ( p >= end ) { return false; }
                if ( !allow_cmt || *p != '/' ) { return true; }
                if ( p + 1 >= end ) { m_skv_comment = 1; ++p; return false; }
                if ( *(p+1) != '/' ) { return true; }
                p += 2;
                if ( details::skip_line_comment(p, end) ) { continue; }
                m_skv_comment = 2; return false;
            }
        };

        for ( ;; ) {
            if ( m_skv_comment != 0 ) {
                if ( m_skv_comment == 1 ) {
                    if ( p >= end ) { return state::incomplete; }
                    if ( *p != '/' ) { return state::invalid; }
                    ++p;
                    m_skv_comment = 2;
                }
                if ( details::skip_line_comment(p, end) ) { m_skv_comment = 0; }
                else { return state::incomplete; }
                continue;
            }
            if ( m_skv_str_mode != 0 ) {
                const auto s = skv_string_advance(p, end);
                if ( s != state::ok ) { return s; }
                const std::uint8_t acode = m_skv_after_str;
                m_skv_after_str = 0;
                if ( acode == skv_str_root ) { return state::ok; }
                skv_apply_after_str_code(acode);
                continue;
            }
            if ( m_skv_lit_n != 0 ) {
                const auto s = skv_consume_literal(p, end);
                if ( s != state::ok ) { return s; }
                if ( m_skv_sp < 0 ) { return state::ok; }
                m_skv_ph[m_skv_sp] = (m_skv_kind[m_skv_sp] == 0)
                    ? skv_phase::obj_expect_comma_or_close
                    : skv_phase::arr_expect_comma_or_close
                ;
                continue;
            }
            if ( m_skv_num ) {
                while ( p < end && details::is_num_char(*p) ) { ++p; }
                if ( p >= end ) { return state::incomplete; }
                if ( allow_inf_nan && *p == 'I' ) {
                    skv_schedule_literal("Infinity", 8, 0);
                    m_skv_num = false;
                    continue;
                }
                m_skv_num = false;
                if ( m_skv_sp < 0 ) { return state::ok; }
                m_skv_ph[m_skv_sp] = (m_skv_kind[m_skv_sp] == 0)
                    ? skv_phase::obj_expect_comma_or_close
                    : skv_phase::arr_expect_comma_or_close
                ;
                continue;
            }
            if ( m_skv_sp < 0 ) {
                if ( !skip_ws() ) { return state::incomplete; }
                const char c = *p;
                if ( c == '"' ) {
                    ++p;
                    m_skv_str_mode = 1;
                    m_skv_after_str = skv_str_root;
                    continue;
                }
                if ( c == '{' ) {
                    ++p;
                    const auto s = skv_push_obj_frame();
                    if ( s != state::ok ) { return s; }
                    continue;
                }
                if ( c == '[' ) {
                    ++p;
                    const auto s = skv_push_arr_frame();
                    if ( s != state::ok ) { return s; }
                    continue;
                }
                if ( c == 't' ) {
                    ++p;
                    skv_schedule_literal("true", 4, 1);
                    continue;
                }
                if ( c == 'f' ) {
                    ++p;
                    skv_schedule_literal("false", 5, 1);
                    continue;
                }
                if ( c == 'n' ) {
                    ++p;
                    skv_schedule_literal("null", 4, 1);
                    continue;
                }
                if ( allow_inf_nan ) {
                    if ( c == 'N' ) { ++p; skv_schedule_literal("NaN", 3, 1); continue; }
                    if ( c == 'I' ) { ++p; skv_schedule_literal("Infinity", 8, 1); continue; }
                    if ( c == '-' && p + 1 < end && *(p+1) == 'I' ) {
                        ++p; skv_schedule_literal("-Infinity", 9, 1); continue;
                    }
                }
                if ( c == '-' || (c >= '0' && c <= '9') ) {
                    m_skv_num = true;
                    continue;
                }
                return state::invalid;
            }
            const std::uint8_t kind = m_skv_kind[m_skv_sp];
            const skv_phase ph = m_skv_ph[m_skv_sp];
            if ( kind == 0 ) {
                if ( ph == skv_phase::obj_expect_key_or_close ) {
                    if ( !skip_ws() ) { return state::incomplete; }
                    if ( *p == '}' ) {
                        ++p;
                        if ( skv_pop_container() ) { return state::ok; }
                        continue;
                    }
                    if ( *p != '"' ) { return state::invalid; }
                    ++p;
                    m_skv_str_mode = 1;
                    m_skv_after_str = skv_str_obj_key;
                    continue;
                }
                if ( ph == skv_phase::obj_expect_colon ) {
                    if ( !skip_ws() ) { return state::incomplete; }
                    if ( *p != ':' ) { return state::invalid; }
                    ++p;
                    m_skv_ph[m_skv_sp] = skv_phase::obj_expect_value;
                    continue;
                }
                if ( ph == skv_phase::obj_expect_value ) {
                    const int sp_before = m_skv_sp;
                    const auto s = skv_read_value_token(p, end, skv_str_obj_val, fl);
                    if ( s != state::ok ) { return s; }
                    if ( m_skv_str_mode != 0 ) { continue; }
                    if ( m_skv_lit_n != 0 ) { continue; }
                    if ( m_skv_num ) { continue; }
                    if ( m_skv_sp > sp_before ) { continue; }
                    m_skv_ph[m_skv_sp] = skv_phase::obj_expect_comma_or_close;
                    continue;
                }
                if ( ph == skv_phase::obj_expect_comma_or_close ) {
                    if ( !skip_ws() ) { return state::incomplete; }
                    if ( *p == ',' ) {
                        ++p;
                        m_skv_ph[m_skv_sp] = skv_phase::obj_expect_key_or_close;
                        continue;
                    }
                    if ( *p == '}' ) {
                        ++p;
                        if ( skv_pop_container() ) { return state::ok; }
                        continue;
                    }
                    return state::invalid;
                }
            } else {
                if ( ph == skv_phase::arr_expect_value_or_close ) {
                    if ( !skip_ws() ) { return state::incomplete; }
                    if ( *p == ']' ) {
                        ++p;
                        if ( skv_pop_container() ) { return state::ok; }
                        continue;
                    }
                    const int sp_before = m_skv_sp;
                    const auto s = skv_read_value_token(p, end, skv_str_arr_val, fl);
                    if ( s != state::ok ) { return s; }
                    if ( m_skv_str_mode != 0 ) { continue; }
                    if ( m_skv_lit_n != 0 ) { continue; }
                    if ( m_skv_num ) { continue; }
                    if ( m_skv_sp > sp_before ) { continue; }
                    m_skv_ph[m_skv_sp] = skv_phase::arr_expect_comma_or_close;
                    continue;
                }
                if ( ph == skv_phase::arr_expect_comma_or_close ) {
                    if ( !skip_ws() ) { return state::incomplete; }
                    if ( *p == ',' ) {
                        ++p;
                        m_skv_ph[m_skv_sp] = skv_phase::arr_expect_value_or_close;
                        continue;
                    }
                    if ( *p == ']' ) {
                        ++p;
                        if ( skv_pop_container() ) { return state::ok; }
                        continue;
                    }
                    return state::invalid;
                }
            }
        }
    }

    state apply_int(string_view_t val, std::string *accum, value_kind kind = value_kind::number) {
        const auto top = stack_top();
        const bool key_is_sv = top.map_setter && top.map_setter->map_key_is_string_view();
        if ( m_key_has_esc && !key_is_sv ) {
            const auto s = decode_key(accum);
            if ( s != state::ok ) { return s; }
        }
        const auto key_sv = current_key(accum);
        if ( top.holder != nullptr ) {
            const auto *setter = top.holder->get(key_sv);
            if ( !setter ) { return state::invalid; }
            if ( !setter->set_leaf(top.addr, val, kind) ) { return state::invalid; }
        }
        else if ( top.map_setter ) {
            if ( !top.map_setter->set_map_leaf(top.addr, key_sv, val, kind) ) { return state::invalid; }
        }
        else {
            if ( !top.arr_setter->set_leaf(top.addr, val, kind) ) { return state::invalid; }
        }
        reset_key();
        return state::ok;
    }

    state apply_null(std::string *accum) {
        if ( m_stack_idx < 0 ) { reset_key(); return state::ok; }
        const auto top = stack_top();
        const bool key_is_sv = top.map_setter && top.map_setter->map_key_is_string_view();
        if ( m_key_has_esc && !key_is_sv ) {
            const auto s = decode_key(accum);
            if ( s != state::ok ) { return s; }
        }
        if ( top.holder != nullptr ) {
            const auto key_sv = current_key(accum);
            const auto *setter = top.holder->get(key_sv);
            if ( !setter ) { return state::invalid; }
            setter->set_null_leaf(top.addr);
        } else if ( top.map_setter ) {
            top.map_setter->set_map_null_leaf(top.addr, current_key(accum));
        } else {
            top.arr_setter->set_null_leaf(top.addr);
        }
        reset_key();
        return state::ok;
    }

    state apply_str(string_view_t val, bool has_escapes, std::string *accum) {
        const auto top = stack_top();
        if ( top.holder != nullptr ) {
            if ( m_key_has_esc ) {
                const auto s = decode_key(accum);
                if ( s != state::ok ) { return s; }
            }
            const auto *setter = top.holder->get(current_key(accum));
            if ( !setter ) { return state::invalid; }
            if ( m_accumulating && (setter->is_string_view() || setter->is_value_t()) ) {
                return state::sv_cross_chunk;
            }
            if ( has_escapes && (setter->is_value_t() || (setter->is_string() && !setter->is_string_view())) ) {
                const auto s = decode_value(accum, val);
                if ( s != state::ok ) { return s; }
            }
            if ( !setter->set_leaf(top.addr, val, value_kind::string) ) { return state::invalid; }
        } else if ( top.map_setter ) {
            if ( m_accumulating && (top.map_setter->map_value_is_string_view()
                    || top.map_setter->map_value_is_value_t()) )
            { return state::sv_cross_chunk; }
            if ( has_escapes && (!top.map_setter->map_value_is_string_view()
                    || top.map_setter->map_value_is_value_t()) )
            {
                const auto s = decode_value(accum, val);
                if ( s != state::ok ) { return s; }
            }
            if ( m_key_has_esc && !top.map_setter->map_key_is_string_view() ) {
                const auto s = decode_key(accum);
                if ( s != state::ok ) { return s; }
            }
            if ( !top.map_setter->set_map_leaf(top.addr, current_key(accum), val, value_kind::string) ) {
                return state::invalid;
            }
        } else {
            if ( m_accumulating && (top.arr_setter->element_is_string_view()
                    || top.arr_setter->element_is_value_t()) )
            { return state::sv_cross_chunk; }
            if ( has_escapes && (!top.arr_setter->element_is_string_view()
                    || top.arr_setter->element_is_value_t()) )
            {
                const auto s = decode_value(accum, val);
                if ( s != state::ok ) { return s; }
            }
            if ( !top.arr_setter->set_leaf(top.addr, val, value_kind::string) ) { return state::invalid; }
        }
        reset_key();
        return state::ok;
    }

    state enter_object(std::string *accum) {
        if ( m_stack_idx < 0 ) {
            enter_object_root_push(bool_constant<has_metadata<C>::value>{});
        } else if ( m_key_len > 0 ) {
            const auto top = stack_top();
            if ( m_key_has_esc ) {
                const auto s = decode_key(accum);
                if ( s != state::ok ) { return s; }
            }
            const auto key_sv = current_key(accum);
            if ( top.map_setter != nullptr ) {
                void *addr = top.map_setter->prepare_map_value(top.addr, key_sv);
                if ( !addr ) { return state::invalid; }
                const auto *meta = top.map_setter->get_map_value_metadata();
                if ( meta ) {
                    if ( !stack_push({meta, nullptr, nullptr, addr}) ) { return state::invalid; }
                } else {
                    const auto *nested = top.map_setter->get_nested_map_root_setter();
                    if ( !nested ) { return state::invalid; }
                    if ( !stack_push({nullptr, nullptr, nested, addr}) ) { return state::invalid; }
                }
                reset_key();
            } else {
                const auto *setter = top.holder->get(key_sv);
                if ( !setter ) { return state::invalid; }
                if ( setter->is_object() && !setter->has_metadata() ) {
                    if ( !stack_push({nullptr, nullptr, setter, top.addr}) ) { return state::invalid; }
                } else {
                    if ( !setter->has_metadata() ) { return state::invalid; }
                    const auto *object = setter->get_metadata();
                    void *addr = setter->member_address(top.addr);
                    if ( !stack_push({object, nullptr, nullptr, addr}) ) { return state::invalid; }
                }
                reset_key();
            }
        } else {
            const auto top = stack_top();
            if ( !top.arr_setter ) { return state::invalid; }
            void *elem = top.arr_setter->prepare(top.addr);
            if ( !elem ) { return state::invalid; }
            const auto *meta = top.arr_setter->get_element_metadata();
            if ( meta ) {
                if ( !stack_push({meta, nullptr, nullptr, elem}) ) { return state::invalid; }
            } else {
                const auto *nested = top.arr_setter->get_nested_array_root_setter();
                if ( !nested ) { return state::invalid; }
                if ( !stack_push({nullptr, nested, nullptr, elem}) ) { return state::invalid; }
            }
        }
        return state::ok;
    }

    state enter_array(std::string *accum) {
        if ( m_stack_idx < 0 )
        { enter_array_root_push(bool_constant<has_metadata<C>::value>{}); }
        else if ( m_key_len > 0 ) {
            const auto top = stack_top();
            if ( m_key_has_esc ) {
                const auto s = decode_key(accum);
                if ( s != state::ok ) { return s; }
            }
            if ( top.map_setter != nullptr ) {
                const auto key_sv = current_key(accum);
                void *arr_addr = top.map_setter->prepare_map_value(top.addr, key_sv);
                if ( !arr_addr ) { return state::invalid; }
                const auto *arr_root = top.map_setter->get_map_value_array_root_setter();
                if ( !arr_root ) { return state::invalid; }
                if ( !stack_push({nullptr, arr_root, nullptr, arr_addr}) ) { return state::invalid; }
                reset_key();
                return state::ok;
            }
            const auto *setter = top.holder->get(current_key(accum));
            if ( !setter || !setter->is_array() ) { return state::invalid; }
            if ( !stack_push({nullptr, setter, nullptr, top.addr}) ) { return state::invalid; }
            reset_key();
        } else {
            const auto top = stack_top();
            if ( !top.arr_setter ) { return state::invalid; }
            void *elem = top.arr_setter->prepare(top.addr);
            if ( !elem ) { return state::invalid; }
            const auto *meta = top.arr_setter->get_element_metadata();
            if ( meta ) {
                if ( !stack_push({meta, nullptr, nullptr, elem}) ) { return state::invalid; }
            } else {
                const auto *nested = top.arr_setter->get_nested_array_root_setter();
                if ( !nested ) { return state::invalid; }
                if ( !stack_push({nullptr, nested, nullptr, elem}) ) { return state::invalid; }
            }
        }
        return state::ok;
    }

    state colon_value_begin(std::string *accum, flags fl) {
        m_skip_next_value = false;
        if ( m_stack_idx < 0 ) { return state::ok; }
        const auto top = stack_top();
        if ( top.map_setter != nullptr ) { return state::ok; }
        if ( top.holder == nullptr ) { return state::ok; }
        if ( m_key_len == 0 ) { return state::ok; }
        if ( m_key_has_esc ) {
            const auto d = decode_key(accum);
            if ( d != state::ok ) { return d; }
        }
        if ( top.holder->get(current_key(accum)) != nullptr ) { return state::ok; }
        if ( !details::has_flag(fl, flags::skip_unknown_keys) ) {
            return state::unknown_key;
        }
        m_skip_next_value = true;
        skv_reset();
        return state::ok;
    }

    state emit_number(std::string *accum) {
        string_view_t sv;
        if ( m_accumulating ) {
            if ( !accum ) { return state::no_buffer; }
            sv = string_view_t{accum->data() + m_value_off, accum->size() - m_value_off};
        } else
        { sv = string_view_t{m_seg_start, m_str_len}; }
        const auto s = apply_int(sv, accum);
        if ( s != state::ok ) { return s; }
        after_value();
        return state::ok;
    }

    template<bool InSource>
    state emit_string(std::string *accum) {
        string_view_t sv;
        sv = (InSource || !m_accumulating)
            ? string_view_t{m_seg_start, m_str_len}
            : (m_is_key
                ? string_view_t{*accum}
                : string_view_t{accum->data() + m_value_off, accum->size() - m_value_off})
        ;
        if ( !m_is_key ) {
            const bool has_esc = !InSource && m_has_esc;
            const auto s = apply_str(sv, has_esc, accum);
            if ( s != state::ok ) { return s; }
            after_value();
        } else {
            if ( !InSource ) {
                if ( m_accumulating ) {
                    const auto top = stack_top();
                    if ( top.map_setter && top.map_setter->map_key_is_string_view() ) {
                        return state::invalid;
                    }
                    m_key_in_accum = true;
                    m_key_off = 0;
                    m_key_len = sv.size();
                    m_key_inline_ptr = nullptr;
                } else {
                    m_key_in_accum = false;
                    m_key_inline_ptr = m_seg_start;
                    m_key_off = 0;
                    m_key_len = m_str_len;
                }
                m_key_has_esc = m_has_esc;
            } else {
                m_key_in_accum = false;
                m_key_inline_ptr = m_seg_start;
                m_key_off = 0;
                m_key_len = m_str_len;
                m_key_has_esc = false;
            }
            m_js = json_state::colon;
        }
        return state::ok;
    }

    template<bool InSource>
    state parse_impl(const char *p, const char *const end, std::string *accum, flags fl, char *write_base, const char **next_out = nullptr) {
        if ( m_accumulating && !accum ) { return state::no_buffer; }

        if ( m_js == json_state::in_string || m_js == json_state::in_number || m_js == json_state::in_literal )
        { m_seg_start = p; }

        const bool allow_cmt = details::has_flag(fl, flags::allow_comments);
        const bool allow_inf_nan = details::has_flag(fl, flags::allow_infinity_and_nan);
        const bool allow_invalid_utf8 = details::has_flag(fl, flags::allow_invalid_utf8);
        m_allow_lone_surrogates  = details::has_flag(fl, flags::allow_invalid_utf16);
        m_allow_invalid_escape   = details::has_flag(fl, flags::allow_invalid_escapes);

        while ( p < end ) {
            switch ( m_js ) {
                case json_state::value:
                case json_state::value_or_arr_end:
                case json_state::key_or_obj_end:
                case json_state::colon:
                case json_state::comma_or_obj_end:
                case json_state::comma_or_arr_end: {
                    for ( ;; ) {
                        while ( p < end && details::is_ws(*p) ) ++p;
                        if ( p >= end ) { return state::incomplete; }
                        if ( !allow_cmt || *p != '/' ) { break; }
                        if ( p + 1 >= end ) {
                            m_comment_ret_js = m_js;
                            m_js = json_state::in_comment_slash;
                            return state::incomplete;
                        }
                        if ( *(p+1) != '/' ) { break; }
                        p += 2;
                        if ( details::skip_line_comment(p, end) ) { continue; }
                        m_comment_ret_js = m_js;
                        m_js = json_state::in_line_comment;
                        return state::incomplete;
                    }
                    if ( m_skip_next_value ) {
                        if ( m_js != json_state::value ) { return state::invalid; }
                        const auto sk = skip_discard_json_value<InSource>(p, end, accum, fl);
                        if ( sk != state::ok ) { return sk; }
                        m_skip_next_value = false;
                        skv_reset();
                        reset_key();
                        after_value();
                        break;
                    }
                    const char c = *p;
                    if ( m_js == json_state::colon ) {
                        if ( c != ':' ) { return state::invalid; }
                        ++p;
                        const auto cs = colon_value_begin(accum, fl);
                        if ( cs != state::ok ) { return cs; }
                        m_js = json_state::value;
                        break;
                    }
                    if ( m_js == json_state::comma_or_obj_end ) {
                        if ( c == ',' ) { ++p; m_js = json_state::key_or_obj_end; break; }
                        if ( c == '}' ) { ++p; stack_pop(); ctx_pop(); after_value(); break; }
                        return state::invalid;
                    }
                    if ( m_js == json_state::comma_or_arr_end ) {
                        if ( c == ',' ) { ++p; m_js = json_state::value; break; }
                        if ( c == ']' ) { ++p; stack_pop(); ctx_pop(); after_value(); break; }
                        return state::invalid;
                    }
                    if ( m_js == json_state::key_or_obj_end ) {
                        if ( c == '}' ) { ++p; stack_pop(); ctx_pop(); after_value(); break; }
                        if ( c != '"' ) { return state::invalid; }
                        m_is_key = true;
                        m_has_esc = false;
                        m_accumulating = false;
                        m_utf8_remain = 0;
                        m_seg_start = ++p;
                        m_js = json_state::in_string;
                        break;
                    }
                    if ( m_js == json_state::value_or_arr_end && c == ']' ) {
                        ++p;
                        stack_pop();
                        ctx_pop();
                        after_value();
                        break;
                    }
                    switch ( c ) {
                        case '{': {
                            ++p;
                            const auto s = enter_object(accum);
                            if ( s != state::ok ) { return s; }
                            if ( !ctx_push(json_context::object) ) { return state::invalid; }
                            m_js = json_state::key_or_obj_end;
                            break;
                        }
                        case '[': {
                            ++p;
                            const auto s = enter_array(accum);
                            if ( s != state::ok ) { return s; }
                            if ( !ctx_push(json_context::array) ) { return state::invalid; }
                            m_js = json_state::value_or_arr_end;
                            break;
                        }
                        case '"':
                            m_is_key = false;
                            m_has_esc = false;
                            m_accumulating = false;
                            m_utf8_remain = 0;
                            m_seg_start = ++p;
                            m_js = json_state::in_string;
                            break;
                        case 't':
                            m_lit_str = "true";
                            m_lit_len = 4;
                            m_lit_pos = 1;
                            ++p;
                            m_js = json_state::in_literal;
                            break;
                        case 'f':
                            m_lit_str = "false";
                            m_lit_len = 5;
                            m_lit_pos = 1;
                            ++p;
                            m_js = json_state::in_literal;
                            break;
                        case 'n':
                            m_lit_str = "null";
                            m_lit_len = 4;
                            m_lit_pos = 1;
                            ++p;
                            m_js = json_state::in_literal;
                            break;
                        default:
                            if ( allow_inf_nan ) {
                                if ( c == 'N' ) {
                                    m_lit_str = "NaN"; m_lit_len = 3; m_lit_pos = 1; ++p;
                                    m_js = json_state::in_literal; break;
                                }
                                if ( c == 'I' ) {
                                    m_lit_str = "Infinity"; m_lit_len = 8; m_lit_pos = 1; ++p;
                                    m_js = json_state::in_literal; break;
                                }
                                if ( c == '-' && p + 1 < end && *(p+1) == 'I' ) {
                                    m_lit_str = "-Infinity"; m_lit_len = 9; m_lit_pos = 1; ++p;
                                    m_js = json_state::in_literal; break;
                                }
                            }
                            if ( c == '-' || (c >= '0' && c <= '9') ) {
                                m_accumulating = false;
                                m_seg_start = p;
                                m_js = json_state::in_number;
                                break;
                            }
                            return state::invalid;
                    }
                    break;
                }
                case json_state::in_string: {
                    if ( InSource ) {
                        char *dst = write_base + (m_seg_start - static_cast<const char*>(write_base));
                        while ( p < end ) {
                            const char c = *p;
                            if ( c == '"' ) {
                                if ( !allow_invalid_utf8 && m_utf8_remain != 0 ) { return state::invalid; }
                                m_str_len = static_cast<std::size_t>(dst - m_seg_start);
                                ++p;
                                const auto s = emit_string<true>(nullptr);
                                if ( s != state::ok ) { return s; }
                                goto next_token;
                            }
                            if ( c == '\\' ) {
                                if ( !allow_invalid_utf8 && m_utf8_remain != 0 ) { return state::invalid; }
                                ++p;
                                if ( p >= end ) {
                                    m_js = json_state::in_escape;
                                    break;
                                }
                                if ( !details::decode_escape_into(p, end, dst, m_allow_lone_surrogates, m_allow_invalid_escape) ) { return state::invalid; }
                                continue;
                            }
                            if ( !allow_invalid_utf8 ) {
                                const auto b = static_cast<unsigned char>(c);
                                if ( b >= 0x80u ) {
                                    if ( m_utf8_remain > 0u ) {
                                        if ( (b & 0xC0u) != 0x80u ) { return state::invalid; }
                                        --m_utf8_remain;
                                    } else {
                                        if ( b < 0xC2u || b > 0xF4u ) { return state::invalid; }
                                        m_utf8_remain = (b < 0xE0u) ? 1u : (b < 0xF0u) ? 2u : 3u;
                                    }
                                }
                            }
                            *dst++ = c;
                            ++p;
                        }
                        break;
                    } else {
                        while ( p < end ) {
                            const char c = *p;
                            if ( c == '"' ) {
                                if ( !allow_invalid_utf8 && m_utf8_remain != 0 ) { return state::invalid; }
                                m_str_len = static_cast<std::size_t>(p - m_seg_start);
                                if ( m_accumulating ) { accum->append(m_seg_start, p); }
                                ++p;
                                const auto s = emit_string<false>(accum);
                                if ( s != state::ok ) { return s; }
                                goto next_token;
                            }
                            if ( c == '\\' ) {
                                if ( !allow_invalid_utf8 && m_utf8_remain != 0 ) { return state::invalid; }
                                m_has_esc = true;
                                ++p;
                                if ( p >= end ) {
                                    if ( !flush_segment(accum, p) ) { return state::no_buffer; }
                                    m_js = json_state::in_escape;
                                    return sv_or_incomplete(accum);
                                }
                                if ( *p == 'u' ) {
                                    ++p;
                                    m_uni_remain = 4;
                                    while ( p < end && m_uni_remain > 0 ) { ++p; --m_uni_remain; }
                                    if ( m_uni_remain > 0 ) {
                                        if ( !flush_segment(accum, p) ) { return state::no_buffer; }
                                        m_js = json_state::in_unicode;
                                        return sv_or_incomplete(accum);
                                    }
                                    std::uint32_t cp;
                                    if ( !details::hex4(p - 4, cp) ) { return state::invalid; }
                                } else {
                                    ++p;
                                }
                                continue;
                            }
                            if ( !allow_invalid_utf8 ) {
                                const auto b = static_cast<unsigned char>(c);
                                if ( b >= 0x80u ) {
                                    if ( m_utf8_remain > 0u ) {
                                        if ( (b & 0xC0u) != 0x80u ) { return state::invalid; }
                                        --m_utf8_remain;
                                    } else {
                                        if ( b < 0xC2u || b > 0xF4u ) { return state::invalid; }
                                        m_utf8_remain = (b < 0xE0u) ? 1u : (b < 0xF0u) ? 2u : 3u;
                                    }
                                }
                            }
                            ++p;
                        }
                        if ( !flush_segment(accum, p) ) { return state::no_buffer; }
                        return sv_or_incomplete(accum);
                    }
        next_token:
                    break;
                }
                case json_state::in_escape: {
                    if ( InSource ) {
                        return state::invalid;
                    } else {
                        if ( p >= end ) { return sv_or_incomplete(accum); }
                        if ( *p == 'u' ) {
                            *accum += *p++;
                            m_uni_remain = 4;
                            m_js = json_state::in_unicode;
                        } else {
                            *accum += *p++;
                            m_seg_start = p;
                            m_js = json_state::in_string;
                        }
                    }
                    break;
                }
                case json_state::in_unicode: {
                    if ( InSource ) {
                        return state::invalid;
                    } else {
                        while ( p < end && m_uni_remain > 0 ) {
                            *accum += *p++;
                            --m_uni_remain;
                        }
                        if ( m_uni_remain == 0 ) {
                            std::uint32_t cp;
                            if ( !details::hex4(accum->data() + accum->size() - 4, cp) ) { return state::invalid; }
                            m_seg_start = p;
                            m_js = json_state::in_string;
                        }
                    }
                    break;
                }
                case json_state::in_number: {
                    while ( p < end && details::is_num_char(*p) ) ++p;
                    if ( p < end ) {
                        m_str_len = static_cast<std::size_t>(p - m_seg_start);
                        if ( allow_inf_nan && *p == 'I' ) {
                            const bool is_just_minus = !m_accumulating
                                ? (m_str_len == 1 && *m_seg_start == '-')
                                : (m_str_len == 0 && accum
                                   && accum->size() - m_value_off == 1
                                   && (*accum)[m_value_off] == '-');
                            if ( is_just_minus ) {
                                if ( m_accumulating ) {
                                    if ( !accum ) { return state::no_buffer; }
                                    accum->resize(m_value_off);
                                    m_accumulating = false;
                                }
                                m_lit_str = "-Infinity"; m_lit_len = 9; m_lit_pos = 1;
                                m_js = json_state::in_literal; break;
                            }
                        }
                        if ( !InSource ) {
                            if ( m_accumulating ) { accum->append(m_seg_start, p); }
                        }
                        const auto s = emit_number(accum);
                        if ( s != state::ok ) { return s; }
                    } else {
                        if ( InSource ) {
                            // m_js stays in_number; tail returns sv_cross_chunk.
                            break;
                        } else {
                            if ( !flush_segment(accum, p) ) { return state::no_buffer; }

                            return state::incomplete;
                        }
                    }
                    break;
                }
                case json_state::in_literal: {
                    while ( p < end && m_lit_pos < m_lit_len ) {
                        if ( *p != m_lit_str[m_lit_pos] ) { return state::invalid; }
                        ++p;
                        ++m_lit_pos;
                    }
                    if ( m_lit_pos == m_lit_len ) {
                        if ( m_lit_str[0] == 'n' ) {
                            const auto s = apply_null(accum);
                            if ( s != state::ok ) { return s; }
                        } else if ( m_lit_str[0] == 'N' || m_lit_str[0] == 'I' || m_lit_str[0] == '-' ) {
                            const auto s = apply_int({m_lit_str, m_lit_len}, accum);
                            if ( s != state::ok ) { return s; }
                        } else {
                            const auto s = apply_int(
                                 {m_lit_str, m_lit_len}
                                ,accum
                                ,value_kind::boolean
                            );
                            if ( s != state::ok ) { return s; }
                        }
                        after_value();
                    }
                    break;
                }
                case json_state::done: {
                    for ( ;; ) {
                        while ( p < end && details::is_ws(*p) ) ++p;
                        if ( p >= end ) { return state::ok; }
                        if ( allow_cmt && *p == '/' ) {
                            if ( p + 1 >= end ) {
                                m_comment_ret_js = json_state::done;
                                m_js = json_state::in_comment_slash;
                                return state::incomplete;
                            }
                            if ( *(p+1) == '/' ) {
                                p += 2;
                                if ( details::skip_line_comment(p, end) ) { continue; }
                                m_comment_ret_js = json_state::done;
                                m_js = json_state::in_line_comment;
                                return state::ok;
                            }
                        }
                        if ( next_out ) { *next_out = p; }
                        return state::record_end;
                    }
                }
                case json_state::in_comment_slash: {
                    if ( p >= end ) { return state::incomplete; }
                    if ( *p != '/' ) {
                        if ( m_comment_ret_js == json_state::done ) {
                            m_js = json_state::done;
                            if ( next_out ) { *next_out = p; }
                            return state::record_end;
                        }
                        return state::invalid;
                    }
                    ++p;
                    if ( details::skip_line_comment(p, end) ) { m_js = m_comment_ret_js; break; }
                    if ( m_comment_ret_js == json_state::done ) { m_js = m_comment_ret_js; return state::ok; }
                    m_js = json_state::in_line_comment;
                    return state::incomplete;
                }
                case json_state::in_line_comment: {
                    if ( details::skip_line_comment(p, end) ) { m_js = m_comment_ret_js; break; }
                    if ( m_comment_ret_js == json_state::done ) { m_js = json_state::done; return state::ok; }
                    return state::incomplete;
                }
            }
        }
        if ( m_js == json_state::done ) { return state::ok; }
        if ( m_js == json_state::in_line_comment && m_comment_ret_js == json_state::done ) { return state::ok; }
        if ( InSource ) {
            switch ( m_js ) {
                case json_state::in_string:
                case json_state::in_escape:
                case json_state::in_unicode:
                case json_state::in_number:
                case json_state::in_literal: return state::sv_cross_chunk;
                default: break;
            }
        } else {
            if ( m_js == json_state::in_unicode ) {
                return sv_or_incomplete(accum);
            }
        }
        return state::incomplete;
    }

public:
    parser(C *cls, std::string *accum = nullptr, flags fl = flags::none) noexcept
        :m_cls{cls}
        ,m_accum{accum}
        ,m_key_inline_ptr{}
        ,m_key_off{}
        ,m_key_len{}
        ,m_value_off{}
        ,m_seg_start{}
        ,m_str_len{}
        ,m_uni_remain{}
        ,m_lit_str{}
        ,m_lit_pos{}
        ,m_lit_len{}
        ,m_skv_lit_src{}
        ,m_skv_lit_i{}
        ,m_skv_lit_n{}
        ,m_stack{}
        ,m_stack_idx{-1}
        ,m_ctx_depth{-1}
        ,m_skv_sp{-1}
        ,m_js{json_state::value}
        ,m_comment_ret_js{json_state::value}
        ,m_key_in_accum{}
        ,m_key_has_esc{}
        ,m_has_esc{}
        ,m_is_key{}
        ,m_accumulating{}
        ,m_skip_next_value{}
        ,m_skv_num{}
        ,m_skv_str_mode{}
        ,m_skv_u4_remain{}
        ,m_skv_after_str{}
        ,m_skv_comment{}
        ,m_utf8_remain{}
        ,m_allow_lone_surrogates{}
        ,m_allow_invalid_escape{}
        ,m_flags{fl}
        ,m_ctx_buf{}
        ,m_skv_kind{}
        ,m_skv_ph{}
    {}

    void reset() noexcept {
        *this = parser{m_cls, m_accum, m_flags};
        if ( m_accum ) { m_accum->clear(); }
    }

    cursor parse(const char *ptr, std::size_t size) {
        cursor c;
        const char *const end = ptr + size;
        const char *next = end;
        c.m_st = parse_impl<false>(ptr, end, m_accum, m_flags, nullptr, &next);
        c.m_next = next;
        c.m_remaining = static_cast<std::size_t>(end - next);
        return c;
    }

    cursor parse_m(char *buf, std::size_t size) noexcept {
        cursor c;
        const char *const end = buf + size;
        const char *next = end;
        c.m_st = parse_impl<true>(buf, end, nullptr, m_flags, buf, &next);
        c.m_next = next;
        c.m_remaining = static_cast<std::size_t>(end - next);
        return c;
    }

    state parse_next(cursor *cur) {
        reset();
        const char *const end = cur->m_next + cur->m_remaining;
        const char *next = end;
        const auto st = parse_impl<false>(cur->m_next, end, m_accum, m_flags, nullptr, &next);
        cur->m_remaining = static_cast<std::size_t>(end - next);
        cur->m_next = next;
        cur->m_st = st;
        return st;
    }

    state parse_next_m(cursor *cur) noexcept {
        reset();
        char *base = const_cast<char *>(cur->m_next);
        const char *const end = cur->m_next + cur->m_remaining;
        const char *next = end;
        const auto st = parse_impl<true>(base, end, nullptr, m_flags, base, &next);
        cur->m_remaining = static_cast<std::size_t>(end - next);
        cur->m_next = next;
        cur->m_st = st;
        return st;
    }
};

template<typename C, int MaxNesting = static_cast<int>(stack_depth<C>()) + 8>
auto make_parser(C *cls, std::string *accum = nullptr, flags fl = flags::none) noexcept
{ return parser<C, MaxNesting>{cls, accum, fl}; }

/*************************************************************************************************/

template<typename T>
std::size_t required_bytes(const T &obj, serialize_flags fl = serialize_flags::none) noexcept {
    static_assert(
         has_metadata<T>::value || details::is_object_or_array<T>::value
        ,"T must have JSONREFL_METADATA or be a standard container"
    );
    return details::required_bytes(obj, fl, std::size_t{0});
}

template<typename T>
char* to_buffer(char *ptr, const T &obj, serialize_flags fl = serialize_flags::none) noexcept {
    static_assert(
         has_metadata<T>::value || details::is_object_or_array<T>::value
        ,"T must have JSONREFL_METADATA or be a standard container"
    );
    return details::write_json(ptr, obj, fl, std::size_t{0});
}

template<typename T>
std::string to_string(const T &obj, serialize_flags fl = serialize_flags::none) {
    static_assert(
         has_metadata<T>::value || details::is_object_or_array<T>::value
        ,"T must have JSONREFL_METADATA or be a standard container"
    );
    const auto n = required_bytes(obj, fl);
    std::string result(n, '\0');
    if ( n != 0u ) {
        const auto end = to_buffer(&result[0], obj, fl);
        result.resize(static_cast<std::size_t>(end - &result[0]));
    }
    return result;
}

template<typename T, typename Fn, typename AllocFn = void*(*)(std::size_t)>
bool to_chunked_buffer(
     char *buf
    ,std::size_t buf_size
    ,const T &obj
    ,Fn &&fn
    ,serialize_flags fl = serialize_flags::none
    ,AllocFn alloc_fn = &std::malloc
    ,bool owned = false
) {
    static_assert(
         has_metadata<T>::value || details::is_object_or_array<T>::value
        ,"T must have JSONREFL_METADATA or be a standard container"
    );
    assert((!buf == owned) && "buf==nullptr requires owned==true, and owned==true requires buf==nullptr");
    if ( owned ) {
        buf = static_cast<char *>(alloc_fn(buf_size));
        if ( !buf ) { return false; }
    }
    auto trampoline = [](void *ud, const void *data, std::size_t size) -> bool
    { return (*static_cast<std::remove_reference_t<Fn>*>(ud))(data, size); };
    details::chunked_writer w{buf, buf_size, trampoline, &fn};
    const bool streamed = details::pretty_set(fl)
        ? details::stream_json<true>(w, obj, std::size_t{0}, nullptr, 0, fl)
        : details::stream_json<false>(w, obj, std::size_t{0}, nullptr, 0, fl)
    ;
    if ( w.ok && streamed && w.pos > 0 ) { w.flush(); }
    return w.ok && streamed;
}

/*************************************************************************************************/

} // ns jsonrefl
