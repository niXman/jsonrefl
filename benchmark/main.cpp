
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

#include "../tests/main.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

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

int main() {
    static const auto test_data = make_test_data();
    const auto N = test_data.size();

    const auto rb_compact = jsonrefl::required_bytes(test_data);
    const auto rb_pretty  = jsonrefl::required_bytes(test_data, true);
    auto buf_c = std::make_unique<char[]>(rb_compact);
    auto buf_p = std::make_unique<char[]>(rb_pretty);

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

    using clock = std::chrono::high_resolution_clock;
    auto ns = [](auto d) { return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count(); };

    std::cout << "\n--- benchmark: vector<response_t>[" << N << "] ---\n";
    std::cout << "total bytes: compact=" << rb_compact
              << " pretty=" << rb_pretty << "\n";

    constexpr auto ITERS = 10000u;
    char chunk_buf[1500];

    for ( int w = 0; w < 100; ++w ) {
        auto s = jsonrefl::to_string(test_data); (void)s;
        jsonrefl::to_buffer(buf_c.get(), test_data);
        jsonrefl::to_chunked_buffer(chunk_buf, sizeof(chunk_buf), test_data,
            [](const void *, std::size_t) -> bool { return true; });
    }

    auto t0 = clock::now();
    for ( auto i = 0u; i < ITERS; ++i ) { auto s = jsonrefl::to_string(test_data); (void)s; }
    auto t1 = clock::now();
    for ( auto i = 0u; i < ITERS; ++i ) { auto s = jsonrefl::to_string(test_data, true); (void)s; }
    auto t2 = clock::now();
    for ( auto i = 0u; i < ITERS; ++i ) { auto rb = jsonrefl::required_bytes(test_data); (void)rb; }
    auto t3 = clock::now();
    for ( auto i = 0u; i < ITERS; ++i ) { auto rb = jsonrefl::required_bytes(test_data, true); (void)rb; }
    auto t4 = clock::now();
    for ( auto i = 0u; i < ITERS; ++i ) { jsonrefl::to_buffer(buf_c.get(), test_data); }
    auto t5 = clock::now();
    for ( auto i = 0u; i < ITERS; ++i ) { jsonrefl::to_buffer(buf_p.get(), test_data, true); }
    auto t6 = clock::now();
    for ( auto i = 0u; i < ITERS; ++i ) {
        jsonrefl::to_chunked_buffer(chunk_buf, sizeof(chunk_buf), test_data,
            [](const void *, std::size_t) -> bool { return true; });
    }
    auto t7 = clock::now();
    for ( auto i = 0u; i < ITERS; ++i ) {
        jsonrefl::to_chunked_buffer(chunk_buf, sizeof(chunk_buf), test_data,
            [](const void *, std::size_t) -> bool { return true; }, true);
    }
    auto t8 = clock::now();

    std::cout << ITERS << " iterations:\n";
    std::cout << "to_string  compact: " << ns(t1-t0)/ITERS << " ns/iter\n";
    std::cout << "to_string  pretty:  " << ns(t2-t1)/ITERS << " ns/iter\n";
    std::cout << "req_bytes  compact: " << ns(t3-t2)/ITERS << " ns/iter\n";
    std::cout << "req_bytes  pretty:  " << ns(t4-t3)/ITERS << " ns/iter\n";
    std::cout << "to_buffer  compact: " << ns(t5-t4)/ITERS << " ns/iter\n";
    std::cout << "to_buffer  pretty:  " << ns(t6-t5)/ITERS << " ns/iter\n";
    std::cout << "chunked    compact: " << ns(t7-t6)/ITERS << " ns/iter\n";
    std::cout << "chunked    pretty:  " << ns(t8-t7)/ITERS << " ns/iter\n";

    return 0;
}

/*************************************************************************************************/
