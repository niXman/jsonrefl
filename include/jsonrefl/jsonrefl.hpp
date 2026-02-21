
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
#include <optional>
#include <vector>
#include <list>
#include <string>
#include <utility>
#include <charconv>
#include <exception>
#include <ostream>
#include <cassert>
#include <cstdint>
#include <cstring>

/*************************************************************************************************/

#define __JSONREFL_CAT_I(a, b) a ## b
#define __JSONREFL_CAT(a, b) __JSONREFL_CAT_I(a, b)

#ifdef __GNUC__
#define __JSONREFL_UNLIKELY(...) (__builtin_expect(!!(__VA_ARGS__), 0))
#else
#define __JSONREFL_UNLIKELY(...) (__VA_ARGS__)
#endif

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

#define __JSONREFL_NARG_IMPL(...) __JSONREFL_ARG_N(__VA_ARGS__)
#define __JSONREFL_NARG(...) __JSONREFL_NARG_IMPL(__VA_ARGS__, __JSONREFL_REVERSE_SEQ_N())

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
    __JSONREFL_ENUM_ARGS_9(macro, data, _0,  _1, _2, _3, _4, _5, _6, _7, _8) macro(9, data, _9)
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

#define __JSONREFL_ENUM_ARGS_IMPL(macro, data, n, ...) \
    __JSONREFL_CAT(__JSONREFL_ENUM_ARGS_, n)(macro, data, __VA_ARGS__)

#define __JSONREFL_ENUM_ARGS(macro, data, ...) \
    __JSONREFL_ENUM_ARGS_IMPL(macro, data, __JSONREFL_NARG(__VA_ARGS__), __VA_ARGS__)

/*************************************************************************************************/

#define __JSONREFL_OBJECT_MEMBER(index, data, elem) \
    ,::jsonrefl::object_member(#elem, data elem)

#define JSONREFL_METADATA(_type, ...)                                                  \
    inline constexpr auto __jsonrefl_meta_##_type = ::jsonrefl::object_holder(      \
        #_type                                                                         \
        __JSONREFL_ENUM_ARGS(__JSONREFL_OBJECT_MEMBER, &_type::, __VA_ARGS__)          \
    );                                                                                 \
    constexpr const auto* __jsonrefl_adl_meta(_type*) noexcept {                       \
        return &__jsonrefl_meta_##_type;                                               \
    }

/*************************************************************************************************/

#define __JSONREFL_PAIR_SECOND(type, name) \
    name

#define __JSONREFL_STRUCT_FIELD(index, data, elem) \
    __JSONREFL_STRUCT_FIELD_I elem

#define __JSONREFL_STRUCT_FIELD_I(type, name) \
    type name;

#define __JSONREFL_STRUCT_META(index, data, elem) \
    ,__JSONREFL_STRUCT_META_I(data, __JSONREFL_PAIR_SECOND elem)

#define __JSONREFL_STRUCT_META_I(data, name) \
    ::jsonrefl::object_member(#name, data name)

#define JSONREFL_STRUCT(_type, ...)                                                    \
    struct _type {                                                                     \
        __JSONREFL_ENUM_ARGS(__JSONREFL_STRUCT_FIELD, ~, __VA_ARGS__)                  \
    };                                                                                 \
    inline constexpr auto __jsonrefl_meta_##_type = ::jsonrefl::object_holder(      \
        #_type                                                                         \
        __JSONREFL_ENUM_ARGS(__JSONREFL_STRUCT_META, &_type::, __VA_ARGS__)            \
    );                                                                                 \
    constexpr const auto* __jsonrefl_adl_meta(_type*) noexcept {                       \
        return &__jsonrefl_meta_##_type;                                               \
    }

/*************************************************************************************************/

namespace jsonrefl {

/*************************************************************************************************/

class exception final : public std::exception {
    const char *m_msg;
public:
    explicit exception(const char *msg) noexcept : m_msg{msg} {}
    const char* what() const noexcept override { return m_msg; }
};

/*************************************************************************************************/

namespace details {
    constexpr std::nullptr_t __jsonrefl_adl_meta(...) noexcept { return nullptr; }

    template<typename T>
    constexpr auto __jsonrefl_resolve_meta() noexcept {
        return __jsonrefl_adl_meta(static_cast<T *>(nullptr));
    }
} // ns details

template<typename T>
struct has_metadata : std::bool_constant<
    !std::is_null_pointer_v<decltype(details::__jsonrefl_resolve_meta<T>())>
> {};

template<typename T>
constexpr decltype(auto) metadata() noexcept {
    return *details::__jsonrefl_resolve_meta<T>();
}

/*************************************************************************************************/
// calculate the required maximum stack size

namespace details {

template<typename T>
constexpr T mymax(T v)
{ return v; }

template<typename T>
constexpr T mymax(T l, T r)
{ return (l > r) ? l : r; }

template<typename T, typename... Ts>
constexpr auto mymax(T a0, T a1, Ts... ts)
{ return mymax(mymax(a0, a1), ts...); }

} // ns details

template<typename T>
constexpr std::size_t stack_depth() noexcept;


namespace details {
/*************************************************************************************************/

template<typename It, typename T>
constexpr It upper_bound(It first, It last, const T &value) noexcept {
    auto count = last - first;
    while ( count > 0 ) {
        auto step = count / 2;
        auto mid = first + step;
        if ( !(value < *mid) ) {
            first = mid + 1;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

template<typename It, typename T>
constexpr It lower_bound(It first, It last, const T &value) noexcept {
    auto count = last - first;
    while ( count > 0 ) {
        auto step = count / 2;
        auto mid = first + step;
        if ( mid->first < value ) {
            first = mid + 1;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

/*************************************************************************************************/

constexpr std::uint32_t fnv1a(std::string_view str) {
    auto seed = 0x811c9dc5u;
    for ( auto it = str.cbegin(); it != str.cend(); ++it ) {
        seed = (seed ^ static_cast<std::uint32_t>(*it)) * 0x01000193u;
    }

    return seed;
}

/*************************************************************************************************/

template<typename>
struct is_string_like_t: std::false_type {};

template<>
struct is_string_like_t<std::string>: std::true_type {};

template<>
struct is_string_like_t<std::string_view>: std::true_type {};

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
struct is_object_or_array: std::bool_constant<is_object_type<T>::value || is_array_type<T>::value> {};

template<typename>
struct is_optional_type : std::false_type {};

template<typename T>
struct is_optional_type<std::optional<T>> : std::true_type {};

template<typename T>
struct unwrap_optional { using type = T; };

template<typename T>
struct unwrap_optional<std::optional<T>> { using type = T; };

template<typename T>
using unwrap_optional_t = typename unwrap_optional<T>::type;

inline constexpr std::string_view k_indent_step = "   ";

struct escape_table {
    std::uint8_t extra[256];
    char replacement[256];

    constexpr escape_table() noexcept : extra{}, replacement{} {
        extra[unsigned('"')]  = 1; replacement[unsigned('"')]  = '"';
        extra[unsigned('\\')] = 1; replacement[unsigned('\\')] = '\\';
        extra[unsigned('\n')] = 1; replacement[unsigned('\n')] = 'n';
        extra[unsigned('\r')] = 1; replacement[unsigned('\r')] = 'r';
        extra[unsigned('\t')] = 1; replacement[unsigned('\t')] = 't';
    }
};
inline constexpr escape_table k_esc{};

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
    if ( v != v ) return 3;
    std::size_t n = (v < 0.0) ? 1 : 0;
    const double av = v < 0.0 ? -v : v;
    if ( av * 0.0 != 0.0 ) return n + 3;
    const auto it = upper_bound(pow10, pow10 + sizeof(pow10)/sizeof(pow10[0]), av);
    auto int_digits = static_cast<std::size_t>(it - pow10);
    if ( int_digits == 0 ) int_digits = 1;
    return n + int_digits + 7;
}

template<typename U>
std::size_t required_bytes(const U &v, bool pretty, std::size_t indent) noexcept {
    if constexpr ( has_metadata<U>::value ) {
        return metadata<U>().required_bytes(&v, pretty, indent);
    } else if constexpr ( is_string_like_t<U>::value ) {
        std::size_t n = 2;
        for ( char c : v ) {
            n += 1 + k_esc.extra[static_cast<unsigned char>(c)];
        }
        return n;
    } else if constexpr ( std::is_same_v<U, bool> ) {
        return v ? 4 : 5;
    } else if constexpr ( std::is_integral_v<U> ) {
        if ( v == 0 ) return 1;
        std::size_t n = 0;
        if constexpr ( std::is_signed_v<U> ) {
            if ( v < 0 ) { n = 1; }
        }
        auto uv = static_cast<std::make_unsigned_t<U>>(v < 0 ? -v : v);
        while ( uv ) { ++n; uv /= 10; }
        return n;
    } else if constexpr ( std::is_floating_point_v<U> ) {
        return float_chars_count(static_cast<double>(v));
    } else if constexpr ( is_array_type<U>::value ) {
        if ( v.empty() ) return 2;
        const auto child = pretty ? indent + k_indent_step.size() : std::size_t{0};
        std::size_t n = 1;
        if ( pretty ) n += 1;
        bool first = true;
        for ( const auto &elem : v ) {
            if ( !first ) {
                n += 1;
                if ( pretty ) n += 1;
            }
            if ( pretty ) n += child;
            n += required_bytes(elem, pretty, child);
            first = false;
        }
        if ( pretty ) { n += 1; n += indent; }
        n += 1;
        return n;
    } else if constexpr ( is_object_type<U>::value ) {
        if ( v.empty() ) return 2;
        const auto child = pretty ? indent + k_indent_step.size() : std::size_t{0};
        std::size_t n = 1;
        if ( pretty ) n += 1;
        bool first = true;
        for ( const auto &[k, val] : v ) {
            if ( !first ) {
                n += 1;
                if ( pretty ) n += 1;
            }
            if ( pretty ) n += child;
            n += required_bytes(k, pretty, child);
            n += pretty ? 2 : 1;
            n += required_bytes(val, pretty, child);
            first = false;
        }
        if ( pretty ) { n += 1; n += indent; }
        n += 1;
        return n;
    } else if constexpr ( is_optional_type<U>::value ) {
        return v.has_value() ? required_bytes(*v, pretty, indent) : 4;
    } else {
        return 4;
    }
}

template<typename U>
char* write_json(char *ptr, const U &v, bool pretty, std::size_t indent) noexcept {
    if constexpr ( has_metadata<U>::value ) {
        return metadata<U>().to_buffer(&v, ptr, pretty, indent);
    } else if constexpr ( is_string_like_t<U>::value ) {
        *ptr++ = '"';
        for ( char c : v ) {
            const char r = k_esc.replacement[static_cast<unsigned char>(c)];
            if ( r ) { *ptr++ = '\\'; *ptr++ = r; }
            else { *ptr++ = c; }
        }
        *ptr++ = '"';
        return ptr;
    } else if constexpr ( std::is_same_v<U, bool> ) {
        std::memcpy(ptr, v ? "true" : "false", v ? 4 : 5);
        return ptr + (v ? 4 : 5);
    } else if constexpr ( std::is_integral_v<U> ) {
        auto res = std::to_chars(ptr, ptr + 24, v);
        return res.ptr;
    } else if constexpr ( std::is_floating_point_v<U> ) {
        auto len = float_chars_count(static_cast<double>(v));
        auto res = std::to_chars(ptr, ptr + len, static_cast<double>(v), std::chars_format::fixed, 6);
        return res.ptr;
    } else if constexpr ( is_array_type<U>::value ) {
        if ( v.empty() ) { std::memcpy(ptr, "[]", 2); return ptr + 2; }
        const auto child = pretty ? indent + k_indent_step.size() : std::size_t{0};
        *ptr++ = '[';
        if ( pretty ) *ptr++ = '\n';
        bool first = true;
        for ( const auto &elem : v ) {
            if ( !first ) {
                *ptr++ = ',';
                if ( pretty ) *ptr++ = '\n';
            }
            if ( pretty ) { std::memset(ptr, ' ', child); ptr += child; }
            ptr = write_json(ptr, elem, pretty, child);
            first = false;
        }
        if ( pretty ) {
            *ptr++ = '\n';
            std::memset(ptr, ' ', indent); ptr += indent;
        }
        *ptr++ = ']';
        return ptr;
    } else if constexpr ( is_object_type<U>::value ) {
        if ( v.empty() ) { std::memcpy(ptr, "{}", 2); return ptr + 2; }
        const auto child = pretty ? indent + k_indent_step.size() : std::size_t{0};
        *ptr++ = '{';
        if ( pretty ) *ptr++ = '\n';
        bool first = true;
        for ( const auto &[k, val] : v ) {
            if ( !first ) {
                *ptr++ = ',';
                if ( pretty ) *ptr++ = '\n';
            }
            if ( pretty ) { std::memset(ptr, ' ', child); ptr += child; }
            ptr = write_json(ptr, k, pretty, child);
            if ( pretty ) {
                *ptr++ = ':'; *ptr++ = ' ';
            } else {
                *ptr++ = ':';
            }
            ptr = write_json(ptr, val, pretty, child);
            first = false;
        }
        if ( pretty ) {
            *ptr++ = '\n';
            std::memset(ptr, ' ', indent); ptr += indent;
        }
        *ptr++ = '}';
        return ptr;
    } else if constexpr ( is_optional_type<U>::value ) {
        if ( v.has_value() ) return write_json(ptr, *v, pretty, indent);
        std::memcpy(ptr, "null", 4);
        return ptr + 4;
    } else {
        std::memcpy(ptr, "null", 4);
        return ptr + 4;
    }
}

struct chunked_writer {
    using flush_fn_t = bool(*)(void *ctx, const void *data, std::size_t size);

    char *const buf;
    const std::size_t capacity;
    std::size_t pos;
    flush_fn_t flush_fn;
    void *ctx;
    bool ok;

    chunked_writer(char *b, std::size_t cap, flush_fn_t fn, void *c) noexcept
        : buf{b}, capacity{cap}, pos{0}, flush_fn{fn}, ctx{c}, ok{true}
    {}

    bool put(char c) noexcept {
        if __JSONREFL_UNLIKELY( pos == capacity && !flush() ) return false;
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
            if __JSONREFL_UNLIKELY( pos == capacity && !flush() ) return false;
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
            if __JSONREFL_UNLIKELY( pos == capacity && !flush() ) return false;
        }
        return ok;
    }

    template<bool Pretty>
    bool write_field_key(std::string_view name, bool first, std::size_t indent) noexcept {
        const std::size_t pfx = first ? std::size_t{0} : (Pretty ? std::size_t{2} : std::size_t{1});
        const std::size_t ind = Pretty ? indent : std::size_t{0};
        const std::size_t total = pfx + ind + 1 + name.size() + (Pretty ? 3 : 2);
        if ( capacity - pos >= total ) {
            if ( !first ) {
                buf[pos++] = ',';
                if constexpr (Pretty) buf[pos++] = '\n';
            }
            if constexpr (Pretty) { std::memset(buf + pos, ' ', indent); pos += indent; }
            buf[pos++] = '"';
            std::memcpy(buf + pos, name.data(), name.size());
            pos += name.size();
            if constexpr (Pretty) {
                std::memcpy(buf + pos, "\": ", 3); pos += 3;
            } else {
                std::memcpy(buf + pos, "\":", 2); pos += 2;
            }
            return true;
        }
        if ( !first ) {
            if ( !put(',') ) return false;
            if constexpr (Pretty) { if ( !put('\n') ) return false; }
        }
        if constexpr (Pretty) { if ( !fill(' ', indent) ) return false; }
        if ( !put('"') ) return false;
        if ( !write(name.data(), name.size()) ) return false;
        if constexpr (Pretty) { return write("\": ", 3); }
        else { return write("\":", 2); }
    }

    char* reserve(std::size_t n) noexcept {
        if __JSONREFL_UNLIKELY( capacity - pos < n ) {
            if ( !flush() ) return nullptr;
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

template<bool Pretty, typename U>
bool stream_json(chunked_writer &w, const U &v, std::size_t indent) noexcept;

/*************************************************************************************************/

struct object_holder_base;

struct setter_base {
    virtual bool is_object() const noexcept = 0;
    virtual bool is_array() const noexcept = 0;
    virtual bool is_int() const noexcept = 0;
    virtual bool is_uint() const noexcept = 0;
    virtual bool is_double() const noexcept = 0;
    virtual bool is_string_like() const noexcept = 0;
    virtual bool is_string_view() const noexcept = 0;
    virtual char type_id() const noexcept {
        return is_object()
            ? 'O'
            : is_array()
                ? 'A'
                : is_int()
                    ? 'I'
                    : is_uint()
                        ? 'U'
                        : is_double()
                            ? 'F'
                            : is_string_like()
                                ? 'S'
                                : '-'
        ;
    }

    virtual bool has_metadata() const noexcept = 0;
    virtual const object_holder_base* get_metadata() const noexcept = 0;
    virtual const object_holder_base* get_element_metadata() const noexcept { return nullptr; }

    virtual void* member_address(void *obj) const noexcept = 0;
    virtual const void* member_address(const void *obj) const noexcept = 0;

    virtual void* prepare(void *obj) const = 0;
    virtual void commit(void *obj, void *elem) const noexcept = 0;
    virtual void set(void *obj, std::string_view) const = 0;
    virtual void set(void *obj, std::string_view key, std::string_view val) const = 0;
    virtual std::size_t value_required_bytes(const void *obj, bool pretty, std::size_t indent) const noexcept = 0;
    virtual char* value_to_buffer(const void *obj, char *ptr, bool pretty, std::size_t indent) const noexcept = 0;
    virtual bool value_to_stream(const void *obj, chunked_writer &w, bool pretty, std::size_t indent) const noexcept = 0;
};

template<typename C, typename T>
class setter_t final: public setter_base {
    T C::*m_ptr;

    // bool
    template<typename U, std::enable_if_t<std::is_same_v<U, bool>, bool> = true>
    static void set_impl(U &v, std::string_view str) noexcept
    { v = (str == "true" || str == "1"); }
    // integrals (excluding bool)
    template<typename U, std::enable_if_t<std::is_integral_v<U> && !std::is_same_v<U, bool>, bool> = true>
    static void set_impl(U &v, std::string_view str) noexcept {
        const bool is_hex = (str.length() > 2 && (str[0] == '0') && (str[1] == 'x' || str[1] == 'X'));
        const auto res = std::from_chars(str.begin(), str.end(), v, is_hex ? 16 : 10);
        (void)res;
        assert(res.ec == std::errc{});
    }
    // floating point
    template<typename U, std::enable_if_t<std::is_floating_point_v<U>, bool> = true>
    static void set_impl(U &v, std::string_view str) noexcept {
        const bool is_hex = (str.length() > 2 && (str[0] == '0') && (str[1] == 'x' || str[1] == 'X'));
        auto res = std::from_chars(str.begin(), str.end(), v, is_hex ? std::chars_format::hex : std::chars_format::general);
        (void)res;
        assert(res.ec == std::errc{});
    }
    // string_view
    template<typename U, std::enable_if_t<std::is_same_v<U, std::string_view>, bool> = true>
    static void set_impl(U &v, std::string_view str) noexcept { v = str; }
    // string
    template<typename U, std::enable_if_t<std::is_same_v<U, std::string>, bool> = true>
    static void set_impl(U &v, std::string_view str) { v = str; }
    // vector of primitives / strings (objects with metadata are handled via prepare)
    template<typename V>
    static void set_impl(std::vector<V> &v, std::string_view str) {
        if constexpr ( !jsonrefl::has_metadata<V>::value ) {
            auto &r = v.emplace_back();
            set_impl(r, str);
        }
    }
    // list of primitives / strings (objects with metadata are handled via prepare)
    template<typename V>
    static void set_impl(std::list<V> &v, std::string_view str) {
        if constexpr ( !jsonrefl::has_metadata<V>::value ) {
            auto &r = v.emplace_back();
            set_impl(r, str);
        }
    }
    // map — populated via set, not set_impl
    template<typename K, typename V>
    static void set_impl(std::map<K, V> & /*v*/, std::string_view /*str*/) noexcept {}
    // unordered_map — populated via set, not set_impl
    template<typename K, typename V>
    static void set_impl(std::unordered_map<K, V> & /*v*/, std::string_view /*str*/) noexcept {}
    // optional — emplace and delegate
    template<typename V>
    static void set_impl(std::optional<V> &v, std::string_view str) {
        if ( !v ) v.emplace();
        set_impl(*v, str);
    }

    // for object (maps) — not used directly; maps are populated via set
    template<typename U, std::enable_if_t<is_object_type<U>::value, bool> = true>
    static void* obj_prepare(U & /*u*/, void * /*obj*/) noexcept { return nullptr; }
    template<typename U, std::enable_if_t<is_object_type<U>::value, bool> = true>
    static void obj_commit(U & /*u*/, void * /*obj*/, void * /*elem*/) noexcept {}

    // for array
    template<typename U, std::enable_if_t<is_array_type<U>::value, bool> = true>
    static void* arr_prepare(U &u, void * /*arr*/) {
        u.emplace_back();
        auto &back = u.back();
        if constexpr ( is_optional_type<typename U::value_type>::value ) {
            back.emplace();
            return std::addressof(*back);
        } else {
            return &back;
        }
    }
    template<typename U, std::enable_if_t<is_array_type<U>::value, bool> = true>
    static void arr_commit(U & /*u*/, void * /*arr*/, void * /*elem*/) noexcept {}

    template<typename U, std::enable_if_t<!is_object_or_array<U>::value, bool> = true>
    static void* arr_prepare(U &, void */*arr*/) noexcept { return nullptr; }
    template<typename U, std::enable_if_t<!is_object_or_array<U>::value, bool> = true>
    static void arr_commit(U &, void */*arr*/, void */*elem*/) noexcept {}

    using inner_type = unwrap_optional_t<T>;

    static auto& ensure_ref(T &v) {
        if constexpr ( is_optional_type<T>::value ) {
            if ( !v ) v.emplace();
            return *v;
        } else {
            return v;
        }
    }

public:
    constexpr setter_t(T C::*ptr) noexcept
        :setter_base{}
        ,m_ptr{ptr}
    {}

    bool is_object() const noexcept override { return is_object_type<inner_type>::value; }
    bool is_array() const noexcept override { return is_array_type<inner_type>::value; }
    bool is_int() const noexcept override { return std::is_integral_v<inner_type>; }
    bool is_uint() const noexcept override { return is_int() && std::is_unsigned_v<inner_type>; }
    bool is_double() const noexcept override { return std::is_floating_point_v<inner_type>; }
    bool is_string_like() const noexcept override { return is_string_like_t<inner_type>::value; }
    bool is_string_view() const noexcept override { return std::is_same_v<inner_type, std::string_view>; }

    bool has_metadata() const noexcept override
    { return jsonrefl::has_metadata<inner_type>::value; }
    const object_holder_base* get_metadata() const noexcept override {
        if constexpr ( jsonrefl::has_metadata<inner_type>::value ) {
            return std::addressof(metadata<inner_type>());
        } else {
            return nullptr;
        }
    }
    const object_holder_base* get_element_metadata() const noexcept override {
        if constexpr ( is_array_type<inner_type>::value ) {
            using elem_type = typename inner_type::value_type;
            using raw_elem = unwrap_optional_t<elem_type>;
            if constexpr ( jsonrefl::has_metadata<raw_elem>::value ) {
                return std::addressof(metadata<raw_elem>());
            }
        }
        return nullptr;
    }

    void* member_address(void *obj) const noexcept override {
        if constexpr ( is_optional_type<T>::value ) {
            auto &opt = static_cast<C *>(obj)->*m_ptr;
            if ( !opt ) opt.emplace();
            return std::addressof(*opt);
        } else {
            return std::addressof(static_cast<C *>(obj)->*m_ptr);
        }
    }
    const void* member_address(const void *obj) const noexcept override {
        if constexpr ( is_optional_type<T>::value ) {
            const auto &opt = static_cast<const C *>(obj)->*m_ptr;
            return opt.has_value() ? std::addressof(*opt) : nullptr;
        } else {
            return std::addressof(static_cast<const C *>(obj)->*m_ptr);
        }
    }

    void* prepare(void *obj) const override {
        assert(is_array() || is_object());
        auto &inner = ensure_ref(static_cast<C *>(obj)->*m_ptr);
        if constexpr ( is_object_type<inner_type>::value ) {
            return obj_prepare(inner, obj);
        } else {
            return arr_prepare(inner, obj);
        }
    }
    void commit(void *obj, void *elem) const noexcept override {
        assert(is_array() || is_object());
        if constexpr ( is_optional_type<T>::value ) {
            auto &inner = *(static_cast<C *>(obj)->*m_ptr);
            if constexpr ( is_object_type<inner_type>::value ) {
                obj_commit(inner, obj, elem);
            } else {
                arr_commit(inner, obj, elem);
            }
        } else if constexpr ( is_object_type<T>::value ) {
            obj_commit(static_cast<C *>(obj)->*m_ptr, obj, elem);
        } else {
            arr_commit(static_cast<C *>(obj)->*m_ptr, obj, elem);
        }
    }

    void set(void *obj, std::string_view str) const override {
        if constexpr ( !jsonrefl::has_metadata<inner_type>::value ) {
            set_impl(static_cast<C *>(obj)->*m_ptr, str);
        }
    }

    void set(void *obj, std::string_view key, std::string_view val) const override {
        if constexpr ( is_object_type<inner_type>::value ) {
            auto &m = ensure_ref(static_cast<C *>(obj)->*m_ptr);
            typename inner_type::key_type k;
            set_impl(k, key);
            typename inner_type::mapped_type v;
            set_impl(v, val);
            m.emplace(std::move(k), std::move(v));
        }
    }

    std::size_t value_required_bytes(const void *obj, bool pretty, std::size_t indent) const noexcept override {
        return required_bytes(static_cast<const C *>(obj)->*m_ptr, pretty, indent);
    }

    char* value_to_buffer(const void *obj, char *ptr, bool pretty, std::size_t indent) const noexcept override {
        return write_json(ptr, static_cast<const C *>(obj)->*m_ptr, pretty, indent);
    }

    bool value_to_stream(const void *obj, chunked_writer &w, bool pretty, std::size_t indent) const noexcept override {
        return pretty
            ? stream_json<true>(w, static_cast<const C *>(obj)->*m_ptr, indent)
            : stream_json<false>(w, static_cast<const C *>(obj)->*m_ptr, indent);
    }
};

template<typename T>
class root_setter_t final: public setter_base {
    template<typename U, std::enable_if_t<std::is_integral_v<U>, bool> = true>
    static void conv(U &v, std::string_view str) noexcept {
        const bool is_hex = (str.length() > 2 && (str[0] == '0') && (str[1] == 'x' || str[1] == 'X'));
        std::from_chars(str.begin(), str.end(), v, is_hex ? 16 : 10);
    }
    template<typename U, std::enable_if_t<std::is_floating_point_v<U>, bool> = true>
    static void conv(U &v, std::string_view str) noexcept {
        const bool is_hex = (str.length() > 2 && (str[0] == '0') && (str[1] == 'x' || str[1] == 'X'));
        std::from_chars(str.begin(), str.end(), v, is_hex ? std::chars_format::hex : std::chars_format::general);
    }
    template<typename U, std::enable_if_t<std::is_same_v<U, std::string_view>, bool> = true>
    static void conv(U &v, std::string_view str) noexcept { v = str; }
    template<typename U, std::enable_if_t<std::is_same_v<U, std::string>, bool> = true>
    static void conv(U &v, std::string_view str) { v = str; }

public:
    bool is_object() const noexcept override { return is_object_type<T>::value; }
    bool is_array() const noexcept override { return is_array_type<T>::value; }
    bool is_int() const noexcept override { return false; }
    bool is_uint() const noexcept override { return false; }
    bool is_double() const noexcept override { return false; }
    bool is_string_like() const noexcept override { return false; }
    bool is_string_view() const noexcept override { return false; }
    bool has_metadata() const noexcept override { return false; }
    const object_holder_base* get_metadata() const noexcept override { return nullptr; }

    void* member_address(void *obj) const noexcept override { return obj; }
    const void* member_address(const void *obj) const noexcept override { return const_cast<void*>(obj); }

    void* prepare(void *) const override { return nullptr; }
    void commit(void *, void *) const noexcept override {}

    void set(void *obj, std::string_view str) const override {
        if constexpr ( is_array_type<T>::value ) {
            using value_type = typename T::value_type;
            if constexpr ( !jsonrefl::has_metadata<value_type>::value ) {
                auto &r = static_cast<T*>(obj)->emplace_back();
                conv(r, str);
            }
        }
    }

    void set(void *obj, std::string_view key, std::string_view val) const override {
        if constexpr ( is_object_type<T>::value ) {
            auto &m = *static_cast<T*>(obj);
            typename T::key_type k;
            conv(k, key);
            typename T::mapped_type v;
            conv(v, val);
            m.emplace(std::move(k), std::move(v));
        }
    }

    std::size_t value_required_bytes(const void *, bool, std::size_t) const noexcept override { return 4; }
    char* value_to_buffer(const void *, char *ptr, bool, std::size_t) const noexcept override {
        std::memcpy(ptr, "null", 4); return ptr + 4;
    }

    bool value_to_stream(const void *, chunked_writer &w, bool, std::size_t) const noexcept override {
        return w.write("null", 4);
    }
};

template<typename T>
const root_setter_t<T>* root_setter_ptr() noexcept {
    static const root_setter_t<T> instance;
    return &instance;
}

/*************************************************************************************************/

template<class T>
constexpr void cswap(T &a, T &b) {
    auto tmp = a;
    a = b;
    b = tmp;
}

template<class T, class U>
constexpr void cswap(std::pair<T, U> &a, std::pair<T, U> &b) {
    cswap(a.first, b.first);
    cswap(a.second, b.second);
}

template<typename Iterator, class Compare>
constexpr Iterator qsort_partition(Iterator left, Iterator right, Compare const &compare) {
    auto pivot = left + (right - left) / 2;
    cswap(*right, *pivot);
    pivot = right;
    for ( auto it = left; 0 < right - it; ++it ) {
        if ( compare(*it, *pivot) ) {
            cswap(*it, *left);
            left++;
        }
    }
    cswap(*pivot, *left);
    pivot = left;
    return pivot;
}

template<typename Iterator, class Compare>
constexpr void qsort(Iterator left, Iterator right, Compare const &compare) {
    while ( 0 < right - left ) {
        auto new_pivot = qsort_partition(left, right, compare);
        qsort(left, new_pivot, compare);
        left = new_pivot + 1;
    }
}

template<typename Container, class Compare>
constexpr Container qsort(Container array, Compare const &compare) {
    qsort(array.begin(), array.end() - 1, compare);
    return array;
}

template<typename T = void>
struct less {
    constexpr bool operator()(const T &a, const T &b) const noexcept(noexcept(a < b))
    { return a < b; }
};

template<>
struct less<void> {
    template<typename A, typename B>
    constexpr bool operator()(const A &a, const B &b) const noexcept(noexcept(a < b))
    { return a < b; }
};

template<std::size_t N, typename T, typename CmpLess = less<T>>
struct sorted_vector {
    using storage_type = std::array<T, N>;
private:
    storage_type m_data;
    constexpr sorted_vector(storage_type arr)
        :m_data{qsort(std::move(arr), CmpLess{})}
    {}
public:
    template<typename ...U>
    constexpr sorted_vector(U ...elems)
        :sorted_vector{storage_type{std::move(elems)...}}
    {}

    constexpr auto  size()  const noexcept { return N; }
    constexpr auto* begin() const noexcept { return m_data.begin(); }
    constexpr auto* end  () const noexcept { return m_data.end(); }
    constexpr auto& operator[](std::size_t i) const noexcept { return m_data[i]; }
};

template<typename T, typename CmpLess>
struct sorted_vector<1, T, CmpLess> {
    using storage_type = std::array<T, 1>;
private:
    storage_type m_data;
public:
    constexpr sorted_vector(T x)
        :m_data{std::move(x)}
    {}

    constexpr auto  size()  const noexcept { return 1; }
    constexpr auto* begin() const noexcept { return m_data.begin(); }
    constexpr auto* end  () const noexcept { return m_data.end(); }
    constexpr auto& operator[](std::size_t i) const noexcept { return m_data[i]; }
};

template<typename T, typename CmpLess>
struct sorted_vector<0, T, CmpLess> {
    constexpr sorted_vector() {}

    constexpr auto  size()  const noexcept { return 0; }
    constexpr T*    begin() const noexcept { return nullptr; }
    constexpr T*    end  () const noexcept { return nullptr; }
    constexpr auto& operator[](std::size_t /*i*/) const { throw jsonrefl::exception("zero size vector!"); }
};

template<typename CmpLess, typename T, typename ...Ts>
constexpr auto make_sorted_vector(CmpLess, T &&t, Ts && ...ts) {
    return sorted_vector<1 + sizeof...(Ts), std::decay_t<T>, CmpLess>{
        std::forward<T>(t), std::forward<Ts>(ts)...};
}

/*************************************************************************************************/

template<std::size_t ...Ids, typename CmpLess, typename ...TupleElems>
constexpr auto make_index(std::index_sequence<Ids...>, const CmpLess &less, const std::tuple<TupleElems...> &tuple) noexcept {
    auto res = make_sorted_vector(
        less
        ,std::make_pair(
             std::get<Ids>(tuple).hash
            ,static_cast<const setter_base *>(&(std::get<Ids>(tuple).member))
        )...
    );
    return res;
}

/*************************************************************************************************/

struct object_holder_base {
    using index_elem_type = std::pair<std::uint32_t, const setter_base *>;

private:
    const index_elem_type *const m_beg;
    const index_elem_type *const m_end;

public:
    constexpr object_holder_base(
         const index_elem_type * const beg
        ,const index_elem_type * const end
    )
        :m_beg{beg}
        ,m_end{end}
    {}

    constexpr const auto* begin() const noexcept { return m_beg; }
    constexpr const auto* end()   const noexcept { return m_end; }
    constexpr auto size() const noexcept { return m_end - m_beg; }
    constexpr auto empty() const noexcept { return size() == 0; }

    const setter_base* get(std::string_view key) const noexcept {
        if ( key.empty() ) {
            return nullptr;
        }

        const auto hash = fnv1a(key);
        const auto *it = details::lower_bound(m_beg, m_end, hash);
        return (it != m_end && it->first == hash)
            ? it->second
            : nullptr
        ;
    }

    virtual std::size_t required_bytes(const void * /*obj*/, bool /*pretty*/, std::size_t /*indent*/) const noexcept { return 2; }
    virtual char* to_buffer(const void * /*obj*/, char *ptr, bool /*pretty*/, std::size_t /*indent*/) const noexcept {
        std::memcpy(ptr, "{}", 2); return ptr + 2;
    }
    virtual bool to_stream(const void * /*obj*/, chunked_writer &w, bool /*pretty*/, std::size_t /*indent*/) const noexcept {
        return w.write("{}", 2);
    }
};

/*************************************************************************************************/

template<typename ...Types>
class object_holder_t final: public object_holder_base {
    using cmp_less = less<std::pair<std::uint32_t, const setter_base *>>;
    using index_type = sorted_vector<
         sizeof...(Types)
        ,object_holder_base::index_elem_type
        ,cmp_less
    >;

    const std::string_view m_name;
    const std::tuple<Types...> m_tuple;
    const index_type m_index;
    const std::size_t m_max_name_len;
public:
    template<typename ...Args>
    constexpr object_holder_t(std::string_view name, Args && ...args)
        :object_holder_base{std::begin(m_index), std::end(m_index)}
        ,m_name{name}
        ,m_tuple{std::forward<Args>(args)...}
        ,m_index{make_index(std::index_sequence_for<Types...>{}, cmp_less{}, m_tuple)}
        ,m_max_name_len{details::mymax(args.name.length()...)}
    {}

    constexpr auto name() const noexcept { return m_name; }

    std::ostream& dump(std::ostream &os) const {
        // print tuple (not sorted)
        os << "-- hash --  -- name --  -- address -- type --" << std::endl;
        std::apply(
            [&os, this](const auto & ...elems){
                static const auto print_cb = [this](std::ostream &os, const auto &elem){
                    static const auto name_min_len = sizeof("-- name --") - 1;
                    static const auto spaces = std::string_view{"                                "}; // 32

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
                (print_cb(os, elems), ...);
            }
            ,m_tuple
        );

        // print index
        os << "-- hash --  -- address --" << std::endl;
        auto flags = os.flags();
        for ( const auto &it: m_index ) {
            os << "0x";

            auto width = os.width(8);
            auto fillc = os.fill('0');
            os << std::hex << it.first;

            os.width(width);
            os.fill(fillc);
            os << ": " << it.second << std::endl;
        }
        os.flags(flags);

        return os;
    }

    std::size_t required_bytes(const void *obj, bool pretty, std::size_t indent) const noexcept override {
        std::size_t n = 1;
        const auto child = pretty ? indent + k_indent_step.size() : std::size_t{0};
        if ( pretty && sizeof...(Types) > 0 ) n += 1;
        bool first = true;
        std::apply(
            [&n, &first, obj, pretty, child](const auto & ...elems) {
                const auto count = [&n, &first, obj, pretty, child](const auto &elem) {
                    if ( !first ) {
                        n += 1;
                        if ( pretty ) n += 1;
                    }
                    if ( pretty ) n += child;
                    n += 1 + elem.name.size() + (pretty ? 3 : 2);
                    n += elem.member.value_required_bytes(obj, pretty, child);
                    first = false;
                };
                (count(elems), ...);
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

    char* to_buffer(const void *obj, char *ptr, bool pretty, std::size_t indent) const noexcept override {
        const auto child = pretty ? indent + k_indent_step.size() : std::size_t{0};
        *ptr++ = '{';
        if ( pretty && sizeof...(Types) > 0 ) *ptr++ = '\n';
        bool first = true;
        std::apply(
            [&ptr, &first, obj, pretty, child](const auto & ...elems) {
                const auto write = [&ptr, &first, obj, pretty, child](const auto &elem) {
                    if ( !first ) {
                        *ptr++ = ',';
                        if ( pretty ) *ptr++ = '\n';
                    }
                    if ( pretty ) { std::memset(ptr, ' ', child); ptr += child; }
                    *ptr++ = '"';
                    std::memcpy(ptr, elem.name.data(), elem.name.size());
                    ptr += elem.name.size();
                    if ( pretty ) {
                        std::memcpy(ptr, "\": ", 3); ptr += 3;
                    } else {
                        std::memcpy(ptr, "\":", 2); ptr += 2;
                    }
                    ptr = elem.member.value_to_buffer(obj, ptr, pretty, child);
                    first = false;
                };
                (write(elems), ...);
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

    bool to_stream(const void *obj, chunked_writer &w, bool pretty, std::size_t indent) const noexcept override {
        return pretty
            ? to_stream_impl<true>(obj, w, indent)
            : to_stream_impl<false>(obj, w, indent);
    }

    template<bool Pretty>
    bool to_stream_impl(const void *obj, chunked_writer &w, std::size_t indent) const noexcept {
        const auto child = Pretty ? indent + k_indent_step.size() : std::size_t{0};
        if ( !w.put('{') ) return false;
        if constexpr (Pretty) { if ( sizeof...(Types) > 0 && !w.put('\n') ) return false; }
        bool first = true;
        bool ok = true;
        std::apply(
            [&](const auto & ...elems) {
                const auto write_one = [&](const auto &elem) {
                    if ( !ok ) return;
                    if ( !w.template write_field_key<Pretty>(elem.name, first, child) ) { ok = false; return; }
                    if ( !elem.member.value_to_stream(obj, w, Pretty, child) ) { ok = false; return; }
                    first = false;
                };
                (write_one(elems), ...);
            }
            ,m_tuple
        );
        if ( !ok ) return false;
        if constexpr (Pretty) {
            if ( !first ) {
                if ( !w.put('\n') ) return false;
                if ( !w.fill(' ', indent) ) return false;
            }
        }
        return w.put('}');
    }
};

/*************************************************************************************************/

template<bool Pretty, typename U>
bool stream_json(chunked_writer &w, const U &v, std::size_t indent) noexcept {
    if constexpr ( has_metadata<U>::value ) {
        return metadata<U>().to_stream(&v, w, Pretty, indent);
    } else if constexpr ( is_string_like_t<U>::value ) {
        if ( !w.put('"') ) return false;
        const char *src = v.data();
        const char *const end = src + v.size();
        while ( src != end ) {
            const char *run = src;
            while ( run != end && !k_esc.replacement[static_cast<unsigned char>(*run)] )
                ++run;
            if ( run != src ) {
                if ( !w.write(src, static_cast<std::size_t>(run - src)) ) return false;
            }
            if ( run != end ) {
                const char esc[2] = {'\\', k_esc.replacement[static_cast<unsigned char>(*run)]};
                if ( !w.write(esc, 2) ) return false;
                ++run;
            }
            src = run;
        }
        return w.put('"');
    } else if constexpr ( std::is_same_v<U, bool> ) {
        return v ? w.write("true", 4) : w.write("false", 5);
    } else if constexpr ( std::is_integral_v<U> ) {
        if ( auto *p = w.reserve(24) ) {
            auto res = std::to_chars(p, p + 24, v);
            w.advance(static_cast<std::size_t>(res.ptr - p));
            return true;
        }
        char tmp[24];
        auto res = std::to_chars(tmp, tmp + sizeof(tmp), v);
        return w.write(tmp, static_cast<std::size_t>(res.ptr - tmp));
    } else if constexpr ( std::is_floating_point_v<U> ) {
        if ( auto *p = w.reserve(32) ) {
            auto res = std::to_chars(p, p + 32, static_cast<double>(v), std::chars_format::fixed, 6);
            w.advance(static_cast<std::size_t>(res.ptr - p));
            return true;
        }
        char tmp[32];
        auto res = std::to_chars(tmp, tmp + sizeof(tmp), static_cast<double>(v), std::chars_format::fixed, 6);
        return w.write(tmp, static_cast<std::size_t>(res.ptr - tmp));
    } else if constexpr ( is_array_type<U>::value ) {
        if ( v.empty() ) return w.write("[]", 2);
        const auto child = Pretty ? indent + k_indent_step.size() : std::size_t{0};
        if ( !w.put('[') ) return false;
        if constexpr (Pretty) { if ( !w.put('\n') ) return false; }
        bool first = true;
        for ( const auto &elem : v ) {
            if ( !first ) {
                if ( !w.put(',') ) return false;
                if constexpr (Pretty) { if ( !w.put('\n') ) return false; }
            }
            if constexpr (Pretty) { if ( !w.fill(' ', child) ) return false; }
            if ( !stream_json<Pretty>(w, elem, child) ) return false;
            first = false;
        }
        if constexpr (Pretty) {
            if ( !w.put('\n') ) return false;
            if ( !w.fill(' ', indent) ) return false;
        }
        return w.put(']');
    } else if constexpr ( is_object_type<U>::value ) {
        if ( v.empty() ) return w.write("{}", 2);
        const auto child = Pretty ? indent + k_indent_step.size() : std::size_t{0};
        if ( !w.put('{') ) return false;
        if constexpr (Pretty) { if ( !w.put('\n') ) return false; }
        bool first = true;
        for ( const auto &[k, val] : v ) {
            if ( !first ) {
                if ( !w.put(',') ) return false;
                if constexpr (Pretty) { if ( !w.put('\n') ) return false; }
            }
            if constexpr (Pretty) { if ( !w.fill(' ', child) ) return false; }
            if ( !stream_json<Pretty>(w, k, child) ) return false;
            if constexpr (Pretty) { if ( !w.write(": ", 2) ) return false; }
            else { if ( !w.put(':') ) return false; }
            if ( !stream_json<Pretty>(w, val, child) ) return false;
            first = false;
        }
        if constexpr (Pretty) {
            if ( !w.put('\n') ) return false;
            if ( !w.fill(' ', indent) ) return false;
        }
        return w.put('}');
    } else if constexpr ( is_optional_type<U>::value ) {
        if ( v.has_value() ) return stream_json<Pretty>(w, *v, indent);
        return w.write("null", 4);
    } else {
        return w.write("null", 4);
    }
}

} // ns details

template<typename T, typename C>
struct object_member_t {
    std::uint32_t hash;
    std::string_view name;
    details::setter_t<C, T> member;
};

namespace details {

template<typename>
struct member_type_of;

template<typename T, typename C>
struct member_type_of<object_member_t<T, C>> { using type = T; };

template<typename ...Types>
constexpr std::size_t max_member_depth(const object_holder_t<Types...> &) noexcept {
    if constexpr ( sizeof...(Types) == 0 ) {
        return 0u;
    } else if constexpr ( sizeof...(Types) == 1 ) {
        return ::jsonrefl::stack_depth<typename member_type_of<Types>::type...>();
    } else {
        return mymax(::jsonrefl::stack_depth<typename member_type_of<Types>::type>()...);
    }
}

} // ns details

template<typename T>
constexpr std::size_t stack_depth() noexcept {
    if constexpr ( has_metadata<T>::value ) {
        return 1 + details::max_member_depth(metadata<T>());
    } else if constexpr ( details::is_array_type<T>::value ) {
        return 1 + stack_depth<typename T::value_type>();
    } else if constexpr ( details::is_object_type<T>::value ) {
        return 1 + stack_depth<typename T::mapped_type>();
    } else if constexpr ( details::is_optional_type<T>::value ) {
        return stack_depth<typename T::value_type>();
    } else {
        return 0u;
    }
}


// Used for initialize the object of type `object_member_t`
template<std::size_t N, typename T, typename C>
constexpr auto object_member(const char (&str)[N], T C::*mptr) noexcept {
    return object_member_t<T, C>{details::fnv1a(str), str, mptr};
}

// Used as:
// auto o = object_holder(
//      "type name"
//     ,object_member("member0 name", &type::member0)
//     ,object_member("member1 name", &type::member1)
// );
//
// so, `o` is the final storage for all members metadata for `type name`
template<typename ...Members>
constexpr auto object_holder(std::string_view name, Members && ...members) noexcept {
    return details::object_holder_t<Members...>{name, std::forward<Members>(members)...};
}

template<typename T>
std::size_t required_bytes(const T &obj, bool pretty = false) noexcept {
    static_assert(
         has_metadata<T>::value || details::is_object_or_array<T>::value
        ,"T must have JSONREFL_METADATA or be a standard container"
    );
    return details::required_bytes(obj, pretty, std::size_t{0});
}

template<typename T>
char* to_buffer(char *ptr, const T &obj, bool pretty = false) noexcept {
    static_assert(
         has_metadata<T>::value || details::is_object_or_array<T>::value
        ,"T must have JSONREFL_METADATA or be a standard container"
    );
    return details::write_json(ptr, obj, pretty, std::size_t{0});
}

template<typename T>
std::string to_string(const T &obj, bool pretty = false) {
    static_assert(
         has_metadata<T>::value || details::is_object_or_array<T>::value
        ,"T must have JSONREFL_METADATA or be a standard container"
    );
    const auto n = required_bytes(obj, pretty);
    std::string result(n, '\0');
    to_buffer(result.data(), obj, pretty);
    return result;
}

template<typename T, typename Fn>
bool to_chunked_buffer(char *buf, std::size_t buf_size, const T &obj, Fn &&fn, bool pretty = false) {
    static_assert(
         has_metadata<T>::value || details::is_object_or_array<T>::value
        ,"T must have JSONREFL_METADATA or be a standard container"
    );
    auto trampoline = [](void *ud, const void *data, std::size_t size) -> bool
    { return (*static_cast<std::remove_reference_t<Fn>*>(ud))(data, size); };
    details::chunked_writer w{buf, buf_size, trampoline, &fn};
    if ( pretty ) details::stream_json<true>(w, obj, std::size_t{0});
    else          details::stream_json<false>(w, obj, std::size_t{0});
    if ( w.ok && w.pos > 0 ) w.flush();
    return w.ok;
}

/*************************************************************************************************/

template<typename C>
class parser {
    static_assert(has_metadata<C>::value
        || details::is_object_or_array<C>::value);

    C *m_cls;
    std::string_view m_key;

    struct stack_elem {
        const details::object_holder_base *holder;
        const details::setter_base *arr_setter;
        const details::setter_base *map_setter;
        void *addr;
    };
    std::array<stack_elem, stack_depth<C>()> m_stack;
    int m_stack_idx;

private:
    void stack_push(stack_elem elem) noexcept {
        assert(m_stack_idx < int(m_stack.size()));
        m_stack[++m_stack_idx] = elem;
    }
    void stack_pop() noexcept {
        assert(m_stack_idx >= 0);
        m_stack[m_stack_idx--] = stack_elem{};
    }
    stack_elem stack_top() const noexcept {
        assert(m_stack_idx >= 0);
        return m_stack[m_stack_idx];
    }

public:
    parser(C *cls)
        :m_cls{cls}
        ,m_key{}
        ,m_stack{}
        ,m_stack_idx{-1}
    {}

    void on_key(std::string_view key) {
        m_key = key;
    }
    void on_int(std::string_view val) {
        const auto top = stack_top();
        if ( top.map_setter ) {
            top.map_setter->set(top.addr, m_key, val);
        } else if ( top.arr_setter ) {
            top.arr_setter->set(top.addr, val);
        } else {
            const auto *setter = top.holder->get(m_key);
            assert(setter);
            setter->set(top.addr, val);
        }
        m_key = {};
    }
    bool on_str(std::string_view val, bool has_escapes = false) {
        const auto top = stack_top();
        if ( top.map_setter ) {
            top.map_setter->set(top.addr, m_key, val);
        } else if ( top.arr_setter ) {
            top.arr_setter->set(top.addr, val);
        } else {
            const auto *setter = top.holder->get(m_key);
            assert(setter);
            if ( m_accumulating && setter->is_string_view() ) return false;
            setter->set(top.addr, val);
            if ( has_escapes && setter->is_string_like() && !setter->is_string_view() ) {
                auto *str = static_cast<std::string *>(setter->member_address(top.addr));
                unescape_inplace(*str);
            }
        }
        m_key = {};
        return true;
    }
    void on_null() noexcept {
        m_key = {};
    }

    void open_object() {
        if ( m_stack_idx < 0 ) {
            if constexpr ( details::is_object_type<C>::value
                && !has_metadata<C>::value )
            {
                stack_push({nullptr, nullptr, details::root_setter_ptr<C>(), m_cls});
            } else if constexpr ( has_metadata<C>::value ) {
                stack_push({std::addressof(metadata<C>()), nullptr, nullptr, m_cls});
            }
        } else if ( !m_key.empty() ) {
            const auto top = stack_top();
            const auto *setter = top.holder->get(m_key);
            assert(setter);
            if ( setter->is_object() && !setter->has_metadata() ) {
                stack_push({nullptr, nullptr, setter, top.addr});
            } else {
                assert(setter->has_metadata());
                const auto *object = setter->get_metadata();
                void *addr = setter->member_address(top.addr);
                stack_push({object, nullptr, nullptr, addr});
            }
            m_key = {};
        } else {
            const auto top = stack_top();
            assert(top.arr_setter);
            void *elem = top.arr_setter->prepare(top.addr);
            const auto *meta = top.arr_setter->get_element_metadata();
            assert(meta);
            stack_push({meta, nullptr, nullptr, elem});
        }
    }
    void close_object() {
        stack_pop();
    }

    void open_array() {
        if ( m_stack_idx < 0 ) {
            if constexpr ( details::is_array_type<C>::value
                && !has_metadata<C>::value )
            {
                stack_push({nullptr, details::root_setter_ptr<C>(), nullptr, m_cls});
            } else if constexpr ( has_metadata<C>::value ) {
                stack_push({std::addressof(metadata<C>()), nullptr, nullptr, m_cls});
            }
        } else {
            assert(!m_key.empty());
            const auto top = stack_top();
            const auto *setter = top.holder->get(m_key);
            assert(setter);
            assert(setter->is_array());
            stack_push({nullptr, setter, nullptr, top.addr});
            m_key = {};
        }
    }
    void close_array() {
        stack_pop();
    }

private:
    enum class jstate: uint8_t {
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
    };
    enum class jctx : uint8_t { object, array };

    static constexpr int MAX_NESTING = 64;

    jstate m_js{jstate::value};
    jctx   m_ctx_buf[MAX_NESTING]{};
    int    m_ctx_depth{-1};

    const char *m_seg_start{};
    std::size_t m_str_len{};
    bool  m_has_esc{};
    bool  m_is_key{};
    std::string m_accum;
    bool  m_accumulating{};

    int   m_uni_remain{};

    const char *m_lit_str{};
    int   m_lit_pos{};
    int   m_lit_len{};

    void ctx_push(jctx c) noexcept { m_ctx_buf[++m_ctx_depth] = c; }
    void ctx_pop() noexcept { --m_ctx_depth; }
    jctx ctx_top() const noexcept { return m_ctx_buf[m_ctx_depth]; }

    void after_value() noexcept {
        if ( m_ctx_depth < 0 ) {
            m_js = jstate::done;
        } else if ( ctx_top() == jctx::object ) {
            m_js = jstate::comma_or_obj_end;
        } else {
            m_js = jstate::comma_or_arr_end;
        }
    }

    void flush_segment(const char *seg_end) {
        if ( !m_accumulating ) {
            m_accum.assign(m_seg_start, seg_end);
            m_accumulating = true;
        } else {
            m_accum.append(m_seg_start, seg_end);
        }
    }

    bool emit_string() {
        std::string_view sv = m_accumulating
            ? std::string_view{m_accum}
            : std::string_view{m_seg_start, m_str_len}
        ;
        if ( m_is_key ) {
            on_key(sv);
            m_js = jstate::colon;
        } else {
            if ( !on_str(sv, m_has_esc) ) return false;
            after_value();
        }
        return true;
    }

    void emit_number() {
        std::string_view sv = m_accumulating
            ? std::string_view{m_accum}
            : std::string_view{m_seg_start, m_str_len}
        ;
        on_int(sv);
        after_value();
    }

    static bool is_ws(char c) noexcept {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    static bool is_num_char(char c) noexcept {
        return (c >= '0' && c <= '9')
            || c == '.' || c == '-'
            || c == '+'
            || c == 'e' || c == 'E'
        ;
    }

    static std::uint32_t hex4(const char *p) noexcept {
        std::uint32_t v = 0;
        for ( auto i = 0u; i < 4u; ++i ) {
            v <<= 4;
            char c = p[i];
            if ( c >= '0' && c <= '9' )      v += static_cast<unsigned>(c - '0');
            else if ( c >= 'a' && c <= 'f' ) v += 10u + static_cast<unsigned>(c - 'a');
            else if ( c >= 'A' && c <= 'F' ) v += 10u + static_cast<unsigned>(c - 'A');
        }
        return v;
    }

    static void unescape_inplace(std::string &s) {
        char *dst = s.data();
        const char *src = s.data();
        const char *end = src + s.size();
        while ( src < end ) {
            if ( *src != '\\' ) { *dst++ = *src++; continue; }
            ++src;
            switch ( *src++ ) {
                case '"':  *dst++ = '"';  break;
                case '\\': *dst++ = '\\'; break;
                case '/':  *dst++ = '/';  break;
                case 'b':  *dst++ = '\b'; break;
                case 'f':  *dst++ = '\f'; break;
                case 'n':  *dst++ = '\n'; break;
                case 'r':  *dst++ = '\r'; break;
                case 't':  *dst++ = '\t'; break;
                case 'u': {
                    std::uint32_t cp = hex4(src);
                    src += 4;
                    if ( cp >= 0xD800u && cp <= 0xDBFFu
                        && (end - src) >= 6
                        && src[0] == '\\' && src[1] == 'u' )
                    {
                        std::uint32_t lo = hex4(src + 2);
                        if ( lo >= 0xDC00u && lo <= 0xDFFFu ) {
                            cp = 0x10000u + ((cp - 0xD800u) << 10) + (lo - 0xDC00u);
                            src += 6;
                        }
                    }
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
                    break;
                }
                default: *dst++ = *(src - 1); break;
            }
        }
        s.resize(static_cast<std::size_t>(dst - s.data()));
    }

public:
    bool parse(std::string_view chunk) {
        const char *p = chunk.data();
        const char *const end = p + chunk.size();

        if ( m_js == jstate::in_string || m_js == jstate::in_number || m_js == jstate::in_literal ) {
            m_seg_start = p;
        }

        while ( p < end ) {
            switch ( m_js ) {
            case jstate::value:
            case jstate::value_or_arr_end:
            case jstate::key_or_obj_end:
            case jstate::colon:
            case jstate::comma_or_obj_end:
            case jstate::comma_or_arr_end: {
                while ( p < end && is_ws(*p) ) ++p;
                if ( p >= end ) return true;

                const char c = *p;

                if ( m_js == jstate::colon ) {
                    if ( c != ':' ) return false;
                    ++p;
                    m_js = jstate::value;
                    break;
                }
                if ( m_js == jstate::comma_or_obj_end ) {
                    if ( c == ',' ) { ++p; m_js = jstate::key_or_obj_end; break; }
                    if ( c == '}' ) { ++p; close_object(); ctx_pop(); after_value(); break; }
                    return false;
                }
                if ( m_js == jstate::comma_or_arr_end ) {
                    if ( c == ',' ) { ++p; m_js = jstate::value; break; }
                    if ( c == ']' ) { ++p; close_array(); ctx_pop(); after_value(); break; }
                    return false;
                }
                if ( m_js == jstate::key_or_obj_end ) {
                    if ( c == '}' ) { ++p; close_object(); ctx_pop(); after_value(); break; }
                    if ( c != '"' ) return false;
                    m_is_key = true;
                    m_has_esc = false;
                    m_accumulating = false;
                    m_seg_start = ++p;
                    m_js = jstate::in_string;
                    break;
                }

                if ( m_js == jstate::value_or_arr_end && c == ']' ) {
                    ++p;
                    close_array();
                    ctx_pop();
                    after_value();
                    break;
                }

                switch ( c ) {
                case '{':
                    ++p;
                    open_object();
                    ctx_push(jctx::object);
                    m_js = jstate::key_or_obj_end;
                    break;
                case '[':
                    ++p;
                    open_array();
                    ctx_push(jctx::array);
                    m_js = jstate::value_or_arr_end;
                    break;
                case '"':
                    m_is_key = false;
                    m_has_esc = false;
                    m_accumulating = false;
                    m_seg_start = ++p;
                    m_js = jstate::in_string;
                    break;
                case 't':
                    m_lit_str = "true";
                    m_lit_len = 4;
                    m_lit_pos = 1;
                    ++p;
                    m_js = jstate::in_literal;
                    break;
                case 'f':
                    m_lit_str = "false";
                    m_lit_len = 5;
                    m_lit_pos = 1;
                    ++p;
                    m_js = jstate::in_literal;
                    break;
                case 'n':
                    m_lit_str = "null";
                    m_lit_len = 4;
                    m_lit_pos = 1;
                    ++p;
                    m_js = jstate::in_literal;
                    break;
                default:
                    if ( c == '-' || (c >= '0' && c <= '9') ) {
                        m_accumulating = false;
                        m_seg_start = p++;
                        m_js = jstate::in_number;
                        break;
                    }
                    return false;
                }
                break;
            }

            case jstate::in_string: {
                while ( p < end ) {
                    const char c = *p;
                    if ( c == '"' ) {
                        m_str_len = static_cast<std::size_t>(p - m_seg_start);
                        if ( m_accumulating ) m_accum.append(m_seg_start, p);
                        ++p;
                        if ( !emit_string() ) return false;
                        goto next_token;
                    }
                    if ( c == '\\' ) {
                        m_has_esc = true;
                        ++p;
                        if ( p >= end ) {
                            flush_segment(p);
                            m_js = jstate::in_escape;
                            return true;
                        }
                        if ( *p == 'u' ) {
                            ++p;
                            m_uni_remain = 4;
                            while ( p < end && m_uni_remain > 0 ) { ++p; --m_uni_remain; }
                            if ( m_uni_remain > 0 ) {
                                flush_segment(p);
                                m_js = jstate::in_unicode;
                                return true;
                            }
                        } else {
                            ++p;
                        }
                        continue;
                    }
                    ++p;
                }
                flush_segment(p);
                return true;
            next_token:
                break;
            }

            case jstate::in_escape: {
                if ( p >= end ) return true;
                if ( *p == 'u' ) {
                    m_accum += *p++;
                    m_uni_remain = 4;
                    m_js = jstate::in_unicode;
                } else {
                    m_accum += *p++;
                    m_seg_start = p;
                    m_js = jstate::in_string;
                }
                break;
            }

            case jstate::in_unicode: {
                while ( p < end && m_uni_remain > 0 ) {
                    m_accum += *p++;
                    --m_uni_remain;
                }
                if ( m_uni_remain == 0 ) {
                    m_seg_start = p;
                    m_js = jstate::in_string;
                }
                break;
            }

            case jstate::in_number: {
                while ( p < end && is_num_char(*p) ) ++p;
                if ( p < end ) {
                    m_str_len = static_cast<std::size_t>(p - m_seg_start);
                    if ( m_accumulating ) m_accum.append(m_seg_start, p);
                    emit_number();
                } else {
                    flush_segment(p);
                    return true;
                }
                break;
            }

            case jstate::in_literal: {
                while ( p < end && m_lit_pos < m_lit_len ) {
                    if ( *p != m_lit_str[m_lit_pos] ) return false;
                    ++p;
                    ++m_lit_pos;
                }
                if ( m_lit_pos == m_lit_len ) {
                    if ( m_lit_str[0] == 'n' ) {
                        on_null();
                    } else {
                        on_int({m_lit_str, static_cast<std::size_t>(m_lit_len)});
                    }
                    after_value();
                }
                break;
            }

            case jstate::done: {
                while ( p < end && is_ws(*p) ) ++p;
                if ( p < end ) return false;
                return true;
            }
            }
        }
        return true;
    }

    bool finished() const noexcept {
        return m_js == jstate::done;
    }

    void reset() noexcept {
        m_key = {};
        m_stack = {};
        m_stack_idx = -1;
        m_js = jstate::value;
        m_ctx_depth = -1;
        m_seg_start = {};
        m_str_len = {};
        m_has_esc = {};
        m_is_key = {};
        m_accum.clear();
        m_accumulating = {};
        m_uni_remain = {};
        m_lit_str = {};
        m_lit_pos = {};
        m_lit_len = {};
    }
};

template<typename C>
auto make_parser(C *cls) {
    return parser<C>{cls};
}

/*************************************************************************************************/

} // ns jsonrefl
