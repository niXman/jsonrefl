
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

#include <jsonrefl/jsonrefl.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <climits>

/*************************************************************************************************/

#define CHECK(expr)                                                          \
    do {                                                                     \
        if ( !(expr) ) [[unlikely]] {                                        \
            std::fprintf(stderr, "CHECK failed: %s\n  %s:%d in %s()\n",     \
                #expr, __FILE__, __LINE__, __func__);                        \
            std::abort();                                                    \
        }                                                                    \
    } while(0)

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
    } while(0)

/*************************************************************************************************/

static std::string read_file(const std::filesystem::path &p) {
    std::ifstream ifs(p, std::ios::binary | std::ios::ate);
    if ( !ifs ) {
        std::fprintf(stderr, "cannot open: %s\n", p.c_str());
        std::abort();
    }
    auto sz = ifs.tellg();
    ifs.seekg(0);
    std::string buf(static_cast<std::size_t>(sz), '\0');
    ifs.read(buf.data(), sz);
    return buf;
}

static const auto g_base = std::filesystem::path(__FILE__).parent_path() / "roundtrip";
static const auto g_base_jc = std::filesystem::path(__FILE__).parent_path() / "jsonchecker";

template<typename T>
static bool parse_json(const char *filename, T &obj) {
    auto json = read_file(g_base / filename);
    auto p = jsonrefl::make_parser(&obj);
    return p.parse(json);
}

struct jc_result { bool ok; bool finished; };

template<typename T>
static jc_result parse_jc(const char *filename, T &obj) {
    auto json = read_file(g_base_jc / filename);
    auto p = jsonrefl::make_parser(&obj);
    bool ok = p.parse(json);
    return {ok, ok ? p.finished() : false};
}

/*************************************************************************************************/
// type definitions

using roundtrip01 = std::vector<std::string>;           // [null]
using roundtrip02 = std::vector<std::string>;           // [true]
using roundtrip03 = std::vector<std::string>;           // [false]
using roundtrip04 = std::vector<int>;                   // [0]
using roundtrip05 = std::vector<std::string>;           // ["foo"]
using roundtrip06 = std::vector<int>;                   // []
using roundtrip07 = std::map<std::string, std::string>; // {}
using roundtrip08 = std::vector<int>;                   // [0,1]

struct roundtrip09 {
    std::string foo;
};
JSONREFL_METADATA(roundtrip09, foo);

struct roundtrip10 {
    std::optional<std::string> a;
    std::string foo;
};
JSONREFL_METADATA(roundtrip10, a, foo);

using roundtrip11 = std::vector<int>;                   // [-1]
using roundtrip12 = std::vector<int>;                   // [-2147483648]
using roundtrip13 = std::vector<std::int64_t>;          // [-1234567890123456789]
using roundtrip14 = std::vector<std::int64_t>;          // [-9223372036854775808]
using roundtrip15 = std::vector<int>;                   // [1]
using roundtrip16 = std::vector<int>;                   // [2147483647]
using roundtrip17 = std::vector<std::uint64_t>;         // [4294967295]
using roundtrip18 = std::vector<std::int64_t>;          // [1234567890123456789]
using roundtrip19 = std::vector<std::int64_t>;          // [9223372036854775807]
using roundtrip20 = std::vector<double>;                // [0.0]
using roundtrip21 = std::vector<double>;                // [-0.0]
using roundtrip22 = std::vector<double>;                // [1.2345]
using roundtrip23 = std::vector<double>;                // [-1.2345]
using roundtrip24 = std::vector<double>;                // [5e-324]
using roundtrip25 = std::vector<double>;                // [2.225073858507201e-308]
using roundtrip26 = std::vector<double>;                // [2.2250738585072014e-308]
using roundtrip27 = std::vector<double>;                // [1.7976931348623157e308]
using roundtrip_double = std::vector<double>;           // array of 15 doubles

/*************************************************************************************************/
// test functions

void test_roundtrip01() {
    roundtrip01 obj;
    CHECK(parse_json("roundtrip01.json", obj));
    CHECK_EQ(obj.size(), 0u); // null does not create an element
}

void test_roundtrip02() {
    roundtrip02 obj;
    CHECK(parse_json("roundtrip02.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], "true");
}

void test_roundtrip03() {
    roundtrip03 obj;
    CHECK(parse_json("roundtrip03.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], "false");
}

void test_roundtrip04() {
    roundtrip04 obj;
    CHECK(parse_json("roundtrip04.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], 0);
}

void test_roundtrip05() {
    roundtrip05 obj;
    CHECK(parse_json("roundtrip05.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], "foo");
}

void test_roundtrip06() {
    roundtrip06 obj;
    CHECK(parse_json("roundtrip06.json", obj));
    CHECK_EQ(obj.size(), 0u);
}

void test_roundtrip07() {
    roundtrip07 obj;
    CHECK(parse_json("roundtrip07.json", obj));
    CHECK_EQ(obj.size(), 0u);
}

void test_roundtrip08() {
    roundtrip08 obj;
    CHECK(parse_json("roundtrip08.json", obj));
    CHECK_EQ(obj.size(), 2u);
    CHECK_EQ(obj[0], 0);
    CHECK_EQ(obj[1], 1);
}

void test_roundtrip09() {
    roundtrip09 obj{};
    CHECK(parse_json("roundtrip09.json", obj));
    CHECK_EQ(obj.foo, "bar");
}

void test_roundtrip10() {
    roundtrip10 obj{};
    CHECK(parse_json("roundtrip10.json", obj));
    CHECK(!obj.a.has_value());
    CHECK_EQ(obj.foo, "bar");
}

void test_roundtrip11() {
    roundtrip11 obj;
    CHECK(parse_json("roundtrip11.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], -1);
}

void test_roundtrip12() {
    roundtrip12 obj;
    CHECK(parse_json("roundtrip12.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], INT32_MIN);
}

void test_roundtrip13() {
    roundtrip13 obj;
    CHECK(parse_json("roundtrip13.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], -1234567890123456789LL);
}

void test_roundtrip14() {
    roundtrip14 obj;
    CHECK(parse_json("roundtrip14.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], INT64_MIN);
}

void test_roundtrip15() {
    roundtrip15 obj;
    CHECK(parse_json("roundtrip15.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], 1);
}

void test_roundtrip16() {
    roundtrip16 obj;
    CHECK(parse_json("roundtrip16.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], INT32_MAX);
}

void test_roundtrip17() {
    roundtrip17 obj;
    CHECK(parse_json("roundtrip17.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], 4294967295ULL);
}

void test_roundtrip18() {
    roundtrip18 obj;
    CHECK(parse_json("roundtrip18.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], 1234567890123456789LL);
}

void test_roundtrip19() {
    roundtrip19 obj;
    CHECK(parse_json("roundtrip19.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], INT64_MAX);
}

void test_roundtrip20() {
    roundtrip20 obj;
    CHECK(parse_json("roundtrip20.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], 0.0);
}

void test_roundtrip21() {
    roundtrip21 obj;
    CHECK(parse_json("roundtrip21.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], -0.0);
    CHECK(std::signbit(obj[0]));
}

void test_roundtrip22() {
    roundtrip22 obj;
    CHECK(parse_json("roundtrip22.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], 1.2345);
}

void test_roundtrip23() {
    roundtrip23 obj;
    CHECK(parse_json("roundtrip23.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], -1.2345);
}

void test_roundtrip24() {
    roundtrip24 obj;
    CHECK(parse_json("roundtrip24.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK(obj[0] > 0.0);
    CHECK(obj[0] <= 5e-324);
}

void test_roundtrip25() {
    roundtrip25 obj;
    CHECK(parse_json("roundtrip25.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], 2.225073858507201e-308);
}

void test_roundtrip26() {
    roundtrip26 obj;
    CHECK(parse_json("roundtrip26.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], 2.2250738585072014e-308);
}

void test_roundtrip27() {
    roundtrip27 obj;
    CHECK(parse_json("roundtrip27.json", obj));
    CHECK_EQ(obj.size(), 1u);
    CHECK_EQ(obj[0], 1.7976931348623157e308);
}

void test_roundtrip_double() {
    roundtrip_double obj;
    CHECK(parse_json("roundtrip-double.json", obj));
    CHECK_EQ(obj.size(), 15u);
    CHECK_EQ(obj[0], 0.123);
    CHECK_EQ(obj[1], 31.245270191439438);
    CHECK_EQ(obj[2], 121.48791951161945);
    CHECK_EQ(obj[3], 42.229999999999997);
    CHECK_EQ(obj[4], 9.0099999999999998);
    CHECK_EQ(obj[5], 13.449999999999999);
    CHECK_EQ(obj[6], 0.000071);
    CHECK(obj[7] > 0.0);
    CHECK(obj[7] <= 5e-324);
    CHECK_EQ(obj[8], 17.922589369123898);
    CHECK_EQ(obj[9], 17.922589369123899);
    CHECK_EQ(obj[10], 1.7976931348623157e+308);
    CHECK_EQ(obj[11], 98.88339545701953);
    CHECK_EQ(obj[12], 3545.45);
    CHECK_EQ(obj[13], 3545.4500000000007);
    CHECK_EQ(obj[14], -1.7976931348623157e+308);
}

/*************************************************************************************************/
// jsonchecker fail tests
//
// Categories:
//   REJECT  — parser returns false (syntax error detected)
//   INCOMP  — parser returns true but JSON is incomplete (not finished)
//   ACCEPT  — parser returns true and finished (false negative, parser limitation)

// INCOMP: unclosed array
void test_fail02() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail02.json", obj);
    CHECK(r.ok);
    CHECK(!r.finished);
}

// REJECT: unquoted key
void test_fail03() {
    std::map<std::string, std::string> obj;
    auto r = parse_jc("fail03.json", obj);
    CHECK(!r.ok);
}

// REJECT: trailing comma in array
void test_fail04() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail04.json", obj);
    CHECK(!r.ok);
}

// REJECT: double comma in array
void test_fail05() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail05.json", obj);
    CHECK(!r.ok);
}

// REJECT: missing value before comma in array
void test_fail06() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail06.json", obj);
    CHECK(!r.ok);
}

// REJECT: trailing content after closed array
void test_fail07() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail07.json", obj);
    CHECK(!r.ok);
}

// REJECT: extra closing bracket
void test_fail08() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail08.json", obj);
    CHECK(!r.ok);
}

// ACCEPT: trailing comma in object (parser is lenient)
void test_fail09() {
    std::map<std::string, std::string> obj;
    auto r = parse_jc("fail09.json", obj);
    CHECK(r.ok);
    CHECK(r.finished);
}

// REJECT: trailing value after closed object
void test_fail10() {
    std::map<std::string, std::string> obj;
    auto r = parse_jc("fail10.json", obj);
    CHECK(!r.ok);
}

// REJECT: expression instead of value
void test_fail11() {
    std::map<std::string, std::string> obj;
    auto r = parse_jc("fail11.json", obj);
    CHECK(!r.ok);
}

// REJECT: function call instead of value
void test_fail12() {
    std::map<std::string, std::string> obj;
    auto r = parse_jc("fail12.json", obj);
    CHECK(!r.ok);
}

// ACCEPT: leading zeros in number (parser does not validate number structure)
void test_fail13() {
    std::map<std::string, std::string> obj;
    auto r = parse_jc("fail13.json", obj);
    CHECK(r.ok);
    CHECK(r.finished);
}

// REJECT: hex literal
void test_fail14() {
    std::map<std::string, std::string> obj;
    auto r = parse_jc("fail14.json", obj);
    CHECK(!r.ok);
}

// ACCEPT: invalid \x escape (parser does not validate escape sequences)
void test_fail15() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail15.json", obj);
    CHECK(r.ok);
    CHECK(r.finished);
}

// REJECT: bare backslash outside string
void test_fail16() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail16.json", obj);
    CHECK(!r.ok);
}

// ACCEPT: invalid \0 escape (parser does not validate escape sequences)
void test_fail17() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail17.json", obj);
    CHECK(r.ok);
    CHECK(r.finished);
}

// REJECT: missing colon
void test_fail19() {
    std::map<std::string, std::string> obj;
    auto r = parse_jc("fail19.json", obj);
    CHECK(!r.ok);
}

// REJECT: double colon
void test_fail20() {
    std::map<std::string, std::string> obj;
    auto r = parse_jc("fail20.json", obj);
    CHECK(!r.ok);
}

// REJECT: comma instead of colon
void test_fail21() {
    std::map<std::string, std::string> obj;
    auto r = parse_jc("fail21.json", obj);
    CHECK(!r.ok);
}

// REJECT: colon instead of comma in array
void test_fail22() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail22.json", obj);
    CHECK(!r.ok);
}

// REJECT: bad literal "truth"
void test_fail23() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail23.json", obj);
    CHECK(!r.ok);
}

// REJECT: single quotes
void test_fail24() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail24.json", obj);
    CHECK(!r.ok);
}

// ACCEPT: tab character in string (parser does not reject control chars)
void test_fail25() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail25.json", obj);
    CHECK(r.ok);
    CHECK(r.finished);
}

// ACCEPT: invalid backslash-space escape (parser does not validate escapes)
void test_fail26() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail26.json", obj);
    CHECK(r.ok);
    CHECK(r.finished);
}

// ACCEPT: literal newline in string (parser does not reject control chars)
void test_fail27() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail27.json", obj);
    CHECK(r.ok);
    CHECK(r.finished);
}

// ACCEPT: backslash-newline escape (parser does not validate escapes)
void test_fail28() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail28.json", obj);
    CHECK(r.ok);
    CHECK(r.finished);
}

// ACCEPT: incomplete exponent "0e" (parser does not validate number structure)
void test_fail29() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail29.json", obj);
    CHECK(r.ok);
    CHECK(r.finished);
}

// ACCEPT: incomplete exponent "0e+" (parser does not validate number structure)
void test_fail30() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail30.json", obj);
    CHECK(r.ok);
    CHECK(r.finished);
}

// ACCEPT: invalid exponent "0e+-1" (parser does not validate number structure)
void test_fail31() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail31.json", obj);
    CHECK(r.ok);
    CHECK(r.finished);
}

// INCOMP: unclosed object with trailing comma
void test_fail32() {
    std::map<std::string, std::string> obj;
    auto r = parse_jc("fail32.json", obj);
    CHECK(r.ok);
    CHECK(!r.finished);
}

// REJECT: bracket/brace mismatch
void test_fail33() {
    std::vector<std::string> obj;
    auto r = parse_jc("fail33.json", obj);
    CHECK(!r.ok);
}

/*************************************************************************************************/

int main() {
    test_roundtrip01(); std::cout << "roundtrip01: OK" << std::endl;
    test_roundtrip02(); std::cout << "roundtrip02: OK" << std::endl;
    test_roundtrip03(); std::cout << "roundtrip03: OK" << std::endl;
    test_roundtrip04(); std::cout << "roundtrip04: OK" << std::endl;
    test_roundtrip05(); std::cout << "roundtrip05: OK" << std::endl;
    test_roundtrip06(); std::cout << "roundtrip06: OK" << std::endl;
    test_roundtrip07(); std::cout << "roundtrip07: OK" << std::endl;
    test_roundtrip08(); std::cout << "roundtrip08: OK" << std::endl;
    test_roundtrip09(); std::cout << "roundtrip09: OK" << std::endl;
    test_roundtrip10(); std::cout << "roundtrip10: OK" << std::endl;
    test_roundtrip11(); std::cout << "roundtrip11: OK" << std::endl;
    test_roundtrip12(); std::cout << "roundtrip12: OK" << std::endl;
    test_roundtrip13(); std::cout << "roundtrip13: OK" << std::endl;
    test_roundtrip14(); std::cout << "roundtrip14: OK" << std::endl;
    test_roundtrip15(); std::cout << "roundtrip15: OK" << std::endl;
    test_roundtrip16(); std::cout << "roundtrip16: OK" << std::endl;
    test_roundtrip17(); std::cout << "roundtrip17: OK" << std::endl;
    test_roundtrip18(); std::cout << "roundtrip18: OK" << std::endl;
    test_roundtrip19(); std::cout << "roundtrip19: OK" << std::endl;
    test_roundtrip20(); std::cout << "roundtrip20: OK" << std::endl;
    test_roundtrip21(); std::cout << "roundtrip21: OK" << std::endl;
    test_roundtrip22(); std::cout << "roundtrip22: OK" << std::endl;
    test_roundtrip23(); std::cout << "roundtrip23: OK" << std::endl;
    test_roundtrip24(); std::cout << "roundtrip24: OK" << std::endl;
    test_roundtrip25(); std::cout << "roundtrip25: OK" << std::endl;
    test_roundtrip26(); std::cout << "roundtrip26: OK" << std::endl;
    test_roundtrip27(); std::cout << "roundtrip27: OK" << std::endl;
    test_roundtrip_double(); std::cout << "roundtrip-double: OK" << std::endl;

    std::cout << "\nALL ROUNDTRIP TESTS PASSED\n" << std::endl;

    /*************************************/
    // jsonchecker tests

    // pass tests: heterogeneous/deeply-nested content not representable as a single C++ type
    std::cout << "pass01: SKIPPED (heterogeneous array with nested objects/arrays)" << std::endl;
    std::cout << "pass02: SKIPPED (19-level nested arrays)" << std::endl;
    std::cout << "pass03: SKIPPED (nested objects with non-identifier keys)" << std::endl;

    test_fail02(); std::cout << "fail02: OK" << std::endl;
    test_fail03(); std::cout << "fail03: OK" << std::endl;
    test_fail04(); std::cout << "fail04: OK" << std::endl;
    test_fail05(); std::cout << "fail05: OK" << std::endl;
    test_fail06(); std::cout << "fail06: OK" << std::endl;
    test_fail07(); std::cout << "fail07: OK" << std::endl;
    test_fail08(); std::cout << "fail08: OK" << std::endl;
    test_fail09(); std::cout << "fail09: OK" << std::endl;
    test_fail10(); std::cout << "fail10: OK" << std::endl;
    test_fail11(); std::cout << "fail11: OK" << std::endl;
    test_fail12(); std::cout << "fail12: OK" << std::endl;
    test_fail13(); std::cout << "fail13: OK" << std::endl;
    test_fail14(); std::cout << "fail14: OK" << std::endl;
    test_fail15(); std::cout << "fail15: OK" << std::endl;
    test_fail16(); std::cout << "fail16: OK" << std::endl;
    test_fail17(); std::cout << "fail17: OK" << std::endl;
    test_fail19(); std::cout << "fail19: OK" << std::endl;
    test_fail20(); std::cout << "fail20: OK" << std::endl;
    test_fail21(); std::cout << "fail21: OK" << std::endl;
    test_fail22(); std::cout << "fail22: OK" << std::endl;
    test_fail23(); std::cout << "fail23: OK" << std::endl;
    test_fail24(); std::cout << "fail24: OK" << std::endl;
    test_fail25(); std::cout << "fail25: OK" << std::endl;
    test_fail26(); std::cout << "fail26: OK" << std::endl;
    test_fail27(); std::cout << "fail27: OK" << std::endl;
    test_fail28(); std::cout << "fail28: OK" << std::endl;
    test_fail29(); std::cout << "fail29: OK" << std::endl;
    test_fail30(); std::cout << "fail30: OK" << std::endl;
    test_fail31(); std::cout << "fail31: OK" << std::endl;
    test_fail32(); std::cout << "fail32: OK" << std::endl;
    test_fail33(); std::cout << "fail33: OK" << std::endl;

    std::cout << "\nALL JSONCHECKER TESTS PASSED" << std::endl;

    return 0;
}

/*************************************************************************************************/
