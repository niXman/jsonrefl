
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

#pragma once

#include <jsonrefl/jsonrefl.hpp>

namespace myns {

struct result_t {
    jsonrefl::string_view_t symbol;
    std::size_t preventedMatchId;
    std::size_t takerOrderId;
    std::size_t makerOrderId;
    std::size_t tradeGroupId;
    jsonrefl::string_view_t selfTradePreventionMode;
    jsonrefl::string_view_t price;
    jsonrefl::string_view_t makerPreventedQuantity;
    std::size_t transactTime;
};
JSONREFL_METADATA(
    result_t
    ,symbol
    ,preventedMatchId
    ,takerOrderId
    ,makerOrderId
    ,tradeGroupId
    ,selfTradePreventionMode
    ,price
    ,makerPreventedQuantity
    ,transactTime
);

struct rate_limits_t {
    jsonrefl::string_view_t rateLimitType;
    jsonrefl::string_view_t interval;
    std::size_t intervalNum;
    std::size_t limit;
    std::size_t count;
    std::unordered_map<std::string, std::string> extra;
    std::list<int> codes;
};
JSONREFL_METADATA(
    rate_limits_t
    ,rateLimitType
    ,interval
    ,intervalNum
    ,limit
    ,count
    ,extra
    ,codes
);

struct response_t {
    jsonrefl::string_view_t id;
    std::size_t status;
    std::vector<result_t> results;
    std::vector<rate_limits_t> rate_limits;
    std::map<std::string, std::string> headers;
    std::list<std::string> warnings;
};
JSONREFL_METADATA(
    response_t
    ,id
    ,status
    ,results
    ,rate_limits
    ,headers
    ,warnings
);

/*************************************************************************************************/

struct optional_test_t {
    jsonrefl::optional_t<int> opt_int;
    jsonrefl::optional_t<std::string> opt_str;
    jsonrefl::optional_t<double> opt_dbl;
    jsonrefl::optional_t<std::vector<int>> opt_vec;
    jsonrefl::optional_t<std::map<std::string, std::string>> opt_map;
    int regular;
};
JSONREFL_METADATA(
    optional_test_t
    ,opt_int
    ,opt_str
    ,opt_dbl
    ,opt_vec
    ,opt_map
    ,regular
);

/*************************************************************************************************/

} // ns myns

/*************************************************************************************************/

struct int_array {
    std::vector<int> arr;
};
JSONREFL_METADATA(int_array, arr);

struct string_array {
    std::vector<std::string> arr;
};
JSONREFL_METADATA(string_array, arr);

struct sv_array {
    std::vector<jsonrefl::string_view_t> arr;
};
JSONREFL_METADATA(sv_array, arr);

struct int_string {
    int i;
    std::string s;
};
JSONREFL_METADATA(int_string, i, s);

struct value_fields {
    jsonrefl::value_t n;
    jsonrefl::value_t s;
    jsonrefl::value_t b;
    jsonrefl::value_t z;
};
JSONREFL_METADATA(value_fields, n, s, b, z);

struct int_string_array {
    std::vector<int_string> v;
};
JSONREFL_METADATA(int_string_array, v);

struct nested {
    int i;
    int_string n;
    jsonrefl::string_view_t sv;
};
JSONREFL_METADATA(nested, i, n, sv);

struct nested_with_array {
    std::vector<int_string> v;
    nested n;
};
JSONREFL_METADATA(nested_with_array, n, v);

struct nested_nested_nested {
    nested_with_array a;
};
JSONREFL_METADATA(nested_nested_nested, a);

struct bool_test {
    bool flag;
    int val;
};
JSONREFL_METADATA(bool_test, flag, val);

struct null_test {
    jsonrefl::optional_t<int> opt;
    int val;
};
JSONREFL_METADATA(null_test, opt, val);

struct null_arr_test {
    jsonrefl::optional_t<std::vector<int>> arr;
    int val;
};
JSONREFL_METADATA(null_arr_test, arr, val);

struct kv_si {
    std::string s;
    int i;
};
JSONREFL_METADATA(kv_si, s, i);

struct kv_ss {
    std::string s;
    std::string t;
};
JSONREFL_METADATA(kv_ss, s, t);

/** simple struct row inside `nest_vv_pair_xy::rows` */
struct pair_xy {
    int x{};
    int y{};
};
JSONREFL_METADATA(pair_xy, x, y);

/** nested `std::vector<std::vector<…>>` lives in a struct field: root JSON is `{"rows":[[…],…]}`. */
struct nest_vv_string {
    std::vector<std::vector<std::string>> rows;
};
JSONREFL_METADATA(nest_vv_string, rows);

struct nest_vv_int {
    std::vector<std::vector<int>> rows;
};
JSONREFL_METADATA(nest_vv_int, rows);

struct nest_vv_pair_xy {
    std::vector<std::vector<pair_xy>> rows;
};
JSONREFL_METADATA(nest_vv_pair_xy, rows);

/***************************************************************************************************/

/** Nested `std::map` as JSON objects: outer keys map to inner string-key maps. */

struct nest_mm_int {
    std::map<std::string, std::map<std::string, int>> grids;
};
JSONREFL_METADATA(nest_mm_int, grids);

/** Mixed `std::map` holding `std::unordered_map` values (both serialise as JSON objects). */

struct nest_mum_int {
    std::map<std::string, std::unordered_map<std::string, int>> layers;
};
JSONREFL_METADATA(nest_mum_int, layers);

struct nest_mmm_int {
    std::map<std::string, std::map<std::string, std::map<std::string, int>>> cube;
};
JSONREFL_METADATA(nest_mmm_int, cube);

/***************************************************************************************************/

JSONREFL_STRUCT(
    point2d
    ,(double, x)
    ,(double, y)
);

/*************************************************************************************************/

enum class flavor_ec : int { vanilla = -1, chocolate = 7, strawberry = 42 };

enum legacy_num : unsigned short { leg_a = 10, leg_b = 200 };

struct with_enums {
    flavor_ec ec_field;
    legacy_num raw_enum_field;
    jsonrefl::optional_t<flavor_ec> opt_ec;
    std::vector<flavor_ec> ec_vec;
};
JSONREFL_METADATA(
    with_enums
    ,ec_field
    ,raw_enum_field
    ,opt_ec
    ,ec_vec
);

/*************************************************************************************************/

struct float_fields_t {
    float  f;
    double d;
};
JSONREFL_METADATA(float_fields_t, f, d);

struct map_vec_int_t {
    std::map<std::string, std::vector<int>> m;
};
JSONREFL_METADATA(map_vec_int_t, m);

struct two_fields_t {
    int   n;
    float f;
};
JSONREFL_METADATA(two_fields_t, n, f);

/*************************************************************************************************/
