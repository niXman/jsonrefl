// Demo: jsonrefl::cursor from parse() and parse_next().
//
// One buffer holds two objects. The cursor reports:
//   status()            — ok, record_end, incomplete, …
//   remaining()         — unread bytes still sitting in that same buffer
//   buffer_releasable() — no pending key and no string_view_t / value_t
//                         still points into the buffer
//
// remaining() > 0 means the cursor itself still points at the unread tail.
// Keep the buffer until that tail is consumed, even when buffer_releasable()
// is true.

#include <jsonrefl/jsonrefl.hpp>

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

/*************************************************************************************************/

struct event {
    int id{};
    std::string msg;
};
JSONREFL_METADATA(event, id, msg);

/*************************************************************************************************/

const char *state_label(jsonrefl::state st) {
    switch ( st ) {
    case jsonrefl::state::ok: return "ok";
    case jsonrefl::state::incomplete: return "incomplete";
    case jsonrefl::state::invalid: return "invalid";
    case jsonrefl::state::unknown_key: return "unknown_key";
    case jsonrefl::state::record_end: return "record_end";
    case jsonrefl::state::no_buffer: return "no_buffer";
    case jsonrefl::state::sv_cross_chunk: return "sv_cross_chunk";
    }

    return "?";
}

void print_cursor(const char *step, const jsonrefl::cursor &cur) {
    std::cout << step
        << " status=" << state_label(cur.status())
        << " remaining=" << cur.remaining()
        << " buffer_releasable=" << (cur.buffer_releasable() ? "true" : "false")
        << '\n';
}

bool expect_cursor(
     const jsonrefl::cursor &cur
    ,jsonrefl::state st
    ,std::size_t remaining
    ,bool releasable
) {
    return cur.status() == st
        && cur.remaining() == remaining
        && cur.buffer_releasable() == releasable;
}

/*************************************************************************************************/

int sequential_records() {
    const char rec0[] = R"({"id":1,"msg":"hello"})";
    const char rec1[] = R"({"id":2,"msg":"world"})";
    const std::string buf = std::string{rec0} + '\n' + rec1;
    // Whitespace between records is skipped. remaining() starts at the next document.
    const std::size_t tail_after_first = sizeof(rec1) - 1u;

    event obj{};
    std::string accum;
    auto pp = jsonrefl::make_parser(&obj, &accum);

    std::cout << "---- two records, one buffer ----\n";
    std::cout << "buffer (" << buf.size() << " bytes): " << buf << "\n";

    std::vector<event> log;
    std::size_t first_remaining = 0;
    auto cur = pp.parse(buf.data(), buf.size());
    for ( ;; ) {
        print_cursor("  parse", cur);
        if ( cur.status() != jsonrefl::state::ok && cur.status() != jsonrefl::state::record_end ) {
            std::cerr << "unexpected status\n";

            return 1;
        }
        if ( log.empty() ) {
            first_remaining = cur.remaining();
            if ( !expect_cursor(cur, jsonrefl::state::record_end, tail_after_first, true) ) {
                std::cerr << "first cursor mismatch\n";

                return 1;
            }
        }
        std::cout << "  event id=" << obj.id << " msg=\"" << obj.msg << "\"\n";
        log.push_back(obj);
        if ( cur.status() == jsonrefl::state::ok ) {
            break;
        }
        obj = {};
        pp.parse_next(&cur);
    }

    if ( log.size() != 2u || log[0].id != 1 || log[0].msg != "hello" || log[1].id != 2 || log[1].msg != "world" ) {
        std::cerr << "record mismatch\n";

        return 1;
    }
    if ( !expect_cursor(cur, jsonrefl::state::ok, 0u, true) ) {
        std::cerr << "final cursor mismatch\n";

        return 1;
    }

    std::cout << "  first remaining=" << first_remaining << " (second record; the newline was skipped)\n\n";

    return 0;
}

int pending_key_keeps_buffer() {
    event obj{};
    std::string accum;
    auto pp = jsonrefl::make_parser(&obj, &accum);

    const char chunk0[] = R"({"id":1,"msg":)";
    const char chunk1[] = R"("tail"})";

    std::cout << "---- chunk ends on a pending key ----\n";
    auto cur = pp.parse(chunk0, sizeof(chunk0) - 1u);
    print_cursor("  chunk0", cur);
    if ( !expect_cursor(cur, jsonrefl::state::incomplete, 0u, false) ) {
        std::cerr << "expected incomplete with the feed retained\n";

        return 1;
    }

    cur = pp.parse(chunk1, sizeof(chunk1) - 1u);
    print_cursor("  chunk1", cur);
    std::cout << "  event id=" << obj.id << " msg=\"" << obj.msg << "\"\n";
    if ( !expect_cursor(cur, jsonrefl::state::ok, 0u, true) || obj.id != 1 || obj.msg != "tail" ) {
        std::cerr << "second chunk mismatch\n";

        return 1;
    }

    return 0;
}

int main() {
    if ( sequential_records() != 0 ) {
        return 1;
    }
    if ( pending_key_keeps_buffer() != 0 ) {
        return 1;
    }

    return 0;
}
