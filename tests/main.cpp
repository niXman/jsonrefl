
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
#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

/*************************************************************************************************/

#define CHECK(expr)                                                        \
    do {                                                                   \
        if ( !(expr) ) {                                                   \
            std::cerr                                                      \
                << "CHECK failed: " #expr "\n"                             \
                << "  " << __FILE__ << ":" << __LINE__                     \
                << " in " << __func__ << "()\n"                            \
            ;                                                              \
            return false;                                                  \
        }                                                                  \
    } while ( 0 )

#define CHECK_EQ(a, b)                                                     \
    do {                                                                   \
        const auto &_a = (a);                                              \
        const auto &_b = (b);                                              \
        if ( !(_a == _b) ) {                                               \
            std::cerr                                                      \
                << "CHECK_EQ failed: " #a " == " #b "\n"                   \
                << "  lhs: " << _a << "\n  rhs: " << _b                    \
                << "\n  " << __FILE__ << ":" << __LINE__                   \
                << " in " << __func__ << "()\n"                            \
            ;                                                              \
            return false;                                                  \
        }                                                                  \
    } while ( 0 )

#define JSONREFL_TEST(fn)                                                  \
    ([]() -> bool {                                                        \
        std::cout << "[ RUN ] " << __COUNTER__ << ' ' << #fn << "() ... "  \
            << std::flush;                                                 \
        const bool ok = (fn)();                                            \
        if ( ok ) { std::cout << "OK" << std::endl; }                      \
        return ok;                                                         \
    })()

/***************************************************************************************************/
// shared test helpers (referenced by multiple test groups)
/***************************************************************************************************/

static std::vector<std::string> split_lines(const std::string &s) {
    std::vector<std::string> lines;
    std::istringstream iss(s);
    for ( std::string line; std::getline(iss, line); )
        if ( !line.empty() ) lines.push_back(line);
    return lines;
}

inline bool is_split_across(const std::vector<std::string> &chunks, jsonrefl::string_view_t token) {
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
}

/***************************************************************************************************/
// metadata reflection (constexpr metadata generation)
/***************************************************************************************************/

bool test_metadata_int_string() {
    static_assert(jsonrefl::has_metadata<int_string>::value, "int_string metadata");

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

    return true;
}

bool test_metadata_nested() {
    static_assert(jsonrefl::has_metadata<nested>::value, "nested metadata");

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

    return true;
}

bool test_metadata_nested_with_array() {
    static_assert(jsonrefl::has_metadata<nested_with_array>::value, "nested_with_array metadata");

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

    return true;
}

bool test_metadata_result_t() {
    static_assert(jsonrefl::has_metadata<myns::result_t>::value, "myns::result_t metadata");

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

    CHECK(m.get("nonexistent") == nullptr);

    return true;
}

bool test_metadata_rate_limits_t() {
    static_assert(jsonrefl::has_metadata<myns::rate_limits_t>::value, "myns::rate_limits_t metadata");

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

    return true;
}

bool test_metadata_response_t() {
    static_assert(jsonrefl::has_metadata<myns::response_t>::value, "myns::response_t metadata");

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

    return true;
}

bool test_calc_max_stack_depth() {
    struct empty {};

    constexpr auto v0 = jsonrefl::stack_depth<empty>();
    static_assert(v0 == 0, "stack_depth<empty>");

    constexpr auto v1 = jsonrefl::stack_depth<int_array>();
    static_assert(v1 == 2, "stack_depth<int_array>");

    constexpr auto v2 = jsonrefl::stack_depth<int_string>();
    static_assert(v2 == 1, "stack_depth<int_string>");

    constexpr auto v3 = jsonrefl::stack_depth<nested>();
    static_assert(v3 == 2, "stack_depth<nested>");

    constexpr auto v4 = jsonrefl::stack_depth<nested_with_array>();
    static_assert(v4 == 3, "stack_depth<nested_with_array>");

    constexpr auto v5 = jsonrefl::stack_depth<nested_nested_nested>();
    static_assert(v5 == 4, "stack_depth<nested_nested_nested>");

    constexpr auto v6 = jsonrefl::stack_depth<nest_mm_int>();
    static_assert(v6 == 3, "stack_depth<nest_mm_int>");

    constexpr auto v7 = jsonrefl::stack_depth<nest_mmm_int>();
    static_assert(v7 == 4, "stack_depth<nest_mmm_int>");

    return true;
}

bool test_dump() {
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
        jsonrefl::string_view_t name;
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

    return true;
}

/***************************************************************************************************/
// perfect hash function (PHF) construction (compile-time member lookup)
/***************************************************************************************************/

bool test_phf_next_pow2_is_power_of_two() {
    namespace d = jsonrefl::details;

    static_assert(d::next_pow2(1u) == 1u, "next_pow2(1u)");
    static_assert(d::next_pow2(2u) == 2u, "next_pow2(2u)");
    static_assert(d::next_pow2(3u) == 4u, "next_pow2(3u)");
    static_assert(d::next_pow2(4u) == 4u, "next_pow2(4u)");
    static_assert(d::next_pow2(5u) == 8u, "next_pow2(5u)");
    static_assert(d::next_pow2(7u) == 8u, "next_pow2(7u)");
    static_assert(d::next_pow2(8u) == 8u, "next_pow2(8u)");
    static_assert(d::next_pow2(9u) == 16u, "next_pow2(9u)");
    static_assert(d::next_pow2(16u) == 16u, "next_pow2(16u)");
    static_assert(d::next_pow2(17u) == 32u, "next_pow2(17u)");

    return true;
}

bool test_phf_fnv1a_distinct_for_distinct_keys() {
    namespace d = jsonrefl::details;

    constexpr auto h_symbol             = d::fnv1a("symbol", sizeof("symbol") - 1u);
    constexpr auto h_preventedMatchId   = d::fnv1a("preventedMatchId", sizeof("preventedMatchId") - 1u);
    constexpr auto h_takerOrderId       = d::fnv1a("takerOrderId", sizeof("takerOrderId") - 1u);
    constexpr auto h_makerOrderId       = d::fnv1a("makerOrderId", sizeof("makerOrderId") - 1u);
    constexpr auto h_tradeGroupId       = d::fnv1a("tradeGroupId", sizeof("tradeGroupId") - 1u);

    static_assert(h_symbol             != h_preventedMatchId, "fnv1a collision: symbol vs preventedMatchId");
    static_assert(h_symbol             != h_takerOrderId, "fnv1a collision: symbol vs takerOrderId");
    static_assert(h_preventedMatchId   != h_takerOrderId, "fnv1a collision: preventedMatchId vs takerOrderId");
    static_assert(h_takerOrderId       != h_makerOrderId, "fnv1a collision: takerOrderId vs makerOrderId");
    static_assert(h_makerOrderId       != h_tradeGroupId, "fnv1a collision: makerOrderId vs tradeGroupId");
    static_assert(
        d::fnv1a("symbol", sizeof("symbol") - 1u) == d::fnv1a("symbol", sizeof("symbol") - 1u)
        ,"fnv1a stable for same key"
    );

    return true;
}

bool test_phf_empty_index_constexpr() {
    namespace d = jsonrefl::details;

    constexpr auto idx = d::make_phf_index<0, 1>(d::index_array_type<0>{});

    static_assert(idx.get(0u) == nullptr, "empty phf index: slot 0");
    static_assert(idx.get(0xDEADBEEFu) == nullptr, "empty phf index: arbitrary hash");

    CHECK(idx.get(0u) == nullptr);
    CHECK(idx.get(0xDEADBEEFu) == nullptr);

    return true;
}

bool test_phf_built_from_object_holder_for_result_t() {
    static const auto holder = jsonrefl::object_holder(
         "result_t"
        ,jsonrefl::object_member("symbol",                  &myns::result_t::symbol)
        ,jsonrefl::object_member("preventedMatchId",        &myns::result_t::preventedMatchId)
        ,jsonrefl::object_member("takerOrderId",            &myns::result_t::takerOrderId)
        ,jsonrefl::object_member("makerOrderId",            &myns::result_t::makerOrderId)
        ,jsonrefl::object_member("tradeGroupId",            &myns::result_t::tradeGroupId)
        ,jsonrefl::object_member("selfTradePreventionMode", &myns::result_t::selfTradePreventionMode)
        ,jsonrefl::object_member("price",                   &myns::result_t::price)
        ,jsonrefl::object_member("makerPreventedQuantity",  &myns::result_t::makerPreventedQuantity)
        ,jsonrefl::object_member("transactTime",            &myns::result_t::transactTime)
    );

    CHECK_EQ(holder.size(), 9u);
    CHECK(!holder.empty());

    static const jsonrefl::string_view_t names[] = {
         "symbol"
        ,"preventedMatchId"
        ,"takerOrderId"
        ,"makerOrderId"
        ,"tradeGroupId"
        ,"selfTradePreventionMode"
        ,"price"
        ,"makerPreventedQuantity"
        ,"transactTime"
    };
    constexpr auto N = sizeof(names) / sizeof(names[0]);
    static_assert(N == 9u, "result_t field count in holder test");

    const jsonrefl::details::setter_base *setters[N]{};
    for ( auto i = 0u; i < N; ++i ) {
        setters[i] = holder.get(names[i]);
        CHECK(setters[i] != nullptr);
    }

    for ( auto i = 0u; i < N; ++i ) {
        for ( auto j = i + 1; j < N; ++j ) {
            CHECK(setters[i] != setters[j]);
        }
    }

    CHECK(holder.get("nope") == nullptr);
    CHECK(holder.get("Symbol") == nullptr); // case-sensitive
    CHECK(holder.get("symbol ") == nullptr);
    CHECK(holder.get("") == nullptr);

    return true;
}

bool test_phf_built_from_object_holder_for_int_string() {
    static const auto holder = jsonrefl::object_holder(
         "int_string"
        ,jsonrefl::object_member("i", &int_string::i)
        ,jsonrefl::object_member("s", &int_string::s)
    );

    CHECK_EQ(holder.size(), 2u);

    const auto *si = holder.get("i");
    const auto *ss = holder.get("s");
    CHECK(si != nullptr);
    CHECK(ss != nullptr);
    CHECK(si != ss);

    CHECK(holder.get("z") == nullptr);
    CHECK(holder.get("ii") == nullptr);

    return true;
}

bool test_phf_lookup_matches_metadata_lookup() {
    const auto &meta = jsonrefl::metadata<myns::result_t>();
    CHECK_EQ(meta.size(), 9u);

    static const jsonrefl::string_view_t names[] = {
         "symbol"
        ,"preventedMatchId"
        ,"takerOrderId"
        ,"makerOrderId"
        ,"tradeGroupId"
        ,"selfTradePreventionMode"
        ,"price"
        ,"makerPreventedQuantity"
        ,"transactTime"
    };
    constexpr auto N = sizeof(names) / sizeof(names[0]);

    for ( auto i = 0u; i < N; ++i ) {
        CHECK(meta.get(names[i]) != nullptr);
    }
    CHECK(meta.get("missing") == nullptr);

    return true;
}

bool test_phf_strategy_linear_for_small_struct() {
    using meta_t = std::remove_reference_t<decltype(jsonrefl::metadata<myns::result_t>())>;
    static_assert(meta_t::uses_minimal_index() == true, "result_t metadata uses minimal (linear) index");

    static const auto holder = jsonrefl::object_holder(
         "kv_si"
        ,jsonrefl::object_member("s", &kv_si::s)
        ,jsonrefl::object_member("i", &kv_si::i)
    );
    using h_t = std::remove_const_t<std::remove_reference_t<decltype(holder)>>;
    static_assert(h_t::uses_minimal_index() == true, "small object_holder uses minimal index");

    return true;
}

bool test_phf_linear_index_layout_matches_declaration_order() {
    // demonstrate the *raw* layout of the built linear index for result_t.
    // result_t has 9 fields, so the holder uses linear_index<9>.
    // the construction guarantees:
    //   m_index.hashes [i] == fnv1a(name_i)
    //   m_index.setters[i] == &m_tuple<i>.member        // == metadata.get(name_i)
    // i.e. slot index == position in declaration order (true MPHF).
    static const jsonrefl::string_view_t names[] = {
         "symbol"
        ,"preventedMatchId"
        ,"takerOrderId"
        ,"makerOrderId"
        ,"tradeGroupId"
        ,"selfTradePreventionMode"
        ,"price"
        ,"makerPreventedQuantity"
        ,"transactTime"
    };
    constexpr auto N = sizeof(names) / sizeof(names[0]);
    static_assert(N == 9u, "result_t field count in linear layout test");

    const auto &meta = jsonrefl::metadata<myns::result_t>();
    using meta_t = std::remove_reference_t<decltype(meta)>;
    static_assert(meta_t::uses_minimal_index() == true, "result_t linear index layout");

    const auto &idx = meta.index();
    CHECK_EQ(meta.size(), N);

    for ( std::size_t i = 0; i < N; ++i ) {
        // hash slot [i] holds fnv1a of the i-th declared name
        CHECK_EQ(idx.hashes[i], jsonrefl::details::fnv1a(names[i]));

        // setter slot [i] points at the same setter the public name lookup
        // resolves to for the i-th declared name (true MPHF: slot == decl pos)
        const auto *via_name = meta.get(names[i]);
        CHECK(via_name != nullptr);
        CHECK_EQ(idx.setters[i], via_name);
    }

    // perfect-hash sanity: every setter slot is unique
    for ( std::size_t i = 0; i < N; ++i ) {
        for ( std::size_t j = i + 1; j < N; ++j ) {
            CHECK(idx.setters[i] != idx.setters[j]);
            CHECK(idx.hashes [i] != idx.hashes [j]);
        }
    }

    return true;
}

bool test_phf_strategy_sparse_for_large_struct() {
    static const auto holder17 = jsonrefl::object_holder(
         "big17"
        ,jsonrefl::object_member("aa", &int_string::i)
        ,jsonrefl::object_member("ab", &int_string::i)
        ,jsonrefl::object_member("ac", &int_string::i)
        ,jsonrefl::object_member("ad", &int_string::i)
        ,jsonrefl::object_member("ae", &int_string::i)
        ,jsonrefl::object_member("af", &int_string::i)
        ,jsonrefl::object_member("ag", &int_string::i)
        ,jsonrefl::object_member("ah", &int_string::i)
        ,jsonrefl::object_member("ai", &int_string::i)
        ,jsonrefl::object_member("aj", &int_string::i)
        ,jsonrefl::object_member("ak", &int_string::i)
        ,jsonrefl::object_member("al", &int_string::i)
        ,jsonrefl::object_member("am", &int_string::i)
        ,jsonrefl::object_member("an", &int_string::i)
        ,jsonrefl::object_member("ao", &int_string::i)
        ,jsonrefl::object_member("ap", &int_string::i)
        ,jsonrefl::object_member("aq", &int_string::i)
    );
    using h_t = std::remove_const_t<std::remove_reference_t<decltype(holder17)>>;
    static_assert(h_t::uses_minimal_index() == false, "17-member holder uses sparse index");

    CHECK_EQ(holder17.size(), 17u);

    static const jsonrefl::string_view_t names[] = {
        "aa","ab","ac","ad","ae","af","ag","ah","ai",
        "aj","ak","al","am","an","ao","ap","aq"
    };
    constexpr auto N = sizeof(names) / sizeof(names[0]);
    static_assert(N == 17u, "big17 field count");

    const jsonrefl::details::setter_base *setters[N]{};
    for ( std::size_t i = 0; i < N; ++i ) {
        setters[i] = holder17.get(names[i]);
        CHECK(setters[i] != nullptr);
    }
    for ( std::size_t i = 0; i < N; ++i ) {
        for ( std::size_t j = i + 1; j < N; ++j ) {
            CHECK(setters[i] != setters[j]);
        }
    }
    CHECK(holder17.get("zz") == nullptr);
    CHECK(holder17.get("") == nullptr);

    return true;
}

/***************************************************************************************************/
// primitive arrays (singleshot parse, serialize)
/***************************************************************************************************/

bool test_int_array() {
    static_assert(jsonrefl::has_metadata<int_array>::value, "int_array metadata");

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

    return true;
}

bool test_string_array() {
    static_assert(jsonrefl::has_metadata<string_array>::value, "string_array metadata");

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

    return true;
}

bool test_sv_array() {
    static_assert(jsonrefl::has_metadata<sv_array>::value, "sv_array metadata");

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

    return true;
}

/***************************************************************************************************/
// optional<T> field handling
/***************************************************************************************************/

bool test_optional() {
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

        auto json_p = jsonrefl::to_string(obj, jsonrefl::serialize_flags::pretty);
        CHECK_EQ(jsonrefl::required_bytes(obj, jsonrefl::serialize_flags::pretty), json_p.size());
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

    return true;
}

/***************************************************************************************************/
// primitive parsing: arrays / int+string / nested
/***************************************************************************************************/

bool test_parse_int_array() {
    const jsonrefl::string_view_t json = R"({"arr":[3, 2, 1, 4, 5, 6]})";
    int_array obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json.data(), json.size()).status() != jsonrefl::state::ok ) { return false; }
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
    const jsonrefl::string_view_t json = R"({"arr":["hello","world","foo"]})";
    string_array obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json.data(), json.size()).status() != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.arr.size(), 3u);
    CHECK_EQ(obj.arr[0], "hello");
    CHECK_EQ(obj.arr[1], "world");
    CHECK_EQ(obj.arr[2], "foo");

    return true;
}

bool test_parse_sv_array() {
    const jsonrefl::string_view_t json = R"({"arr":["alpha","beta","gamma"]})";
    sv_array obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json.data(), json.size()).status() != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.arr.size(), 3u);
    CHECK_EQ(obj.arr[0], "alpha");
    CHECK_EQ(obj.arr[1], "beta");
    CHECK_EQ(obj.arr[2], "gamma");

    return true;
}

bool test_parse_int_string() {
    const jsonrefl::string_view_t json = R"({"i":42,"s":"hello"})";
    int_string obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json.data(), json.size()).status() != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.i, 42);
    CHECK_EQ(obj.s, "hello");

    return true;
}

bool test_parse_int_string_array() {
    const jsonrefl::string_view_t json = R"({"v":[{"i":1,"s":"one"},{"i":2,"s":"two"},{"i":3,"s":"three"}]})";
    int_string_array obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json.data(), json.size()).status() != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.v.size(), 3u);
    CHECK_EQ(obj.v[0].i, 1);
    CHECK_EQ(obj.v[0].s, "one");
    CHECK_EQ(obj.v[1].i, 2);
    CHECK_EQ(obj.v[1].s, "two");
    CHECK_EQ(obj.v[2].i, 3);
    CHECK_EQ(obj.v[2].s, "three");

    return true;
}

/***************************************************************************************************/
// nested struct parsing
/***************************************************************************************************/

bool test_parse_nested() {
    const jsonrefl::string_view_t json = R"({"i":10,"n":{"i":20,"s":"inner"},"sv":"view"})";
    nested obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json.data(), json.size()).status() != jsonrefl::state::ok ) { return false; }
    CHECK_EQ(obj.i, 10);
    CHECK_EQ(obj.n.i, 20);
    CHECK_EQ(obj.n.s, "inner");
    CHECK_EQ(obj.sv, "view");

    return true;
}

bool test_parse_nested_with_array() {
    const jsonrefl::string_view_t json = R"({"n":{"i":5,"n":{"i":6,"s":"deep"},"sv":"sv_val"},"v":[{"i":7,"s":"a"},{"i":8,"s":"b"}]})";
    nested_with_array obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json.data(), json.size()).status() != jsonrefl::state::ok ) { return false; }
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
    const jsonrefl::string_view_t json = R"({"a":{"n":{"i":1,"n":{"i":2,"s":"leaf"},"sv":"sv1"},"v":[{"i":3,"s":"x"}]}})";
    nested_nested_nested obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json.data(), json.size()).status() != jsonrefl::state::ok ) { return false; }
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
    const jsonrefl::string_view_t json = R"({"symbol":"BTCUSDT","preventedMatchId":42,"takerOrderId":100,"makerOrderId":200,"tradeGroupId":300,"selfTradePreventionMode":"NONE","price":"50000.00","makerPreventedQuantity":"0.5","transactTime":1700000000})";
    myns::result_t obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json.data(), json.size()).status() != jsonrefl::state::ok ) { return false; }
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
    const jsonrefl::string_view_t json = R"({"rateLimitType":"ORDERS","interval":"SECOND","intervalNum":10,"limit":50,"count":3,"extra":{"source":"api","region":"us"},"codes":[100,200,300]})";
    myns::rate_limits_t obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json.data(), json.size()).status() != jsonrefl::state::ok ) { return false; }
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
    const jsonrefl::string_view_t json = R"({"id":"resp-001","status":200,"results":[{"symbol":"BTCUSDT","preventedMatchId":1,"takerOrderId":100,"makerOrderId":200,"tradeGroupId":300,"selfTradePreventionMode":"EXPIRE_MAKER","price":"50000.00","makerPreventedQuantity":"0.001","transactTime":1700000000}],"rate_limits":[{"rateLimitType":"ORDERS","interval":"SECOND","intervalNum":10,"limit":50,"count":3,"extra":{"src":"api"},"codes":[100,200]}],"headers":{"Content-Type":"application/json"},"warnings":["warning1","warning2"]})";
    myns::response_t obj{};
    auto p = jsonrefl::make_parser(&obj);
    if ( p.parse(json.data(), json.size()).status() != jsonrefl::state::ok ) { return false; }
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

/***************************************************************************************************/
// literal handling (true/false/null) singleshot
/***************************************************************************************************/

bool test_literal_true() {
    bool_test obj{false, 0};
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t json_sv_0 = R"({"flag":true,"val":1})";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.val, 1);
    CHECK_EQ(obj.flag, true);

    return true;
}

bool test_literal_false() {
    bool_test obj{true, 0};
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t json_sv_0 = R"({"flag":false,"val":2})";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.val, 2);
    CHECK_EQ(obj.flag, false);

    return true;
}

bool test_literal_null_opt_untouched() {
    null_test obj{};
    obj.opt = 42;
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t json_sv_0 = R"({"opt":null,"val":3})";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.val, 3);
    CHECK(obj.opt.has_value());
    CHECK_EQ(*obj.opt, 42);

    return true;
}

bool test_literal_null_opt_default() {
    null_test obj{};
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t json_sv_0 = R"({"opt":null,"val":5})";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.val, 5);
    CHECK(!obj.opt.has_value());

    return true;
}

bool test_literal_null_opt_vec_untouched() {
    null_arr_test obj{};
    obj.arr = std::vector<int>{1, 2, 3};
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t json_sv_0 = R"({"arr":null,"val":10})";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.val, 10);
    CHECK(obj.arr.has_value());
    CHECK_EQ(obj.arr->size(), 3u);

    return true;
}

bool test_literal_null_opt_vec_default() {
    null_arr_test obj{};
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t json_sv_0 = R"({"arr":null,"val":11})";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.val, 11);
    CHECK(!obj.arr.has_value());

    return true;
}

/***************************************************************************************************/
// literal handling chunked
/***************************************************************************************************/

bool test_literal_true_chunked() {
    bool_test obj{false, 0};
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t json_sv_0 = R"({"flag":tr)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_1 = R"(ue,"val":7})";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.val, 7);
    CHECK_EQ(obj.flag, true);

    return true;
}

bool test_literal_false_chunked() {
    bool_test obj{true, 0};
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t json_sv_0 = R"({"flag":fal)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_1 = R"(se,"val":8})";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.val, 8);
    CHECK_EQ(obj.flag, false);

    return true;
}

bool test_literal_null_chunked() {
    null_test obj{};
    obj.opt = 99;
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t json_sv_0 = R"({"opt":nu)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_1 = R"(ll,"val":9})";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.val, 9);
    CHECK(obj.opt.has_value());
    CHECK_EQ(*obj.opt, 99);

    return true;
}

/***************************************************************************************************/
// parser state / control flow (no_buffer, reset, accum)
/***************************************************************************************************/

bool test_parse_string_chunked_ok() {
    int_string obj{};
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({"i":42,"s":"hel)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_1 = R"(lo"})";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.i, 42);
    CHECK_EQ(obj.s, "hello");

    return true;
}

bool test_parse_no_buffer() {
    int_string obj{};
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t json_sv_0 = R"({"i":42,"s":"hel)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::no_buffer);

    return true;
}

bool test_parse_no_buffer_mid_accum() {
    int_string obj{};
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t a = R"({"i":42,"s":"hel)";
    const jsonrefl::string_view_t b = R"(lo"})";
    CHECK(p.parse(a.data(), a.size()).status() == jsonrefl::state::incomplete);
    CHECK(p.parse(b.data(), b.size()).status() == jsonrefl::state::ok);

    return true;
}


bool test_parse_no_accum_no_escape_ok() {
    std::vector<jsonrefl::string_view_t> obj;
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t js = R"(["plain","raw"])";
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 2u);

    return true;
}

bool test_parse_no_buffer_skipped_for_sv_key_with_escape() {
    std::map<jsonrefl::string_view_t, int> obj;
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t js = R"({"k\ney":1})";
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 1u);

    return true;
}

bool test_parse_reset_after_failure() {
    nested obj{};
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({"i":10,"n":{"i":20,"s":"inner"},"sv":"vi)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::sv_cross_chunk);

    p.reset();
    obj = nested{};

    const jsonrefl::string_view_t json_sv_1 = R"({"i":10,"n":{"i":20,"s":"inner"},"sv":"view"})";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.i, 10);
    CHECK_EQ(obj.n.i, 20);
    CHECK_EQ(obj.n.s, "inner");
    CHECK_EQ(obj.sv, "view");

    return true;
}

/***************************************************************************************************/
// zero-copy string_view parsing (singleshot)
/***************************************************************************************************/

bool test_zero_copy_nested_sv() {
    const jsonrefl::string_view_t json = R"({"i":10,"n":{"i":20,"s":"inner"},"sv":"view"})";
    const char *json_begin = json.data();
    const char *json_end   = json.data() + json.size();

    nested obj{};
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(json.data(), json.size()).status() == jsonrefl::state::ok);

    CHECK_EQ(obj.sv, "view");
    CHECK(obj.sv.data() >= json_begin && obj.sv.data() + obj.sv.size() <= json_end);

    return true;
}

bool test_zero_copy_sv_array() {
    const jsonrefl::string_view_t json = R"({"arr":["alpha","beta","gamma"]})";
    const char *json_begin = json.data();
    const char *json_end   = json.data() + json.size();

    sv_array obj{};
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(json.data(), json.size()).status() == jsonrefl::state::ok);

    CHECK_EQ(obj.arr.size(), 3u);
    for ( const auto &sv : obj.arr ) {
        CHECK(sv.data() >= json_begin && sv.data() + sv.size() <= json_end);
    }

    return true;
}

bool test_zero_copy_result_t() {
    const jsonrefl::string_view_t json = R"({"symbol":"BTCUSDT","preventedMatchId":1,"takerOrderId":100,"makerOrderId":200,"tradeGroupId":300,"selfTradePreventionMode":"NONE","price":"50000.00","makerPreventedQuantity":"0.5","transactTime":1700000000})";
    const char *json_begin = json.data();
    const char *json_end   = json.data() + json.size();

    myns::result_t obj{};
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(json.data(), json.size()).status() == jsonrefl::state::ok);

    CHECK(obj.symbol.data() >= json_begin && obj.symbol.data() + obj.symbol.size() <= json_end);
    CHECK(obj.selfTradePreventionMode.data() >= json_begin && obj.selfTradePreventionMode.data() + obj.selfTradePreventionMode.size() <= json_end);
    CHECK(obj.price.data() >= json_begin && obj.price.data() + obj.price.size() <= json_end);
    CHECK(obj.makerPreventedQuantity.data() >= json_begin && obj.makerPreventedQuantity.data() + obj.makerPreventedQuantity.size() <= json_end);

    return true;
}

/***************************************************************************************************/
// string_view chunked rejection (sv_cross_chunk)
/***************************************************************************************************/

bool test_parse_sv_chunked_reject() {
    nested obj{};
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({"i":10,"n":{"i":20,"s":"inner"},"sv":"vi)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::sv_cross_chunk);

    return true;
}

bool test_parse_sv_array_chunked_reject() {
    sv_array obj{};
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({"arr":["he)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::sv_cross_chunk);

    return true;
}

bool test_parse_map_sv_value_chunked_reject() {
    std::map<std::string, jsonrefl::string_view_t> obj;
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({"key":"he)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::sv_cross_chunk);

    return true;
}

bool test_parse_map_sv_key_chunked_reject() {
    std::map<jsonrefl::string_view_t, int> obj;
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({"abcde)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::sv_cross_chunk);

    return true;
}

bool test_parse_sv_array_singleshot_still_ok() {
    sv_array obj{};
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t json_sv_0 = R"({"arr":["alpha","beta","gamma"]})";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.arr.size(), 3u);
    CHECK_EQ(obj.arr[0], "alpha");

    return true;
}

bool test_parse_map_sv_key_singleshot_still_ok() {
    std::map<jsonrefl::string_view_t, int> obj;
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t json_sv_0 = R"({"a":1,"b":2})";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 2u);
    CHECK_EQ(obj.at("a"), 1);

    return true;
}

/***************************************************************************************************/
// escape sequence decoding (singleshot, with accum)
/***************************************************************************************************/

bool test_map_escape_in_key_and_value() {
    std::map<std::string, std::string> obj;
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t js = R"({"k\ney":"va\nl\tue"})";
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 1u);
    const std::string dk{"k\ney"};
    const std::string dv{"va\nl\tue"};
    CHECK_EQ(obj.at(dk), dv);
    CHECK_EQ(dk.size(), 4u);
    CHECK_EQ(dv.size(), 7u);
    CHECK_EQ(obj.at(dk).size(), dv.size());

    return true;
}

bool test_map_escape_unicode_in_key_and_value() {
    std::map<std::string, std::string> obj;
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t js = R"({"a\u0042c":"x\u00e9y"})";
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 1u);
    const std::string dk{"aBc"};
    const std::string dv{"x\xC3\xA9y"};
    CHECK_EQ(obj.at(dk), dv);
    CHECK_EQ(dk.size(), 3u);
    CHECK_EQ(dv.size(), 4u);
    CHECK_EQ(obj.at(dk).size(), dv.size());

    return true;
}

bool test_map_escape_in_key_with_int_value() {
    std::map<std::string, int> obj;
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t js = R"({"k\"y":42,"\\path":7})";
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 2u);
    const std::string k0{"k\"y"};
    const std::string k1{"\\path"};
    CHECK_EQ(obj.at(k0), 42);
    CHECK_EQ(obj.at(k1), 7);
    CHECK_EQ(k0.size(), 3u);
    CHECK_EQ(k1.size(), 5u);

    return true;
}

bool test_array_escape_in_string_elements() {
    std::vector<std::string> obj;
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t js = R"(["one\ntwo","three\tfour","fi\u00e9ve"])";
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 3u);
    const std::string e0{"one\ntwo"};
    const std::string e1{"three\tfour"};
    const std::string e2{"fi\xC3\xA9ve"};
    CHECK_EQ(obj[0], e0);
    CHECK_EQ(obj[1], e1);
    CHECK_EQ(obj[2], e2);
    CHECK_EQ(e0.size(), 7u);
    CHECK_EQ(e1.size(), 10u);
    CHECK_EQ(e2.size(), 6u);
    CHECK_EQ(obj[0].size(), e0.size());
    CHECK_EQ(obj[1].size(), e1.size());
    CHECK_EQ(obj[2].size(), e2.size());

    return true;
}

bool test_struct_unicode_escape_in_field_name() {
    kv_si obj{};
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t js = R"({"\u0073":"hello","\u0069":42})";
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.s, std::string("hello"));
    CHECK_EQ(obj.i, 42);
    CHECK_EQ(obj.s.size(), 5u);

    return true;
}

/***************************************************************************************************/
// escape signaling: parse() returns no_buffer when accum is missing
/***************************************************************************************************/

bool test_parse_no_buffer_escape_in_map_key() {
    std::map<std::string, std::string> obj;
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t js = R"({"k\ney":"value"})";
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::no_buffer);

    return true;
}

bool test_parse_no_buffer_escape_in_map_value() {
    std::map<std::string, std::string> obj;
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t js = R"({"key":"va\nlue"})";
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::no_buffer);

    return true;
}

bool test_parse_no_buffer_escape_in_map_int_key() {
    std::map<std::string, int> obj;
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t js = R"({"k\"y":42})";
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::no_buffer);
    return true;
}

bool test_parse_no_buffer_escape_in_array_element() {
    std::vector<std::string> obj;
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t js = R"(["he\nllo"])";
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::no_buffer);

    return true;
}

bool test_parse_no_buffer_escape_in_struct_field_name() {
    kv_si obj{};
    auto p = jsonrefl::make_parser(&obj);
    const jsonrefl::string_view_t js = R"({"\u0073":"hi","\u0069":1})";
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::no_buffer);

    return true;
}

/***************************************************************************************************/
// chunked parsing: leaf atomicity / no-split primitives
/***************************************************************************************************/

bool test_chunked_no_split_bool_true() {
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

    return true;
}

bool test_chunked_no_split_bool_false() {
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

    return true;
}

bool test_chunked_no_split_null() {
    null_test obj{};
    obj.opt = jsonrefl::optional_t<int>{};
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

    return true;
}

bool test_chunked_no_split_map_keys() {
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

    return true;
}

bool test_chunked_no_split_nested_keys_nums() {
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

    return true;
}

bool test_chunked_atomic_leaf_terminator() {
    // mix of numbers, literals and strings, all of which must stay
    // glued to their trailing `,`/`}`.
    nested_with_array obj{};
    obj.n.i = 12345;
    obj.n.n.i = -99;
    obj.n.n.s = "abc";
    obj.n.sv = "xy";
    obj.v.push_back(int_string{1, "p"});
    obj.v.push_back(int_string{2, "q"});

    std::vector<std::string> chunks;
    char cbuf[16];
    const auto ok = jsonrefl::to_chunked_buffer(cbuf, sizeof(cbuf), obj,
        [&](const void *d, std::size_t s) -> bool {
            chunks.emplace_back(static_cast<const char*>(d), s);
            return true;
        }
    );
    CHECK(ok);
    CHECK(chunks.size() > 1u); // sanity: actually multi-chunk

    // every leaf+terminator pair we expect in the output:
    //   numbers `12345,`, `-99,`, `1,`, `2,`,
    //   string-leaf followed by `,` or `}`,
    //   key colons emitted atomically with the key.
    CHECK(!is_split_across(chunks, "12345,"));
    CHECK(!is_split_across(chunks, "-99,"));
    CHECK(!is_split_across(chunks, "\"abc\","));
    CHECK(!is_split_across(chunks, "\"xy\""));
    CHECK(!is_split_across(chunks, "1,"));
    CHECK(!is_split_across(chunks, "2,"));
    CHECK(!is_split_across(chunks, "\"p\""));
    CHECK(!is_split_across(chunks, "\"q\""));
    // last-leaf-in-container leaves are emitted with their closing brace(s)
    // glued to them: `"xy"}` ends the inner struct, `"q"}]` ends the array.
    CHECK(!is_split_across(chunks, "\"xy\"}"));
    CHECK(!is_split_across(chunks, "\"q\"}"));

    return true;
}

bool test_chunked_long_string_roundtrip() {
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
    CHECK(p.parse(chunked.data(), chunked.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(parsed.i, 7);
    CHECK_EQ(parsed.s, long_value);

    return true;
}

bool test_chunked_long_string_chunked_deser() {
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
    auto p = jsonrefl::make_parser(&parsed, &accum);
    jsonrefl::state last = jsonrefl::state::incomplete;
    for ( const auto &chunk : chunks ) {
        last = p.parse(chunk.data(), chunk.size()).status();
        if ( last == jsonrefl::state::invalid ) { CHECK(false); break; }
    }
    CHECK(last == jsonrefl::state::ok);
    CHECK_EQ(parsed.i, 42);
    CHECK_EQ(parsed.s, long_value);

    return true;
}

/***************************************************************************************************/
// chunked parsing: complex containers (split keys/values, escapes)
/***************************************************************************************************/

bool test_chunked_map_string_string_split_key_value() {
    std::map<std::string, std::string> obj;
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({"hel)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_1 = R"(lo":"wor)";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_2 = R"(ld"})";
    CHECK(p.parse(json_sv_2.data(), json_sv_2.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj.at("hello"), std::string("world"));

    return true;
}

bool test_chunked_map_string_string_two_pairs_all_split() {
    std::map<std::string, std::string> obj;
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({"firs)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_1 = R"(t":"on)";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_2 = R"(e","seco)";
    CHECK(p.parse(json_sv_2.data(), json_sv_2.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_3 = R"(nd":"tw)";
    CHECK(p.parse(json_sv_3.data(), json_sv_3.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_4 = R"(o"})";
    CHECK(p.parse(json_sv_4.data(), json_sv_4.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 2u);
    CHECK_EQ(obj.at("first"), std::string("one"));
    CHECK_EQ(obj.at("second"), std::string("two"));

    return true;
}

bool test_chunked_map_string_int_split_key_and_value() {
    std::map<std::string, int> obj;
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({"alph)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_1 = R"(a":12)";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_2 = R"(345})";
    CHECK(p.parse(json_sv_2.data(), json_sv_2.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj.at("alpha"), 12345);

    return true;
}

bool test_chunked_struct_split_key_then_split_value() {
    kv_si obj{};
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({")";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_1 = R"(s":"hel)";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_2 = R"(lo","i":4)";
    CHECK(p.parse(json_sv_2.data(), json_sv_2.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_3 = R"(2})";
    CHECK(p.parse(json_sv_3.data(), json_sv_3.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.s, std::string("hello"));
    CHECK_EQ(obj.i, 42);

    return true;
}

bool test_chunked_struct_two_string_fields_all_split() {
    kv_ss obj{};
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({")";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_1 = R"(s":"hel)";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_2 = R"(lo",")";
    CHECK(p.parse(json_sv_2.data(), json_sv_2.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_3 = R"(t":"wor)";
    CHECK(p.parse(json_sv_3.data(), json_sv_3.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_4 = R"(ld"})";
    CHECK(p.parse(json_sv_4.data(), json_sv_4.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.s, std::string("hello"));
    CHECK_EQ(obj.t, std::string("world"));

    return true;
}

bool test_chunked_map_mixed_split_alignment() {
    std::map<std::string, std::string> obj;
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({"k1":"a","ke)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_1 = R"(y2":"b","k)";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_2 = R"(3":"valu)";
    CHECK(p.parse(json_sv_2.data(), json_sv_2.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_3 = R"(e3"})";
    CHECK(p.parse(json_sv_3.data(), json_sv_3.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 3u);
    CHECK_EQ(obj.at("k1"), std::string("a"));
    CHECK_EQ(obj.at("key2"), std::string("b"));
    CHECK_EQ(obj.at("k3"), std::string("value3"));

    return true;
}

bool test_chunked_struct_split_value_with_escape() {
    kv_ss obj{};
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({"s":"v\n)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_1 = R"(al","t":"u")";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_2 = R"(})";
    CHECK(p.parse(json_sv_2.data(), json_sv_2.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.s, std::string("v\nal"));
    CHECK_EQ(obj.t, std::string("u"));
    CHECK_EQ(obj.s.size(), 4u);
    CHECK_EQ(obj.t.size(), 1u);

    return true;
}

bool test_chunked_map_escape_in_key_and_value() {
    std::map<std::string, std::string> obj;
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"({"k\n)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_1 = R"(ey":"va\t)";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_2 = R"(lue"})";
    CHECK(p.parse(json_sv_2.data(), json_sv_2.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 1u);
    const std::string dk{"k\ney"};
    const std::string dv{"va\tlue"};
    CHECK_EQ(obj.at(dk), dv);
    CHECK_EQ(dk.size(), 4u);
    CHECK_EQ(dv.size(), 6u);
    CHECK_EQ(obj.at(dk).size(), dv.size());

    return true;
}

bool test_chunked_array_escape_in_elements() {
    std::vector<std::string> obj;
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    const jsonrefl::string_view_t json_sv_0 = R"(["he\n)";
    CHECK(p.parse(json_sv_0.data(), json_sv_0.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_1 = R"(llo","wo\u00e9)";
    CHECK(p.parse(json_sv_1.data(), json_sv_1.size()).status() == jsonrefl::state::incomplete);
    const jsonrefl::string_view_t json_sv_2 = R"(rld"])";
    CHECK(p.parse(json_sv_2.data(), json_sv_2.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 2u);
    const std::string e0{"he\nllo"};
    const std::string e1{"wo\xC3\xA9rld"};
    CHECK_EQ(obj[0], e0);
    CHECK_EQ(obj[1], e1);
    CHECK_EQ(e0.size(), 6u);
    CHECK_EQ(e1.size(), 7u);
    CHECK_EQ(obj[0].size(), e0.size());
    CHECK_EQ(obj[1].size(), e1.size());

    return true;
}

/***************************************************************************************************/
// in-source parsing (parse_m): primitives + escape decoding in-place
/***************************************************************************************************/

bool test_in_source_int_string() {
    char buf[] = R"({"i":123,"s":"hello"})";
    int_string obj{};
    auto p = jsonrefl::make_parser(&obj);
    auto s = p.parse_m(buf, sizeof(buf) - 1);
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK_EQ(obj.i, 123);
    CHECK_EQ(obj.s, "hello");

    return true;
}

bool test_in_source_string_escapes() {
    char buf[] = R"({"i":7,"s":"a\nb\tc\"d\\e\/f"})";
    int_string obj{};
    auto p = jsonrefl::make_parser(&obj);
    auto s = p.parse_m(buf, sizeof(buf) - 1);
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK_EQ(obj.i, 7);
    const std::string exp{"a\nb\tc\"d\\e/f"};
    CHECK_EQ(obj.s, exp);
    CHECK_EQ(exp.size(), 11u);
    CHECK_EQ(obj.s.size(), exp.size());

    return true;
}

bool test_in_source_unicode_bmp() {
    char buf[] = R"({"s":"\u00e9\u00e8 \u4e2d"})";
    std::map<std::string, std::string> obj;
    auto p = jsonrefl::make_parser(&obj);
    auto s = p.parse_m(buf, sizeof(buf) - 1);
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK_EQ(obj.at("s"), std::string("\xC3\xA9\xC3\xA8 \xE4\xB8\xAD"));
    CHECK_EQ(obj.at("s").size(), 8u);

    return true;
}

bool test_in_source_unicode_surrogate() {
    char buf[] = R"({"s":"\uD83D\uDE00"})";
    std::map<std::string, std::string> obj;
    auto p = jsonrefl::make_parser(&obj);
    auto s = p.parse_m(buf, sizeof(buf) - 1);
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK_EQ(obj.at("s"), std::string("\xF0\x9F\x98\x80"));
    CHECK_EQ(obj.at("s").size(), 4u);

    return true;
}

bool test_in_source_sv_zero_copy() {
    char buf[] = R"({"i":1,"n":{"i":2,"s":"abc"},"sv":"plain"})";
    nested obj{};
    auto p = jsonrefl::make_parser(&obj);
    auto s = p.parse_m(buf, sizeof(buf) - 1);
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK_EQ(obj.i, 1);
    CHECK_EQ(obj.n.i, 2);
    CHECK_EQ(obj.n.s, "abc");
    CHECK_EQ(obj.sv, "plain");
    const char *bptr = buf;
    const char *bend = buf + sizeof(buf) - 1;
    CHECK(obj.sv.data() >= bptr && obj.sv.data() + obj.sv.size() <= bend);

    return true;
}

bool test_in_source_sv_decoded_into_buffer() {
    char buf[] = R"({"i":0,"n":{"i":0,"s":""},"sv":"a\nb\tc"})";
    nested obj{};
    auto p = jsonrefl::make_parser(&obj);
    auto s = p.parse_m(buf, sizeof(buf) - 1);
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK_EQ(obj.sv, std::string("a\nb\tc"));
    const std::string expsv{"a\nb\tc"};
    CHECK_EQ(expsv.size(), 5u);
    CHECK_EQ(obj.sv.size(), expsv.size());
    const char *bptr = buf;
    const char *bend = buf + sizeof(buf) - 1;
    CHECK(obj.sv.data() >= bptr && obj.sv.data() + obj.sv.size() <= bend);

    return true;
}

bool test_in_source_sv_array() {
    char buf[] = R"({"arr":["one","tw\no","th\u0041ree"]})";
    sv_array obj{};
    auto p = jsonrefl::make_parser(&obj);
    auto s = p.parse_m(buf, sizeof(buf) - 1);
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK_EQ(obj.arr.size(), 3u);
    CHECK_EQ(obj.arr[0], "one");
    CHECK_EQ(obj.arr[1], std::string("tw\no"));
    CHECK_EQ(obj.arr[2], "thAree");
    CHECK_EQ(obj.arr[1].size(), 4u);
    CHECK_EQ(obj.arr[2].size(), 6u);
    for ( const auto &sv: obj.arr ) {
        CHECK(sv.data() >= buf && sv.data() + sv.size() <= buf + sizeof(buf) - 1);
    }

    return true;
}

bool test_in_source_extra_data() {
    char buf[] = R"({"i":1,"s":"x"} extra)";
    int_string obj{};
    auto p = jsonrefl::make_parser(&obj);
    auto s = p.parse_m(buf, sizeof(buf) - 1);
    CHECK(s.status() == jsonrefl::state::record_end);

    return true;
}

bool test_in_source_invalid() {
    char buf[] = R"({"i":@,"s":"x"})";
    int_string obj{};
    auto p = jsonrefl::make_parser(&obj);
    auto s = p.parse_m(buf, sizeof(buf) - 1);
    CHECK(s.status() == jsonrefl::state::invalid);

    return true;
}

bool test_in_source_invalid_escape() {
    char buf[] = R"({"i":1,"s":"\q"})";
    int_string obj{};
    auto p = jsonrefl::make_parser(&obj);
    auto s = p.parse_m(buf, sizeof(buf) - 1);
    CHECK(s.status() == jsonrefl::state::invalid);

    return true;
}

bool test_in_source_sv_cross_chunk_string() {
    char buf[] = R"({"i":1,"s":"abc)";
    int_string obj{};
    auto p = jsonrefl::make_parser(&obj);
    auto s = p.parse_m(buf, sizeof(buf) - 1);
    CHECK(s.status() == jsonrefl::state::sv_cross_chunk);

    return true;
}

bool test_parse_m_escape_no_accum_ok() {
    kv_ss obj{};
    static const jsonrefl::string_view_t k_json{R"({"s":"va\nl","t":"x\ty"})"};
    std::vector<char> buf(k_json.begin(), k_json.end());
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse_m(buf.data(), buf.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.s, std::string("va\nl"));
    CHECK_EQ(obj.t, std::string("x\ty"));
    CHECK_EQ(obj.s.size(), 4u);
    CHECK_EQ(obj.t.size(), 3u);

    return true;
}

bool test_parse_m_escape_in_map() {
    std::map<std::string, std::string> obj;
    static const jsonrefl::string_view_t k_json{R"({"k\ney":"va\tlue"})"};
    std::vector<char> buf(k_json.begin(), k_json.end());
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse_m(buf.data(), buf.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 1u);
    const std::string dk{"k\ney"};
    const std::string dv{"va\tlue"};
    CHECK_EQ(obj.at(dk), dv);
    CHECK_EQ(dk.size(), 4u);
    CHECK_EQ(dv.size(), 6u);
    CHECK_EQ(obj.at(dk).size(), dv.size());

    return true;
}

/***************************************************************************************************/
// root-level container parsing (vector<struct>, map<string,struct>)
/***************************************************************************************************/

bool test_root_vector_struct_singleshot() {
    std::vector<kv_ss> obj;
    const jsonrefl::string_view_t js = R"([{"s":"alpha","t":"one"},{"s":"beta","t":"two"},{"s":"gamma","t":"three"}])";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 3u);
    CHECK_EQ(obj[0].s, std::string("alpha"));
    CHECK_EQ(obj[0].t, std::string("one"));
    CHECK_EQ(obj[1].s, std::string("beta"));
    CHECK_EQ(obj[1].t, std::string("two"));
    CHECK_EQ(obj[2].s, std::string("gamma"));
    CHECK_EQ(obj[2].t, std::string("three"));

    return true;
}

bool test_root_vector_struct_with_int_singleshot() {
    std::vector<kv_si> obj;
    const jsonrefl::string_view_t js = R"([{"s":"alpha","i":1},{"s":"beta","i":2}])";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 2u);
    CHECK_EQ(obj[0].s, std::string("alpha"));
    CHECK_EQ(obj[0].i, 1);
    CHECK_EQ(obj[1].s, std::string("beta"));
    CHECK_EQ(obj[1].i, 2);

    return true;
}

bool test_root_vector_struct_chunked_4byte() {
    std::vector<kv_ss> obj;
    const jsonrefl::string_view_t js = R"([{"s":"alpha","t":"one"},{"s":"beta","t":"two"},{"s":"gamma","t":"three"}])";
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    for ( std::size_t off = 0; off < js.size(); off += 4 ) {
        const auto take = std::min<std::size_t>(4, js.size() - off);
        const auto s = p.parse(js.data() + off, take);
        const bool last = (off + take >= js.size());
        CHECK(s.status() == (last ? jsonrefl::state::ok : jsonrefl::state::incomplete));
    }
    CHECK_EQ(obj.size(), 3u);
    CHECK_EQ(obj[0].s, std::string("alpha"));
    CHECK_EQ(obj[0].t, std::string("one"));
    CHECK_EQ(obj[1].s, std::string("beta"));
    CHECK_EQ(obj[1].t, std::string("two"));
    CHECK_EQ(obj[2].s, std::string("gamma"));
    CHECK_EQ(obj[2].t, std::string("three"));

    return true;
}

bool test_root_vector_struct_chunked_with_escapes() {
    std::vector<kv_ss> obj;
    const jsonrefl::string_view_t js = R"([{"s":"a\nb","t":"c\td"},{"s":"e\ff","t":"g\\h"}])";
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    for ( std::size_t off = 0; off < js.size(); off += 3 ) {
        const auto take = std::min<std::size_t>(3, js.size() - off);
        const auto s = p.parse(js.data() + off, take);
        const bool last = (off + take >= js.size());
        CHECK(s.status() == (last ? jsonrefl::state::ok : jsonrefl::state::incomplete));
    }
    CHECK_EQ(obj.size(), 2u);
    const std::string s0{"a\nb"};
    const std::string t0{"c\td"};
    const std::string s1{"e\ff"};
    const std::string t1{"g\\h"};
    CHECK_EQ(obj[0].s, s0);
    CHECK_EQ(obj[0].t, t0);
    CHECK_EQ(obj[1].s, s1);
    CHECK_EQ(obj[1].t, t1);
    CHECK_EQ(s0.size(), 3u);
    CHECK_EQ(t0.size(), 3u);
    CHECK_EQ(s1.size(), 3u);
    CHECK_EQ(t1.size(), 3u);
    CHECK_EQ(obj[0].s.size(), s0.size());
    CHECK_EQ(obj[0].t.size(), t0.size());
    CHECK_EQ(obj[1].s.size(), s1.size());
    CHECK_EQ(obj[1].t.size(), t1.size());

    return true;
}

bool test_root_map_struct_singleshot() {
    std::map<std::string, kv_ss> obj;
    const jsonrefl::string_view_t js = R"({"a":{"s":"alpha","t":"one"},"b":{"s":"beta","t":"two"}})";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 2u);
    CHECK_EQ(obj.at("a").s, std::string("alpha"));
    CHECK_EQ(obj.at("a").t, std::string("one"));
    CHECK_EQ(obj.at("b").s, std::string("beta"));
    CHECK_EQ(obj.at("b").t, std::string("two"));

    return true;
}

bool test_root_map_struct_chunked_4byte() {
    std::map<std::string, kv_ss> obj;
    const jsonrefl::string_view_t js = R"({"a":{"s":"alpha","t":"one"},"b":{"s":"beta","t":"two"}})";
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    jsonrefl::state last = jsonrefl::state::incomplete;
    for ( std::size_t off = 0; off < js.size(); off += 4 ) {
        const auto take = std::min<std::size_t>(4, js.size() - off);
        last = p.parse(js.data() + off, take).status();
        const bool is_last = (off + take >= js.size());
        CHECK(last == (is_last ? jsonrefl::state::ok : jsonrefl::state::incomplete));
    }
    CHECK_EQ(obj.size(), 2u);
    CHECK_EQ(obj.at("a").s, std::string("alpha"));
    CHECK_EQ(obj.at("b").t, std::string("two"));

    return true;
}

bool test_nested_vector_vector_string_parse_and_roundtrip() {
    nest_vv_string obj{};
    const jsonrefl::string_view_t js =
        R"({"rows":[["a","bc"],[],["",""],["z"]]})";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);

    CHECK_EQ(obj.rows.size(), 4u);
    CHECK_EQ(obj.rows[0].size(), 2u);
    CHECK_EQ(obj.rows[0][0], std::string("a"));
    CHECK_EQ(obj.rows[0][1], std::string("bc"));
    CHECK_EQ(obj.rows[1].size(), 0u);
    CHECK_EQ(obj.rows[2].size(), 2u);
    CHECK_EQ(obj.rows[2][0], std::string());
    CHECK_EQ(obj.rows[2][1], std::string());
    CHECK_EQ(obj.rows[3].size(), 1u);
    CHECK_EQ(obj.rows[3][0], std::string("z"));

    const auto json = jsonrefl::to_string(obj);
    nest_vv_string round{};
    auto p2 = jsonrefl::make_parser(&round);
    CHECK(p2.parse(json.data(), json.size()).status() == jsonrefl::state::ok);

    CHECK_EQ(round.rows.size(), obj.rows.size());
    for ( std::size_t i = 0; i < obj.rows.size(); ++i ) {
        CHECK_EQ(round.rows[i].size(), obj.rows[i].size());
        for ( std::size_t j = 0; j < obj.rows[i].size(); ++j ) {
            CHECK_EQ(round.rows[i][j], obj.rows[i][j]);
        }
    }

    return true;
}

bool test_nested_vector_vector_int_parse_and_roundtrip() {
    nest_vv_int obj{};
    const jsonrefl::string_view_t js = R"({"rows":[[-1,0,2147483647],[42],[],[0]]})";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);

    CHECK_EQ(obj.rows.size(), 4u);
    CHECK_EQ(obj.rows[0].size(), 3u);
    CHECK_EQ(obj.rows[0][0], -1);
    CHECK_EQ(obj.rows[0][1], 0);
    CHECK_EQ(obj.rows[0][2], 2147483647);
    CHECK_EQ(obj.rows[1].size(), 1u);
    CHECK_EQ(obj.rows[1][0], 42);
    CHECK_EQ(obj.rows[2].size(), 0u);
    CHECK_EQ(obj.rows[3].size(), 1u);
    CHECK_EQ(obj.rows[3][0], 0);

    const auto json = jsonrefl::to_string(obj);
    nest_vv_int round{};
    auto p2 = jsonrefl::make_parser(&round);
    CHECK(p2.parse(json.data(), json.size()).status() == jsonrefl::state::ok);

    CHECK_EQ(round.rows.size(), obj.rows.size());
    for ( std::size_t i = 0; i < obj.rows.size(); ++i ) {
        CHECK_EQ(round.rows[i].size(), obj.rows[i].size());
        for ( std::size_t j = 0; j < obj.rows[i].size(); ++j ) {
            CHECK_EQ(round.rows[i][j], obj.rows[i][j]);
        }
    }

    return true;
}

bool test_nested_vector_vector_struct_parse_and_roundtrip() {
    nest_vv_pair_xy obj{};
    const jsonrefl::string_view_t js =
        R"({"rows":[[{"x":1,"y":2},{"x":3,"y":4}],[{"x":0,"y":0}],[],[{"x":-7,"y":999}]]})";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);

    CHECK_EQ(obj.rows.size(), 4u);
    CHECK_EQ(obj.rows[0].size(), 2u);
    CHECK_EQ(obj.rows[0][0].x, 1);
    CHECK_EQ(obj.rows[0][0].y, 2);
    CHECK_EQ(obj.rows[0][1].x, 3);
    CHECK_EQ(obj.rows[0][1].y, 4);
    CHECK_EQ(obj.rows[1].size(), 1u);
    CHECK_EQ(obj.rows[1][0].x, 0);
    CHECK_EQ(obj.rows[1][0].y, 0);
    CHECK_EQ(obj.rows[2].size(), 0u);
    CHECK_EQ(obj.rows[3].size(), 1u);
    CHECK_EQ(obj.rows[3][0].x, -7);
    CHECK_EQ(obj.rows[3][0].y, 999);

    const auto json = jsonrefl::to_string(obj);
    nest_vv_pair_xy round{};
    auto p2 = jsonrefl::make_parser(&round);
    CHECK(p2.parse(json.data(), json.size()).status() == jsonrefl::state::ok);

    CHECK_EQ(round.rows.size(), obj.rows.size());
    for ( std::size_t i = 0; i < obj.rows.size(); ++i ) {
        CHECK_EQ(round.rows[i].size(), obj.rows[i].size());
        for ( std::size_t j = 0; j < obj.rows[i].size(); ++j ) {
            CHECK_EQ(round.rows[i][j].x, obj.rows[i][j].x);
            CHECK_EQ(round.rows[i][j].y, obj.rows[i][j].y);
        }
    }

    return true;
}

bool test_root_vector_vector_string_parse_and_roundtrip() {
    std::vector<std::vector<std::string>> obj{};
    const jsonrefl::string_view_t js = R"([["a","bc"],[],["",""],["z"]])";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 4u);
    CHECK_EQ(obj[0][1], std::string("bc"));
    CHECK(obj[1].empty());
    CHECK_EQ(obj[3][0], std::string("z"));
    const auto json = jsonrefl::to_string(obj);
    std::vector<std::vector<std::string>> round{};
    auto p2 = jsonrefl::make_parser(&round);
    CHECK(p2.parse(json.data(), json.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(round.size(), obj.size());
    for ( std::size_t i = 0; i < obj.size(); ++i ) {
        CHECK_EQ(round[i].size(), obj[i].size());
        for ( std::size_t j = 0; j < obj[i].size(); ++j ) {
            CHECK_EQ(round[i][j], obj[i][j]);
        }
    }

    return true;
}

bool test_root_vector_vector_int_parse_and_roundtrip() {
    std::vector<std::vector<int>> obj{};
    const jsonrefl::string_view_t js = R"([[-1,7],[99],[],[0]])";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 4u);
    CHECK_EQ(obj[0][0], -1);
    CHECK_EQ(obj[0][1], 7);
    CHECK_EQ(obj[1][0], 99);
    CHECK(obj[2].empty());
    const auto json = jsonrefl::to_string(obj);
    std::vector<std::vector<int>> round{};
    auto p2 = jsonrefl::make_parser(&round);
    CHECK(p2.parse(json.data(), json.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(round.size(), obj.size());
    for ( std::size_t i = 0; i < obj.size(); ++i ) {
        CHECK_EQ(round[i].size(), obj[i].size());
        for ( std::size_t j = 0; j < obj[i].size(); ++j ) {
            CHECK_EQ(round[i][j], obj[i][j]);
        }
    }

    return true;
}

bool test_root_vector_vector_struct_parse_and_roundtrip() {
    std::vector<std::vector<pair_xy>> obj{};
    const jsonrefl::string_view_t js =
        R"([[{"x":1,"y":2}],[{"x":0,"y":0}],[],[{"x":-1,"y":3}]])";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 4u);
    CHECK_EQ(obj[0][0].x, 1);
    CHECK_EQ(obj[2].size(), 0u);
    CHECK_EQ(obj[3][0].y, 3);
    const auto json = jsonrefl::to_string(obj);
    std::vector<std::vector<pair_xy>> round{};
    auto p2 = jsonrefl::make_parser(&round);
    CHECK(p2.parse(json.data(), json.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(round.size(), obj.size());
    for ( std::size_t i = 0; i < obj.size(); ++i ) {
        CHECK_EQ(round[i].size(), obj[i].size());
        for ( std::size_t j = 0; j < obj[i].size(); ++j ) {
            CHECK_EQ(round[i][j].x, obj[i][j].x);
            CHECK_EQ(round[i][j].y, obj[i][j].y);
        }
    }

    return true;
}

bool test_nested_map_map_int_parse_and_roundtrip() {
    nest_mm_int obj{};
    static const jsonrefl::string_view_t js =
        R"({"grids":{"a":{"x":1,"y":2},"empty":{}}})";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.grids.size(), 2u);
    CHECK_EQ(obj.grids.at("a").at("x"), 1);
    CHECK_EQ(obj.grids.at("a").at("y"), 2);
    CHECK(obj.grids.at("empty").empty());

    const auto json = jsonrefl::to_string(obj);
    nest_mm_int round{};
    auto p2 = jsonrefl::make_parser(&round);
    CHECK(p2.parse(json.data(), json.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(round.grids.size(), obj.grids.size());
    CHECK_EQ(round.grids.at("a").size(), obj.grids.at("a").size());
    CHECK_EQ(round.grids.at("a").at("x"), obj.grids.at("a").at("x"));
    CHECK_EQ(round.grids.at("a").at("y"), obj.grids.at("a").at("y"));
    CHECK(round.grids.at("empty").empty());

    return true;
}

bool test_nested_map_unordered_map_int_parse_and_roundtrip() {
    nest_mum_int obj{};
    static const jsonrefl::string_view_t js = R"({"layers":{"L1":{"u":-1,"v":42}}})";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.layers.size(), 1u);
    CHECK_EQ(obj.layers.at("L1").size(), 2u);
    CHECK_EQ(obj.layers.at("L1").at("u"), -1);
    CHECK_EQ(obj.layers.at("L1").at("v"), 42);

    const auto json = jsonrefl::to_string(obj);
    nest_mum_int round{};
    auto p2 = jsonrefl::make_parser(&round);
    CHECK(p2.parse(json.data(), json.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(round.layers.size(), obj.layers.size());
    CHECK_EQ(round.layers.at("L1").at("u"), obj.layers.at("L1").at("u"));
    CHECK_EQ(round.layers.at("L1").at("v"), obj.layers.at("L1").at("v"));

    return true;
}

bool test_nested_map_three_levels_parse_and_roundtrip() {
    nest_mmm_int obj{};
    static const jsonrefl::string_view_t js =
        R"({"cube":{"d1":{"s1":{"cell":100}},"empty":{}}})";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.cube.size(), 2u);
    CHECK_EQ(obj.cube.at("d1").at("s1").at("cell"), 100);
    CHECK(obj.cube.at("empty").empty());

    const auto json = jsonrefl::to_string(obj);
    nest_mmm_int round{};
    auto p2 = jsonrefl::make_parser(&round);
    CHECK(p2.parse(json.data(), json.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(round.cube.at("d1").at("s1").at("cell"), 100);
    CHECK(round.cube.at("empty").empty());

    return true;
}

bool test_root_map_map_int_parse_and_roundtrip() {
    std::map<std::string, std::map<std::string, int>> obj{};
    static const jsonrefl::string_view_t js = R"({"o":{"i":7,"j":0},"p":{}})";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.size(), 2u);
    CHECK_EQ(obj.at("o").at("i"), 7);
    CHECK_EQ(obj.at("o").at("j"), 0);
    CHECK(obj.at("p").empty());

    const auto json = jsonrefl::to_string(obj);
    std::map<std::string, std::map<std::string, int>> round{};
    auto p2 = jsonrefl::make_parser(&round);
    CHECK(p2.parse(json.data(), json.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(round.at("o").at("i"), obj.at("o").at("i"));
    CHECK(round.at("p").empty());

    return true;
}

/***************************************************************************************************/
// end-to-end round-trip (parse_m chunked, atomic-leaf C2 contract)
/***************************************************************************************************/

bool test_parse_m_chunked_round_trip() {
    // round-trip via to_chunked_buffer + chunked parse_m.
    std::vector<kv_ss> src;
    for ( auto i = 0u; i < 8u; ++i ) {
        kv_ss e;
        e.s = "field-" + std::to_string(i);
        e.t = "value-" + std::to_string(i * 31 + 7);
        src.push_back(std::move(e));
    }

    // each chunk needs to be its own buffer so older string_views
    // remain valid (C1).
    // collect them into a list of std::vector<char>.
    std::vector<std::vector<char>> chunks;
    constexpr std::size_t k_chunk = 64; // > max leaf + framing, keeps C2
    char tmp[k_chunk];
    jsonrefl::to_chunked_buffer(tmp, sizeof(tmp), src,
        [&](const void *data, std::size_t n) -> bool {
            chunks.emplace_back(
                 static_cast<const char*>(data)
                ,static_cast<const char*>(data) + n)
            ;
            return true;
        }
    );
    CHECK(chunks.size() >= 2u); // sanity: actually multi-chunk

    std::vector<kv_ss> sink;
    auto p = jsonrefl::make_parser(&sink);
    jsonrefl::state st = jsonrefl::state::incomplete;
    for ( auto &c: chunks ) {
        st = p.parse_m(c.data(), c.size()).status();
        const bool is_last = (&c == &chunks.back());
        CHECK(st == (is_last
                        ? jsonrefl::state::ok
                        : jsonrefl::state::incomplete)
        );
    }
    CHECK_EQ(sink.size(), src.size());
    for ( auto i = 0u; i < src.size(); ++i ) {
        CHECK_EQ(sink[i].s, src[i].s);
        CHECK_EQ(sink[i].t, src[i].t);
    }

    return true;
}

bool test_parse_m_chunked_round_trip_small_buf() {
    std::vector<kv_si> src;
    for ( auto i = 0u; i < 12u; ++i ) {
        kv_si e;
        e.s = "f-" + std::to_string(i);
        e.i = static_cast<int>(i * 7) - 17;
        src.push_back(std::move(e));
    }

    std::vector<std::vector<char>> chunks;
    char tmp[16]; // intentionally tight
    const auto ok = jsonrefl::to_chunked_buffer(tmp, sizeof(tmp), src,
        [&](const void *data, std::size_t n) -> bool {
            chunks.emplace_back(
                 static_cast<const char*>(data)
                ,static_cast<const char*>(data) + n)
            ;
            return true;
        }
    );
    CHECK(ok);
    CHECK(chunks.size() >= 4u); // sanity: many chunks

    std::vector<kv_si> sink;
    auto p = jsonrefl::make_parser(&sink);
    jsonrefl::state st = jsonrefl::state::incomplete;
    for ( auto &c: chunks ) {
        st = p.parse_m(c.data(), c.size()).status();
        const bool is_last = (&c == &chunks.back());
        // any sv_cross_chunk here would mean the writer broke C2.
        CHECK(st == (is_last
                        ? jsonrefl::state::ok
                        : jsonrefl::state::incomplete)
        );
    }
    CHECK_EQ(sink.size(), src.size());
    for ( auto i = 0u; i < src.size(); ++i ) {
        CHECK_EQ(sink[i].s, src[i].s);
        CHECK_EQ(sink[i].i, src[i].i);
    }

    return true;
}

bool test_parse_m_chunked_clean_boundary() {
    char buf1[] = R"({"i":42,)";
    char buf2[] = R"("s":"hi"})";
    kv_si obj{};
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse_m(buf1, sizeof(buf1) - 1).status() == jsonrefl::state::incomplete);
    CHECK(p.parse_m(buf2, sizeof(buf2) - 1).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.i, 42);
    CHECK_EQ(obj.s, std::string("hi"));

    return true;
}

/***************************************************************************************************/
// enumerations as JSON integers
/***************************************************************************************************/

bool test_metadata_enums() {
    static_assert(jsonrefl::has_metadata<with_enums>::value, "with_enums metadata");

    constexpr auto &m = jsonrefl::metadata<with_enums>();
    CHECK_EQ(m.name(), "with_enums");
    CHECK_EQ(m.size(), 4u);

    const auto *e0 = m.get("ec_field");
    CHECK(e0);
    CHECK_EQ(e0->is_int(), true);
    CHECK_EQ(e0->is_uint(), false);

    const auto *e1 = m.get("raw_enum_field");
    CHECK(e1);
    CHECK_EQ(e1->is_int(), true);
    CHECK_EQ(e1->is_uint(), true);

    const auto *e2 = m.get("opt_ec");
    CHECK(e2);
    CHECK_EQ(e2->is_int(), true);
    CHECK_EQ(e2->is_uint(), false);

    const auto *e3 = m.get("ec_vec");
    CHECK(e3);
    CHECK_EQ(e3->is_array(), true);

    return true;
}

bool test_enum_roundtrip_serialize_parse() {
    with_enums src{};
    src.ec_field = flavor_ec::strawberry;
    src.raw_enum_field = leg_b;
    src.opt_ec = flavor_ec::chocolate;
    src.ec_vec = {flavor_ec::vanilla, flavor_ec::chocolate};

    const auto json = jsonrefl::to_string(src);
    CHECK_EQ(jsonrefl::required_bytes(src), json.size());
    CHECK(json.find("\"ec_field\":42") != std::string::npos);
    CHECK(json.find("\"raw_enum_field\":200") != std::string::npos);
    CHECK(json.find("\"opt_ec\":7") != std::string::npos);
    CHECK(json.find("\"ec_vec\":[-1,7]") != std::string::npos);

    with_enums out{};
    auto p = jsonrefl::make_parser(&out);
    CHECK(p.parse(json.data(), json.size()).status() == jsonrefl::state::ok);
    CHECK(out.ec_field == flavor_ec::strawberry);
    CHECK(out.raw_enum_field == leg_b);
    CHECK(out.opt_ec.has_value());
    CHECK(*out.opt_ec == flavor_ec::chocolate);
    CHECK_EQ(out.ec_vec.size(), 2u);
    CHECK(out.ec_vec[0] == flavor_ec::vanilla);
    CHECK(out.ec_vec[1] == flavor_ec::chocolate);

    return true;
}

bool test_enum_parse_optional_and_empty_array() {
    const jsonrefl::string_view_t json = R"({"ec_field":42,"raw_enum_field":10,"opt_ec":null,"ec_vec":[]})";
    with_enums obj{};
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(json.data(), json.size()).status() == jsonrefl::state::ok);
    CHECK(obj.ec_field == flavor_ec::strawberry);
    CHECK(obj.raw_enum_field == leg_a);
    CHECK(!obj.opt_ec.has_value());
    CHECK(obj.ec_vec.empty());

    return true;
}

bool test_root_vector_enum_roundtrip() {
    std::vector<flavor_ec> src{flavor_ec::vanilla, flavor_ec::strawberry};
    const auto json = jsonrefl::to_string(src);
    CHECK_EQ(json, std::string("[-1,42]"));

    std::vector<flavor_ec> out{};
    auto p = jsonrefl::make_parser(&out);
    CHECK(p.parse(json.data(), json.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(out.size(), 2u);
    CHECK(out[0] == flavor_ec::vanilla);
    CHECK(out[1] == flavor_ec::strawberry);

    return true;
}

bool test_map_string_enum_roundtrip() {
    std::map<std::string, flavor_ec> src{{"a", flavor_ec::chocolate}, {"b", flavor_ec::vanilla}};
    const auto json = jsonrefl::to_string(src);
    CHECK(json.find("\"a\":7") != std::string::npos);
    CHECK(json.find("\"b\":-1") != std::string::npos);

    std::map<std::string, flavor_ec> out{};
    auto p = jsonrefl::make_parser(&out);
    CHECK(p.parse(json.data(), json.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(out.size(), 2u);
    CHECK(out["a"] == flavor_ec::chocolate);
    CHECK(out["b"] == flavor_ec::vanilla);

    return true;
}

/***************************************************************************************************/
// C2 contract violations (parse_m runtime detection)
/***************************************************************************************************/

bool test_parse_m_c2_violation_string() {
    char buf1[] = R"({"s":"hel)";
    kv_ss obj{};
    auto p = jsonrefl::make_parser(&obj);
    const auto s1 = p.parse_m(buf1, sizeof(buf1) - 1);
    CHECK(s1.status() == jsonrefl::state::sv_cross_chunk);

    p.reset();
    obj = {};
    char whole[] = R"({"s":"hello","t":"x"})";
    CHECK(p.parse_m(whole, sizeof(whole) - 1).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.s, std::string("hello"));
    CHECK_EQ(obj.t, std::string("x"));

    return true;
}

bool test_parse_m_c2_violation_number() {
    char buf1[] = R"({"i":12)";
    kv_si obj{};
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse_m(buf1, sizeof(buf1) - 1).status() == jsonrefl::state::sv_cross_chunk);

    return true;
}

bool test_parse_m_c2_violation_key() {
    char buf1[] = R"({"se)";
    kv_ss obj{};
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse_m(buf1, sizeof(buf1) - 1).status() == jsonrefl::state::sv_cross_chunk);

    return true;
}

bool test_parse_m_c2_violation_literal() {
    char buf1[] = R"({"opt":nu)";
    null_test obj{};
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse_m(buf1, sizeof(buf1) - 1).status() == jsonrefl::state::sv_cross_chunk);

    return true;
}

bool test_unknown_key_rejected() {
    int_string obj{};
    static const jsonrefl::string_view_t k_json{R"({"i":42,"ghost":0,"s":"hi"})"};
    jsonrefl::parser<int_string> p(&obj);
    CHECK(p.parse(k_json.data(), k_json.size()).status() == jsonrefl::state::unknown_key);
    CHECK_EQ(obj.i, 42);
    CHECK(obj.s.empty());
    return true;
}

bool test_skip_unknown_keys_object_value() {
    int_string obj{};
    static const jsonrefl::string_view_t k_json{R"({"i":42,"ghost":0,"s":"hi"})"};
    jsonrefl::parser<int_string> p(&obj, nullptr, jsonrefl::flags::skip_unknown_keys);
    CHECK(p.parse(k_json.data(), k_json.size()).status()
          == jsonrefl::state::ok);
    CHECK_EQ(obj.i, 42);
    CHECK_EQ(obj.s, std::string("hi"));
    return true;
}

bool test_skip_unknown_keys_nested_container() {
    int_string obj{};
    static const jsonrefl::string_view_t k_json{R"({"i":1,"ghost":{"a":[1,2],"b":true},"s":"z"})"};
    jsonrefl::parser<int_string> p(&obj, nullptr, jsonrefl::flags::skip_unknown_keys);
    CHECK(p.parse(k_json.data(), k_json.size()).status()
          == jsonrefl::state::ok);
    CHECK_EQ(obj.i, 1);
    CHECK_EQ(obj.s, std::string("z"));
    return true;
}

bool test_value_t_to_conversions() {
    jsonrefl::value_t n;
    n.assign("127", jsonrefl::value_kind::integer);
    CHECK(n.to_int8());
    const auto n8 = n.to_int8();
    CHECK(n8);
    CHECK_EQ(static_cast<int>(*n8), 127);

    jsonrefl::value_t bad;
    bad.assign("128", jsonrefl::value_kind::integer);
    CHECK(!bad.to_int8());

    jsonrefl::value_t ubad;
    ubad.assign("256", jsonrefl::value_kind::integer);
    CHECK(!ubad.to_uint8());

    jsonrefl::value_t trail;
    trail.assign("42 ", jsonrefl::value_kind::integer);
    CHECK(!trail.to_int32());

    jsonrefl::value_t bool_v;
    bool_v.assign("true", jsonrefl::value_kind::boolean);
    const auto bool_parsed = bool_v.to_bool();
    CHECK(bool_parsed);
    CHECK_EQ(*bool_parsed, true);

    jsonrefl::value_t fv;
    fv.assign("3.14", jsonrefl::value_kind::floating);
    CHECK(fv.to_double());
    CHECK(fv.to_float());
    CHECK(!fv.to_int32());

    jsonrefl::value_t iv;
    iv.assign("42", jsonrefl::value_kind::integer);
    CHECK(iv.to_int32());
    CHECK(!iv.to_double());

    jsonrefl::value_t sv;
    sv.assign("hello", jsonrefl::value_kind::string);
    const auto hello = sv.to_string();
    CHECK(hello);
    CHECK_EQ(*hello, std::string("hello"));

    jsonrefl::value_t empty;
    CHECK(empty.kind() == jsonrefl::value_kind::null);
    CHECK(!empty.to_string());

    return true;
}

bool test_value_t_integer_floating_kinds() {
    std::map<std::string, jsonrefl::value_t> obj;
    const jsonrefl::string_view_t js = R"({"i":-42,"z":0,"f":3.14,"e":1e5})";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK(obj.at("i").kind() == jsonrefl::value_kind::integer);
    CHECK(obj.at("z").kind() == jsonrefl::value_kind::integer);
    CHECK(obj.at("f").kind() == jsonrefl::value_kind::floating);
    CHECK(obj.at("e").kind() == jsonrefl::value_kind::floating);

    const auto i = obj.at("i").to_int32();
    CHECK(i);
    CHECK_EQ(*i, -42);
    const auto d = obj.at("f").to_double();
    CHECK(d);
    CHECK(d && *d > 3.13 && *d < 3.15);
    CHECK(!obj.at("f").to_int32());
    CHECK(!obj.at("i").to_double());

    return true;
}

bool test_value_t_nonstandard_float_kind() {
    std::map<std::string, jsonrefl::value_t> obj;
    const jsonrefl::string_view_t js = R"({"nan":NaN,"inf":Infinity})";
    auto p = jsonrefl::make_parser(
         &obj
        ,nullptr
        ,jsonrefl::flags::allow_infinity_and_nan
    );
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK(obj.at("nan").kind() == jsonrefl::value_kind::floating);
    CHECK(obj.at("inf").kind() == jsonrefl::value_kind::floating);
    CHECK(obj.at("nan").to_double());
    CHECK(obj.at("inf").to_double());

    return true;
}

bool test_value_t_parse_and_convert() {
    value_fields obj{};
    const jsonrefl::string_view_t js = R"({"n":42,"s":"hi","b":true,"z":null})";
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK(obj.n.kind() == jsonrefl::value_kind::integer);
    CHECK(obj.s.kind() == jsonrefl::value_kind::string);
    CHECK(obj.b.kind() == jsonrefl::value_kind::boolean);
    CHECK(obj.z.kind() == jsonrefl::value_kind::null);
    CHECK(obj.n.to_int32());
    const auto n_parsed = obj.n.to_int32();
    CHECK(n_parsed);
    CHECK_EQ(*n_parsed, 42);
    const auto s_str = obj.s.to_string();
    CHECK(s_str);
    CHECK_EQ(*s_str, std::string("hi"));
    const auto b_parsed = obj.b.to_bool();
    CHECK(b_parsed);
    CHECK_EQ(*b_parsed, true);
    CHECK(!obj.z.to_int32());

    return true;
}

bool test_value_t_parse_m_zero_copy() {
    char buf[] = R"({"n":99,"s":"abc"})";
    value_fields obj{};
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse_m(buf, sizeof(buf) - 1).status() == jsonrefl::state::ok);
    CHECK(obj.n.data() >= buf && obj.n.data() < buf + sizeof(buf));
    CHECK(obj.s.data() >= buf && obj.s.data() < buf + sizeof(buf));
    CHECK(obj.n.to_int32());
    const auto n_parsed = obj.n.to_int32();
    CHECK(n_parsed);
    CHECK_EQ(*n_parsed, 99);
    const auto s_str = obj.s.to_string();
    CHECK(s_str);
    CHECK_EQ(*s_str, std::string("abc"));

    return true;
}

bool test_value_t_roundtrip() {
    value_fields in{};
    in.n.assign("7", jsonrefl::value_kind::integer);
    in.s.assign("x\"y", jsonrefl::value_kind::string);
    in.b.assign("false", jsonrefl::value_kind::boolean);
    in.z.assign({}, jsonrefl::value_kind::null);

    const auto js = jsonrefl::to_string(in);
    value_fields out{};
    std::string accum;
    auto p = jsonrefl::make_parser(&out, &accum);
    CHECK(p.parse(js.data(), js.size()).status() == jsonrefl::state::ok);
    CHECK(out.n.to_int32());
    const auto n_parsed = out.n.to_int32();
    CHECK(n_parsed);
    CHECK_EQ(*n_parsed, 7);
    const auto s_str = out.s.to_string();
    CHECK(s_str);
    CHECK_EQ(*s_str, std::string("x\"y"));
    const auto b_parsed = out.b.to_bool();
    CHECK(b_parsed);
    CHECK_EQ(*b_parsed, false);
    CHECK(out.z.kind() == jsonrefl::value_kind::null);

    return true;
}

/***************************************************************************************************/

JSONREFL_STRUCT(float_fields, (double, d), (float, f));

JSONREFL_STRUCT(
    i64_fields
    ,(int64_t,  ival)
    ,(uint64_t, uval)
);

/***************************************************************************************************/

bool test_parse_m_c2_escape_split_sv_cross_chunk() {
    char buf1[] = R"({"s":"a\)";
    kv_ss obj{};
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse_m(buf1, sizeof(buf1) - 1).status() == jsonrefl::state::sv_cross_chunk);

    p.reset();
    obj = {};
    char whole[] = R"({"s":"a\nb","t":"x"})";
    CHECK(p.parse_m(whole, sizeof(whole) - 1).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.s, std::string("a\nb"));
    CHECK_EQ(obj.t, std::string("x"));

    return true;
}

bool test_parse_extra_data_trailing_garbage() {
    int_string obj{};
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    static const char j[] = R"({"i":1,"s":"x"} oops)";
    CHECK(p.parse(j, sizeof(j) - 1).status() == jsonrefl::state::record_end);
    return true;
}

bool test_float_required_bytes_matches_to_string() {
    {
        float_fields obj{1.0, 2.0f};
        const auto s = jsonrefl::to_string(obj);
        CHECK_EQ(jsonrefl::required_bytes(obj), s.size());
        float_fields parsed{};
        auto p = jsonrefl::make_parser(&parsed);
        CHECK(p.parse(s.data(), s.size()).status() == jsonrefl::state::ok);
        CHECK(parsed.d == 1.0);
        CHECK(parsed.f == 2.0f);
    }
    {
        float_fields obj{-1.5, 0.0f};
        const auto s = jsonrefl::to_string(obj);
        CHECK_EQ(jsonrefl::required_bytes(obj), s.size());
    }
    return true;
}

bool test_integer_extremes_roundtrip() {
    {
        i64_fields obj{std::numeric_limits<int64_t>::min(), 0u};
        const auto s = jsonrefl::to_string(obj);
        CHECK_EQ(jsonrefl::required_bytes(obj), s.size());
        i64_fields parsed{};
        auto p = jsonrefl::make_parser(&parsed);
        CHECK(p.parse(s.data(), s.size()).status() == jsonrefl::state::ok);
        CHECK(parsed.ival == std::numeric_limits<int64_t>::min());
        CHECK(parsed.uval == 0u);
    }
    {
        i64_fields obj{std::numeric_limits<int64_t>::max(), std::numeric_limits<uint64_t>::max()};
        const auto s = jsonrefl::to_string(obj);
        CHECK_EQ(jsonrefl::required_bytes(obj), s.size());
        i64_fields parsed{};
        auto p = jsonrefl::make_parser(&parsed);
        CHECK(p.parse(s.data(), s.size()).status() == jsonrefl::state::ok);
        CHECK(parsed.ival == std::numeric_limits<int64_t>::max());
        CHECK(parsed.uval == std::numeric_limits<uint64_t>::max());
    }
    return true;
}

bool test_to_chunked_buffer_flush_failure() {
    int_string obj{1, "hello"};
    int calls = 0;
    char buf[64];
    const bool ok = jsonrefl::to_chunked_buffer(buf, sizeof(buf), obj,
        [&](const void *, std::size_t) -> bool { return ++calls != 1; }
    );
    CHECK(!ok);
    return true;
}

bool test_malformed_json_parse() {
    {
        int_string obj{};
        std::string accum;
        auto p = jsonrefl::make_parser(&obj, &accum);
        static const char j[] = R"({"i":1,"s":"x")";
        CHECK(p.parse(j, sizeof(j) - 1).status() == jsonrefl::state::incomplete);
    }
    {
        int_string obj{};
        std::string accum;
        auto p = jsonrefl::make_parser(&obj, &accum);
        static const char j[] = R"({"i":"notanint","s":"x"})";
        CHECK(p.parse(j, sizeof(j) - 1).status() == jsonrefl::state::invalid);
    }
    {
        int_string obj{};
        std::string accum;
        auto p = jsonrefl::make_parser(&obj, &accum);
        static const char j[] = "not json";
        CHECK(p.parse(j, sizeof(j) - 1).status() == jsonrefl::state::invalid);
    }
    return true;
}

bool test_unknown_key_reset_then_parse_ok() {
    int_string obj{};
    auto p = jsonrefl::make_parser(&obj);
    static const char bad[] = R"({"i":7,"ghost":99,"s":"q"})";
    CHECK(p.parse(bad, sizeof(bad) - 1).status() == jsonrefl::state::unknown_key);

    p.reset();
    obj = {};
    static const char good[] = R"({"i":42,"s":"hello"})";
    CHECK(p.parse(good, sizeof(good) - 1).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.i, 42);
    CHECK_EQ(obj.s, std::string("hello"));

    return true;
}

bool test_duplicate_key_last_wins() {
    int_string obj{};
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    static const char j[] = R"({"i":1,"s":"first","s":"second"})";
    CHECK(p.parse(j, sizeof(j) - 1).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.s, std::string("second"));
    return true;
}

bool test_deep_nesting_returns_invalid() {
    std::vector<int> v;
    auto p = jsonrefl::make_parser(&v);
    std::string deep;
    for ( int i = 0; i < 20; ++i ) { deep += '['; }
    deep += "1";
    for ( int i = 0; i < 20; ++i ) { deep += ']'; }
    CHECK(p.parse(deep.data(), deep.size()).status() == jsonrefl::state::invalid);
    return true;
}

bool test_nan_inf_serialise_as_null() {
    const double nan_val  = std::numeric_limits<double>::quiet_NaN();
    const double inf_val  = std::numeric_limits<double>::infinity();
    const double ninf_val = -std::numeric_limits<double>::infinity();

    float_fields_t obj{static_cast<float>(nan_val), inf_val};
    const auto s = jsonrefl::to_string(obj);
    CHECK(s.find("null") != std::string::npos);
    CHECK(s.find("nan")  == std::string::npos);
    CHECK(s.find("inf")  == std::string::npos);

    float_fields_t obj2{static_cast<float>(inf_val), ninf_val};
    const auto s2 = jsonrefl::to_string(obj2);
    CHECK(s2.find("null") != std::string::npos);
    CHECK(s2.find("nan")  == std::string::npos);
    CHECK(s2.find("inf")  == std::string::npos);
    return true;
}

bool test_map_string_vector_int_parse_and_roundtrip() {
    const char *json = R"({"m":{"a":[1,2,3],"b":[4,5]}})";
    map_vec_int_t obj;
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse(json, ::strlen(json)).status() == jsonrefl::state::ok);
    CHECK(obj.m.size() == 2u);
    CHECK((obj.m["a"] == std::vector<int>{1, 2, 3}));
    CHECK((obj.m["b"] == std::vector<int>{4, 5}));
    const auto out = jsonrefl::to_string(obj);
    CHECK(out.find("\"a\"") != std::string::npos);
    return true;
}

bool test_to_string_size_exact() {
    two_fields_t obj{42, 1.5f};
    const auto s = jsonrefl::to_string(obj);
    CHECK(!s.empty());
    CHECK(s.back() != '\0');
    CHECK(s.find('\0') == std::string::npos);
    return true;
}

/***************************************************************************************************/

bool test_comments_line_basic() {
    two_fields_t obj{};
    auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_comments);
    const std::string input =
        "{ // open object\n"
        "  \"n\": // before value\n"
        "  42, // after comma\n"
        "  \"f\": 1.5 // tail\n"
        "}";
    const auto s = p.parse(input.data(), input.size());
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK(obj.n == 42);
    CHECK(obj.f == 1.5f);
    return true;
}


bool test_comments_disallowed() {
    two_fields_t obj{};
    auto p = jsonrefl::make_parser(&obj);
    const std::string input = "{// comment\n\"n\":1,\"f\":0.0}";
    const auto s = p.parse(input.data(), input.size());
    CHECK(s.status() == jsonrefl::state::invalid);
    return true;
}

bool test_comments_after_document() {
    two_fields_t obj{};
    auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_comments);
    const std::string input = "{\"n\":1,\"f\":2.0} // trailing comment\n";
    const auto s = p.parse(input.data(), input.size());
    CHECK(s.status() == jsonrefl::state::ok);
    return true;
}

bool test_comments_line_no_newline_at_eof() {
    two_fields_t obj{};
    auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_comments);
    const std::string input = "{\"n\":7,\"f\":3.0} // no newline at end";
    const auto s = p.parse(input.data(), input.size());
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK(obj.n == 7);
    return true;
}


bool test_comments_line_spanning_chunks() {
    two_fields_t obj{};
    auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_comments);
    const std::string a = "{\"n\":// line comment";
    const std::string b = "\n3,\"f\":0.0}";
    auto s = p.parse(a.data(), a.size());
    CHECK(s.status() == jsonrefl::state::incomplete);
    s = p.parse(b.data(), b.size());
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK(obj.n == 3);
    return true;
}

bool test_comments_slash_at_chunk_boundary() {
    two_fields_t obj{};
    auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_comments);
    const std::string a = "{\"n\":/";
    const std::string b = "/line\n9,\"f\":0.0}";
    auto s = p.parse(a.data(), a.size());
    CHECK(s.status() == jsonrefl::state::incomplete);
    s = p.parse(b.data(), b.size());
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK(obj.n == 9);
    return true;
}


bool test_comments_with_skip_unknown_keys() {
    two_fields_t obj{};
    constexpr auto fl = jsonrefl::flags::allow_comments | jsonrefl::flags::skip_unknown_keys;
    auto p = jsonrefl::make_parser(&obj, nullptr, fl);
    // Combines allow_comments + skip_unknown_keys: comments around unknown field are ok
    const std::string input =
        "{\n"
        "  // comment before known key\n"
        "  \"n\": 1,\n"
        "  \"unknown\": 99, // comment after unknown value\n"
        "  \"f\": 2.5\n"
        "}";
    const auto s = p.parse(input.data(), input.size());
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK(obj.n == 1);
    CHECK(obj.f == 2.5f);
    return true;
}

/***************************************************************************************************/

bool test_invalid_utf16_lone_high_surrogate() {
    kv_ss obj{};
    // InSource path: decode_escape_into called inline, rejects lone surrogate immediately.
    {
        char buf[] = R"({"s":"\uD83D","t":"ok"})";
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse_m(buf, sizeof(buf) - 1).status() == jsonrefl::state::invalid);
    }
    {
        char buf[] = R"({"s":"\uD83D","t":"ok"})";
        auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_invalid_utf16);
        CHECK(p.parse_m(buf, sizeof(buf) - 1).status() == jsonrefl::state::ok);
        // \uD83D is a high surrogate → WTF-8: 0xED 0xA0 0xBD
        CHECK(obj.s.size() == 3);
        CHECK(static_cast<unsigned char>(obj.s[0]) == 0xED);
        CHECK(static_cast<unsigned char>(obj.s[1]) == 0xA0);
        CHECK(static_cast<unsigned char>(obj.s[2]) == 0xBD);
        CHECK(obj.t == "ok");
    }
    // Chunked path: decode happens via unescape_range in decode_value (needs accum).
    {
        const char inp[] = R"({"s":"\uD83D","t":"ok"})";
        {
            std::string accum;
            auto p = jsonrefl::make_parser(&obj, &accum);
            CHECK(p.parse(inp, sizeof(inp) - 1).status() == jsonrefl::state::invalid);
        }
        {
            std::string accum;
            auto p = jsonrefl::make_parser(&obj, &accum, jsonrefl::flags::allow_invalid_utf16);
            CHECK(p.parse(inp, sizeof(inp) - 1).status() == jsonrefl::state::ok);
            CHECK(obj.s.size() == 3);
            CHECK(static_cast<unsigned char>(obj.s[0]) == 0xED);
        }
    }
    return true;
}

bool test_invalid_utf16_lone_low_surrogate() {
    kv_ss obj{};
    {
        char buf[] = R"({"s":"\uDC00","t":"x"})";
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse_m(buf, sizeof(buf) - 1).status() == jsonrefl::state::invalid);
    }
    {
        char buf[] = R"({"s":"\uDC00","t":"x"})";
        auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_invalid_utf16);
        CHECK(p.parse_m(buf, sizeof(buf) - 1).status() == jsonrefl::state::ok);
        // \uDC00 is a low surrogate → WTF-8: 0xED 0xB0 0x80
        CHECK(obj.s.size() == 3);
        CHECK(static_cast<unsigned char>(obj.s[0]) == 0xED);
        CHECK(static_cast<unsigned char>(obj.s[1]) == 0xB0);
        CHECK(static_cast<unsigned char>(obj.s[2]) == 0x80);
    }
    return true;
}

bool test_invalid_utf16_valid_pair_unaffected() {
    kv_ss obj{};
    // Valid surrogate pair \uD83D\uDE00 = U+1F600 = 😀 (4-byte UTF-8: F0 9F 98 80)
    char buf[] = R"({"s":"\uD83D\uDE00","t":"x"})";
    auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_invalid_utf16);
    CHECK(p.parse_m(buf, sizeof(buf) - 1).status() == jsonrefl::state::ok);
    CHECK(obj.s.size() == 4);
    CHECK(static_cast<unsigned char>(obj.s[0]) == 0xF0);
    CHECK(static_cast<unsigned char>(obj.s[1]) == 0x9F);
    CHECK(static_cast<unsigned char>(obj.s[2]) == 0x98);
    CHECK(static_cast<unsigned char>(obj.s[3]) == 0x80);
    return true;
}

bool test_invalid_utf8_raw_bytes_rejected_by_default() {
    kv_ss obj{};
    // Isolated continuation byte 0x80 in string value (InSource path)
    std::string buf1 = std::string("{\"s\":\"") + char(0x80) + "\",\"t\":\"x\"}";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse_m(&buf1[0], buf1.size()).status() == jsonrefl::state::invalid);
    return true;
}

bool test_invalid_utf8_raw_bytes_allowed_with_flag() {
    kv_ss obj{};
    // 0xC0 0xAF — over-long encoding (invalid) — allowed by flag
    std::string buf1 = std::string("{\"s\":\"") + char(0xC0) + char(0xAF) + "\",\"t\":\"x\"}";
    {
        auto p = jsonrefl::make_parser(&obj);
        CHECK(p.parse_m(&buf1[0], buf1.size()).status() == jsonrefl::state::invalid);
    }
    {
        auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_invalid_utf8);
        CHECK(p.parse_m(&buf1[0], buf1.size()).status() == jsonrefl::state::ok);
        CHECK(obj.s.size() == 2);
        CHECK(static_cast<unsigned char>(obj.s[0]) == 0xC0);
        CHECK(static_cast<unsigned char>(obj.s[1]) == 0xAF);
    }
    return true;
}

bool test_invalid_utf8_truncated_sequence_rejected() {
    kv_ss obj{};
    // 0xE2 starts a 3-byte sequence but string ends after it
    std::string buf1 = std::string("{\"s\":\"") + char(0xE2) + "\",\"t\":\"x\"}";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse_m(&buf1[0], buf1.size()).status() == jsonrefl::state::invalid);
    return true;
}

bool test_invalid_utf8_valid_multibyte_unaffected() {
    kv_ss obj{};
    // "héllo" — é is U+00E9 = 0xC3 0xA9 — valid UTF-8, accepted without flag
    char buf[] = "{\"s\":\"h\xC3\xA9llo\",\"t\":\"x\"}";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse_m(buf, sizeof(buf) - 1).status() == jsonrefl::state::ok);
    CHECK(obj.s == "h\xC3\xA9llo");
    return true;
}

bool test_invalid_utf8_chunked_cross_boundary() {
    kv_ss obj{};
    std::string accum;
    auto p = jsonrefl::make_parser(&obj, &accum);
    // Split a valid 2-byte sequence (0xC3 0xA9 = é) across chunk boundary
    const std::string part1 = std::string("{\"s\":\"h") + char(0xC3);
    const std::string part2 = std::string("\xA9llo\",\"t\":\"x\"}");
    CHECK(p.parse(part1.data(), part1.size()).status() == jsonrefl::state::incomplete);
    CHECK(p.parse(part2.data(), part2.size()).status() == jsonrefl::state::ok);
    CHECK(obj.s == "h\xC3\xA9llo");
    return true;
}

bool test_invalid_utf8_bad_continuation_rejected() {
    kv_ss obj{};
    // 0xE2 starts 3-byte sequence; second byte 0x41 ('A') is not a continuation byte
    std::string buf1 = std::string("{\"s\":\"") + char(0xE2) + char(0x41) + "\",\"t\":\"x\"}";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse_m(&buf1[0], buf1.size()).status() == jsonrefl::state::invalid);
    return true;
}

bool test_invalid_escapes_rejected_by_default() {
    kv_ss obj{};
    const char inp[] = "{\"s\":\"hello\\xworld\",\"t\":\"ok\"}";
    auto p = jsonrefl::make_parser(&obj);
    CHECK(p.parse_m(const_cast<char *>(inp), sizeof(inp) - 1).status() == jsonrefl::state::invalid);
    return true;
}

bool test_invalid_escapes_allowed_with_flag() {
    kv_ss obj{};
    // \x — unknown escape; should be passed through as-is: '\' + 'x'
    char inp[] = "{\"s\":\"a\\xb\",\"t\":\"ok\"}";
    auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_invalid_escapes);
    CHECK(p.parse_m(inp, sizeof(inp) - 1).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.s, std::string("a\\xb"));
    CHECK_EQ(obj.t, std::string("ok"));
    return true;
}

bool test_invalid_escapes_space_allowed() {
    kv_ss obj{};
    // backslash-space: invalid per RFC, passed through with flag
    char inp[] = "{\"s\":\"a\\ b\",\"t\":\"ok\"}";
    auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_invalid_escapes);
    CHECK(p.parse_m(inp, sizeof(inp) - 1).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.s, std::string("a\\ b"));
    return true;
}

bool test_invalid_escapes_non_insource() {
    kv_ss obj{};
    std::string accum;
    // non-InSource path: \x passed through via unescape_range
    const char inp[] = "{\"s\":\"hello\\xworld\",\"t\":\"!\"}";
    auto p = jsonrefl::make_parser(&obj, &accum, jsonrefl::flags::allow_invalid_escapes);
    CHECK(p.parse(inp, sizeof(inp) - 1).status() == jsonrefl::state::ok);
    CHECK_EQ(obj.s, std::string("hello\\xworld"));
    return true;
}

/***************************************************************************************************/
// parse_next / parse_next_m — sequential JSON records
/***************************************************************************************************/

template<typename T>
static std::vector<T> collect_next(const char *buf, std::size_t len, jsonrefl::flags fl = jsonrefl::flags::none) {
    std::vector<T> results;
    T obj{};
    std::string accum;
    auto pp = jsonrefl::make_parser(&obj, &accum, fl);

    auto cur = pp.parse(buf, len);
    for ( ;; ) {
        if ( cur.status() != jsonrefl::state::ok && cur.status() != jsonrefl::state::record_end ) { break; }
        results.push_back(std::move(obj));
        if ( cur.status() == jsonrefl::state::ok ) { break; }
        obj = {};
        pp.parse_next(&cur);
    }

    return results;
}

template<typename T>
static std::vector<T> collect_next_m(std::string buf, jsonrefl::flags fl = jsonrefl::flags::none) {
    std::vector<T> results;
    T obj{};
    auto pp = jsonrefl::make_parser(&obj, nullptr, fl);

    auto cur = pp.parse_m(&buf[0], buf.size());
    for ( ;; ) {
        if ( cur.status() != jsonrefl::state::ok && cur.status() != jsonrefl::state::record_end ) { break; }
        results.push_back(obj);
        if ( cur.status() == jsonrefl::state::ok ) { break; }
        obj = {};
        pp.parse_next_m(&cur);
    }

    return results;
}

bool test_seq_two_objects() {
    static constexpr char buf[] = R"({"i":1,"s":"alpha"}{"i":2,"s":"beta"})";
    auto recs = collect_next<int_string>(buf, sizeof(buf) - 1);
    CHECK_EQ(recs.size(), 2u);
    CHECK_EQ(recs[0].i, 1); CHECK_EQ(recs[0].s, std::string("alpha"));
    CHECK_EQ(recs[1].i, 2); CHECK_EQ(recs[1].s, std::string("beta"));
    return true;
}

bool test_seq_two_arrays() {
    static constexpr char buf[] = "[1,2,3][4,5,6]";
    auto recs = collect_next<std::vector<int>>(buf, sizeof(buf) - 1);
    CHECK_EQ(recs.size(), 2u);
    CHECK(recs[0] == (std::vector<int>{1,2,3}));
    CHECK(recs[1] == (std::vector<int>{4,5,6}));
    return true;
}

bool test_seq_two_strings() {
    static constexpr char buf[] = R"({"s":"hello","t":"a"}{"s":"world","t":"b"})";
    auto recs = collect_next<kv_ss>(buf, sizeof(buf) - 1);
    CHECK_EQ(recs.size(), 2u);
    CHECK_EQ(recs[0].s, std::string("hello"));
    CHECK_EQ(recs[1].s, std::string("world"));
    return true;
}

bool test_seq_two_booleans_adjacent() {
    static constexpr char buf[] = R"({"flag":true,"val":1}{"flag":false,"val":2})";
    auto recs = collect_next<bool_test>(buf, sizeof(buf) - 1);
    CHECK_EQ(recs.size(), 2u);
    CHECK(recs[0].flag == true);  CHECK_EQ(recs[0].val, 1);
    CHECK(recs[1].flag == false); CHECK_EQ(recs[1].val, 2);
    return true;
}

bool test_seq_two_nulls_adjacent() {
    static constexpr char buf[] = R"({"opt":null,"val":1}{"opt":null,"val":2})";
    auto recs = collect_next<null_test>(buf, sizeof(buf) - 1);
    CHECK_EQ(recs.size(), 2u);
    CHECK(!recs[0].opt); CHECK_EQ(recs[0].val, 1);
    CHECK(!recs[1].opt); CHECK_EQ(recs[1].val, 2);
    return true;
}

bool test_seq_two_numbers_whitespace() {
    static constexpr char buf[] = "[42] [99]";
    auto recs = collect_next<std::vector<int>>(buf, sizeof(buf) - 1);
    CHECK_EQ(recs.size(), 2u);
    CHECK(recs[0] == (std::vector<int>{42}));
    CHECK(recs[1] == (std::vector<int>{99}));
    return true;
}

bool test_seq_array_then_object() {
    static constexpr char buf[] = R"([8,9]{"i":10,"s":"x"})";

    std::vector<int> arr;
    auto pp1 = jsonrefl::make_parser(&arr);
    auto cur = pp1.parse(buf, sizeof(buf) - 1);
    CHECK(cur.status() == jsonrefl::state::record_end);
    CHECK(arr == (std::vector<int>{8,9}));

    int_string obj{};
    std::string accum;
    auto pp2 = jsonrefl::make_parser(&obj, &accum);
    CHECK(pp2.parse_next(&cur) == jsonrefl::state::ok);
    CHECK_EQ(obj.i, 10);
    CHECK_EQ(obj.s, std::string("x"));
    return true;
}

bool test_seq_three_records() {
    static constexpr char buf[] = R"({"i":10,"s":"x"}{"i":20,"s":"y"}{"i":30,"s":"z"})";
    auto recs = collect_next<int_string>(buf, sizeof(buf) - 1);
    CHECK_EQ(recs.size(), 3u);
    CHECK_EQ(recs[0].i, 10); CHECK_EQ(recs[0].s, std::string("x"));
    CHECK_EQ(recs[1].i, 20); CHECK_EQ(recs[1].s, std::string("y"));
    CHECK_EQ(recs[2].i, 30); CHECK_EQ(recs[2].s, std::string("z"));
    return true;
}

bool test_seq_whitespace_between() {
    static constexpr char buf[] = "  [1]  \n  [2]  \n  [3]  ";
    auto recs = collect_next<std::vector<int>>(buf, sizeof(buf) - 1);
    CHECK_EQ(recs.size(), 3u);
    CHECK(recs[0] == (std::vector<int>{1}));
    CHECK(recs[1] == (std::vector<int>{2}));
    CHECK(recs[2] == (std::vector<int>{3}));
    return true;
}

bool test_seq_single_record() {
    static constexpr char buf[] = R"({"i":5,"s":"one"})";
    auto recs = collect_next<int_string>(buf, sizeof(buf) - 1);
    CHECK_EQ(recs.size(), 1u);
    CHECK_EQ(recs[0].i, 5);
    CHECK_EQ(recs[0].s, std::string("one"));
    return true;
}

bool test_seq_strings_with_escapes() {
    static constexpr char buf[] = R"({"s":"hel\nlo","t":"a"}{"s":"wor\tld","t":"b"})";
    auto recs = collect_next<kv_ss>(buf, sizeof(buf) - 1);
    CHECK_EQ(recs.size(), 2u);
    CHECK_EQ(recs[0].s, std::string("hel\nlo"));
    CHECK_EQ(recs[0].t, std::string("a"));
    CHECK_EQ(recs[1].s, std::string("wor\tld"));
    CHECK_EQ(recs[1].t, std::string("b"));
    return true;
}

bool test_seq_with_comments() {
    static constexpr char buf[] =
        "[1,2]\n"
        "// second record\n"
        "[3,4]\n"
        "// third record\n"
        "[5,6]";
    auto recs = collect_next<std::vector<int>>(buf, sizeof(buf) - 1, jsonrefl::flags::allow_comments);
    CHECK_EQ(recs.size(), 3u);
    CHECK(recs[0] == (std::vector<int>{1,2}));
    CHECK(recs[1] == (std::vector<int>{3,4}));
    CHECK(recs[2] == (std::vector<int>{5,6}));
    return true;
}

bool test_seq_chunked_cross_chunk() {
    // doc1 = {"i":1,"s":"a"}   doc2 = {"i":2,"s":"b"}
    // split: chunk1 = {"i":1,"s":"a"}{"i":2,"s":   chunk2 = "b"}
    const std::string chunk1 = R"({"i":1,"s":"a"}{"i":2,"s":)";
    const std::string chunk2 = R"("b"})";

    std::vector<int_string> results;
    int_string obj{};
    std::string accum;
    auto pp = jsonrefl::make_parser(&obj, &accum);

    auto cur = pp.parse(chunk1.data(), chunk1.size());
    CHECK(cur.status() == jsonrefl::state::record_end);
    CHECK_EQ(obj.i, 1); CHECK_EQ(obj.s, std::string("a"));
    results.push_back(obj);

    std::string rest = chunk1.substr(chunk1.size() - cur.remaining()) + chunk2;
    obj = {}; pp.reset();
    cur = pp.parse(rest.data(), rest.size());
    CHECK(cur.status() == jsonrefl::state::ok);
    CHECK_EQ(obj.i, 2); CHECK_EQ(obj.s, std::string("b"));
    results.push_back(obj);

    CHECK_EQ(results.size(), 2u);
    return true;
}

bool test_seq_next_m_two_objects() {
    std::string buf = R"({"i":1,"s":"a"}{"i":2,"s":"b"})";
    auto recs = collect_next_m<int_string>(buf);
    CHECK_EQ(recs.size(), 2u);
    CHECK_EQ(recs[0].i, 1); CHECK_EQ(recs[0].s, std::string("a"));
    CHECK_EQ(recs[1].i, 2); CHECK_EQ(recs[1].s, std::string("b"));
    return true;
}

bool test_seq_next_m_with_escapes() {
    std::string buf = "{\"s\":\"a\\nb\",\"t\":\"x\"}{\"s\":\"c\\td\",\"t\":\"y\"}";
    auto recs = collect_next_m<kv_ss>(buf);
    CHECK_EQ(recs.size(), 2u);
    CHECK_EQ(recs[0].s, std::string("a\nb"));
    CHECK_EQ(recs[1].s, std::string("c\td"));
    return true;
}

bool test_seq_comment_slash_at_chunk_boundary() {
    // doc1 = [1], then lone '/' at end of chunk1; chunk2 = "[2]" (not a comment)
    const std::string chunk1 = "[1]  /";
    const std::string chunk2 = "[2]";

    std::vector<int> obj{};
    std::string accum;
    auto pp = jsonrefl::make_parser(&obj, &accum, jsonrefl::flags::allow_comments);

    auto cur = pp.parse(chunk1.data(), chunk1.size());
    // buffer exhausted mid-slash ambiguity
    CHECK(cur.status() == jsonrefl::state::record_end || cur.status() == jsonrefl::state::incomplete);

    if ( cur.status() == jsonrefl::state::incomplete ) {
        // parser needs one more byte to resolve '/' vs '//'; feed chunk2 ('[' not '/')
        cur = pp.parse(chunk2.data(), chunk2.size());
        CHECK(cur.status() == jsonrefl::state::record_end);
        CHECK(obj == (std::vector<int>{1}));
        obj = {};
        const auto st = pp.parse_next(&cur);
        CHECK(st == jsonrefl::state::ok);
        CHECK(obj == (std::vector<int>{2}));
    } else {
        CHECK(obj == (std::vector<int>{1}));
        obj = {};
        const auto st = pp.parse_next(&cur);
        CHECK(st == jsonrefl::state::invalid || st == jsonrefl::state::ok);
    }
    return true;
}

bool test_seq_extra_data_unchanged() {
    static constexpr char buf[] = R"({"i":1,"s":"a"}{"i":2,"s":"b"})";
    int_string obj{};
    std::string accum;
    auto pp = jsonrefl::make_parser(&obj, &accum);
    auto cur = pp.parse(buf, sizeof(buf) - 1);
    CHECK(cur.status() == jsonrefl::state::record_end);
    CHECK(cur.remaining() == sizeof(R"({"i":2,"s":"b"})") - 1);
    return true;
}

// NDJSON-style: newline-delimited records
bool test_seq_ndjson() {
    static constexpr char buf[] =
        "{\"i\":1,\"s\":\"one\"}\n"
        "{\"i\":2,\"s\":\"two\"}\n"
        "{\"i\":3,\"s\":\"three\"}\n";
    auto recs = collect_next<int_string>(buf, sizeof(buf) - 1);
    CHECK_EQ(recs.size(), 3u);
    CHECK_EQ(recs[0].i, 1); CHECK_EQ(recs[0].s, std::string("one"));
    CHECK_EQ(recs[1].i, 2); CHECK_EQ(recs[1].s, std::string("two"));
    CHECK_EQ(recs[2].i, 3); CHECK_EQ(recs[2].s, std::string("three"));
    return true;
}

bool test_inf_nan_basic() {
    float_fields_t obj{};
    auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_infinity_and_nan);
    const char inp[] = R"({"f": NaN, "d": Infinity})";
    const auto s = p.parse(inp, sizeof(inp) - 1);
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK(std::isnan(obj.f));
    CHECK(std::isinf(obj.d) && obj.d > 0);
    return true;
}

bool test_inf_nan_neg_inf() {
    float_fields_t obj{};
    auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_infinity_and_nan);
    const char inp[] = R"({"f": -Infinity, "d": -Infinity})";
    const auto s = p.parse(inp, sizeof(inp) - 1);
    CHECK(s.status() == jsonrefl::state::ok);
    CHECK(std::isinf(obj.f) && obj.f < 0);
    CHECK(std::isinf(obj.d) && obj.d < 0);
    return true;
}

bool test_inf_nan_all_three() {
    float_fields_t obj{};
    {
        auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_infinity_and_nan);
        const char inp[] = R"({"f": NaN, "d": -Infinity})";
        const auto s = p.parse(inp, sizeof(inp) - 1);
        CHECK(s.status() == jsonrefl::state::ok);
        CHECK(std::isnan(obj.f));
        CHECK(std::isinf(obj.d) && obj.d < 0);
    }
    {
        auto p = jsonrefl::make_parser(&obj, nullptr, jsonrefl::flags::allow_infinity_and_nan);
        const char inp[] = R"({"f": Infinity, "d": NaN})";
        const auto s = p.parse(inp, sizeof(inp) - 1);
        CHECK(s.status() == jsonrefl::state::ok);
        CHECK(std::isinf(obj.f) && obj.f > 0);
        CHECK(std::isnan(obj.d));
    }
    return true;
}

bool test_inf_nan_disallowed() {
    float_fields_t obj{};
    {
        auto p = jsonrefl::make_parser(&obj);
        const char inp[] = R"({"f": NaN, "d": 1.0})";
        const auto s = p.parse(inp, sizeof(inp) - 1);
        CHECK(s.status() == jsonrefl::state::invalid);
    }
    {
        auto p = jsonrefl::make_parser(&obj);
        const char inp[] = R"({"f": Infinity, "d": 1.0})";
        const auto s = p.parse(inp, sizeof(inp) - 1);
        CHECK(s.status() == jsonrefl::state::invalid);
    }
    {
        auto p = jsonrefl::make_parser(&obj);
        const char inp[] = R"({"f": -Infinity, "d": 1.0})";
        const auto s = p.parse(inp, sizeof(inp) - 1);
        CHECK(s.status() == jsonrefl::state::invalid);
    }
    return true;
}

bool test_inf_nan_chunked() {
    {
        float_fields_t obj{};
        std::string buf;
        auto p = jsonrefl::make_parser(&obj, &buf, jsonrefl::flags::allow_infinity_and_nan);
        const std::string input = R"({"f": NaN, "d": Infinity})";
        for ( std::size_t i = 0; i < input.size(); ++i ) {
            auto s = p.parse(input.data() + i, 1);
            if ( i + 1 < input.size() ) {
                CHECK(s.status() == jsonrefl::state::incomplete);
            } else {
                CHECK(s.status() == jsonrefl::state::ok);
            }
        }
        CHECK(std::isnan(obj.f));
        CHECK(std::isinf(obj.d) && obj.d > 0);
    }
    {
        float_fields_t obj{};
        std::string buf;
        auto p = jsonrefl::make_parser(&obj, &buf, jsonrefl::flags::allow_infinity_and_nan);
        const std::string input = R"({"f": -Infinity, "d": NaN})";
        for ( std::size_t i = 0; i < input.size(); ++i ) {
            auto s = p.parse(input.data() + i, 1);
            if ( i + 1 < input.size() ) {
                CHECK(s.status() == jsonrefl::state::incomplete);
            } else {
                CHECK(s.status() == jsonrefl::state::ok);
            }
        }
        CHECK(std::isinf(obj.f) && obj.f < 0);
        CHECK(std::isnan(obj.d));
    }
    return true;
}

bool test_inf_nan_skip_unknown() {
    two_fields_t obj{};
    constexpr auto fl = jsonrefl::flags::skip_unknown_keys | jsonrefl::flags::allow_infinity_and_nan;
    {
        auto p = jsonrefl::make_parser(&obj, nullptr, fl);
        const char inp[] = R"({"n": 7, "x": NaN, "f": 3.0})";
        const auto s = p.parse(inp, sizeof(inp) - 1);
        CHECK(s.status() == jsonrefl::state::ok);
        CHECK(obj.n == 7);
        CHECK(obj.f == 3.0f);
    }
    {
        auto p = jsonrefl::make_parser(&obj, nullptr, fl);
        const char inp[] = R"({"n": 7, "x": Infinity, "f": 3.0})";
        const auto s = p.parse(inp, sizeof(inp) - 1);
        CHECK(s.status() == jsonrefl::state::ok);
    }
    {
        auto p = jsonrefl::make_parser(&obj, nullptr, fl);
        const char inp[] = R"({"n": 7, "x": -Infinity, "f": 3.0})";
        const auto s = p.parse(inp, sizeof(inp) - 1);
        CHECK(s.status() == jsonrefl::state::ok);
    }
    return true;
}

/***************************************************************************************************/

bool test_flags_combine_operators() {
    using F = jsonrefl::flags;
    constexpr auto fl = F::allow_comments | F::skip_unknown_keys | F::allow_infinity_and_nan;
    const auto u = static_cast<std::uint8_t>(fl);
    CHECK_EQ(u, static_cast<std::uint8_t>(2 | 1 | 4));
    CHECK(static_cast<std::uint8_t>(fl & F::allow_comments) != 0);
    CHECK(static_cast<std::uint8_t>(fl & F::skip_unknown_keys) != 0);
    CHECK(static_cast<std::uint8_t>(fl & F::allow_infinity_and_nan) != 0);
    CHECK(static_cast<std::uint8_t>(fl & F::allow_invalid_utf8) == 0);
    return true;
}

/***************************************************************************************************/

JSONREFL_STRUCT_DOC(
     doc_cfg
    ,(int,  port, "listening port")
    ,(bool, tls,  "enable TLS")
);

struct doc_meta_t {
    int a;
    int b;
};
JSONREFL_METADATA_DOC(doc_meta_t, (a, "alpha"), (b, "beta"));

bool test_doc_comments_pretty_with_comments() {
    using SF = jsonrefl::serialize_flags;
    doc_cfg obj{8080, true};
    const auto s = jsonrefl::to_string(obj, SF::pretty | SF::comments);
    CHECK_EQ(jsonrefl::required_bytes(obj, SF::pretty | SF::comments), s.size());
    CHECK(s.find("// listening port") != std::string::npos);
    CHECK(s.find("// enable TLS") != std::string::npos);
    CHECK(s.find("// listening port") < s.find("\"port\""));
    return true;
}

bool test_doc_comments_pretty_without_comments() {
    using SF = jsonrefl::serialize_flags;
    doc_cfg obj{8080, true};
    const auto s = jsonrefl::to_string(obj, SF::pretty);
    CHECK_EQ(jsonrefl::required_bytes(obj, SF::pretty), s.size());
    CHECK(s.find("//") == std::string::npos);
    CHECK(s.find('\n') != std::string::npos);
    return true;
}

bool test_doc_comments_compact_ignores_comments() {
    using SF = jsonrefl::serialize_flags;
    doc_cfg obj{8080, true};
    const auto compact = jsonrefl::to_string(obj);
    const auto with_c  = jsonrefl::to_string(obj, SF::comments);
    CHECK_EQ(compact, with_c);
    CHECK(with_c.find("//") == std::string::npos);
    CHECK(with_c.find('\n') == std::string::npos);
    return true;
}

bool test_doc_comments_roundtrip() {
    using SF = jsonrefl::serialize_flags;
    doc_cfg obj{1234, false};
    const auto s = jsonrefl::to_string(obj, SF::pretty | SF::comments);
    doc_cfg parsed{};
    auto p = jsonrefl::make_parser(&parsed, nullptr, jsonrefl::flags::allow_comments);
    CHECK(p.parse(s.data(), s.size()).status() == jsonrefl::state::ok);
    CHECK_EQ(parsed.port, 1234);
    CHECK_EQ(parsed.tls, false);
    return true;
}

bool test_doc_comments_metadata_macro() {
    using SF = jsonrefl::serialize_flags;
    doc_meta_t obj{1, 2};
    const auto s = jsonrefl::to_string(obj, SF::pretty | SF::comments);
    CHECK_EQ(jsonrefl::required_bytes(obj, SF::pretty | SF::comments), s.size());
    CHECK(s.find("// alpha") != std::string::npos);
    CHECK(s.find("// beta") != std::string::npos);
    return true;
}

bool test_doc_comments_empty_doc_no_emit() {
    using SF = jsonrefl::serialize_flags;
    float_fields obj{1.0, 2.0f};
    const auto plain = jsonrefl::to_string(obj, SF::pretty);
    const auto withc = jsonrefl::to_string(obj, SF::pretty | SF::comments);
    CHECK_EQ(plain, withc);
    CHECK(withc.find("//") == std::string::npos);
    return true;
}

bool test_doc_comments_chunked() {
    using SF = jsonrefl::serialize_flags;
    doc_cfg obj{8080, true};
    std::string out;
    char cbuf[8];
    const bool ok = jsonrefl::to_chunked_buffer(cbuf, sizeof(cbuf), obj,
        [&](const void *d, std::size_t n) -> bool {
            out.append(static_cast<const char*>(d), n);
            return true;
        }, SF::pretty | SF::comments);
    CHECK(ok);
    CHECK(out.find("// listening port") != std::string::npos);
    CHECK(out.find("// enable TLS") != std::string::npos);
    CHECK_EQ(out, jsonrefl::to_string(obj, SF::pretty | SF::comments));
    return true;
}

static bool test_version_macros() {
    // Keep in sync with JSONREFL_VERSION_* in include/jsonrefl/jsonrefl.hpp.
    constexpr int expected_major = 1;
    constexpr int expected_minor = 0;
    constexpr int expected_bugfix = 0;
    constexpr std::uint32_t expected_version =
        static_cast<std::uint32_t>(
            expected_major * 100000u
            + expected_minor * 100u
            + static_cast<unsigned>(expected_bugfix)
        );
    constexpr const char *expected_version_string = "1.0.0";

    CHECK_EQ(JSONREFL_VERSION_MAJOR, expected_major);
    CHECK_EQ(JSONREFL_VERSION_MINOR, expected_minor);
    CHECK_EQ(JSONREFL_VERSION_BUGFIX, expected_bugfix);
    CHECK_EQ(static_cast<std::uint32_t>(JSONREFL_VERSION), expected_version);
    CHECK_EQ(JSONREFL_VERSION_GET_MAJOR(JSONREFL_VERSION), expected_major);
    CHECK_EQ(JSONREFL_VERSION_GET_MINOR(JSONREFL_VERSION), expected_minor);
    CHECK_EQ(JSONREFL_VERSION_GET_BUGFIX(JSONREFL_VERSION), expected_bugfix);
    CHECK(std::strcmp(JSONREFL_VERSION_STRING, expected_version_string) == 0);

    return true;
}

/***************************************************************************************************/
// schema_id: compile-time structural fingerprint
/***************************************************************************************************/

namespace schema_test {

struct base_v1     { int a; std::string b; };
JSONREFL_METADATA(base_v1, a, b);

struct base_clone  { int a; std::string b; };            // byte-for-byte the same shape as base_v1
JSONREFL_METADATA(base_clone, a, b);

struct added       { int a; std::string b; int c; };     // field added
JSONREFL_METADATA(added, a, b, c);

struct removed     { int a; };                            // field removed
JSONREFL_METADATA(removed, a);

struct renamed     { int a; std::string b_renamed; };     // field renamed
JSONREFL_METADATA(renamed, a, b_renamed);

struct reordered   { std::string b; int a; };             // fields reordered
JSONREFL_METADATA(reordered, b, a);

struct retyped_cat { int a; int b; };                     // b: string -> int (category tag differs)
JSONREFL_METADATA(retyped_cat, a, b);

struct width_int   { int a; };
JSONREFL_METADATA(width_int, a);

struct width_short { short a; };                          // same tag 'I' as int, differs only by sizeof
JSONREFL_METADATA(width_short, a);

struct inner_a     { int x; };
JSONREFL_METADATA(inner_a, x);

struct inner_b     { int x; long y; };                    // nested type gains a field
JSONREFL_METADATA(inner_b, x, y);

struct outer_a     { inner_a n; };
JSONREFL_METADATA(outer_a, n);

struct outer_b     { inner_b n; };                        // same field name/order, different nested shape
JSONREFL_METADATA(outer_b, n);

struct vec_int     { std::vector<int> v; };
JSONREFL_METADATA(vec_int, v);

struct vec_dbl     { std::vector<double> v; };            // array element type differs
JSONREFL_METADATA(vec_dbl, v);

struct map_int     { std::map<std::string, int> m; };
JSONREFL_METADATA(map_int, m);

struct map_dbl     { std::map<std::string, double> m; };  // map value type differs
JSONREFL_METADATA(map_dbl, m);

JSONREFL_STRUCT(via_struct_macro, (int, a), (std::string, b)); // JSONREFL_STRUCT path must match METADATA

// strongest guarantee: purely compile-time.
static_assert(jsonrefl::schema_id<base_v1>() == jsonrefl::schema_id<base_clone>(),   "identical shape must match");
static_assert(jsonrefl::schema_id<base_v1>() == jsonrefl::schema_id<via_struct_macro>(), "STRUCT vs METADATA must match");
static_assert(jsonrefl::schema_id<base_v1>() != jsonrefl::schema_id<added>(),         "add must differ");
static_assert(jsonrefl::schema_id<base_v1>() != jsonrefl::schema_id<removed>(),       "remove must differ");
static_assert(jsonrefl::schema_id<base_v1>() != jsonrefl::schema_id<renamed>(),       "rename must differ");
static_assert(jsonrefl::schema_id<base_v1>() != jsonrefl::schema_id<reordered>(),     "reorder must differ");
static_assert(jsonrefl::schema_id<base_v1>() != jsonrefl::schema_id<retyped_cat>(),   "retype (category) must differ");
static_assert(jsonrefl::schema_id<width_int>() != jsonrefl::schema_id<width_short>(), "retype (width) must differ");
static_assert(jsonrefl::schema_id<outer_a>() != jsonrefl::schema_id<outer_b>(),       "nested shape change must differ");
static_assert(jsonrefl::schema_id<vec_int>() != jsonrefl::schema_id<vec_dbl>(),       "array element type must differ");
static_assert(jsonrefl::schema_id<map_int>() != jsonrefl::schema_id<map_dbl>(),       "map value type must differ");

} // ns schema_test

bool test_schema_id_identical_shapes_match() {
    CHECK_EQ(jsonrefl::schema_id<schema_test::base_v1>(), jsonrefl::schema_id<schema_test::base_clone>());
    CHECK_EQ(jsonrefl::schema_id<schema_test::base_v1>(), jsonrefl::schema_id<schema_test::via_struct_macro>());
    return true;
}

bool test_schema_id_detects_add_remove_rename_reorder() {
    const auto base = jsonrefl::schema_id<schema_test::base_v1>();
    CHECK(base != jsonrefl::schema_id<schema_test::added>());
    CHECK(base != jsonrefl::schema_id<schema_test::removed>());
    CHECK(base != jsonrefl::schema_id<schema_test::renamed>());
    CHECK(base != jsonrefl::schema_id<schema_test::reordered>());
    return true;
}

bool test_schema_id_detects_retype() {
    CHECK(jsonrefl::schema_id<schema_test::base_v1>() != jsonrefl::schema_id<schema_test::retyped_cat>());
    CHECK(jsonrefl::schema_id<schema_test::width_int>() != jsonrefl::schema_id<schema_test::width_short>());
    return true;
}

bool test_schema_id_recurses_into_nested_and_containers() {
    CHECK(jsonrefl::schema_id<schema_test::outer_a>() != jsonrefl::schema_id<schema_test::outer_b>());
    CHECK(jsonrefl::schema_id<schema_test::vec_int>() != jsonrefl::schema_id<schema_test::vec_dbl>());
    CHECK(jsonrefl::schema_id<schema_test::map_int>() != jsonrefl::schema_id<schema_test::map_dbl>());
    return true;
}

bool test_schema_id_is_constexpr_and_deterministic() {
    constexpr auto a = jsonrefl::schema_id<schema_test::base_v1>();
    constexpr auto b = jsonrefl::schema_id<schema_test::base_v1>();
    static_assert(a == b, "schema_id must be a deterministic constant expression");
    CHECK_EQ(a, jsonrefl::schema_id<schema_test::base_v1>());
    CHECK(a != 0u);
    return true;
}

/***************************************************************************************************/

int main() {
    const bool ok =
           JSONREFL_TEST(test_version_macros)
        && JSONREFL_TEST(test_metadata_int_string)
        && JSONREFL_TEST(test_metadata_nested)
        && JSONREFL_TEST(test_metadata_nested_with_array)
        && JSONREFL_TEST(test_metadata_result_t)
        && JSONREFL_TEST(test_metadata_rate_limits_t)
        && JSONREFL_TEST(test_metadata_response_t)
        && JSONREFL_TEST(test_calc_max_stack_depth)
        && JSONREFL_TEST(test_dump)
        && JSONREFL_TEST(test_phf_next_pow2_is_power_of_two)
        && JSONREFL_TEST(test_phf_fnv1a_distinct_for_distinct_keys)
        && JSONREFL_TEST(test_phf_empty_index_constexpr)
        && JSONREFL_TEST(test_phf_built_from_object_holder_for_result_t)
        && JSONREFL_TEST(test_phf_built_from_object_holder_for_int_string)
        && JSONREFL_TEST(test_phf_lookup_matches_metadata_lookup)
        && JSONREFL_TEST(test_phf_strategy_linear_for_small_struct)
        && JSONREFL_TEST(test_phf_linear_index_layout_matches_declaration_order)
        && JSONREFL_TEST(test_phf_strategy_sparse_for_large_struct)
        && JSONREFL_TEST(test_int_array)
        && JSONREFL_TEST(test_string_array)
        && JSONREFL_TEST(test_sv_array)
        && JSONREFL_TEST(test_optional)
        && JSONREFL_TEST(test_parse_int_array)
        && JSONREFL_TEST(test_parse_string_array)
        && JSONREFL_TEST(test_parse_sv_array)
        && JSONREFL_TEST(test_parse_int_string)
        && JSONREFL_TEST(test_parse_int_string_array)
        && JSONREFL_TEST(test_parse_nested)
        && JSONREFL_TEST(test_parse_nested_with_array)
        && JSONREFL_TEST(test_parse_nested_nested_nested)
        && JSONREFL_TEST(test_parse_result_t)
        && JSONREFL_TEST(test_parse_rate_limits_t)
        && JSONREFL_TEST(test_parse_response_t)
        && JSONREFL_TEST(test_unknown_key_rejected)
        && JSONREFL_TEST(test_skip_unknown_keys_object_value)
        && JSONREFL_TEST(test_skip_unknown_keys_nested_container)
        && JSONREFL_TEST(test_literal_true)
        && JSONREFL_TEST(test_literal_false)
        && JSONREFL_TEST(test_literal_null_opt_untouched)
        && JSONREFL_TEST(test_literal_null_opt_default)
        && JSONREFL_TEST(test_literal_null_opt_vec_untouched)
        && JSONREFL_TEST(test_literal_null_opt_vec_default)
        && JSONREFL_TEST(test_literal_true_chunked)
        && JSONREFL_TEST(test_literal_false_chunked)
        && JSONREFL_TEST(test_literal_null_chunked)
        && JSONREFL_TEST(test_parse_string_chunked_ok)
        && JSONREFL_TEST(test_parse_no_buffer)
        && JSONREFL_TEST(test_parse_no_buffer_mid_accum)
        && JSONREFL_TEST(test_parse_no_accum_no_escape_ok)
        && JSONREFL_TEST(test_parse_no_buffer_skipped_for_sv_key_with_escape)
        && JSONREFL_TEST(test_parse_reset_after_failure)
        && JSONREFL_TEST(test_zero_copy_nested_sv)
        && JSONREFL_TEST(test_zero_copy_sv_array)
        && JSONREFL_TEST(test_zero_copy_result_t)
        && JSONREFL_TEST(test_parse_sv_chunked_reject)
        && JSONREFL_TEST(test_parse_sv_array_chunked_reject)
        && JSONREFL_TEST(test_parse_map_sv_value_chunked_reject)
        && JSONREFL_TEST(test_parse_map_sv_key_chunked_reject)
        && JSONREFL_TEST(test_parse_sv_array_singleshot_still_ok)
        && JSONREFL_TEST(test_parse_map_sv_key_singleshot_still_ok)
        && JSONREFL_TEST(test_map_escape_in_key_and_value)
        && JSONREFL_TEST(test_map_escape_unicode_in_key_and_value)
        && JSONREFL_TEST(test_map_escape_in_key_with_int_value)
        && JSONREFL_TEST(test_array_escape_in_string_elements)
        && JSONREFL_TEST(test_struct_unicode_escape_in_field_name)
        && JSONREFL_TEST(test_parse_no_buffer_escape_in_map_key)
        && JSONREFL_TEST(test_parse_no_buffer_escape_in_map_value)
        && JSONREFL_TEST(test_parse_no_buffer_escape_in_map_int_key)
        && JSONREFL_TEST(test_parse_no_buffer_escape_in_array_element)
        && JSONREFL_TEST(test_parse_no_buffer_escape_in_struct_field_name)
        && JSONREFL_TEST(test_chunked_no_split_bool_true)
        && JSONREFL_TEST(test_chunked_no_split_bool_false)
        && JSONREFL_TEST(test_chunked_no_split_null)
        && JSONREFL_TEST(test_chunked_no_split_map_keys)
        && JSONREFL_TEST(test_chunked_no_split_nested_keys_nums)
        && JSONREFL_TEST(test_chunked_atomic_leaf_terminator)
        && JSONREFL_TEST(test_chunked_long_string_roundtrip)
        && JSONREFL_TEST(test_chunked_long_string_chunked_deser)
        && JSONREFL_TEST(test_chunked_map_string_string_split_key_value)
        && JSONREFL_TEST(test_chunked_map_string_string_two_pairs_all_split)
        && JSONREFL_TEST(test_chunked_map_string_int_split_key_and_value)
        && JSONREFL_TEST(test_chunked_struct_split_key_then_split_value)
        && JSONREFL_TEST(test_chunked_struct_two_string_fields_all_split)
        && JSONREFL_TEST(test_chunked_map_mixed_split_alignment)
        && JSONREFL_TEST(test_chunked_struct_split_value_with_escape)
        && JSONREFL_TEST(test_chunked_map_escape_in_key_and_value)
        && JSONREFL_TEST(test_chunked_array_escape_in_elements)
        && JSONREFL_TEST(test_in_source_int_string)
        && JSONREFL_TEST(test_in_source_string_escapes)
        && JSONREFL_TEST(test_in_source_unicode_bmp)
        && JSONREFL_TEST(test_in_source_unicode_surrogate)
        && JSONREFL_TEST(test_in_source_sv_zero_copy)
        && JSONREFL_TEST(test_in_source_sv_decoded_into_buffer)
        && JSONREFL_TEST(test_in_source_sv_array)
        && JSONREFL_TEST(test_in_source_extra_data)
        && JSONREFL_TEST(test_in_source_invalid)
        && JSONREFL_TEST(test_in_source_invalid_escape)
        && JSONREFL_TEST(test_in_source_sv_cross_chunk_string)
        && JSONREFL_TEST(test_value_t_to_conversions)
        && JSONREFL_TEST(test_value_t_integer_floating_kinds)
        && JSONREFL_TEST(test_value_t_nonstandard_float_kind)
        && JSONREFL_TEST(test_value_t_parse_and_convert)
        && JSONREFL_TEST(test_value_t_parse_m_zero_copy)
        && JSONREFL_TEST(test_value_t_roundtrip)
        && JSONREFL_TEST(test_parse_m_escape_no_accum_ok)
        && JSONREFL_TEST(test_parse_m_escape_in_map)
        && JSONREFL_TEST(test_root_vector_struct_singleshot)
        && JSONREFL_TEST(test_root_vector_struct_with_int_singleshot)
        && JSONREFL_TEST(test_root_vector_struct_chunked_4byte)
        && JSONREFL_TEST(test_root_vector_struct_chunked_with_escapes)
        && JSONREFL_TEST(test_root_map_struct_singleshot)
        && JSONREFL_TEST(test_root_map_struct_chunked_4byte)
        && JSONREFL_TEST(test_nested_vector_vector_string_parse_and_roundtrip)
        && JSONREFL_TEST(test_nested_vector_vector_int_parse_and_roundtrip)
        && JSONREFL_TEST(test_nested_vector_vector_struct_parse_and_roundtrip)
        && JSONREFL_TEST(test_root_vector_vector_string_parse_and_roundtrip)
        && JSONREFL_TEST(test_root_vector_vector_int_parse_and_roundtrip)
        && JSONREFL_TEST(test_root_vector_vector_struct_parse_and_roundtrip)
        && JSONREFL_TEST(test_nested_map_map_int_parse_and_roundtrip)
        && JSONREFL_TEST(test_nested_map_unordered_map_int_parse_and_roundtrip)
        && JSONREFL_TEST(test_nested_map_three_levels_parse_and_roundtrip)
        && JSONREFL_TEST(test_root_map_map_int_parse_and_roundtrip)
        && JSONREFL_TEST(test_parse_m_chunked_round_trip)
        && JSONREFL_TEST(test_parse_m_chunked_round_trip_small_buf)
        && JSONREFL_TEST(test_parse_m_chunked_clean_boundary)
        && JSONREFL_TEST(test_metadata_enums)
        && JSONREFL_TEST(test_enum_roundtrip_serialize_parse)
        && JSONREFL_TEST(test_enum_parse_optional_and_empty_array)
        && JSONREFL_TEST(test_root_vector_enum_roundtrip)
        && JSONREFL_TEST(test_map_string_enum_roundtrip)
        && JSONREFL_TEST(test_parse_m_c2_violation_string)
        && JSONREFL_TEST(test_parse_m_c2_violation_number)
        && JSONREFL_TEST(test_parse_m_c2_violation_key)
        && JSONREFL_TEST(test_parse_m_c2_violation_literal)
        && JSONREFL_TEST(test_parse_m_c2_escape_split_sv_cross_chunk)
        && JSONREFL_TEST(test_parse_extra_data_trailing_garbage)
        && JSONREFL_TEST(test_float_required_bytes_matches_to_string)
        && JSONREFL_TEST(test_integer_extremes_roundtrip)
        && JSONREFL_TEST(test_to_chunked_buffer_flush_failure)
        && JSONREFL_TEST(test_malformed_json_parse)
        && JSONREFL_TEST(test_unknown_key_reset_then_parse_ok)
        && JSONREFL_TEST(test_duplicate_key_last_wins)
        && JSONREFL_TEST(test_deep_nesting_returns_invalid)
        && JSONREFL_TEST(test_nan_inf_serialise_as_null)
        && JSONREFL_TEST(test_map_string_vector_int_parse_and_roundtrip)
        && JSONREFL_TEST(test_to_string_size_exact)
        && JSONREFL_TEST(test_comments_line_basic)
        && JSONREFL_TEST(test_comments_disallowed)
        && JSONREFL_TEST(test_comments_after_document)
        && JSONREFL_TEST(test_comments_line_no_newline_at_eof)
        && JSONREFL_TEST(test_comments_line_spanning_chunks)
        && JSONREFL_TEST(test_comments_slash_at_chunk_boundary)
        && JSONREFL_TEST(test_comments_with_skip_unknown_keys)
        && JSONREFL_TEST(test_invalid_utf16_lone_high_surrogate)
        && JSONREFL_TEST(test_invalid_utf16_lone_low_surrogate)
        && JSONREFL_TEST(test_invalid_utf16_valid_pair_unaffected)
        && JSONREFL_TEST(test_invalid_utf8_raw_bytes_rejected_by_default)
        && JSONREFL_TEST(test_invalid_utf8_raw_bytes_allowed_with_flag)
        && JSONREFL_TEST(test_invalid_utf8_truncated_sequence_rejected)
        && JSONREFL_TEST(test_invalid_utf8_valid_multibyte_unaffected)
        && JSONREFL_TEST(test_invalid_utf8_chunked_cross_boundary)
        && JSONREFL_TEST(test_invalid_utf8_bad_continuation_rejected)
        && JSONREFL_TEST(test_invalid_escapes_rejected_by_default)
        && JSONREFL_TEST(test_invalid_escapes_allowed_with_flag)
        && JSONREFL_TEST(test_invalid_escapes_space_allowed)
        && JSONREFL_TEST(test_invalid_escapes_non_insource)
        && JSONREFL_TEST(test_seq_two_objects)
        && JSONREFL_TEST(test_seq_two_arrays)
        && JSONREFL_TEST(test_seq_two_strings)
        && JSONREFL_TEST(test_seq_two_booleans_adjacent)
        && JSONREFL_TEST(test_seq_two_nulls_adjacent)
        && JSONREFL_TEST(test_seq_two_numbers_whitespace)
        && JSONREFL_TEST(test_seq_array_then_object)
        && JSONREFL_TEST(test_seq_three_records)
        && JSONREFL_TEST(test_seq_whitespace_between)
        && JSONREFL_TEST(test_seq_single_record)
        && JSONREFL_TEST(test_seq_strings_with_escapes)
        && JSONREFL_TEST(test_seq_with_comments)
        && JSONREFL_TEST(test_seq_chunked_cross_chunk)
        && JSONREFL_TEST(test_seq_next_m_two_objects)
        && JSONREFL_TEST(test_seq_next_m_with_escapes)
        && JSONREFL_TEST(test_seq_comment_slash_at_chunk_boundary)
        && JSONREFL_TEST(test_seq_extra_data_unchanged)
        && JSONREFL_TEST(test_seq_ndjson)
        && JSONREFL_TEST(test_inf_nan_basic)
        && JSONREFL_TEST(test_inf_nan_neg_inf)
        && JSONREFL_TEST(test_inf_nan_all_three)
        && JSONREFL_TEST(test_inf_nan_disallowed)
        && JSONREFL_TEST(test_inf_nan_chunked)
        && JSONREFL_TEST(test_inf_nan_skip_unknown)
        && JSONREFL_TEST(test_doc_comments_pretty_with_comments)
        && JSONREFL_TEST(test_doc_comments_pretty_without_comments)
        && JSONREFL_TEST(test_doc_comments_compact_ignores_comments)
        && JSONREFL_TEST(test_doc_comments_roundtrip)
        && JSONREFL_TEST(test_doc_comments_metadata_macro)
        && JSONREFL_TEST(test_doc_comments_empty_doc_no_emit)
        && JSONREFL_TEST(test_doc_comments_chunked)
        && JSONREFL_TEST(test_flags_combine_operators)
    ;

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*************************************************************************************************/
