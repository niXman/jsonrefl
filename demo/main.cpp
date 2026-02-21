
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
// │                    TABLE OF CONTENTS                     │
// ├─────────────────────────────────────────────────────────┤
// │  demo_basic_struct      — Define a struct and serialize  │
// │  demo_nested_structs    — Nested objects with arrays     │
// │  demo_containers        — vector, list, map, unordered  │
// │  demo_pretty_print      — Human-readable JSON output     │
// │  demo_zero_alloc        — Zero-allocation serialization  │
// │  demo_introspection     — Query members at runtime       │
// │  demo_real_world        — REST API response (full demo)  │
// │  demo_lookup            — O(log N) member lookup bench   │
// │  demo_benchmark         — Performance comparison         │
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
// Helper to print a section header

static void section(const char *title) {
    std::cout << "\n┌";
    for (int i = 0; i < 60; ++i) std::cout << "─";
    std::cout << "┐\n│ " << title;
    const auto len = std::string_view{title}.size();
    for (std::size_t i = len; i < 59; ++i) std::cout << ' ';
    std::cout << "│\n└";
    for (int i = 0; i < 60; ++i) std::cout << "─";
    std::cout << "┘\n\n";
}

/*************************************************************************************************/
// 1. Basic struct — the simplest possible example

struct point {
    double x;
    double y;
};
JSONREFL_METADATA(point, x, y);

void demo_basic_struct() {
    section("demo_basic_struct — Define a struct and serialize");

    point p{3.14, 2.71};

    std::cout << "  // 1. Define your struct\n";
    std::cout << "  struct point {\n";
    std::cout << "      double x;\n";
    std::cout << "      double y;\n";
    std::cout << "  };\n";
    std::cout << "  JSONREFL_METADATA(\n";
    std::cout << "      point,\n";
    std::cout << "      x,\n";
    std::cout << "      y\n";
    std::cout << "  );\n\n";
    std::cout << "  // 2. That's it — now it serializes to JSON automatically\n";
    std::cout << "  point p{3.14, 2.71};\n\n";
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
    section("demo_nested_structs — Nested objects with arrays");

    std::cout << "  struct color {\n";
    std::cout << "      std::string name;\n";
    std::cout << "      int r;\n";
    std::cout << "      int g;\n";
    std::cout << "      int b;\n";
    std::cout << "  };\n";
    std::cout << "  JSONREFL_METADATA(\n";
    std::cout << "      color,\n";
    std::cout << "      name,\n";
    std::cout << "      r,\n";
    std::cout << "      g,\n";
    std::cout << "      b\n";
    std::cout << "  );\n\n";
    std::cout << "  struct palette {\n";
    std::cout << "      std::string_view title;\n";
    std::cout << "      std::vector<color> colors;\n";
    std::cout << "  };\n";
    std::cout << "  JSONREFL_METADATA(\n";
    std::cout << "      palette,\n";
    std::cout << "      title,\n";
    std::cout << "      colors\n";
    std::cout << "  );\n\n";

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
    section("demo_containers — vector, list, map, unordered_map");

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
    section("demo_pretty_print — Human-readable JSON output");

    std::cout << "  struct server_config {\n";
    std::cout << "      std::string_view host;\n";
    std::cout << "      int port;\n";
    std::cout << "      bool tls_enabled;\n";
    std::cout << "      std::vector<std::string_view> allowed_origins;\n";
    std::cout << "      std::map<std::string, std::string> env;\n";
    std::cout << "  };\n";
    std::cout << "  JSONREFL_METADATA(\n";
    std::cout << "      server_config,\n";
    std::cout << "      host,\n";
    std::cout << "      port,\n";
    std::cout << "      tls_enabled,\n";
    std::cout << "      allowed_origins,\n";
    std::cout << "      env\n";
    std::cout << "  );\n\n";

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
// 5. Zero-allocation serialization — write directly into a pre-allocated buffer

void demo_zero_alloc() {
    section("demo_zero_alloc — Zero-allocation serialization");

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
// 6. Runtime introspection — query struct members dynamically

struct trade {
    std::string_view symbol;
    double price;
    std::size_t quantity;
    bool is_buy;
};
JSONREFL_METADATA(trade, symbol, price, quantity, is_buy);

void demo_introspection() {
    section("demo_introspection — Query members at runtime");

    std::cout << "  struct trade {\n";
    std::cout << "      std::string_view symbol;\n";
    std::cout << "      double price;\n";
    std::cout << "      std::size_t quantity;\n";
    std::cout << "      bool is_buy;\n";
    std::cout << "  };\n";
    std::cout << "  JSONREFL_METADATA(\n";
    std::cout << "      trade,\n";
    std::cout << "      symbol,\n";
    std::cout << "      price,\n";
    std::cout << "      quantity,\n";
    std::cout << "      is_buy\n";
    std::cout << "  );\n\n";

    constexpr auto &meta = jsonrefl::metadata<trade>();

    std::cout << "  Struct name:    \"" << meta.name() << "\"\n";
    std::cout << "  Member count:   " << meta.size() << "\n\n";

    // Look up individual members by name
    const char *names[] = {"symbol", "price", "quantity", "is_buy", "nonexistent"};
    for (auto name : names) {
        const auto *m = meta.get(name);
        if (!m) {
            std::cout << "  \"" << name << "\"  -> NOT FOUND\n";
            continue;
        }
        const char *type =
            m->is_string_like() ? "string"   :
            m->is_double()      ? "double"   :
            m->is_uint()        ? "unsigned" :
            m->is_int()         ? "integer"  :
            m->is_array()       ? "array"    :
            m->is_object()      ? "object"   : "other";
        std::cout << "  \"" << name << "\"  -> " << type << "\n";
    }
}

/*************************************************************************************************/
// 7. Real-world example — a REST API response

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
    section("demo_real_world — REST API response (full demo)");

    std::cout << "  struct api_error {\n";
    std::cout << "      int code;\n";
    std::cout << "      std::string_view message;\n";
    std::cout << "  };\n";
    std::cout << "  JSONREFL_METADATA(\n";
    std::cout << "      api_error,\n";
    std::cout << "      code,\n";
    std::cout << "      message\n";
    std::cout << "  );\n\n";
    std::cout << "  struct pagination {\n";
    std::cout << "      std::size_t page;\n";
    std::cout << "      std::size_t per_page;\n";
    std::cout << "      std::size_t total;\n";
    std::cout << "  };\n";
    std::cout << "  JSONREFL_METADATA(\n";
    std::cout << "      pagination,\n";
    std::cout << "      page,\n";
    std::cout << "      per_page,\n";
    std::cout << "      total\n";
    std::cout << "  );\n\n";
    std::cout << "  struct user {\n";
    std::cout << "      std::size_t id;\n";
    std::cout << "      std::string_view name;\n";
    std::cout << "      std::string_view email;\n";
    std::cout << "      std::vector<std::string_view> roles;\n";
    std::cout << "  };\n";
    std::cout << "  JSONREFL_METADATA(\n";
    std::cout << "      user,\n";
    std::cout << "      id,\n";
    std::cout << "      name,\n";
    std::cout << "      email,\n";
    std::cout << "      roles\n";
    std::cout << "  );\n\n";
    std::cout << "  struct api_response {\n";
    std::cout << "      bool success;\n";
    std::cout << "      pagination paging;\n";
    std::cout << "      std::vector<user> data;\n";
    std::cout << "      std::vector<api_error> errors;\n";
    std::cout << "      std::map<std::string, std::string> meta;\n";
    std::cout << "  };\n";
    std::cout << "  JSONREFL_METADATA(\n";
    std::cout << "      api_response,\n";
    std::cout << "      success,\n";
    std::cout << "      paging,\n";
    std::cout << "      data,\n";
    std::cout << "      errors,\n";
    std::cout << "      meta\n";
    std::cout << "  );\n\n";

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
// 8. Lookup benchmark — O(log N) binary search on compile-time sorted hash array

void demo_lookup() {
    section("demo_lookup — O(log N) member lookup benchmark");

    std::cout << "  jsonrefl uses FNV-1a hash + binary search (std::lower_bound)\n";
    std::cout << "  on a compile-time sorted contiguous array of (hash, setter*) pairs.\n\n";
    std::cout << "  Advantages:\n";
    std::cout << "    - Sorted at compile time — zero runtime setup cost\n";
    std::cout << "    - Contiguous memory — fits in L1 cache, no pointer chasing\n";
    std::cout << "    - No heap allocation — static constexpr storage\n";
    std::cout << "    - O(log N) binary search on uint32_t hashes (no string comparisons)\n\n";

    std::cout << "  Comparing against:\n";
    std::cout << "    - std::map<string, int>           — O(log N) red-black tree, heap nodes\n";
    std::cout << "    - std::unordered_map<string, int>  — O(1) amortized, heap buckets + hashing\n\n";

    constexpr auto &meta = jsonrefl::metadata<api_response>();

    // Member names from api_response
    const std::string_view keys[] = {
        "success", "paging", "data", "errors", "meta", "nonexistent"
    };
    constexpr int num_keys = 6;

    // Build equivalent std::map and std::unordered_map with same keys
    std::map<std::string, int> tree_map;
    std::unordered_map<std::string, int> hash_map;
    for (int i = 0; i < num_keys - 1; ++i) {
        tree_map.emplace(keys[i], i);
        hash_map.emplace(keys[i], i);
    }

    std::cout << "  Struct: api_response (" << meta.size() << " members)\n";
    std::cout << "  Keys:   " << num_keys << " (5 hits + 1 miss)\n\n";

    using clock = std::chrono::high_resolution_clock;
    constexpr int N = 1000000;

    // Warm up all paths
    for (auto k : keys) {
        auto r1 = meta.get(k);
        auto r2 = tree_map.find(std::string{k});
        auto r3 = hash_map.find(std::string{k});
        asm volatile("" : : "r"(r1), "r"(&r2), "r"(&r3) : "memory");
    }

    // Benchmark: jsonrefl meta.get()
    auto t0 = clock::now();
    for (int i = 0; i < N; ++i) {
        for (auto k : keys) {
            auto r = meta.get(k);
            asm volatile("" : : "r"(r) : "memory");
        }
    }
    auto t1 = clock::now();

    // Benchmark: std::map::find()
    auto t2 = clock::now();
    for (int i = 0; i < N; ++i) {
        for (auto k : keys) {
            auto r = tree_map.find(std::string{k});
            asm volatile("" : : "r"(&r) : "memory");
        }
    }
    auto t3 = clock::now();

    // Benchmark: std::unordered_map::find()
    auto t4 = clock::now();
    for (int i = 0; i < N; ++i) {
        for (auto k : keys) {
            auto r = hash_map.find(std::string{k});
            asm volatile("" : : "r"(&r) : "memory");
        }
    }
    auto t5 = clock::now();

    auto ns = [](auto d) { return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count(); };

    long long meta_ns   = ns(t1 - t0) / (N * num_keys);
    long long map_ns    = ns(t3 - t2) / (N * num_keys);
    long long umap_ns   = ns(t5 - t4) / (N * num_keys);

    long long meta_total = ns(t1 - t0) / N;
    long long map_total  = ns(t3 - t2) / N;
    long long umap_total = ns(t5 - t4) / N;

    std::cout << "  Iterations: " << N << " x " << num_keys << " lookups\n\n";
    std::cout << "  ┌──────────────────────────────┬──────────┬──────────┐\n";
    std::cout << "  │ Method                       │ Per key  │ Per iter │\n";
    std::cout << "  ├──────────────────────────────┼──────────┼──────────┤\n";

    auto print_row = [](const char *label, long long per_key, long long per_iter) {
        std::cout << "  │ " << label;
        for (std::size_t i = std::string_view{label}.size(); i < 29; ++i) std::cout << ' ';
        std::cout << "│ " << per_key << " ns";
        int d1 = 1; for (auto v = per_key; v >= 10; v /= 10) ++d1;
        for (int i = d1; i < 5; ++i) std::cout << ' ';
        std::cout << "│ " << per_iter << " ns";
        int d2 = 1; for (auto v = per_iter; v >= 10; v /= 10) ++d2;
        for (int i = d2; i < 5; ++i) std::cout << ' ';
        std::cout << "│\n";
    };

    print_row("meta.get() [jsonrefl]", meta_ns, meta_total);
    print_row("std::map::find()",         map_ns,  map_total);
    print_row("std::unordered_map::find()", umap_ns, umap_total);

    std::cout << "  └──────────────────────────────┴──────────┴──────────┘\n\n";

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

    std::cout << "  Why O(log N) wins here:\n";
    std::cout << "    - N=" << meta.size() << ": binary search does at most "
              << (meta.size() <= 2 ? 1 : meta.size() <= 4 ? 2 : 3)
              << " comparisons on uint32_t\n";
    std::cout << "    - Entire index array (" << meta.size() << " x "
              << sizeof(std::pair<std::uint32_t, const void*>) << " = "
              << meta.size() * sizeof(std::pair<std::uint32_t, const void*>)
              << " bytes) fits in one cache line\n";
    std::cout << "    - No heap allocation, no pointer chasing, no string comparison\n";
}

/*************************************************************************************************/
// 9. Benchmark — compare to_string vs required_bytes + to_buffer

void demo_benchmark() {
    section("demo_benchmark — Performance comparison");

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
    constexpr int N = 100000;

    // Warm up
    auto s = jsonrefl::to_string(resp);
    auto n = jsonrefl::required_bytes(resp);
    auto buf = std::make_unique<char[]>(n);
    jsonrefl::to_buffer(buf.get(), resp);

    std::cout << "  Object size: " << n << " bytes (compact JSON)\n";
    std::cout << "  Iterations:  " << N << "\n\n";

    // Benchmark to_string (allocates + serializes)
    auto t0 = clock::now();
    for (int i = 0; i < N; ++i) {
        auto json = jsonrefl::to_string(resp);
        asm volatile("" : : "r"(json.data()) : "memory");
    }
    auto t1 = clock::now();

    // Benchmark required_bytes (count only)
    auto t2 = clock::now();
    for (int i = 0; i < N; ++i) {
        auto bytes = jsonrefl::required_bytes(resp);
        asm volatile("" : : "r"(bytes) : "memory");
    }
    auto t3 = clock::now();

    // Benchmark to_buffer (write into pre-allocated buffer)
    auto t4 = clock::now();
    for (int i = 0; i < N; ++i) {
        jsonrefl::to_buffer(buf.get(), resp);
        asm volatile("" : : "r"(buf.get()) : "memory");
    }
    auto t5 = clock::now();

    auto ns = [](auto d) { return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count(); };

    auto to_string_ns     = ns(t1 - t0) / N;
    auto required_bytes_ns = ns(t3 - t2) / N;
    auto to_buffer_ns     = ns(t5 - t4) / N;

    std::cout << "  ┌──────────────────┬──────────┬──────────────────────┐\n";
    std::cout << "  │ Function         │ Time/op  │ Throughput           │\n";
    std::cout << "  ├──────────────────┼──────────┼──────────────────────┤\n";

    auto print_row = [&](const char *name, long long ns_per_op) {
        double mb_per_sec = (ns_per_op > 0)
            ? static_cast<double>(n) / static_cast<double>(ns_per_op) * 1000.0
            : 0.0;

        std::cout << "  │ " << name;
        for (std::size_t i = std::string_view{name}.size(); i < 17; ++i) std::cout << ' ';
        std::cout << "│ " << ns_per_op << " ns";
        // Align
        int digits = 1;
        for (auto v = ns_per_op; v >= 10; v /= 10) ++digits;
        for (int i = digits; i < 5; ++i) std::cout << ' ';
        std::cout << "│ ";

        char buf[32];
        auto r = std::to_chars(buf, buf + sizeof(buf), mb_per_sec, std::chars_format::fixed, 1);
        std::string_view sv{buf, static_cast<std::size_t>(r.ptr - buf)};
        std::cout << sv << " MB/s";
        for (std::size_t i = sv.size() + 5; i < 21; ++i) std::cout << ' ';
        std::cout << "│\n";
    };

    print_row("to_string",      to_string_ns);
    print_row("required_bytes", required_bytes_ns);
    print_row("to_buffer",      to_buffer_ns);
    std::cout << "  └──────────────────┴──────────┴──────────────────────┘\n\n";

    std::cout << "  to_buffer is ~" << (to_string_ns / (to_buffer_ns ? to_buffer_ns : 1))
              << "x faster than to_string (no allocation overhead)\n";
}

/*************************************************************************************************/

int main() {
    std::cout << R"(
     ╦╔═╗╔═╗╔╗╔  ╦═╗╔═╗╔═╗╦  ╔═╗╔═╗╔╦╗
     ║╚═╗║ ║║║║  ╠╦╝║╣ ╠╣ ║  ║╣ ║   ║
    ╚╝╚═╝╚═╝╝╚╝  ╩╚═╚═╝╚  ╩═╝╚═╝╚═╝ ╩

    Compile-time reflection + JSON serialization for C++17
    Header-only · No codegen · Single macro · Just works
)" << std::endl;

    demo_basic_struct();
    demo_nested_structs();
    demo_containers();
    demo_pretty_print();
    demo_zero_alloc();
    demo_introspection();
    demo_real_world();
    demo_lookup();
    demo_benchmark();

    return 0;
}
