
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

#include "main.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <sstream>
#include <string_view>
#include <vector>

/*************************************************************************************************/

#define CHECK(expr)                                                          \
    do {                                                                     \
        if ( !(expr) ) [[unlikely]] {                                        \
            std::fprintf(stderr, "CHECK failed: %s\n  %s:%d in %s()\n",     \
                #expr, __FILE__, __LINE__, __func__);                        \
            std::abort();                                                    \
        }                                                                    \
    } while ( 0 )

#define CHECK_EQ(a, b)                                                       \
    do {                                                                     \
        const auto &_a = (a);                                                \
        const auto &_b = (b);                                                \
        if ( !(_a == _b) ) [[unlikely]] {                                    \
            std::cerr << "CHECK_EQ failed: " #a " == " #b "\n"              \
                      << "  lhs: " << _a << "\n  rhs: " << _b               \
                      << "\n  " << __FILE__ << ":" << __LINE__               \
                      << " in " << __func__ << "()\n";                       \
            std::abort();                                                    \
        }                                                                    \
    } while ( 0 )

/*************************************************************************************************/

void test_int_array() {
    static_assert(jsonrefl::has_metadata<int_array>::value);

    constexpr auto &m = jsonrefl::metadata<int_array>();
    CHECK_EQ(m.name(), "int_array");
    CHECK_EQ(m.size(), 1u);

    const auto *e0 = m.get("arr");
    CHECK(e0);
    CHECK_EQ(e0->is_int(), false);
    CHECK_EQ(e0->is_uint(), false);
    CHECK_EQ(e0->is_double(), false);
    CHECK_EQ(e0->is_string(), false);
    CHECK_EQ(e0->is_object(), false);
    CHECK_EQ(e0->has_metadata(), false);
    CHECK_EQ(e0->is_array(), true);
}

void test_string_array() {
    static_assert(jsonrefl::has_metadata<string_array>::value);

    constexpr auto &m = jsonrefl::metadata<string_array>();
    CHECK_EQ(m.name(), "string_array");
    CHECK_EQ(m.size(), 1u);

    const auto *e0 = m.get("arr");
    CHECK(e0);
    CHECK_EQ(e0->is_int(), false);
    CHECK_EQ(e0->is_uint(), false);
    CHECK_EQ(e0->is_double(), false);
    CHECK_EQ(e0->is_string(), false);
    CHECK_EQ(e0->is_object(), false);
    CHECK_EQ(e0->has_metadata(), false);
    CHECK_EQ(e0->is_array(), true);
}

void test_sv_array() {
    static_assert(jsonrefl::has_metadata<sv_array>::value);

    constexpr auto &m = jsonrefl::metadata<sv_array>();
    CHECK_EQ(m.name(), "sv_array");
    CHECK_EQ(m.size(), 1u);

    const auto *e0 = m.get("arr");
    CHECK(e0);
    CHECK_EQ(e0->is_int(), false);
    CHECK_EQ(e0->is_uint(), false);
    CHECK_EQ(e0->is_double(), false);
    CHECK_EQ(e0->is_string(), false);
    CHECK_EQ(e0->is_object(), false);
    CHECK_EQ(e0->has_metadata(), false);
    CHECK_EQ(e0->is_array(), true);
}

void test_metadata_int_string() {
    static_assert(jsonrefl::has_metadata<int_string>::value);

    constexpr auto &m = jsonrefl::metadata<int_string>();
    CHECK_EQ(m.name(), "int_string");
    CHECK_EQ(m.size(), 2u);

    const auto *e0 = m.get("i");
    CHECK(e0);
    CHECK_EQ(e0->is_int(), true);
    CHECK_EQ(e0->is_uint(), false);
    CHECK_EQ(e0->is_double(), false);
    CHECK_EQ(e0->is_string(), false);
    CHECK_EQ(e0->is_object(), false);
    CHECK_EQ(e0->has_metadata(), false);
    CHECK_EQ(e0->is_array(), false);

    const auto *e1 = m.get("s");
    CHECK(e1);
    CHECK_EQ(e1->is_int(), false);
    CHECK_EQ(e1->is_uint(), false);
    CHECK_EQ(e1->is_double(), false);
    CHECK_EQ(e1->is_string(), true);
    CHECK_EQ(e1->is_object(), false);
    CHECK_EQ(e1->has_metadata(), false);
    CHECK_EQ(e1->is_array(), false);
}

void test_metadata_nested() {
    static_assert(jsonrefl::has_metadata<nested>::value);

    constexpr auto &m = jsonrefl::metadata<nested>();
    CHECK_EQ(m.name(), "nested");
    CHECK_EQ(m.size(), 3u);

    const auto *e0 = m.get("i");
    CHECK(e0);
    CHECK_EQ(e0->is_int(), true);
    CHECK_EQ(e0->is_uint(), false);
    CHECK_EQ(e0->is_double(), false);
    CHECK_EQ(e0->is_string(), false);
    CHECK_EQ(e0->is_object(), false);
    CHECK_EQ(e0->has_metadata(), false);
    CHECK_EQ(e0->is_array(), false);

    const auto *e1 = m.get("n");
    CHECK(e1);
    CHECK_EQ(e1->is_int(), false);
    CHECK_EQ(e1->is_uint(), false);
    CHECK_EQ(e1->is_double(), false);
    CHECK_EQ(e1->is_string(), false);
    CHECK_EQ(e1->is_object(), false);
    CHECK_EQ(e1->has_metadata(), true);
    CHECK_EQ(e1->is_array(), false);

    const auto *e2 = m.get("sv");
    CHECK(e2);
    CHECK_EQ(e2->is_int(), false);
    CHECK_EQ(e2->is_uint(), false);
    CHECK_EQ(e2->is_double(), false);
    CHECK_EQ(e2->is_string(), true);
    CHECK_EQ(e2->is_object(), false);
    CHECK_EQ(e2->has_metadata(), false);
    CHECK_EQ(e2->is_array(), false);
}

void test_metadata_nested_with_array() {
    static_assert(jsonrefl::has_metadata<nested_with_array>::value);

    constexpr auto &m = jsonrefl::metadata<nested_with_array>();
    CHECK_EQ(m.name(), "nested_with_array");
    CHECK_EQ(m.size(), 2u);

    const auto *e0 = m.get("n");
    CHECK(e0);
    CHECK_EQ(e0->is_int(), false);
    CHECK_EQ(e0->is_uint(), false);
    CHECK_EQ(e0->is_double(), false);
    CHECK_EQ(e0->is_string(), false);
    CHECK_EQ(e0->is_object(), false);
    CHECK_EQ(e0->has_metadata(), true);
    CHECK_EQ(e0->is_array(), false);

    const auto *e1 = m.get("v");
    CHECK(e1);
    CHECK_EQ(e1->is_int(), false);
    CHECK_EQ(e1->is_uint(), false);
    CHECK_EQ(e1->is_double(), false);
    CHECK_EQ(e1->is_string(), false);
    CHECK_EQ(e1->is_object(), false);
    CHECK_EQ(e1->has_metadata(), false);
    CHECK_EQ(e1->is_array(), true);
}

void test_metadata_result_t() {
    static_assert(jsonrefl::has_metadata<myns::result_t>::value);

    constexpr auto &m = jsonrefl::metadata<myns::result_t>();
    CHECK_EQ(m.name(), "result_t");
    CHECK_EQ(m.size(), 9u);

    const auto *e_symbol = m.get("symbol");
    CHECK(e_symbol);
    CHECK_EQ(e_symbol->is_string(), true);
    CHECK_EQ(e_symbol->is_int(), false);
    CHECK_EQ(e_symbol->is_array(), false);

    const auto *e_pmid = m.get("preventedMatchId");
    CHECK(e_pmid);
    CHECK_EQ(e_pmid->is_int(), true);
    CHECK_EQ(e_pmid->is_string(), false);

    const auto *e_toid = m.get("takerOrderId");
    CHECK(e_toid);
    CHECK_EQ(e_toid->is_int(), true);

    const auto *e_moid = m.get("makerOrderId");
    CHECK(e_moid);
    CHECK_EQ(e_moid->is_int(), true);

    const auto *e_tgid = m.get("tradeGroupId");
    CHECK(e_tgid);
    CHECK_EQ(e_tgid->is_int(), true);

    const auto *e_stpm = m.get("selfTradePreventionMode");
    CHECK(e_stpm);
    CHECK_EQ(e_stpm->is_string(), true);

    const auto *e_price = m.get("price");
    CHECK(e_price);
    CHECK_EQ(e_price->is_string(), true);

    const auto *e_mpq = m.get("makerPreventedQuantity");
    CHECK(e_mpq);
    CHECK_EQ(e_mpq->is_string(), true);

    const auto *e_tt = m.get("transactTime");
    CHECK(e_tt);
    CHECK_EQ(e_tt->is_int(), true);

    CHECK_EQ(m.get("nonexistent"), nullptr);
}

void test_metadata_rate_limits_t() {
    static_assert(jsonrefl::has_metadata<myns::rate_limits_t>::value);

    constexpr auto &m = jsonrefl::metadata<myns::rate_limits_t>();
    CHECK_EQ(m.name(), "rate_limits_t");
    CHECK_EQ(m.size(), 7u);

    const auto *e_rlt = m.get("rateLimitType");
    CHECK(e_rlt);
    CHECK_EQ(e_rlt->is_string(), true);

    const auto *e_int = m.get("interval");
    CHECK(e_int);
    CHECK_EQ(e_int->is_string(), true);

    const auto *e_inum = m.get("intervalNum");
    CHECK(e_inum);
    CHECK_EQ(e_inum->is_int(), true);

    const auto *e_lim = m.get("limit");
    CHECK(e_lim);
    CHECK_EQ(e_lim->is_int(), true);

    const auto *e_cnt = m.get("count");
    CHECK(e_cnt);
    CHECK_EQ(e_cnt->is_int(), true);

    const auto *e_extra = m.get("extra");
    CHECK(e_extra);
    CHECK_EQ(e_extra->is_object(), true);
    CHECK_EQ(e_extra->is_array(), false);
    CHECK_EQ(e_extra->has_metadata(), false);

    const auto *e_codes = m.get("codes");
    CHECK(e_codes);
    CHECK_EQ(e_codes->is_array(), true);
    CHECK_EQ(e_codes->is_object(), false);
    CHECK_EQ(e_codes->has_metadata(), false);
}

void test_metadata_response_t() {
    static_assert(jsonrefl::has_metadata<myns::response_t>::value);

    constexpr auto &m = jsonrefl::metadata<myns::response_t>();
    CHECK_EQ(m.name(), "response_t");
    CHECK_EQ(m.size(), 6u);

    const auto *e_id = m.get("id");
    CHECK(e_id);
    CHECK_EQ(e_id->is_string(), true);

    const auto *e_st = m.get("status");
    CHECK(e_st);
    CHECK_EQ(e_st->is_int(), true);

    const auto *e_res = m.get("results");
    CHECK(e_res);
    CHECK_EQ(e_res->is_array(), true);
    CHECK(e_res->get_element_metadata() != nullptr);

    const auto *e_rl = m.get("rate_limits");
    CHECK(e_rl);
    CHECK_EQ(e_rl->is_array(), true);
    CHECK(e_rl->get_element_metadata() != nullptr);

    const auto *e_hdr = m.get("headers");
    CHECK(e_hdr);
    CHECK_EQ(e_hdr->is_object(), true);
    CHECK_EQ(e_hdr->is_array(), false);
    CHECK_EQ(e_hdr->has_metadata(), false);

    const auto *e_warn = m.get("warnings");
    CHECK(e_warn);
    CHECK_EQ(e_warn->is_array(), true);
    CHECK_EQ(e_warn->is_object(), false);
    CHECK_EQ(e_warn->has_metadata(), false);
}

struct empty {};

void test_calc_max_stack_depth() {
    {
        constexpr auto v0 = jsonrefl::stack_depth<empty>();
        static_assert(v0 == 0);
        std::cout << "empty=" << v0 << std::endl << std::endl;
    }

    {
        constexpr auto v0 = jsonrefl::stack_depth<int_array>();
        static_assert(v0 == 2);
        std::cout << "int_array=" << v0 << std::endl << std::endl;
    }

    {
        constexpr auto v1 = jsonrefl::stack_depth<int_string>();
        static_assert(v1 == 1);
        std::cout << "int_string=" << v1 << std::endl << std::endl;
    }

    {
        constexpr auto v2 = jsonrefl::stack_depth<nested>();
        static_assert(v2 == 2);
        std::cout << "nested=" << v2 << std::endl << std::endl;
    }

    {
        constexpr auto v3 = jsonrefl::stack_depth<nested_with_array>();
        static_assert(v3 == 3);
        std::cout << "nested_with_array=" << v3 << std::endl << std::endl;
    }

    {
        constexpr auto v4 = jsonrefl::stack_depth<nested_nested_nested>();
        static_assert(v4 == 4);
        std::cout << "nested_nested_nested=" << v4 << std::endl << std::endl;
    }
}

/*************************************************************************************************/

static std::vector<std::string> split_lines(const std::string &s) {
    std::vector<std::string> lines;
    std::istringstream iss(s);
    for ( std::string line; std::getline(iss, line); )
        if ( !line.empty() ) lines.push_back(line);
    return lines;
}

void test_dump() {
    constexpr auto &meta = jsonrefl::metadata<myns::response_t>();
    std::ostringstream oss;
    meta.dump(oss);
    const auto lines = split_lines(oss.str());

    // response_t has 6 members → 1 header + 6 members + 1 header + 6 index = 14
    CHECK_EQ(lines.size(), 14u);

    // --- section 1: members (lines[0] = header, lines[1..6] = members) ---
    CHECK(lines[0].find("-- hash --") != std::string::npos);
    CHECK(lines[0].find("-- name --") != std::string::npos);

    struct expected_member {
        std::uint32_t hash;
        std::string_view name;
        char type_id;
    };
    // declaration order
    const expected_member expected[] = {
         {0x37386ae0, "id",          'V'}
        ,{0xba4b77ef, "status",      'U'}
        ,{0x6c4f4af5, "results",     'A'}
        ,{0xdb31ea38, "rate_limits", 'A'}
        ,{0xd665d9e9, "headers",     'O'}
        ,{0x4b104b94, "warnings",    'A'}
    };

    std::vector<std::uint32_t> member_hashes;
    for ( int i = 0; i < 6; ++i ) {
        const auto &line = lines[1 + i];

        // verify hash: "0xHHHHHHHH"
        char hash_buf[11];
        std::snprintf(hash_buf, sizeof(hash_buf), "0x%08x", expected[i].hash);
        CHECK(line.find(hash_buf) != std::string::npos);

        // verify name
        CHECK(line.find(std::string(" : ") + std::string(expected[i].name)) != std::string::npos);

        // verify type_id (last meaningful char)
        CHECK_EQ(line.back(), expected[i].type_id);

        member_hashes.push_back(expected[i].hash);
    }

    // --- section 2: index (lines[7] = header, lines[8..13] = index entries) ---
    CHECK(lines[7].find("-- hash --") != std::string::npos);
    CHECK(lines[7].find("-- address --") != std::string::npos);

    std::vector<std::uint32_t> index_hashes;
    for ( int i = 0; i < 6; ++i ) {
        const auto &line = lines[8 + i];
        // parse "0xHHHHHHHH: ..."
        auto hash = static_cast<std::uint32_t>(std::stoul(line.substr(2, 8), nullptr, 16));
        index_hashes.push_back(hash);
    }

    // verify same set of hashes in both sections
    auto sorted_member = member_hashes;
    std::sort(index_hashes.begin(), index_hashes.end());
    std::sort(sorted_member.begin(), sorted_member.end());
    CHECK(sorted_member == index_hashes);

    std::cout << "test_dump: OK" << std::endl;
}

void test_optional() {
    // all nullopt → all fields serialize as "null" except regular
    {
        myns::optional_test_t obj{};
        obj.regular = 42;
        auto json = jsonrefl::to_string(obj);
        CHECK(json.find("\"opt_int\":null") != std::string::npos);
        CHECK(json.find("\"opt_str\":null") != std::string::npos);
        CHECK(json.find("\"opt_dbl\":null") != std::string::npos);
        CHECK(json.find("\"opt_vec\":null") != std::string::npos);
        CHECK(json.find("\"opt_map\":null") != std::string::npos);
        CHECK(json.find("\"regular\":42")   != std::string::npos);
    }
    // with values → serialize inner values
    {
        myns::optional_test_t obj{};
        obj.opt_int = 7;
        obj.opt_str = "hello";
        obj.opt_dbl = 3.14;
        obj.opt_vec = std::vector<int>{1, 2, 3};
        obj.opt_map = std::map<std::string, std::string>{{"a", "b"}};
        obj.regular = 99;
        auto json = jsonrefl::to_string(obj);
        CHECK(json.find("\"opt_int\":7") != std::string::npos);
        CHECK(json.find("\"opt_str\":\"hello\"") != std::string::npos);
        CHECK(json.find("\"opt_vec\":[1,2,3]") != std::string::npos);
        CHECK(json.find("\"opt_map\":{\"a\":\"b\"}") != std::string::npos);
        CHECK(json.find("\"regular\":99") != std::string::npos);
    }
    // required_bytes matches actual serialized length
    {
        myns::optional_test_t obj{};
        obj.opt_int = 100;
        obj.regular = 1;
        auto json = jsonrefl::to_string(obj);
        CHECK_EQ(jsonrefl::required_bytes(obj), json.size());

        auto json_p = jsonrefl::to_string(obj, true);
        CHECK_EQ(jsonrefl::required_bytes(obj, true), json_p.size());
    }
    // to_buffer roundtrip
    {
        myns::optional_test_t obj{};
        obj.opt_str = "world";
        obj.opt_vec = std::vector<int>{10, 20};
        obj.regular = 0;
        auto n = jsonrefl::required_bytes(obj);
        auto buf = std::make_unique<char[]>(n + 1);
        auto end = jsonrefl::to_buffer(buf.get(), obj);
        CHECK_EQ(static_cast<std::size_t>(end - buf.get()), n);
        std::string from_buf(buf.get(), n);
        CHECK_EQ(from_buf, jsonrefl::to_string(obj));
    }

    std::cout << "test_optional: OK" << std::endl;
}

/*************************************************************************************************/

static std::vector<myns::response_t> make_test_data() {
    static constexpr const char *symbols[] = {
        "BTCUSDT", "ETHUSDT", "BNBUSDT", "XRPUSDT", "SOLUSDT",
        "ADAUSDT", "DOGEUSDT", "DOTUSDT", "AVAXUSDT", "MATICUSDT"
    };
    static constexpr const char *modes[] = {
        "EXPIRE_MAKER", "NONE", "EXPIRE_TAKER", "EXPIRE_BOTH"
    };
    static constexpr const char *prices[] = {
        "50000.00", "3000.50", "580.75", "0.6234", "102.40",
        "0.4521", "0.0812", "6.95", "35.60", "0.8930"
    };
    static constexpr const char *quantities[] = {
        "0.001", "0.5", "1.25", "100.0", "50.5",
        "250.0", "1000.0", "10.0", "5.555", "0.01"
    };
    static constexpr const char *rl_types[] = {"ORDERS", "REQUEST_WEIGHT", "RAW_REQUESTS"};
    static constexpr const char *intervals[] = {"SECOND", "MINUTE", "HOUR", "DAY"};
    static constexpr const char *ids[] = {
        "resp-001", "resp-002", "resp-003", "resp-004", "resp-005",
        "resp-006", "resp-007", "resp-008", "resp-009", "resp-010"
    };
    static constexpr const char *warnings_pool[] = {
        "rate limit approaching", "deprecated endpoint",
        "maintenance scheduled", "high latency detected",
        "api version outdated"
    };

    std::vector<myns::response_t> data(10);
    for ( auto i = 0u; i < 10; ++i ) {
        auto &r = data[i];
        r.id = ids[i];
        r.status = 200 + i;

        auto nresults = 2 + (i % 3);
        for ( auto j = 0u; j < nresults; ++j ) {
            myns::result_t res;
            res.symbol = symbols[(i + j) % 10];
            res.preventedMatchId = i * 100 + j;
            res.takerOrderId = 1000 + i * 10 + j;
            res.makerOrderId = 2000 + i * 10 + j;
            res.tradeGroupId = 3000 + i * 10 + j;
            res.selfTradePreventionMode = modes[(i + j) % 4];
            res.price = prices[(i + j) % 10];
            res.makerPreventedQuantity = quantities[(i + j) % 10];
            res.transactTime = 1700000000 + i * 1000 + j;
            r.results.push_back(res);
        }

        auto nrl = 1 + (i % 3);
        for ( auto j = 0u; j < nrl; ++j ) {
            myns::rate_limits_t rl;
            rl.rateLimitType = rl_types[(i + j) % 3];
            rl.interval = intervals[(i + j) % 4];
            rl.intervalNum = 1 + (i + j) % 10;
            rl.limit = 50 * (1 + j);
            rl.count = 1 + i + j;
            rl.extra[std::string("key") + std::to_string(j)] = std::string("val") + std::to_string(i);
            for ( auto k = 0u; k < 2 + j; ++k )
                rl.codes.push_back(static_cast<int>(100 + i * 10 + k));
            r.rate_limits.push_back(std::move(rl));
        }

        r.headers["Content-Type"] = "application/json";
        r.headers[std::string("X-Req-") + std::to_string(i)] = std::string("hdr-") + std::to_string(i);

        for ( auto w = 0u; w < 1 + (i % 3); ++w )
            r.warnings.push_back(warnings_pool[(i + w) % 5]);
    }

    return data;
}

/*************************************************************************************************/

bool test_parse_int_array() {
    const auto json = R"({"arr":[3, 2, 1, 4, 5, 6]})";
    int_array obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json) != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.arr.size(), 6u);
    CHECK_EQ(obj.arr[0], 3);
    CHECK_EQ(obj.arr[1], 2);
    CHECK_EQ(obj.arr[2], 1);
    CHECK_EQ(obj.arr[3], 4);
    CHECK_EQ(obj.arr[4], 5);
    CHECK_EQ(obj.arr[5], 6);
    return true;
}

bool test_parse_string_array() {
    const auto json = R"({"arr":["hello","world","foo"]})";
    string_array obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json) != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.arr.size(), 3u);
    CHECK_EQ(obj.arr[0], "hello");
    CHECK_EQ(obj.arr[1], "world");
    CHECK_EQ(obj.arr[2], "foo");
    return true;
}

bool test_parse_sv_array() {
    const auto json = R"({"arr":["alpha","beta","gamma"]})";
    sv_array obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json) != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.arr.size(), 3u);
    CHECK_EQ(obj.arr[0], "alpha");
    CHECK_EQ(obj.arr[1], "beta");
    CHECK_EQ(obj.arr[2], "gamma");
    return true;
}

bool test_parse_int_string() {
    const auto json = R"({"i":42,"s":"hello"})";
    int_string obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json) != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.i, 42);
    CHECK_EQ(obj.s, "hello");
    return true;
}

bool test_parse_int_string_array() {
    const auto json = R"({"v":[{"i":1,"s":"one"},{"i":2,"s":"two"},{"i":3,"s":"three"}]})";
    int_string_array obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json) != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.v.size(), 3u);
    CHECK_EQ(obj.v[0].i, 1);
    CHECK_EQ(obj.v[0].s, "one");
    CHECK_EQ(obj.v[1].i, 2);
    CHECK_EQ(obj.v[1].s, "two");
    CHECK_EQ(obj.v[2].i, 3);
    CHECK_EQ(obj.v[2].s, "three");
    return true;
}

bool test_parse_nested() {
    const auto json = R"({"i":10,"n":{"i":20,"s":"inner"},"sv":"view"})";
    nested obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json) != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.i, 10);
    CHECK_EQ(obj.n.i, 20);
    CHECK_EQ(obj.n.s, "inner");
    CHECK_EQ(obj.sv, "view");
    return true;
}

bool test_parse_nested_with_array() {
    const auto json = R"({"n":{"i":5,"n":{"i":6,"s":"deep"},"sv":"sv_val"},"v":[{"i":7,"s":"a"},{"i":8,"s":"b"}]})";
    nested_with_array obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json) != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.n.i, 5);
    CHECK_EQ(obj.n.n.i, 6);
    CHECK_EQ(obj.n.n.s, "deep");
    CHECK_EQ(obj.n.sv, "sv_val");
    CHECK_EQ(obj.v.size(), 2u);
    CHECK_EQ(obj.v[0].i, 7);
    CHECK_EQ(obj.v[0].s, "a");
    CHECK_EQ(obj.v[1].i, 8);
    CHECK_EQ(obj.v[1].s, "b");
    return true;
}

bool test_parse_nested_nested_nested() {
    const auto json = R"({"a":{"n":{"i":1,"n":{"i":2,"s":"leaf"},"sv":"sv1"},"v":[{"i":3,"s":"x"}]}})";
    nested_nested_nested obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json) != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.a.n.i, 1);
    CHECK_EQ(obj.a.n.n.i, 2);
    CHECK_EQ(obj.a.n.n.s, "leaf");
    CHECK_EQ(obj.a.n.sv, "sv1");
    CHECK_EQ(obj.a.v.size(), 1u);
    CHECK_EQ(obj.a.v[0].i, 3);
    CHECK_EQ(obj.a.v[0].s, "x");
    return true;
}

bool test_parse_result_t() {
    const auto json = R"({"symbol":"BTCUSDT","preventedMatchId":42,"takerOrderId":100,"makerOrderId":200,"tradeGroupId":300,"selfTradePreventionMode":"NONE","price":"50000.00","makerPreventedQuantity":"0.5","transactTime":1700000000})";
    myns::result_t obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json) != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.symbol, "BTCUSDT");
    CHECK_EQ(obj.preventedMatchId, 42u);
    CHECK_EQ(obj.takerOrderId, 100u);
    CHECK_EQ(obj.makerOrderId, 200u);
    CHECK_EQ(obj.tradeGroupId, 300u);
    CHECK_EQ(obj.selfTradePreventionMode, "NONE");
    CHECK_EQ(obj.price, "50000.00");
    CHECK_EQ(obj.makerPreventedQuantity, "0.5");
    CHECK_EQ(obj.transactTime, 1700000000u);
    return true;
}

bool test_parse_rate_limits_t() {
    const auto json = R"({"rateLimitType":"ORDERS","interval":"SECOND","intervalNum":10,"limit":50,"count":3,"extra":{"source":"api","region":"us"},"codes":[100,200,300]})";
    myns::rate_limits_t obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json) != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.rateLimitType, "ORDERS");
    CHECK_EQ(obj.interval, "SECOND");
    CHECK_EQ(obj.intervalNum, 10u);
    CHECK_EQ(obj.limit, 50u);
    CHECK_EQ(obj.count, 3u);
    CHECK_EQ(obj.extra.size(), 2u);
    CHECK_EQ(obj.extra.at("source"), "api");
    CHECK_EQ(obj.extra.at("region"), "us");
    CHECK_EQ(obj.codes.size(), 3u);
    { auto it = obj.codes.begin(); CHECK_EQ(*it, 100); ++it; CHECK_EQ(*it, 200); ++it; CHECK_EQ(*it, 300); }
    return true;
}

bool test_parse_response_t() {
    const auto json = R"({"id":"resp-001","status":200,"results":[{"symbol":"BTCUSDT","preventedMatchId":1,"takerOrderId":100,"makerOrderId":200,"tradeGroupId":300,"selfTradePreventionMode":"EXPIRE_MAKER","price":"50000.00","makerPreventedQuantity":"0.001","transactTime":1700000000}],"rate_limits":[{"rateLimitType":"ORDERS","interval":"SECOND","intervalNum":10,"limit":50,"count":3,"extra":{"src":"api"},"codes":[100,200]}],"headers":{"Content-Type":"application/json"},"warnings":["warning1","warning2"]})";
    myns::response_t obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json) != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.id, "resp-001");
    CHECK_EQ(obj.status, 200u);
    CHECK_EQ(obj.results.size(), 1u);
    CHECK_EQ(obj.results[0].symbol, "BTCUSDT");
    CHECK_EQ(obj.results[0].preventedMatchId, 1u);
    CHECK_EQ(obj.results[0].takerOrderId, 100u);
    CHECK_EQ(obj.results[0].makerOrderId, 200u);
    CHECK_EQ(obj.results[0].tradeGroupId, 300u);
    CHECK_EQ(obj.results[0].selfTradePreventionMode, "EXPIRE_MAKER");
    CHECK_EQ(obj.results[0].price, "50000.00");
    CHECK_EQ(obj.results[0].makerPreventedQuantity, "0.001");
    CHECK_EQ(obj.results[0].transactTime, 1700000000u);
    CHECK_EQ(obj.rate_limits.size(), 1u);
    CHECK_EQ(obj.rate_limits[0].rateLimitType, "ORDERS");
    CHECK_EQ(obj.rate_limits[0].interval, "SECOND");
    CHECK_EQ(obj.rate_limits[0].intervalNum, 10u);
    CHECK_EQ(obj.rate_limits[0].limit, 50u);
    CHECK_EQ(obj.rate_limits[0].count, 3u);
    CHECK_EQ(obj.rate_limits[0].extra.size(), 1u);
    CHECK_EQ(obj.rate_limits[0].extra.at("src"), "api");
    CHECK_EQ(obj.rate_limits[0].codes.size(), 2u);
    { auto it = obj.rate_limits[0].codes.begin(); CHECK_EQ(*it, 100); ++it; CHECK_EQ(*it, 200); }
    CHECK_EQ(obj.headers.size(), 1u);
    CHECK_EQ(obj.headers.at("Content-Type"), "application/json");
    CHECK_EQ(obj.warnings.size(), 2u);
    { auto it = obj.warnings.begin(); CHECK_EQ(*it, "warning1"); ++it; CHECK_EQ(*it, "warning2"); }
    return true;
}

/*************************************************************************************************/

int main() {
    test_int_array();
    test_string_array();
    test_sv_array();
    test_metadata_int_string();
    test_metadata_nested();
    test_metadata_nested_with_array();
    test_metadata_result_t();
    test_metadata_rate_limits_t();
    test_metadata_response_t();
    test_calc_max_stack_depth();
    test_dump();
    test_optional();

    {
        int_string is;
        auto p = jsonrefl::make_parser(&is);
        p.open_object();
            p.on_key("s");
            p.on_str("string");
            CHECK_EQ(is.s, "string");
            p.on_key("i");
            p.on_int("123");
            CHECK_EQ(is.i, 123);
        p.close_object();
    }

    {
        std::map<std::string_view, std::string_view> root;
        auto p = jsonrefl::make_parser(&root);
        p.open_object();
            p.on_key("s");
            p.on_str("string");
            CHECK_EQ(root["s"], "string");
            p.on_key("i");
            p.on_int("123");
            CHECK_EQ(root["i"], "123");
        p.close_object();
        CHECK_EQ(root.size(), 2u);
    }
    {
        std::map<std::string, int> root;
        auto p = jsonrefl::make_parser(&root);
        p.open_object();
            p.on_key("a");
            p.on_int("10");
            p.on_key("b");
            p.on_int("-20");
            p.on_key("c");
            p.on_int("0");
        p.close_object();
        CHECK_EQ(root.size(), 3u);
        CHECK_EQ(root.at("a"), 10);
        CHECK_EQ(root.at("b"), -20);
        CHECK_EQ(root.at("c"), 0);
        // {"a":10,"b":-20,"c":0} = 22
        CHECK_EQ(jsonrefl::required_bytes(root), 22u);
        {
            char buf[22];
            auto end = jsonrefl::to_buffer(buf, root);
            CHECK_EQ(end - buf, 22);
            CHECK_EQ(std::string_view(buf, 22), "{\"a\":10,\"b\":-20,\"c\":0}");
        }
    }
    {
        std::map<std::string, std::size_t> root;
        auto p = jsonrefl::make_parser(&root);
        p.open_object();
            p.on_key("x");
            p.on_int("999");
            p.on_key("y");
            p.on_int("0");
        p.close_object();
        CHECK_EQ(root.size(), 2u);
        CHECK_EQ(root.at("x"), 999u);
        CHECK_EQ(root.at("y"), 0u);
    }
    {
        std::map<std::string, double> root;
        auto p = jsonrefl::make_parser(&root);
        p.open_object();
            p.on_key("pi");
            p.on_str("3.14");
            p.on_key("e");
            p.on_str("2.718");
            p.on_key("neg");
            p.on_str("-1.5");
        p.close_object();
        CHECK_EQ(root.size(), 3u);
        CHECK(root.at("pi") > 3.13 && root.at("pi") < 3.15);
        CHECK(root.at("e") > 2.71 && root.at("e") < 2.72);
        CHECK(root.at("neg") > -1.51 && root.at("neg") < -1.49);
    }
    {
        std::map<std::string, float> root;
        auto p = jsonrefl::make_parser(&root);
        p.open_object();
            p.on_key("val");
            p.on_str("1.25");
        p.close_object();
        CHECK_EQ(root.size(), 1u);
        CHECK(root.at("val") > 1.24f && root.at("val") < 1.26f);
    }

    {
        std::unordered_map<std::string, std::string> root;
        auto p = jsonrefl::make_parser(&root);
        p.open_object();
            p.on_key("name");
            p.on_str("test");
            p.on_key("value");
            p.on_str("42");
        p.close_object();
        CHECK_EQ(root.size(), 2u);
        CHECK_EQ(root.at("name"), "test");
        CHECK_EQ(root.at("value"), "42");
    }
    {
        std::unordered_map<std::string, int> root;
        auto p = jsonrefl::make_parser(&root);
        p.open_object();
            p.on_key("x");
            p.on_int("100");
            p.on_key("y");
            p.on_int("-50");
            p.on_key("z");
            p.on_int("0");
        p.close_object();
        CHECK_EQ(root.size(), 3u);
        CHECK_EQ(root.at("x"), 100);
        CHECK_EQ(root.at("y"), -50);
        CHECK_EQ(root.at("z"), 0);
    }
    {
        std::unordered_map<std::string, std::size_t> root;
        auto p = jsonrefl::make_parser(&root);
        p.open_object();
            p.on_key("count");
            p.on_int("42");
        p.close_object();
        CHECK_EQ(root.size(), 1u);
        CHECK_EQ(root.at("count"), 42u);
    }
    {
        std::unordered_map<std::string, double> root;
        auto p = jsonrefl::make_parser(&root);
        p.open_object();
            p.on_key("rate");
            p.on_str("0.05");
            p.on_key("total");
            p.on_str("123.456");
        p.close_object();
        CHECK_EQ(root.size(), 2u);
        CHECK(root.at("rate") > 0.049 && root.at("rate") < 0.051);
        CHECK(root.at("total") > 123.45 && root.at("total") < 123.46);
    }
    {
        std::unordered_map<std::string, float> root;
        auto p = jsonrefl::make_parser(&root);
        p.open_object();
            p.on_key("val");
            p.on_str("9.5");
        p.close_object();
        CHECK_EQ(root.size(), 1u);
        CHECK(root.at("val") > 9.4f && root.at("val") < 9.6f);
    }

    {
        std::list<int> root;
        auto p = jsonrefl::make_parser(&root);
        p.open_array();
            p.on_int("10");
            p.on_int("20");
            p.on_int("30");
        p.close_array();
        CHECK_EQ(root.size(), 3u);
        auto it = root.begin();
        CHECK_EQ(*it, 10); ++it;
        CHECK_EQ(*it, 20); ++it;
        CHECK_EQ(*it, 30);
        // [10,20,30] = 10
        CHECK_EQ(jsonrefl::required_bytes(root), 10u);
        {
            char buf[10];
            auto end = jsonrefl::to_buffer(buf, root);
            CHECK_EQ(end - buf, 10);
            CHECK_EQ(std::string_view(buf, 10), "[10,20,30]");
        }
    }

    {
        std::vector<int> root;
        auto p = jsonrefl::make_parser(&root);
        p.open_array();
            p.on_int("1");
            p.on_int("2");
            p.on_int("3");
            p.on_int("4");
            p.on_int("5");
        p.close_array();
        CHECK_EQ(root.size(), 5u);
        CHECK_EQ(root[0], 1);
        CHECK_EQ(root[1], 2);
        CHECK_EQ(root[2], 3);
        CHECK_EQ(root[3], 4);
        CHECK_EQ(root[4], 5);
        // [1,2,3,4,5] = 11
        CHECK_EQ(jsonrefl::required_bytes(root), 11u);
        {
            char buf[11];
            auto end = jsonrefl::to_buffer(buf, root);
            CHECK_EQ(end - buf, 11);
            CHECK_EQ(std::string_view(buf, 11), "[1,2,3,4,5]");
        }
    }
    {
        std::vector<std::string> root;
        auto p = jsonrefl::make_parser(&root);
        p.open_array();
            p.on_str("hello");
            p.on_str("world");
        p.close_array();
        CHECK_EQ(root.size(), 2u);
        CHECK_EQ(root[0], "hello");
        CHECK_EQ(root[1], "world");
        // ["hello","world"] = 17
        CHECK_EQ(jsonrefl::required_bytes(root), 17u);
        // [\n   "hello",\n   "world"\n] = 26
        CHECK_EQ(jsonrefl::required_bytes(root, true), 26u);
    }

    {
        nested n;
        auto p = jsonrefl::make_parser(&n);
        p.open_object();
            p.on_key("i");
            p.on_int("123");
            CHECK_EQ(n.i, 123);
            p.on_key("n");
            p.open_object();
                p.on_key("s");
                p.on_str("string0");
                CHECK_EQ(n.n.s, "string0");
                p.on_key("i");
                p.on_int("321");
                CHECK_EQ(n.n.i, 321);
            p.close_object();
            p.on_key("sv");
            p.on_str("string1");
            CHECK_EQ(n.sv, "string1");
        p.close_object();
    }

    {
        nested_with_array root;
        auto p = jsonrefl::make_parser(&root);

        p.open_object(); // root
            p.on_key("n");
            p.open_object(); // root -> n
                p.on_key("i");
                p.on_int("123"); // root -> n = i
                CHECK_EQ(root.n.i, 123);
                p.on_key("n");
                p.open_object(); // root -> n -> n
                    p.on_key("s");
                    p.on_str("string0"); // root -> n -> n = s
                    CHECK_EQ(root.n.n.s, "string0");
                    p.on_key("i");
                    p.on_int("321"); // root -> n -> n = i
                    CHECK_EQ(root.n.n.i, 321);
                p.close_object(); // root -> n
                p.on_key("sv");
                p.on_str("string1"); // root -> n = sv
                CHECK_EQ(root.n.sv, "string1");
            p.close_object(); // root
            p.on_key("v");
            p.open_array();
            p.close_array();
        p.close_object();
    }

    {
        int_array root;
        auto p = jsonrefl::make_parser(&root);

        p.open_object(); // root
            p.on_key("arr");
            p.open_array();
                p.on_int("123");
                p.on_int("321");
            p.close_array();
        p.close_object();

        CHECK_EQ(root.arr.size(), 2u);
        CHECK_EQ(root.arr[0], 123);
        CHECK_EQ(root.arr[1], 321);
    }

    {
        int_string_array root;
        auto p = jsonrefl::make_parser(&root);

        p.open_object();
            p.on_key("v");
            p.open_array();
                p.open_object();
                    p.on_key("i");
                    p.on_int("42");
                    p.on_key("s");
                    p.on_str("hello");
                p.close_object();
                p.open_object();
                    p.on_key("i");
                    p.on_int("99");
                    p.on_key("s");
                    p.on_str("world");
                p.close_object();
            p.close_array();
        p.close_object();

        CHECK_EQ(root.v.size(), 2u);
        CHECK_EQ(root.v[0].i, 42);
        CHECK_EQ(root.v[0].s, "hello");
        CHECK_EQ(root.v[1].i, 99);
        CHECK_EQ(root.v[1].s, "world");
    }

    {
        myns::result_t r;
        auto p = jsonrefl::make_parser(&r);
        p.open_object();
            p.on_key("symbol");
            p.on_str("BTCUSDT");
            p.on_key("preventedMatchId");
            p.on_int("1");
            p.on_key("takerOrderId");
            p.on_int("100");
            p.on_key("makerOrderId");
            p.on_int("200");
            p.on_key("tradeGroupId");
            p.on_int("300");
            p.on_key("selfTradePreventionMode");
            p.on_str("EXPIRE_MAKER");
            p.on_key("price");
            p.on_str("50000.00");
            p.on_key("makerPreventedQuantity");
            p.on_str("0.001");
            p.on_key("transactTime");
            p.on_int("1234567890");
        p.close_object();

        CHECK_EQ(r.symbol, "BTCUSDT");
        CHECK_EQ(r.preventedMatchId, 1u);
        CHECK_EQ(r.takerOrderId, 100u);
        CHECK_EQ(r.makerOrderId, 200u);
        CHECK_EQ(r.tradeGroupId, 300u);
        CHECK_EQ(r.selfTradePreventionMode, "EXPIRE_MAKER");
        CHECK_EQ(r.price, "50000.00");
        CHECK_EQ(r.makerPreventedQuantity, "0.001");
        CHECK_EQ(r.transactTime, 1234567890u);
    }

    {
        myns::rate_limits_t rl;
        auto p = jsonrefl::make_parser(&rl);
        p.open_object();
            p.on_key("rateLimitType");
            p.on_str("ORDERS");
            p.on_key("interval");
            p.on_str("SECOND");
            p.on_key("intervalNum");
            p.on_int("10");
            p.on_key("limit");
            p.on_int("50");
            p.on_key("count");
            p.on_int("3");
            p.on_key("extra");
            p.open_object();
                p.on_key("source");
                p.on_str("api");
            p.close_object();
        p.close_object();

        CHECK_EQ(rl.rateLimitType, "ORDERS");
        CHECK_EQ(rl.interval, "SECOND");
        CHECK_EQ(rl.intervalNum, 10u);
        CHECK_EQ(rl.limit, 50u);
        CHECK_EQ(rl.count, 3u);
        CHECK_EQ(rl.extra.size(), 1u);
        CHECK_EQ(rl.extra.at("source"), "api");
    }

    {
        myns::response_t resp;
        auto p = jsonrefl::make_parser(&resp);

        p.open_object();
            p.on_key("id");
            p.on_str("resp-001");
            p.on_key("status");
            p.on_int("200");

            p.on_key("results");
            p.open_array();
                p.open_object();
                    p.on_key("symbol");
                    p.on_str("BTCUSDT");
                    p.on_key("preventedMatchId");
                    p.on_int("1");
                    p.on_key("takerOrderId");
                    p.on_int("100");
                    p.on_key("makerOrderId");
                    p.on_int("200");
                    p.on_key("tradeGroupId");
                    p.on_int("300");
                    p.on_key("selfTradePreventionMode");
                    p.on_str("EXPIRE_MAKER");
                    p.on_key("price");
                    p.on_str("50000.00");
                    p.on_key("makerPreventedQuantity");
                    p.on_str("0.001");
                    p.on_key("transactTime");
                    p.on_int("1700000000");
                p.close_object();
                p.open_object();
                    p.on_key("symbol");
                    p.on_str("ETHUSDT");
                    p.on_key("preventedMatchId");
                    p.on_int("2");
                    p.on_key("takerOrderId");
                    p.on_int("101");
                    p.on_key("makerOrderId");
                    p.on_int("201");
                    p.on_key("tradeGroupId");
                    p.on_int("301");
                    p.on_key("selfTradePreventionMode");
                    p.on_str("NONE");
                    p.on_key("price");
                    p.on_str("3000.50");
                    p.on_key("makerPreventedQuantity");
                    p.on_str("0.5");
                    p.on_key("transactTime");
                    p.on_int("1700000001");
                p.close_object();
            p.close_array();

            p.on_key("rate_limits");
            p.open_array();
                p.open_object();
                    p.on_key("rateLimitType");
                    p.on_str("ORDERS");
                    p.on_key("interval");
                    p.on_str("SECOND");
                    p.on_key("intervalNum");
                    p.on_int("10");
                    p.on_key("limit");
                    p.on_int("50");
                    p.on_key("count");
                    p.on_int("3");
                    p.on_key("extra");
                    p.open_object();
                        p.on_key("source");
                        p.on_str("api");
                    p.close_object();
                    p.on_key("codes");
                    p.open_array();
                        p.on_int("100");
                        p.on_int("200");
                    p.close_array();
                p.close_object();
                p.open_object();
                    p.on_key("rateLimitType");
                    p.on_str("REQUEST_WEIGHT");
                    p.on_key("interval");
                    p.on_str("MINUTE");
                    p.on_key("intervalNum");
                    p.on_int("1");
                    p.on_key("limit");
                    p.on_int("1200");
                    p.on_key("count");
                    p.on_int("42");
                    p.on_key("extra");
                    p.open_object();
                        p.on_key("region");
                        p.on_str("us-east");
                    p.close_object();
                    p.on_key("codes");
                    p.open_array();
                        p.on_int("301");
                        p.on_int("429");
                        p.on_int("503");
                    p.close_array();
                p.close_object();
            p.close_array();

            p.on_key("headers");
            p.open_object();
                p.on_key("Content-Type");
                p.on_str("application/json");
                p.on_key("X-Request-Id");
                p.on_str("abc-123");
            p.close_object();

            p.on_key("warnings");
            p.open_array();
                p.on_str("rate limit approaching");
                p.on_str("deprecated endpoint");
            p.close_array();
        p.close_object();

        CHECK_EQ(resp.id, "resp-001");
        CHECK_EQ(resp.status, 200u);

        CHECK_EQ(resp.results.size(), 2u);
        CHECK_EQ(resp.results[0].symbol, "BTCUSDT");
        CHECK_EQ(resp.results[0].preventedMatchId, 1u);
        CHECK_EQ(resp.results[0].takerOrderId, 100u);
        CHECK_EQ(resp.results[0].makerOrderId, 200u);
        CHECK_EQ(resp.results[0].tradeGroupId, 300u);
        CHECK_EQ(resp.results[0].selfTradePreventionMode, "EXPIRE_MAKER");
        CHECK_EQ(resp.results[0].price, "50000.00");
        CHECK_EQ(resp.results[0].makerPreventedQuantity, "0.001");
        CHECK_EQ(resp.results[0].transactTime, 1700000000u);
        CHECK_EQ(resp.results[1].symbol, "ETHUSDT");
        CHECK_EQ(resp.results[1].preventedMatchId, 2u);
        CHECK_EQ(resp.results[1].takerOrderId, 101u);
        CHECK_EQ(resp.results[1].makerOrderId, 201u);
        CHECK_EQ(resp.results[1].tradeGroupId, 301u);
        CHECK_EQ(resp.results[1].selfTradePreventionMode, "NONE");
        CHECK_EQ(resp.results[1].price, "3000.50");
        CHECK_EQ(resp.results[1].makerPreventedQuantity, "0.5");
        CHECK_EQ(resp.results[1].transactTime, 1700000001u);

        CHECK_EQ(resp.rate_limits.size(), 2u);
        CHECK_EQ(resp.rate_limits[0].rateLimitType, "ORDERS");
        CHECK_EQ(resp.rate_limits[0].interval, "SECOND");
        CHECK_EQ(resp.rate_limits[0].intervalNum, 10u);
        CHECK_EQ(resp.rate_limits[0].limit, 50u);
        CHECK_EQ(resp.rate_limits[0].count, 3u);
        CHECK_EQ(resp.rate_limits[0].extra.size(), 1u);
        CHECK_EQ(resp.rate_limits[0].extra.at("source"), "api");
        CHECK_EQ(resp.rate_limits[0].codes.size(), 2u);
        { auto it = resp.rate_limits[0].codes.begin(); CHECK_EQ(*it, 100); ++it; CHECK_EQ(*it, 200); }
        CHECK_EQ(resp.rate_limits[1].rateLimitType, "REQUEST_WEIGHT");
        CHECK_EQ(resp.rate_limits[1].interval, "MINUTE");
        CHECK_EQ(resp.rate_limits[1].intervalNum, 1u);
        CHECK_EQ(resp.rate_limits[1].limit, 1200u);
        CHECK_EQ(resp.rate_limits[1].count, 42u);
        CHECK_EQ(resp.rate_limits[1].extra.size(), 1u);
        CHECK_EQ(resp.rate_limits[1].extra.at("region"), "us-east");
        CHECK_EQ(resp.rate_limits[1].codes.size(), 3u);
        { auto it = resp.rate_limits[1].codes.begin(); CHECK_EQ(*it, 301); ++it; CHECK_EQ(*it, 429); ++it; CHECK_EQ(*it, 503); }

        CHECK_EQ(resp.headers.size(), 2u);
        CHECK_EQ(resp.headers.at("Content-Type"), "application/json");
        CHECK_EQ(resp.headers.at("X-Request-Id"), "abc-123");

        CHECK_EQ(resp.warnings.size(), 2u);
        { auto it = resp.warnings.begin(); CHECK_EQ(*it, "rate limit approaching"); ++it; CHECK_EQ(*it, "deprecated endpoint"); }

        const auto json = jsonrefl::to_string(resp);
        const auto json_pretty = jsonrefl::to_string(resp, true);
        std::cout << "response_t compact:" << std::endl << json << std::endl << std::endl;
        std::cout << "response_t pretty:" << std::endl << json_pretty << std::endl;

        // to_chunked_buffer: test early stop (callback returns false)
        {
            auto total = 0u;
            char chunk_buf[32];
            bool ok = jsonrefl::to_chunked_buffer(chunk_buf, sizeof(chunk_buf), resp,
                [&](const void *, std::size_t size) -> bool {
                    total += size;
                    return total < 100;
                }
            );
            CHECK(!ok);
            CHECK(total >= 100);
        }

        // benchmark on vector of 10 objects
        static const auto test_data = make_test_data();
        const auto N = test_data.size();

        // verify correctness: vector serialization vs per-element
        {
            auto vec_json = jsonrefl::to_string(test_data);
            CHECK_EQ(vec_json.front(), '[');
            CHECK_EQ(vec_json.back(), ']');
            CHECK_EQ(jsonrefl::required_bytes(test_data), vec_json.size());
            {
                auto b = std::make_unique<char[]>(vec_json.size());
                auto e = jsonrefl::to_buffer(b.get(), test_data);
                CHECK_EQ(std::string_view(b.get(), vec_json.size()), vec_json);
                (void)e;
            }
            {
                std::string chunked;
                char cbuf[1500];
                bool ok = jsonrefl::to_chunked_buffer(cbuf, sizeof(cbuf), test_data,
                    [&](const void *data, std::size_t size) -> bool {
                        chunked.append(static_cast<const char*>(data), size);
                        return true;
                    }
                );
                CHECK(ok);
                CHECK_EQ(chunked, vec_json);
            }
        }

        // print chunks received by callback
        {
            auto chunk_no = 0u;
            char cbuf[1500];
            jsonrefl::to_chunked_buffer(cbuf, sizeof(cbuf), test_data,
                [&](const void *data, std::size_t size) -> bool {
                    std::cout << "chunk #" << chunk_no++
                              << " (" << size << " bytes): "
                              << std::string_view(static_cast<const char*>(data), size)
                              << "\n\n";
                    return true;
                }
            );
            std::cout << "total chunks: " << chunk_no << "\n";
        }

        const auto rb_compact = jsonrefl::required_bytes(test_data);
        const auto rb_pretty  = jsonrefl::required_bytes(test_data, true);
        auto buf_c = std::make_unique<char[]>(rb_compact);
        auto buf_p = std::make_unique<char[]>(rb_pretty);

        using clock = std::chrono::high_resolution_clock;
        auto ns = [](auto d) { return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count(); };

        std::cout << "\n--- benchmark: vector<response_t>[" << N << "] ---\n";
        std::cout << "total bytes: compact=" << rb_compact
                  << " pretty=" << rb_pretty << "\n";

        constexpr int ITERS = 10000;
        char chunk_buf[1500];

        // warmup
        for ( int i = 0; i < 100; ++i ) {
            auto s = jsonrefl::to_string(test_data); (void)s;
            jsonrefl::to_buffer(buf_c.get(), test_data);
            jsonrefl::to_chunked_buffer(chunk_buf, sizeof(chunk_buf), test_data,
                [](const void *, std::size_t) -> bool { return true; });
        }

        // to_string compact
        auto t0 = clock::now();
        for ( int i = 0; i < ITERS; ++i ) {
            auto s = jsonrefl::to_string(test_data); (void)s;
        }
        auto t1 = clock::now();
        // to_string pretty
        for ( int i = 0; i < ITERS; ++i ) {
            auto s = jsonrefl::to_string(test_data, true); (void)s;
        }
        auto t2 = clock::now();

        // required_bytes compact
        auto t3 = clock::now();
        for ( int i = 0; i < ITERS; ++i ) {
            auto rb = jsonrefl::required_bytes(test_data); (void)rb;
        }
        auto t4 = clock::now();
        // required_bytes pretty
        for ( int i = 0; i < ITERS; ++i ) {
            auto rb = jsonrefl::required_bytes(test_data, true); (void)rb;
        }
        auto t5 = clock::now();

        // to_buffer compact
        auto t6 = clock::now();
        for ( int i = 0; i < ITERS; ++i ) {
            jsonrefl::to_buffer(buf_c.get(), test_data);
        }
        auto t7 = clock::now();
        // to_buffer pretty
        for ( int i = 0; i < ITERS; ++i ) {
            jsonrefl::to_buffer(buf_p.get(), test_data, true);
        }
        auto t8 = clock::now();

        // to_chunked_buffer compact
        auto t9 = clock::now();
        for ( int i = 0; i < ITERS; ++i ) {
            jsonrefl::to_chunked_buffer(chunk_buf, sizeof(chunk_buf), test_data,
                [](const void *, std::size_t) -> bool { return true; }
            );
        }
        auto t10 = clock::now();
        // to_chunked_buffer pretty
        for ( int i = 0; i < ITERS; ++i ) {
            jsonrefl::to_chunked_buffer(chunk_buf, sizeof(chunk_buf), test_data,
                [](const void *, std::size_t) -> bool { return true; },
                true
            );
        }
        auto t11 = clock::now();

        std::cout << ITERS << " iterations:\n";
        std::cout << "to_string  compact: " << ns(t1 - t0)/ITERS << " ns/iter\n";
        std::cout << "to_string  pretty:  " << ns(t2 - t1)/ITERS << " ns/iter\n";
        std::cout << "req_bytes  compact: " << ns(t4 - t3)/ITERS << " ns/iter\n";
        std::cout << "req_bytes  pretty:  " << ns(t5 - t4)/ITERS << " ns/iter\n";
        std::cout << "to_buffer  compact: " << ns(t7 - t6)/ITERS << " ns/iter\n";
        std::cout << "to_buffer  pretty:  " << ns(t8 - t7)/ITERS << " ns/iter\n";
        std::cout << "chunked(1500)compact: " << ns(t10- t9)/ITERS << " ns/iter\n";
        std::cout << "chunked(1500)pretty:  " << ns(t11-t10)/ITERS << " ns/iter\n";

        CHECK_EQ(json, "{\"id\":\"resp-001\",\"status\":200,\"results\":[{\"symbol\":\"BTCUSDT\",\"preventedMatchId\":1,\"takerOrderId\":100,\"makerOrderId\":200,\"tradeGroupId\":300,\"selfTradePreventionMode\":\"EXPIRE_MAKER\",\"price\":\"50000.00\",\"makerPreventedQuantity\":\"0.001\",\"transactTime\":1700000000},{\"symbol\":\"ETHUSDT\",\"preventedMatchId\":2,\"takerOrderId\":101,\"makerOrderId\":201,\"tradeGroupId\":301,\"selfTradePreventionMode\":\"NONE\",\"price\":\"3000.50\",\"makerPreventedQuantity\":\"0.5\",\"transactTime\":1700000001}],\"rate_limits\":[{\"rateLimitType\":\"ORDERS\",\"interval\":\"SECOND\",\"intervalNum\":10,\"limit\":50,\"count\":3,\"extra\":{\"source\":\"api\"},\"codes\":[100,200]},{\"rateLimitType\":\"REQUEST_WEIGHT\",\"interval\":\"MINUTE\",\"intervalNum\":1,\"limit\":1200,\"count\":42,\"extra\":{\"region\":\"us-east\"},\"codes\":[301,429,503]}],\"headers\":{\"Content-Type\":\"application/json\",\"X-Request-Id\":\"abc-123\"},\"warnings\":[\"rate limit approaching\",\"deprecated endpoint\"]}");

        CHECK_EQ(json_pretty,
            "{\n"
            "   \"id\": \"resp-001\",\n"
            "   \"status\": 200,\n"
            "   \"results\": [\n"
            "      {\n"
            "         \"symbol\": \"BTCUSDT\",\n"
            "         \"preventedMatchId\": 1,\n"
            "         \"takerOrderId\": 100,\n"
            "         \"makerOrderId\": 200,\n"
            "         \"tradeGroupId\": 300,\n"
            "         \"selfTradePreventionMode\": \"EXPIRE_MAKER\",\n"
            "         \"price\": \"50000.00\",\n"
            "         \"makerPreventedQuantity\": \"0.001\",\n"
            "         \"transactTime\": 1700000000\n"
            "      },\n"
            "      {\n"
            "         \"symbol\": \"ETHUSDT\",\n"
            "         \"preventedMatchId\": 2,\n"
            "         \"takerOrderId\": 101,\n"
            "         \"makerOrderId\": 201,\n"
            "         \"tradeGroupId\": 301,\n"
            "         \"selfTradePreventionMode\": \"NONE\",\n"
            "         \"price\": \"3000.50\",\n"
            "         \"makerPreventedQuantity\": \"0.5\",\n"
            "         \"transactTime\": 1700000001\n"
            "      }\n"
            "   ],\n"
            "   \"rate_limits\": [\n"
            "      {\n"
            "         \"rateLimitType\": \"ORDERS\",\n"
            "         \"interval\": \"SECOND\",\n"
            "         \"intervalNum\": 10,\n"
            "         \"limit\": 50,\n"
            "         \"count\": 3,\n"
            "         \"extra\": {\n"
            "            \"source\": \"api\"\n"
            "         },\n"
            "         \"codes\": [\n"
            "            100,\n"
            "            200\n"
            "         ]\n"
            "      },\n"
            "      {\n"
            "         \"rateLimitType\": \"REQUEST_WEIGHT\",\n"
            "         \"interval\": \"MINUTE\",\n"
            "         \"intervalNum\": 1,\n"
            "         \"limit\": 1200,\n"
            "         \"count\": 42,\n"
            "         \"extra\": {\n"
            "            \"region\": \"us-east\"\n"
            "         },\n"
            "         \"codes\": [\n"
            "            301,\n"
            "            429,\n"
            "            503\n"
            "         ]\n"
            "      }\n"
            "   ],\n"
            "   \"headers\": {\n"
            "      \"Content-Type\": \"application/json\",\n"
            "      \"X-Request-Id\": \"abc-123\"\n"
            "   },\n"
            "   \"warnings\": [\n"
            "      \"rate limit approaching\",\n"
            "      \"deprecated endpoint\"\n"
            "   ]\n"
            "}"
        );
    }

    CHECK(test_parse_int_array());
    CHECK(test_parse_string_array());
    CHECK(test_parse_sv_array());
    CHECK(test_parse_int_string());
    CHECK(test_parse_int_string_array());
    CHECK(test_parse_nested());
    CHECK(test_parse_nested_with_array());
    CHECK(test_parse_nested_nested_nested());
    CHECK(test_parse_result_t());
    CHECK(test_parse_rate_limits_t());
    CHECK(test_parse_response_t());

    {
        bool_test obj{false, 0};
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"flag":true,"val":1})") == jsonrefl::state::ok);
        CHECK_EQ(obj.val, 1);
        CHECK_EQ(obj.flag, true);
        std::cout << "test_literal_true: OK" << std::endl;
    }
    {
        bool_test obj{true, 0};
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"flag":false,"val":2})") == jsonrefl::state::ok);
        CHECK_EQ(obj.val, 2);
        CHECK_EQ(obj.flag, false);
        std::cout << "test_literal_false: OK" << std::endl;
    }
    {
        null_test obj{};
        obj.opt = 42;
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"opt":null,"val":3})") == jsonrefl::state::ok);
        CHECK_EQ(obj.val, 3);
        CHECK(obj.opt.has_value());
        CHECK_EQ(*obj.opt, 42);
        std::cout << "test_literal_null_opt_untouched: OK" << std::endl;
    }
    {
        null_test obj{};
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"opt":null,"val":5})") == jsonrefl::state::ok);
        CHECK_EQ(obj.val, 5);
        CHECK(!obj.opt.has_value());
        std::cout << "test_literal_null_opt_default: OK" << std::endl;
    }
    {
        bool_test obj{false, 0};
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"flag":tr)") == jsonrefl::state::incomplete);
        CHECK(p.parse(R"(ue,"val":7})") == jsonrefl::state::ok);
        CHECK_EQ(obj.val, 7);
        CHECK_EQ(obj.flag, true);
        std::cout << "test_literal_true_chunked: OK" << std::endl;
    }
    {
        bool_test obj{true, 0};
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"flag":fal)") == jsonrefl::state::incomplete);
        CHECK(p.parse(R"(se,"val":8})") == jsonrefl::state::ok);
        CHECK_EQ(obj.val, 8);
        CHECK_EQ(obj.flag, false);
        std::cout << "test_literal_false_chunked: OK" << std::endl;
    }
    {
        null_test obj{};
        obj.opt = 99;
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"opt":nu)") == jsonrefl::state::incomplete);
        CHECK(p.parse(R"(ll,"val":9})") == jsonrefl::state::ok);
        CHECK_EQ(obj.val, 9);
        CHECK(obj.opt.has_value());
        CHECK_EQ(*obj.opt, 99);
        std::cout << "test_literal_null_chunked: OK" << std::endl;
    }
    {
        null_arr_test obj{};
        obj.arr = std::vector<int>{1, 2, 3};
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"arr":null,"val":10})") == jsonrefl::state::ok);
        CHECK_EQ(obj.val, 10);
        CHECK(obj.arr.has_value());
        CHECK_EQ(obj.arr->size(), 3u);
        std::cout << "test_literal_null_opt_vec_untouched: OK" << std::endl;
    }
    {
        null_arr_test obj{};
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"arr":null,"val":11})") == jsonrefl::state::ok);
        CHECK_EQ(obj.val, 11);
        CHECK(!obj.arr.has_value());
        std::cout << "test_literal_null_opt_vec_default: OK" << std::endl;
    }
    {
        int_string obj{};
        std::string accum;
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"i":42,"s":"hel)", &accum) == jsonrefl::state::incomplete);
        CHECK(p.parse(R"(lo"})", &accum) == jsonrefl::state::ok);
        CHECK_EQ(obj.i, 42);
        CHECK_EQ(obj.s, "hello");
        std::cout << "test_parse_string_chunked_ok: OK" << std::endl;
    }
    {
        int_string obj{};
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"i":42,"s":"hel)") == jsonrefl::state::no_buffer);
        std::cout << "test_parse_no_buffer: OK" << std::endl;
    }
    {
        int_string obj{};
        std::string accum;
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"i":42,"s":"hel)", &accum) == jsonrefl::state::incomplete);
        CHECK(p.parse(R"(lo"})") == jsonrefl::state::no_buffer);
        std::cout << "test_parse_no_buffer_mid_accum: OK" << std::endl;
    }
    {
        nested obj{};
        std::string accum;
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"i":10,"n":{"i":20,"s":"inner"},"sv":"vi)", &accum) == jsonrefl::state::incomplete);
        CHECK(p.parse(R"(ew"})", &accum) == jsonrefl::state::invalid);
        std::cout << "test_parse_sv_chunked_reject: OK" << std::endl;
    }
    {
        nested obj{};
        std::string accum;
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(R"({"i":10,"n":{"i":20,"s":"inner"},"sv":"vi)", &accum) == jsonrefl::state::incomplete);
        CHECK(p.parse(R"(ew"})", &accum) == jsonrefl::state::invalid);

        p.reset();
        obj = nested{};

        CHECK(p.parse(R"({"i":10,"n":{"i":20,"s":"inner"},"sv":"view"})") == jsonrefl::state::ok);
        CHECK_EQ(obj.i, 10);
        CHECK_EQ(obj.n.i, 20);
        CHECK_EQ(obj.n.s, "inner");
        CHECK_EQ(obj.sv, "view");
        std::cout << "test_parse_reset_after_failure: OK" << std::endl;
    }

    {
        const char json[] = R"({"i":10,"n":{"i":20,"s":"inner"},"sv":"view"})";
        const char *json_begin = json;
        const char *json_end   = json + sizeof(json) - 1;

        nested obj{};
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(json) == jsonrefl::state::ok);

        CHECK_EQ(obj.sv, "view");
        CHECK(obj.sv.data() >= json_begin && obj.sv.data() + obj.sv.size() <= json_end);

        std::cout << "test_zero_copy_nested_sv: OK" << std::endl;
    }
    {
        const char json[] = R"({"arr":["alpha","beta","gamma"]})";
        const char *json_begin = json;
        const char *json_end   = json + sizeof(json) - 1;

        sv_array obj{};
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(json) == jsonrefl::state::ok);

        CHECK_EQ(obj.arr.size(), 3u);
        for ( const auto &sv : obj.arr ) {
            CHECK(sv.data() >= json_begin && sv.data() + sv.size() <= json_end);
        }

        std::cout << "test_zero_copy_sv_array: OK" << std::endl;
    }
    {
        const char json[] = R"({"symbol":"BTCUSDT","preventedMatchId":1,"takerOrderId":100,"makerOrderId":200,"tradeGroupId":300,"selfTradePreventionMode":"NONE","price":"50000.00","makerPreventedQuantity":"0.5","transactTime":1700000000})";
        const char *json_begin = json;
        const char *json_end   = json + sizeof(json) - 1;

        myns::result_t obj{};
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse(json) == jsonrefl::state::ok);

        CHECK(obj.symbol.data() >= json_begin && obj.symbol.data() + obj.symbol.size() <= json_end);
        CHECK(obj.selfTradePreventionMode.data() >= json_begin && obj.selfTradePreventionMode.data() + obj.selfTradePreventionMode.size() <= json_end);
        CHECK(obj.price.data() >= json_begin && obj.price.data() + obj.price.size() <= json_end);
        CHECK(obj.makerPreventedQuantity.data() >= json_begin && obj.makerPreventedQuantity.data() + obj.makerPreventedQuantity.size() <= json_end);

        std::cout << "test_zero_copy_result_t: OK" << std::endl;
    }

    std::cout << "all parse tests: OK" << std::endl;

    {
        struct token_checker {
            std::vector<std::string> chunks;
            void operator()(const void *data, std::size_t size) {
                chunks.emplace_back(static_cast<const char *>(data), size);
            }
        };

        auto is_split_across = [](const std::vector<std::string> &chunks, std::string_view token) -> bool {
            for ( auto i = 0u; i + 1 < chunks.size(); ++i ) {
                const auto &c = chunks[i];
                for ( auto overlap = 1u; overlap < token.size(); ++overlap ) {
                    if ( c.size() >= overlap
                        && c.compare(c.size() - overlap, overlap, token.data(), overlap) == 0
                        && chunks[i + 1].size() >= token.size() - overlap
                        && chunks[i + 1].compare(0, token.size() - overlap, token.data() + overlap, token.size() - overlap) == 0 )
                    {
                        return true;
                    }
                }
            }
            return false;
        };

        {
            bool_test obj{true, 42};
            std::vector<std::string> chunks;
            char cbuf[16];
            jsonrefl::to_chunked_buffer(cbuf, sizeof(cbuf), obj,
                [&](const void *d, std::size_t s) -> bool {
                    chunks.emplace_back(static_cast<const char*>(d), s);
                    return true;
                }
            );
            CHECK(!is_split_across(chunks, "true"));
            CHECK(!is_split_across(chunks, "\"flag\""));
            CHECK(!is_split_across(chunks, "\"val\""));
            CHECK(!is_split_across(chunks, "42"));
            std::cout << "test_chunked_no_split_bool_true: OK" << std::endl;
        }
        {
            bool_test obj{false, 12345};
            std::vector<std::string> chunks;
            char cbuf[16];
            jsonrefl::to_chunked_buffer(cbuf, sizeof(cbuf), obj,
                [&](const void *d, std::size_t s) -> bool {
                    chunks.emplace_back(static_cast<const char*>(d), s);
                    return true;
                }
            );
            CHECK(!is_split_across(chunks, "false"));
            CHECK(!is_split_across(chunks, "\"flag\""));
            CHECK(!is_split_across(chunks, "\"val\""));
            CHECK(!is_split_across(chunks, "12345"));
            std::cout << "test_chunked_no_split_bool_false: OK" << std::endl;
        }
        {
            null_test obj{};
            obj.opt = std::nullopt;
            obj.val = 99;
            std::vector<std::string> chunks;
            char cbuf[16];
            jsonrefl::to_chunked_buffer(cbuf, sizeof(cbuf), obj,
                [&](const void *d, std::size_t s) -> bool {
                    chunks.emplace_back(static_cast<const char*>(d), s);
                    return true;
                }
            );
            CHECK(!is_split_across(chunks, "null"));
            CHECK(!is_split_across(chunks, "\"opt\""));
            CHECK(!is_split_across(chunks, "\"val\""));
            CHECK(!is_split_across(chunks, "99"));
            std::cout << "test_chunked_no_split_null: OK" << std::endl;
        }
        {
            std::map<std::string, int> m = {{"alpha", 1}, {"beta", 2}, {"gamma", 3}};
            std::vector<std::string> chunks;
            char cbuf[16];
            jsonrefl::to_chunked_buffer(cbuf, sizeof(cbuf), m,
                [&](const void *d, std::size_t s) -> bool {
                    chunks.emplace_back(static_cast<const char*>(d), s);
                    return true;
                }
            );
            CHECK(!is_split_across(chunks, "\"alpha\""));
            CHECK(!is_split_across(chunks, "\"beta\""));
            CHECK(!is_split_across(chunks, "\"gamma\""));
            std::cout << "test_chunked_no_split_map_keys: OK" << std::endl;
        }
        {
            nested obj{};
            obj.i = 123456;
            obj.n.i = -789;
            obj.n.s = "hello";
            obj.sv = "world";
            std::vector<std::string> chunks;
            char cbuf[24];
            jsonrefl::to_chunked_buffer(cbuf, sizeof(cbuf), obj,
                [&](const void *d, std::size_t s) -> bool {
                    chunks.emplace_back(static_cast<const char*>(d), s);
                    return true;
                }
            );
            CHECK(!is_split_across(chunks, "\"i\""));
            CHECK(!is_split_across(chunks, "\"n\""));
            CHECK(!is_split_across(chunks, "\"s\""));
            CHECK(!is_split_across(chunks, "\"sv\""));
            CHECK(!is_split_across(chunks, "123456"));
            CHECK(!is_split_across(chunks, "-789"));
            std::cout << "test_chunked_no_split_nested_keys_nums: OK" << std::endl;
        }
    }

    {
        std::string long_value(2000, 'X');
        int_string obj{};
        obj.i = 7;
        obj.s = long_value;

        std::string chunked;
        char cbuf[64];
        bool ok = jsonrefl::to_chunked_buffer(cbuf, sizeof(cbuf), obj,
            [&](const void *d, std::size_t s) -> bool {
                chunked.append(static_cast<const char*>(d), s);
                return true;
            }
        );
        CHECK(ok);

        auto ref = jsonrefl::to_string(obj);
        CHECK_EQ(chunked, ref);

        int_string parsed{};
        auto p = jsonrefl::make_parser(&parsed);
        CHECK(p.parse(chunked) == jsonrefl::state::ok);
        CHECK_EQ(parsed.i, 7);
        CHECK_EQ(parsed.s, long_value);
        std::cout << "test_chunked_long_string_roundtrip: OK" << std::endl;
    }

    {
        std::string long_value(2000, 'Y');
        int_string obj{};
        obj.i = 42;
        obj.s = long_value;

        std::vector<std::string> chunks;
        char cbuf[64];
        jsonrefl::to_chunked_buffer(cbuf, sizeof(cbuf), obj,
            [&](const void *d, std::size_t s) -> bool {
                chunks.emplace_back(static_cast<const char*>(d), s);
                return true;
            }
        );
        CHECK(chunks.size() > 1);

        int_string parsed{};
        std::string accum;
        auto p = jsonrefl::make_parser(&parsed);
        jsonrefl::state last = jsonrefl::state::incomplete;
        for ( const auto &chunk : chunks ) {
            last = p.parse(chunk, &accum);
            if ( last == jsonrefl::state::invalid ) { CHECK(false); break; }
        }
        CHECK(last == jsonrefl::state::ok);
        CHECK_EQ(parsed.i, 42);
        CHECK_EQ(parsed.s, long_value);
        std::cout << "test_chunked_long_string_chunked_deser: OK" << std::endl;
    }

    std::cout << "all chunked token tests: OK" << std::endl;

    return 0;
}

/*************************************************************************************************/
