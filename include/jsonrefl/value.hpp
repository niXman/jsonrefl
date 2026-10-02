
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

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <system_error>
#include <type_traits>

#if !defined(JSONREFL_CXX)
#  if defined(_MSVC_LANG)
#    define JSONREFL_CXX _MSVC_LANG
#  else
#    define JSONREFL_CXX __cplusplus
#  endif
#endif

#if JSONREFL_CXX >= 201703L
#  include <charconv>
#  include <optional>
#  include <string_view>
#else
#  include <boost/optional.hpp>
#  include <boost/utility/string_view.hpp>
#  if defined(JSONREFL_USE_BOOST_CHARCONV)
#    include <boost/charconv.hpp>
#  else
#    include <cerrno>
#    include <cstdlib>
#  endif
#endif

namespace jsonrefl {

/*************************************************************************************************/

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

enum class value_kind: std::uint8_t {
     null = 0
    ,string = 1
    ,integer = 2
    ,floating = 3
    ,boolean = 4
};

/*************************************************************************************************/

namespace details {

enum { k_charconv_buf = 512 };

#if JSONREFL_CXX >= 201703L

using chars_format = std::chars_format;
using from_chars_result = std::from_chars_result;
using to_chars_result = std::to_chars_result;

template<typename ...Args>
inline from_chars_result from_chars(Args&& ...args)
{ return std::from_chars(std::forward<Args>(args)...); }

#elif defined(JSONREFL_USE_BOOST_CHARCONV)

using boost::charconv::chars_format;
using boost::charconv::from_chars_result;
using boost::charconv::to_chars_result;

template<typename ...Args>
inline from_chars_result from_chars(Args&& ...args)
{ return boost::charconv::from_chars(std::forward<Args>(args)...); }

#else

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
    ,char *buf) noexcept
{
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
    ,chars_format) noexcept
{
    from_chars_result res{first, std::errc::invalid_argument};
    if ( first >= last ) { return res; }
    char buf[k_charconv_buf];
    if ( !charconv_copy_nt(first, last, buf) ) { return res; }
    errno = 0;
    char *endp = nullptr;
    const double parsed = std::strtod(buf, &endp);
    if ( endp == buf ) { return res; }
    value = parsed;
    res.ptr = first + static_cast<std::size_t>(endp - buf);
    res.ec = std::errc{};
    return res;
}

inline from_chars_result from_chars_float_g(
     const char *first
    ,const char *last
    ,float &value
    ,chars_format) noexcept
{
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
        if ( buf[0] == '-' ) { return res; }
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
    ,from_chars_result
>::type
from_chars(const char *first, const char *last, T &value, int base) noexcept
{ return from_chars_i(first, last, value, base); }

inline from_chars_result from_chars(
     const char *first
    ,const char *last
    ,double &value
    ,chars_format fmt) noexcept
{ return from_chars_float_g(first, last, value, fmt); }

inline from_chars_result from_chars(
     const char *first
    ,const char *last
    ,float &value
    ,chars_format fmt) noexcept
{ return from_chars_float_g(first, last, value, fmt); }

#endif

inline bool is_hex_json_number(string_view_t str) noexcept
{ return str.length() > 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X'); }

inline bool parse_sv_fully_consumed(
     const char *
    ,const char *last
    ,const char *ptr) noexcept
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

/*************************************************************************************************/

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

    string_view_t to_string_view() const noexcept { return m_sv; }
    std::string to_string() const { return std::string{m_sv.begin(), m_sv.end()}; }

    bool operator==(const value_t &rhs) const noexcept
    { return m_kind == rhs.m_kind && m_sv == rhs.m_sv; }

    bool operator!=(const value_t &rhs) const noexcept
    { return !(*this == rhs); }

    optional_t<bool> to_bool() const noexcept {
        if ( m_kind != value_kind::boolean ) { return optional_t<bool>{}; }

        return details::parse_bool_sv(m_sv);
    }

    optional_t<std::int8_t> to_int8() const noexcept {
        if ( m_kind != value_kind::integer ) { return optional_t<std::int8_t>{}; }

        return details::parse_integral_sv<std::int8_t>(m_sv);
    }

    optional_t<std::uint8_t> to_uint8() const noexcept {
        if ( m_kind != value_kind::integer ) { return optional_t<std::uint8_t>{}; }

        return details::parse_integral_sv<std::uint8_t>(m_sv);
    }

    optional_t<std::int16_t> to_int16() const noexcept {
        if ( m_kind != value_kind::integer ) { return optional_t<std::int16_t>{}; }

        return details::parse_integral_sv<std::int16_t>(m_sv);
    }

    optional_t<std::uint16_t> to_uint16() const noexcept {
        if ( m_kind != value_kind::integer ) { return optional_t<std::uint16_t>{}; }

        return details::parse_integral_sv<std::uint16_t>(m_sv);
    }

    optional_t<std::int32_t> to_int32() const noexcept {
        if ( m_kind != value_kind::integer ) { return optional_t<std::int32_t>{}; }

        return details::parse_integral_sv<std::int32_t>(m_sv);
    }

    optional_t<std::uint32_t> to_uint32() const noexcept {
        if ( m_kind != value_kind::integer ) { return optional_t<std::uint32_t>{}; }

        return details::parse_integral_sv<std::uint32_t>(m_sv);
    }

    optional_t<std::int64_t> to_int64() const noexcept {
        if ( m_kind != value_kind::integer ) { return optional_t<std::int64_t>{}; }

        return details::parse_integral_sv<std::int64_t>(m_sv);
    }

    optional_t<std::uint64_t> to_uint64() const noexcept {
        if ( m_kind != value_kind::integer ) { return optional_t<std::uint64_t>{}; }

        return details::parse_integral_sv<std::uint64_t>(m_sv);
    }

    optional_t<std::size_t> to_size_t() const noexcept {
        if ( m_kind != value_kind::integer ) { return optional_t<std::size_t>{}; }

        return details::parse_integral_sv<std::size_t>(m_sv);
    }

    optional_t<float> to_float() const noexcept {
        if ( m_kind != value_kind::floating ) { return optional_t<float>{}; }

        return details::parse_floating_sv<float>(m_sv);
    }

    optional_t<double> to_double() const noexcept {
        if ( m_kind != value_kind::floating ) { return optional_t<double>{}; }

        return details::parse_floating_sv<double>(m_sv);
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
    to() const noexcept {
        if ( m_kind != value_kind::integer ) { return optional_t<T>{}; }

        return details::parse_integral_sv<T>(m_sv);
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, optional_t<T>>::type
    to() const noexcept {
        if ( m_kind != value_kind::floating ) { return optional_t<T>{}; }

        return details::parse_floating_sv<T>(m_sv);
    }

    template<typename T>
    typename std::enable_if<std::is_enum<T>::value, optional_t<T>>::type
    to() const noexcept {
        if ( m_kind != value_kind::integer ) { return optional_t<T>{}; }

        const auto raw = details::parse_integral_sv<std::underlying_type_t<T>>(m_sv);
        return raw
            ? optional_t<T>{static_cast<T>(*raw)}
            : optional_t<T>{}
        ;
    }
};

/*************************************************************************************************/

} // ns jsonrefl
