
// Demo: chunked JSON array feed via chunked_array_feed.hpp (vector<string> root).

#include "chunked_array_feed.hpp"

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

/*************************************************************************************************/

static std::size_t g_batches = 0;
static std::size_t g_strings_delivered = 0;
static std::vector<std::string> g_all_delivered;

static const char *state_label(jsonrefl::state st)
{
    switch ( st ) {
    case jsonrefl::state::ok:             return "ok";
    case jsonrefl::state::incomplete:     return "incomplete";
    case jsonrefl::state::invalid:        return "invalid";
    case jsonrefl::state::unknown_key:    return "unknown_key";
    case jsonrefl::state::record_end:     return "record_end";
    case jsonrefl::state::no_buffer:      return "no_buffer";
    case jsonrefl::state::sv_cross_chunk: return "sv_cross_chunk";
    }
    return "?";
}

static void print_chunk_raw(const char *data, std::size_t n)
{
    std::cout << "  chunk raw (" << n << " bytes):\n    json: \"";
    for ( std::size_t i = 0; i < n; ++i ) {
        const unsigned char c = static_cast<unsigned char>(data[i]);
        if ( c >= 0x20u && c < 0x7fu && c != '"' && c != '\\' ) {
            std::cout << static_cast<char>(c);
        } else if ( c == '"' ) {
            std::cout << "\\\"";
        } else if ( c == '\\' ) {
            std::cout << "\\\\";
        } else if ( c == '\n' ) {
            std::cout << "\\n";
        } else if ( c == '\r' ) {
            std::cout << "\\r";
        } else if ( c == '\t' ) {
            std::cout << "\\t";
        } else {
            std::cout << "\\x" << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<unsigned>(c) << std::dec << std::setfill(' ');
        }
    }
    std::cout << "\"\n    hex:";
    for ( std::size_t i = 0; i < n; ++i ) {
        if ( i % 16 == 0 ) { std::cout << '\n' << "      "; }
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned>(static_cast<unsigned char>(data[i]))
                  << std::dec << std::setfill(' ');
    }
    std::cout << '\n';
}

template<typename Iter>
static void print_string_range(const char *label, Iter first, Iter last)
{
    const auto count = static_cast<std::size_t>(std::distance(first, last));
    std::cout << "  " << label << " (" << count << "):\n";
    if ( count == 0 ) {
        std::cout << "    (none)\n";
        return;
    }

    constexpr std::size_t k_per_line = 8;
    std::size_t i = 0;
    for ( Iter it = first; it != last; ++it, ++i ) {
        if ( i % k_per_line == 0 ) { std::cout << "    "; }
        if ( i > 0 && i % k_per_line != 0 ) { std::cout << ", "; }
        std::cout << '"' << *it << '"';
        if ( i % k_per_line == k_per_line - 1 || i + 1 == count ) {
            std::cout << '\n';
        }
    }
}

static void print_string_list(const char *label, const std::vector<std::string> &v)
{
    print_string_range(label, v.begin(), v.end());
}

/*************************************************************************************************/

int main()
{
    std::vector<std::string> source;
    source.reserve(128);
    for ( int i = 0; i < 128; ++i ) {
        source.push_back(std::to_string(i));
    }

    std::cout << "source: " << source.size() << " strings, range [\""
              << source.front() << "\" .. \"" << source.back() << "\"]\n\n";

    std::vector<std::string> window;
    window.reserve(16);
    chunked_array_feed::parser_holder<std::vector<std::string>> holder{&window};

    char tmp[32];
    jsonrefl::state last_st = jsonrefl::state::incomplete;
    std::size_t chunk_idx = 0;
    std::size_t chunk_bytes = 0;

    const bool serialized = jsonrefl::to_chunked_buffer(
         tmp
        ,sizeof(tmp)
        ,source
        ,[&](const void *data, std::size_t n) -> bool {
            ++chunk_idx;
            chunk_bytes += n;
            const auto *bytes = static_cast<const char *>(data);

            std::cout << "---- chunk " << chunk_idx << " (" << n << " bytes) ----\n";
            print_chunk_raw(bytes, n);

            const std::size_t parsed = chunked_array_feed::feed_array_chunk(
                 holder.parser()
                ,window
                ,bytes
                ,n
                ,last_st
                ,[&](auto first, auto last) {
                    ++g_batches;
                    g_strings_delivered += static_cast<std::size_t>(std::distance(first, last));
                    g_all_delivered.insert(g_all_delivered.end(), first, last);
                    print_string_range("deserialized strings (this chunk)", first, last);
                }
            );

            if ( parsed == 0 ) {
                print_string_list("deserialized strings (this chunk)", std::vector<std::string>{});
            }

            print_string_list("window after chunk", window);
            std::cout << "  parse -> " << state_label(last_st)
                      << ", new_strings=" << parsed
                      << ", window.size()=" << window.size()
                      << "\n\n";
            std::cout.flush();
            return true;
        }
    );

    if ( !serialized ) {
        std::cerr << "to_chunked_buffer failed\n";
        return 1;
    }

    if ( last_st != jsonrefl::state::ok ) {
        std::cerr << "expected state::ok, got "
                  << state_label(last_st)
                  << '\n';
        return 1;
    }

    std::cout << "==== summary ====\n";
    std::cout << "chunks=" << chunk_idx
              << " bytes=" << chunk_bytes
              << " batches=" << g_batches
              << " strings_delivered=" << g_strings_delivered
              << " final_window=" << window.size()
              << '\n';
    print_string_list("all strings delivered (in order)", g_all_delivered);

    if ( g_strings_delivered != source.size() || g_all_delivered != source ) {
        std::cerr << "delivery mismatch\n";
        return 1;
    }

    return 0;
}
