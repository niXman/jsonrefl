
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
    std::string_view symbol;
    std::size_t preventedMatchId;
    std::size_t takerOrderId;
    std::size_t makerOrderId;
    std::size_t tradeGroupId;
    std::string_view selfTradePreventionMode;
    std::string_view price;
    std::string_view makerPreventedQuantity;
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
    std::string_view rateLimitType;
    std::string_view interval;
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
    std::string_view id;
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
    std::optional<int> opt_int;
    std::optional<std::string> opt_str;
    std::optional<double> opt_dbl;
    std::optional<std::vector<int>> opt_vec;
    std::optional<std::map<std::string, std::string>> opt_map;
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
    std::vector<std::string_view> arr;
};
JSONREFL_METADATA(sv_array, arr);

struct int_string {
    int i;
    std::string s;
};
JSONREFL_METADATA(int_string, i, s);

struct int_string_array {
    std::vector<int_string> v;
};
JSONREFL_METADATA(int_string_array, v);

struct nested {
    int i;
    int_string n;
    std::string_view sv;
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
    std::optional<int> opt;
    int val;
};
JSONREFL_METADATA(null_test, opt, val);

struct null_arr_test {
    std::optional<std::vector<int>> arr;
    int val;
};
JSONREFL_METADATA(null_arr_test, arr, val);

/*************************************************************************************************/
