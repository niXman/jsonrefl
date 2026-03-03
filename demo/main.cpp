
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

// jsonrefl — demo
//
// Compile-time reflection + JSON serialization for C++17.
// No prebuild code generation, no external tools — just one header.
//
// ┌─────────────────────────────────────────────────────────┐
// │                    TABLE OF CONTENTS                    │
// ├─────────────────────────────────────────────────────────┤
// │  demo_basic_struct      — Define a struct and serialize │
// │  demo_nested_structs    — Nested objects with arrays    │
// │  demo_containers        — vector, list, map, unordered  │
// │  demo_pretty_print      — Human-readable JSON output    │
// │  demo_chunked_parsing   — Stream parse in a loop        │
// │  demo_zero_alloc        — Zero-allocation serialization │
// │  demo_introspection     — Query members at runtime      │
// │  demo_real_world        — REST API response (full demo) │
// │  demo_lookup            — O(log N) member lookup bench  │
// │  demo_benchmark         — Performance comparison        │
// └─────────────────────────────────────────────────────────┘

#include <jsonrefl/jsonrefl.hpp>

#include <charconv>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

/*************************************************************************************************/
// 1. Basic struct — the simplest possible example

struct point {
    double x;
    double y;
};
JSONREFL_METADATA(point, x, y);

void demo_basic_struct() {
    std::cout << "demo_basic_struct — Define a struct and serialize" << std::endl;

    point p{3.14, 2.71};
    std::cout << "  to_string(p)        = " << jsonrefl::to_string(p) << "\n";
    std::cout << "  to_string(p, true)  =\n" << jsonrefl::to_string(p, true) << "\n";
}

/*************************************************************************************************/
// 2. Nested structs — objects inside objects, arrays of objects

struct color {
    std::string name;
    int r, g, b;
};
JSONREFL_METADATA(color, name, r, g, b);

struct palette {
    std::string_view title;
    std::vector<color> colors;
};
JSONREFL_METADATA(palette, title, colors);

void demo_nested_structs() {
    std::cout << "demo_nested_structs — Nested objects with arrays" << std::endl;

    palette p;
    p.title = "Sunset";
    p.colors = {
         {"Coral",       255, 127, 80}
        ,{"Gold",        255, 215, 0}
        ,{"Crimson",     220, 20,  60}
        ,{"MidnightBlue", 25, 25,  112}
    };

    std::cout << jsonrefl::to_string(p, true) << "\n";
}

/*************************************************************************************************/
// 3. Standard containers as root types — no struct wrapper needed

void demo_containers() {
    std::cout << "demo_containers — vector, list, map, unordered_map" << std::endl;

    // Vectors serialize as JSON arrays
    std::vector<int> primes = {2, 3, 5, 7, 11, 13};
    std::cout << "  vector<int>:     " << jsonrefl::to_string(primes) << "\n";

    // Lists work the same way
    std::list<std::string> tags = {"c++17", "reflection", "json", "header-only"};
    std::cout << "  list<string>:    " << jsonrefl::to_string(tags) << "\n";

    // Maps serialize as JSON objects
    std::map<std::string, int> scores = {
        {"Alice", 950}, {"Bob", 870}, {"Charlie", 920}
    };
    std::cout << "  map<str,int>:    " << jsonrefl::to_string(scores) << "\n";

    // Vectors of structs — each element becomes a JSON object
    std::vector<point> polygon = {{0, 0}, {10, 0}, {10, 10}, {0, 10}};
    std::cout << "  vector<point>:   " << jsonrefl::to_string(polygon) << "\n";
}

/*************************************************************************************************/
// 4. Pretty-print — indented, human-friendly JSON

struct server_config {
    std::string_view host;
    int port;
    bool tls_enabled;
    std::vector<std::string_view> allowed_origins;
    std::map<std::string, std::string> env;
};
JSONREFL_METADATA(server_config, host, port, tls_enabled, allowed_origins, env);

void demo_pretty_print() {
    std::cout << "demo_pretty_print — Human-readable JSON output" << std::endl;

    server_config cfg;
    cfg.host = "0.0.0.0";
    cfg.port = 8443;
    cfg.tls_enabled = true;
    cfg.allowed_origins = {"https://example.com", "https://api.example.com"};
    cfg.env = {{"LOG_LEVEL", "info"}, {"MAX_CONNS", "1000"}};

    // Compact — optimal for network transfer
    std::cout << "  Compact (" << jsonrefl::required_bytes(cfg) << " bytes):\n";
    std::cout << "  " << jsonrefl::to_string(cfg) << "\n\n";

    // Pretty — optimal for humans
    std::cout << "  Pretty (" << jsonrefl::required_bytes(cfg, true) << " bytes):\n";
    std::cout << jsonrefl::to_string(cfg, true) << "\n";
}

/*************************************************************************************************/
// 5. Chunked parsing in a loop — feed incoming data pieces

struct stream_config {
    std::string host;
    int port;
};
JSONREFL_METADATA(stream_config, host, port);

void demo_chunked_parsing() {
    std::cout << "demo_chunked_parsing — Stream parse in a loop" << std::endl;

    stream_config cfg{};
    std::string accum;
    auto p = jsonrefl::make_parser(&cfg);

    // Emulate chunks arriving from a socket.
    const std::string_view chunks[] = {
         R"({"host":"lo)"
        ,R"(calhost","p)"
        ,R"(ort":8080})"
    };

    jsonrefl::state st = jsonrefl::state::incomplete;
    for ( auto chunk : chunks ) {
        st = p.parse(chunk, &accum);
        if ( st == jsonrefl::state::ok ) {
            break;
        }
        if ( st != jsonrefl::state::incomplete ) {
            std::cout << "  parse failed, state=" << static_cast<int>(st) << "\n";
            return;
        }
    }

    if ( st == jsonrefl::state::ok ) {
        std::cout << "  parsed config: host=" << cfg.host << ", port=" << cfg.port << "\n";
    } else {
        std::cout << "  document incomplete after all chunks\n";
    }
}

/*************************************************************************************************/
// 6. Zero-allocation serialization — write directly into a pre-allocated buffer

void demo_zero_alloc() {
    std::cout << "demo_zero_alloc — Zero-allocation serialization" << std::endl;

    std::vector<point> triangle = {{0, 0}, {5, 8.66}, {10, 0}};

    // Step 1: calculate exact byte count (no allocation)
    const auto n = jsonrefl::required_bytes(triangle, true);
    std::cout << "  required_bytes = " << n << "\n\n";

    // Step 2: allocate once, write directly into the buffer
    auto buf = std::make_unique<char[]>(n + 1);
    char *end = jsonrefl::to_buffer(buf.get(), triangle, true);
    buf[n] = '\0';

    std::cout << "  Written " << (end - buf.get()) << " bytes:\n";
    std::cout << buf.get() << "\n";

    // No intermediate std::string objects were created.
    // Perfect for embedded systems, HFT, or any latency-sensitive code.
}

/*************************************************************************************************/
// 7. Runtime introspection — query struct members dynamically

struct trade {
    std::string_view symbol;
    double price;
    std::size_t quantity;
    bool is_buy;
};
JSONREFL_METADATA(trade, symbol, price, quantity, is_buy);

void demo_introspection() {
    std::cout << "demo_introspection — Query members at runtime" << std::endl;

    constexpr auto &meta = jsonrefl::metadata<trade>();

    std::cout << "  Struct name:    \"" << meta.name() << "\"\n";
    std::cout << "  Member count:   " << meta.size() << "\n\n";

    // Look up individual members by name
    const char *names[] = {"symbol", "price", "quantity", "is_buy", "nonexistent"};
    for ( auto name : names ) {
        const auto *m = meta.get(name);
        if ( !m ) {
            std::cout << "  \"" << name << "\"  -> NOT FOUND\n";
            continue;
        }
        const char *type =
            m->is_string()      ? "string"   :
            m->is_double()      ? "double"   :
            m->is_uint()        ? "unsigned" :
            m->is_int()         ? "integer"  :
            m->is_array()       ? "array"    :
            m->is_object()      ? "object"   : "other";
        std::cout << "  \"" << name << "\"  -> " << type << "\n";
    }
}

/*************************************************************************************************/
// 8. Real-world example — a REST API response

struct api_error {
    int code;
    std::string_view message;
};
JSONREFL_METADATA(api_error, code, message);

struct pagination {
    std::size_t page;
    std::size_t per_page;
    std::size_t total;
};
JSONREFL_METADATA(pagination, page, per_page, total);

struct user {
    std::size_t id;
    std::string_view name;
    std::string_view email;
    std::vector<std::string_view> roles;
};
JSONREFL_METADATA(user, id, name, email, roles);

struct api_response {
    bool success;
    pagination paging;
    std::vector<user> data;
    std::vector<api_error> errors;
    std::map<std::string, std::string> meta;
};
JSONREFL_METADATA(api_response, success, paging, data, errors, meta);

void demo_real_world() {
    std::cout << "demo_real_world — REST API response (full demo)" << std::endl;

    api_response resp;
    resp.success = true;
    resp.paging = {1, 2, 42};
    resp.data = {
        {1001, "Alice",   "alice@example.com",   {"admin", "editor"}},
        {1002, "Bob",     "bob@example.com",     {"viewer"}},
    };
    resp.errors = {};
    resp.meta = {
        {"version", "2.1.0"},
        {"request_id", "req-7f3a9b"},
    };

    std::cout << jsonrefl::to_string(resp, true) << "\n";
}

/*************************************************************************************************/
// 9. Lookup benchmark — compile-time perfect-hash metadata lookup

void demo_lookup() {
    std::cout << "demo_lookup — Perfect-hash member lookup benchmark" << std::endl;

    constexpr auto &meta = jsonrefl::metadata<api_response>();

    // Member names from api_response
    const std::string_view keys[] = {
        "success", "paging", "data", "errors", "meta", "nonexistent"
    };
    constexpr auto num_keys = 6;

    // Build equivalent std::map and std::unordered_map with same keys
    std::map<std::string, int> tree_map;
    std::unordered_map<std::string, int> hash_map;
    for ( auto i = 0; i < num_keys - 1; ++i ) {
        tree_map.emplace(keys[i], i);
        hash_map.emplace(keys[i], i);
    }

    std::cout << "  Struct: api_response (" << meta.size() << " members)\n";
    std::cout << "  Keys:   " << num_keys << " (5 hits + 1 miss)\n\n";

    using clock = std::chrono::high_resolution_clock;
    constexpr auto N = 1000000;

    // Warm up all paths
    for ( auto k : keys ) {
        auto r1 = meta.get(k);
        auto r2 = tree_map.find(std::string{k});
        auto r3 = hash_map.find(std::string{k});
        asm volatile("" : : "r"(r1), "r"(&r2), "r"(&r3) : "memory");
    }

    // Benchmark: jsonrefl meta.get()
    auto t0 = clock::now();
    for ( auto i = 0; i < N; ++i ) {
        for ( auto k : keys ) {
            auto r = meta.get(k);
            asm volatile("" : : "r"(r) : "memory");
        }
    }
    auto t1 = clock::now();

    // Benchmark: std::map::find()
    auto t2 = clock::now();
    for ( auto i = 0; i < N; ++i ) {
        for ( auto k : keys ) {
            auto r = tree_map.find(std::string{k});
            asm volatile("" : : "r"(&r) : "memory");
        }
    }
    auto t3 = clock::now();

    // Benchmark: std::unordered_map::find()
    auto t4 = clock::now();
    for ( auto i = 0; i < N; ++i ) {
        for ( auto k : keys ) {
            auto r = hash_map.find(std::string{k});
            asm volatile("" : : "r"(&r) : "memory");
        }
    }
    auto t5 = clock::now();

    auto ns = [](auto d) { return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count(); };

    std::uint64_t meta_ns   = ns(t1 - t0) / (N * num_keys);
    std::uint64_t map_ns    = ns(t3 - t2) / (N * num_keys);
    std::uint64_t umap_ns   = ns(t5 - t4) / (N * num_keys);

    std::uint64_t meta_total = ns(t1 - t0) / N;
    std::uint64_t map_total  = ns(t3 - t2) / N;
    std::uint64_t umap_total = ns(t5 - t4) / N;

    std::cout << "  Iterations: " << N << " x " << num_keys << " lookups\n\n";
    std::cout << "  │ Method                       │ Per key  │ Per iter │\n";

    auto print_row = [](const char *label, std::uint64_t per_key, std::uint64_t per_iter) {
        std::cout << "  │ " << label;
        for ( auto i = std::string_view{label}.size(); i < 29; ++i ) std::cout << ' ';
        std::cout << "│ " << per_key << " ns";
        auto d1 = 1; for ( auto v = per_key; v >= 10; v /= 10 ) ++d1;
        for ( auto i = d1; i < 5; ++i ) std::cout << ' ';
        std::cout << "│ " << per_iter << " ns";
        auto d2 = 1; for ( auto v = per_iter; v >= 10; v /= 10 ) ++d2;
        for ( auto i = d2; i < 5; ++i ) std::cout << ' ';
        std::cout << "│\n";
    };

    print_row("meta.get() [jsonrefl]", meta_ns, meta_total);
    print_row("std::map::find()",         map_ns,  map_total);
    print_row("std::unordered_map::find()", umap_ns, umap_total);

    double map_speedup  = (meta_ns > 0) ? static_cast<double>(map_ns)  / static_cast<double>(meta_ns) : 0;
    double umap_speedup = (meta_ns > 0) ? static_cast<double>(umap_ns) / static_cast<double>(meta_ns) : 0;

    char buf1[16], buf2[16];
    std::to_chars(buf1, buf1 + sizeof(buf1), map_speedup,  std::chars_format::fixed, 1);
    std::to_chars(buf2, buf2 + sizeof(buf2), umap_speedup, std::chars_format::fixed, 1);

    std::cout << "  meta.get() vs std::map           — ";
    auto r1 = std::to_chars(buf1, buf1 + sizeof(buf1), map_speedup, std::chars_format::fixed, 1);
    std::cout << std::string_view{buf1, static_cast<std::size_t>(r1.ptr - buf1)} << "x faster\n";
    std::cout << "  meta.get() vs std::unordered_map — ";
    auto r2 = std::to_chars(buf2, buf2 + sizeof(buf2), umap_speedup, std::chars_format::fixed, 1);
    std::cout << std::string_view{buf2, static_cast<std::size_t>(r2.ptr - buf2)} << "x faster\n\n";

}

/*************************************************************************************************/
// 10. Benchmark — compare to_string vs required_bytes + to_buffer

void demo_benchmark() {
    std::cout << "demo_benchmark — Performance comparison" << std::endl;

    // Build a moderately complex object
    api_response resp;
    resp.success = true;
    resp.paging = {1, 50, 10000};
    resp.data = {
        {1001, "Alice Johnson", "alice@megacorp.com", {"admin", "editor", "reviewer"}},
        {1002, "Bob Smith",     "bob@megacorp.com",   {"viewer"}},
        {1003, "Charlie Lee",   "charlie@startup.io", {"admin", "billing"}},
    };
    resp.errors = {{429, "Rate limit exceeded"}, {503, "Upstream timeout"}};
    resp.meta = {
        {"version", "2.1.0"},
        {"request_id", "req-7f3a9b"},
        {"region", "us-east-1"},
        {"cache", "miss"},
    };

    using clock = std::chrono::high_resolution_clock;
    constexpr auto N = 100000;

    // Warm up
    auto s = jsonrefl::to_string(resp);
    auto n = jsonrefl::required_bytes(resp);
    auto buf = std::make_unique<char[]>(n);
    jsonrefl::to_buffer(buf.get(), resp);

    std::cout << "  Object size: " << n << " bytes (compact JSON)\n";
    std::cout << "  Iterations:  " << N << "\n\n";

    // Benchmark to_string (allocates + serializes)
    auto t0 = clock::now();
    for ( auto i = 0; i < N; ++i ) {
        auto json = jsonrefl::to_string(resp);
        asm volatile("" : : "r"(json.data()) : "memory");
    }
    auto t1 = clock::now();

    // Benchmark required_bytes (count only)
    auto t2 = clock::now();
    for ( auto i = 0; i < N; ++i ) {
        auto bytes = jsonrefl::required_bytes(resp);
        asm volatile("" : : "r"(bytes) : "memory");
    }
    auto t3 = clock::now();

    // Benchmark to_buffer (write into pre-allocated buffer)
    auto t4 = clock::now();
    for ( auto i = 0; i < N; ++i ) {
        jsonrefl::to_buffer(buf.get(), resp);
        asm volatile("" : : "r"(buf.get()) : "memory");
    }
    auto t5 = clock::now();

    auto ns = [](auto d) { return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count(); };

    auto to_string_ns     = ns(t1 - t0) / N;
    auto required_bytes_ns = ns(t3 - t2) / N;
    auto to_buffer_ns     = ns(t5 - t4) / N;

    std::cout << "  │ Function         │ Time/op  │ Throughput           │\n";

    auto print_row = [&](const char *name, std::uint64_t ns_per_op) {
        double mb_per_sec = (ns_per_op > 0)
            ? static_cast<double>(n) / static_cast<double>(ns_per_op) * 1000.0
            : 0.0;

        std::cout << "  │ " << name;
        for ( auto i = std::string_view{name}.size(); i < 17; ++i ) std::cout << ' ';
        std::cout << "│ " << ns_per_op << " ns";
        // Align
        auto digits = 1;
        for ( auto v = ns_per_op; v >= 10; v /= 10 ) ++digits;
        for ( auto i = digits; i < 5; ++i ) std::cout << ' ';
        std::cout << "│ ";

        char buf[32];
        auto r = std::to_chars(buf, buf + sizeof(buf), mb_per_sec, std::chars_format::fixed, 1);
        std::string_view sv{buf, static_cast<std::size_t>(r.ptr - buf)};
        std::cout << sv << " MB/s";
        for ( auto i = sv.size() + 5; i < 21; ++i ) std::cout << ' ';
        std::cout << "│\n";
    };

    print_row("to_string",      to_string_ns);
    print_row("required_bytes", required_bytes_ns);
    print_row("to_buffer",      to_buffer_ns);

    std::cout << "  to_buffer is ~" << (to_string_ns / (to_buffer_ns ? to_buffer_ns : 1))
              << "x faster than to_string (no allocation overhead)\n";
}

/*************************************************************************************************/

int main() {
    demo_basic_struct();
    demo_nested_structs();
    demo_containers();
    demo_pretty_print();
    demo_chunked_parsing();
    demo_zero_alloc();
    demo_introspection();
    demo_real_world();
    demo_lookup();
    demo_benchmark();

    return 0;
}
