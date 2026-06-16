[![CI](https://github.com/nixman/jsonrefl/actions/workflows/ci.yml/badge.svg)](https://github.com/nixman/jsonrefl/actions/workflows/ci.yml)

# jsonrefl 1.0.0

Header-only C++14 library for compile-time reflection and JSON parsing(zero-copy)/serialization.
No external dependencies. No external code generation. Just one header.

## Table of contents

- [Features](#features)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [Supported Types](#supported-types)
  - [Portable aliases (string_view_t, optional_t)](#portable-aliases-for-string-view-and-optional)
- [Defining metadata](#defining-metadata)
  - [`JSONREFL_METADATA` — for an existing struct](#jsonrefl_metadata--for-an-existing-struct)
  - [`JSONREFL_STRUCT` — declare and register in one shot](#jsonrefl_struct--declare-and-register-in-one-shot)
  - [Nested types and containers](#nested-types-and-containers)
- [Parsing](#parsing)
  - [State codes](#state-codes)
  - [Parse flags](#parse-flags)
  - [`parse()` — single-shot](#parse--single-shot)
  - [`parse()` — streaming (chunked)](#parse--streaming-chunked)
  - [`parse_m()` — in-source single-shot (zero-copy escapes)](#parse_m--in-source-single-shot-zero-copy-escapes)
  - [`parse_m()` — chunked (under contracts C1 + C2)](#parse_m--chunked-under-contracts-c1--c2)
  - [Sequential records (`parse_next` / `parse_next_m`)](#sequential-records-parse_next--parse_next_m)
  - [Resetting the parser](#resetting-the-parser)
  - [Root containers (no wrapping struct)](#root-containers-no-wrapping-struct)
- [Serialization](#serialization)
  - [`to_string()`](#to_string)
  - [`required_bytes()` + `to_buffer()`](#required_bytes--to_buffer)
  - [`to_chunked_buffer()`](#to_chunked_buffer)
  - [Serialization flags](#serialization-flags)
  - [Field doc comments](#field-doc-comments)
  - [Root containers (no wrapping struct)](#root-containers-no-wrapping-struct-1)
- [Internals: hybrid member index](#internals-hybrid-member-index)
  - [Strategy by `N`](#strategy-by-n)
  - [Why two flavours](#why-two-flavours)
  - [Hard-mode construction](#hard-mode-construction)
  - [Inspection](#inspection)
- [License](#license)

## Features

User-facing capabilities (implementation details live in [Internals](#internals-hybrid-member-index)):

- **Header-only** — drop `include/jsonrefl/jsonrefl.hpp` into your project.
- **Registration macros** —
  - `JSONREFL_METADATA(type, members...)` registers an existing struct;
  - `JSONREFL_STRUCT(type, (type, name)...)` declares a struct and registers it in one shot;
  - `JSONREFL_METADATA_DOC` / `JSONREFL_STRUCT_DOC` are the same two macros with an extra per-field doc string (see [Field doc comments](#field-doc-comments)).
- **Rich type support** — nested structs, `std::vector`, `std::list`, `std::map`, `std::unordered_map`, `jsonrefl::optional_t`, `std::string`, `jsonrefl::string_view_t`, `bool`, integers, floats. See [Supported Types](#supported-types) and [portable aliases](#portable-aliases-for-string-view-and-optional).
- **Parsing functions** —
  - `parse()` parses any sequence of *const* chunks (or one whole document) and writes into the target object, without de-escaping;
  - `parse_m()` parses any sequence of *mutable* chunks (or one whole document) and de-escapes inside the same memory and letting `jsonrefl::string_view_t` members be true zero-copy slices of fully-decoded text;
  - `parse_next()` / `parse_next_m()` are the sequential counterparts for multi-document streams (see [Sequential records](#sequential-records-parse_next--parse_next_m)).
- **Three serialization paths** — `to_string()`, `required_bytes()` + `to_buffer()` (one allocation, exact size), and `to_chunked_buffer()` (fixed-size buffer + flush callback for sockets / constrained memory).
- **Pretty-print & doc comments** — every serialization function takes a `serialize_flags` bitmask (`pretty`, `comments`).
- **Compile-time introspection** — query struct name, member count, and member types by name.
- **Compile-time member index** — name → setter resolution is constant-cost on the parser hot path, with a clash-detection guarantee that turns name collisions into build errors.

## Installation

Single header. Either copy it in:

```bash
cp jsonrefl/include/jsonrefl/jsonrefl.hpp /your/project/include/jsonrefl/
```

```cpp
#include <jsonrefl/jsonrefl.hpp>
```

…or add the include path from CMake:

```cmake
cmake_minimum_required(VERSION 3.5)
project(myapp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(path/to/jsonrefl/include)
add_executable(myapp main.cpp)
```

## Quick Start

Define a struct, register it, parse and serialize:

```cpp
#include <jsonrefl/jsonrefl.hpp>
#include <iostream>

struct point {
    double x;
    double y;
};
JSONREFL_METADATA(point, x, y);

int main() {
    point p{};
    auto pp = jsonrefl::make_parser(&p);
    static constexpr char document[] = R"({"x":3.14,"y":2.71})";
    pp.parse(document, sizeof(document) - 1);
    // p.x == 3.14, p.y == 2.71

    std::cout << jsonrefl::to_string(p) << '\n';   // {"x":3.140000,"y":2.710000}
    std::cout << jsonrefl::to_string(p, jsonrefl::serialize_flags::pretty) << '\n';   // pretty-printed
}
```

## Supported Types

### Portable aliases for string view and optional

Prefer **`jsonrefl::string_view_t`** and **`jsonrefl::optional_t<T>`** in reflected structs and in examples so the same client code builds in both language modes supported by this header:

- **C++14** — aliases resolve to **`boost::string_view`** and **`boost::optional<T>`** (the header includes the corresponding Boost headers when `__cplusplus < 201703L`).
- **C++17 and later** — aliases resolve to **`std::string_view`** and **`std::optional<T>`**.

| C++ Type | JSON Representation |
|---|---|
| `bool` | `true` / `false` |
| `int`, `int64_t`, `size_t`, … | number |
| `double`, `float` | number |
| `std::string` | string |
| `jsonrefl::string_view_t` | string (zero-copy) |
| `jsonrefl::optional_t<T>` | value or `null` |
| `std::vector<T>` | array |
| `std::list<T>` | array |
| `std::map<K, V>` | object |
| `std::unordered_map<K, V>` | object |
| struct with `JSONREFL_METADATA` | object |
| nested combinations of the above | nested JSON |

## Defining metadata

Two macros cover both common cases.

### `JSONREFL_METADATA` — for an existing struct

Use when the struct is already defined (third-party header, generated code, your own type with a non-trivial constructor, …):

```cpp
struct config {
    std::string host;
    int port;
};
JSONREFL_METADATA(config, host, port);
```

### `JSONREFL_STRUCT` — declare and register in one shot

Use when the struct exists only as a JSON shape:

```cpp
JSONREFL_STRUCT(
    point,
    (double, x),
    (double, y)
);
```

### Nested types and containers

`JSONREFL_METADATA` composes — nested structs and standard containers Just Work:

```cpp
struct color {
    std::string name;
    int r, g, b;
};
JSONREFL_METADATA(color, name, r, g, b);

struct palette {
    jsonrefl::string_view_t title;
    std::vector<color> colors;
};
JSONREFL_METADATA(palette, title, colors);

palette p{
    "Sunset",
    {
        {"Coral",   255, 127, 80},
        {"Gold",    255, 215,  0},
        {"Crimson", 220,  20, 60},
    }
};
std::cout << jsonrefl::to_string(p, jsonrefl::serialize_flags::pretty) << '\n';
```

```json
{
   "title": "Sunset",
   "colors": [
      {"name": "Coral",   "r": 255, "g": 127, "b": 80},
      {"name": "Gold",    "r": 255, "g": 215, "b":  0},
      {"name": "Crimson", "r": 220, "g":  20, "b": 60}
   ]
}
```

## Parsing

`make_parser(&obj)` returns a `jsonrefl::parser<T>` bound to `obj`. The parser exposes two entry points: `parse()` for general use, `parse_m()` for inplace decoding escapes.

### State codes

`parse()` and `parse_m()` return a lightweight `jsonrefl::cursor` (see [Sequential records](#sequential-records-parse_next--parse_next_m)); `parse_next()` / `parse_next_m()` return `jsonrefl::state` directly. A `cursor` exposes the result through `cur.status()` and the byte count left through `cur.remaining()` — compare via `cur.status() == jsonrefl::state::ok`. The states are:

| Value | Meaning |
|---|---|
| `ok` | Document is complete and the target object is fully populated; no trailing non-whitespace bytes remain |
| `incomplete` | More bytes needed — feed the next chunk |
| `invalid` | JSON is malformed or doesn't match the schema |
| `unknown_key` | An object key has no corresponding struct field. Returned only when `flags::skip_unknown_keys` is **not** set; enable that flag to skip such keys and continue instead. |
| `record_end` | A complete document was parsed and more non-whitespace bytes follow it. `cursor::remaining()` reports how many input bytes are left, starting at the next document. Pass the cursor to `parse_next()` / `parse_next_m()` to read the next one. |
| `no_buffer` | An `accum` scratch buffer was needed (escape decoding or cross-chunk string) but the **`parser`** was constructed **without `accum`** in `make_parser` / ctor. **Never returned by `parse_m()`.** |
| `sv_cross_chunk` | A `jsonrefl::string_view_t`-typed value or key cannot live in the chosen feed shape. From `parse()` chunked: the value/key was split across two chunks (use `std::string`, or feed the whole document at once). From `parse_m()` chunked: contract C2 was violated by the producer. |

### Parse flags

Flags are fixed when the parser is created — pass them as the trailing argument to `make_parser` (default `flags::none`). They apply to every `parse()` / `parse_m()` / `parse_next()` call on that parser. Flags are a bitmask enum — combine them with `|`:

```cpp
using F = jsonrefl::flags;
auto pp = jsonrefl::make_parser(&obj, &accum, F::allow_comments | F::allow_infinity_and_nan);
pp.parse(doc, len);
```

| Flag | Effect |
|---|---|
| `flags::none` | Strict RFC 8259 parsing (default) |
| `flags::skip_unknown_keys` | Silently skip JSON keys that have no corresponding struct field, instead of returning `state::unknown_key` |
| `flags::allow_comments` | Allow C++ single-line comments (`// …`) anywhere whitespace is valid in JSON |
| `flags::allow_infinity_and_nan` | Accept the non-standard literals `Infinity`, `-Infinity`, and `NaN` as floating-point values |
| `flags::allow_invalid_utf16` | Accept lone surrogate `\uXXXX` escapes (e.g. `\uD83D` not followed by a low surrogate) — decoded as WTF-8 three-byte sequences instead of returning `state::invalid` |
| `flags::allow_invalid_utf8` | Pass raw invalid UTF-8 byte sequences inside string values through to the target field without validation; by default, any ill-formed UTF-8 byte causes `state::invalid` |
| `flags::allow_invalid_escapes` | Pass unrecognised `\X` escape sequences through verbatim (`\` + `X`) instead of returning `state::invalid`; the eight RFC-defined escapes and `\uXXXX` are still decoded normally |

#### `allow_comments`

```cpp
static constexpr char doc[] =
    "{\n"
    "  // host to connect to\n"
    "  \"host\": \"localhost\",\n"
    "  \"port\": 8080\n"
    "}";
config cfg{};
auto pp = jsonrefl::make_parser(&cfg, nullptr, jsonrefl::flags::allow_comments);
pp.parse(doc, sizeof(doc) - 1);
```

Only single-line (`//`) comments are supported. Block comments (`/* … */`) are not.

#### `allow_infinity_and_nan`

```cpp
JSONREFL_STRUCT(reading, (double, temp), (double, ratio));

static constexpr char doc[] = R"({"temp":NaN,"ratio":Infinity})";
reading r{};
auto pp = jsonrefl::make_parser(&r, nullptr, jsonrefl::flags::allow_infinity_and_nan);
pp.parse(doc, sizeof(doc) - 1);
// std::isnan(r.temp) == true
// std::isinf(r.ratio) && r.ratio > 0
```

The three accepted literals are `NaN`, `Infinity`, and `-Infinity` (case-sensitive). Without the flag, encountering any of them produces `state::invalid`.

**Serialization note:** The serializers (`to_string`, `to_buffer`, `to_chunked_buffer`) always emit standard JSON — they output `null` for NaN and ±Infinity rather than the non-standard literals.

#### `allow_invalid_utf16`

```cpp
static constexpr char doc[] = R"({"tag":"\uD83D"})"; // lone high surrogate
struct msg { std::string tag; };
JSONREFL_METADATA(msg, tag);

msg m{};
// without the flag -> state::invalid
auto pp = jsonrefl::make_parser(&m, nullptr, jsonrefl::flags::allow_invalid_utf16);
pp.parse(doc, sizeof(doc) - 1);
// m.tag contains the WTF-8 encoding of U+D83D: "\xED\xA0\xBD"
```

Lone high surrogates (`\uD800`–`\uDBFF`) and lone low surrogates (`\uDC00`–`\uDFFF`) are each encoded as a three-byte WTF-8 sequence instead of producing an error.

#### `allow_invalid_escapes`

```cpp
struct msg { std::string text; std::string tag; };
JSONREFL_METADATA(msg, text, tag);

char inp[] = R"({"text":"hello\xworld","tag":"\q"})";
msg m{};
// without the flag -> state::invalid  (\x and \q are not valid JSON escapes)
auto pp = jsonrefl::make_parser(&m, nullptr, jsonrefl::flags::allow_invalid_escapes);
pp.parse_m(inp, sizeof(inp) - 1);
// m.text == "hello\\xworld"
// m.tag  == "\\q"
```

The eight RFC-defined escapes (`\"`, `\\`, `\/`, `\b`, `\f`, `\n`, `\r`, `\t`) and Unicode escapes (`\uXXXX`) continue to be decoded normally. Only escapes whose second character is not one of those nine are passed through as the two-byte sequence `\` + `<char>`.

#### `allow_invalid_utf8`

```cpp
static constexpr char doc[] = "{\x22v\x22:\x22\xC0\xAF\x22}"; // overlong sequence
struct row { std::string v; };
JSONREFL_METADATA(row, v);

row r{};
// without the flag -> state::invalid
auto pp = jsonrefl::make_parser(&r, nullptr, jsonrefl::flags::allow_invalid_utf8);
pp.parse(doc, sizeof(doc) - 1);
// r.v contains the raw bytes as-is
```

Without this flag the parser validates each byte of a string value and returns `state::invalid` on any ill-formed UTF-8 (invalid lead byte, bad continuation, overlong encoding, truncated sequence).

### `parse()` — single-shot

`cursor parse(const char *ptr, std::size_t size)` — there is **no overload** that takes `string_view`/`std::string` directly; use `ptr` and `size`. If your span is already a **`jsonrefl::string_view_t` `sv`**, forward it as **`parse(sv.data(), sv.size())`**. No **`accum`** is needed unless the document contains strings with escapes that must be decoded into **`std::string`** members (`accum` is bound in **`make_parser`**, see [chunked `parse()`](#parse--streaming-chunked)). Parse flags, if any, are bound in `make_parser` too.

```cpp
config cfg{};
auto pp = jsonrefl::make_parser(&cfg);

static constexpr char document[] = R"({"host":"localhost","port":8080})";
auto st = pp.parse(document, sizeof(document) - 1);
// st.status() == jsonrefl::state::ok
// cfg.host == "localhost", cfg.port == 8080
```

For `jsonrefl::string_view_t` members the result points directly into the input buffer — the buffer must outlive the target object.

### `parse()` — streaming (chunked)

Feed chunks one at a time. Bind an **`accum`** scratch buffer with **`make_parser(&cfg, &accum)`** — the parser uses it to hold partial leaves that span chunk boundaries and to decode escape sequences into **`std::string`** members. **`parse()`** itself always takes **`(ptr, size)`** only:

```cpp
config cfg{};
std::string accum;
auto pp = jsonrefl::make_parser(&cfg, &accum);

static constexpr char part0[] = R"({"host":"local)";
static constexpr char part1[] = R"(host","port":8080})";
pp.parse(part0, sizeof(part0) - 1);  // -> incomplete
pp.parse(part1, sizeof(part1) - 1);  // -> ok
```

Realistic loop with `recv()`:

```cpp
config cfg{};
std::string accum;
auto pp = jsonrefl::make_parser(&cfg, &accum);
std::array<char, 4096> buf{};

for (;;) {
    const auto n = ::recv(fd, buf.data(), buf.size(), 0);
    if (n <= 0) { break; }

    const auto st = pp.parse(buf.data(), static_cast<std::size_t>(n)).status();
    if (st == jsonrefl::state::ok)         { /* cfg fully parsed */         break; }
    if (st != jsonrefl::state::incomplete) { /* invalid / record_end / … */ break; }
}
```

Notes on chunked `parse()`:

- **No `accum` in `make_parser`** ⇒ as soon as the parser would need to buffer cross-chunk bytes or decode escapes, it returns `state::no_buffer`. `accum` may stay empty between calls — the parser only writes into it when needed.
- **`jsonrefl::string_view_t` members under chunked feed** — if the value or key happens to span a chunk boundary, the parser cannot represent it as a contiguous slice of input. It returns `state::sv_cross_chunk` immediately on the chunk where the split is detected (no need to feed the next one to learn this). Use `std::string`, or pass the whole document in one `parse()` call, or use `parse_m()` on a mutable buffer.

### `parse_m()` — in-source single-shot (zero-copy escapes)

`parse_m(char *ptr, std::size_t size)` parses a *mutable* buffer in place. Escape sequences (`\n`, `\t`, `\"`, `\uXXXX`, surrogate pairs) are decoded **directly into the same buffer**, shrinking each string. `jsonrefl::string_view_t` members then point at fully-decoded slices — no `std::string` fallback, no `accum`, `state::no_buffer` is never returned:

```cpp
char buf[] = R"({"id":"42","msg":"hello\nworld","tag":"\uD83D\uDE00"})";

struct evt {
    jsonrefl::string_view_t id;
    jsonrefl::string_view_t msg;
    jsonrefl::string_view_t tag;
};
JSONREFL_METADATA(evt, id, msg, tag);

evt e{};
auto pp = jsonrefl::make_parser(&e);
auto st = pp.parse_m(buf, sizeof(buf) - 1);
// st.status() == jsonrefl::state::ok
// e.id  -> "42"               (slice of buf)
// e.msg -> "hello\nworld"     (decoded \n, slice of buf)
// e.tag -> "\xF0\x9F\x98\x80" (decoded surrogate pair, slice of buf)
// buf must outlive e
```

`parse_m()` single-shot return states: `ok`, `incomplete` (buffer was shorter than the document), `invalid`, `record_end` (trailing bytes after a complete document). `no_buffer` is never returned.

### `parse_m()` — chunked (under contracts C1 + C2)

`parse_m()` may also be called repeatedly with **different** mutable buffers, streaming a document through the parser without ever copying. Two contracts must hold:

- **C1 — buffer lifetime.** Every buffer fed to `parse_m()` must outlive any `jsonrefl::string_view_t`-typed field that ended up pointing into it (typically: lifetime of the target object). Buffers are independent regions of memory — you can `recv()` each chunk into a fresh `std::vector<char>` and store them.
- **C2 — atomic leaves.** Every leaf JSON value (`string`, `number`, `true`, `false`, `null`) and every key, *together with the chain of bytes that terminates it*, lies entirely inside one buffer. The terminating chain is whatever bytes immediately follow the leaf in serialised form: a single `,` between siblings, or one or more closing `}`/`]` (when the leaf is the last item of one or more nested containers collapsing at once), optionally followed by a trailing `,`. Structural pieces in the middle of containers and whitespace between tokens may fall on any byte boundary.

When both contracts hold, `parse_m()` returns `incomplete` after each non-final chunk and `ok` on the last one. `jsonrefl::string_view_t` members from earlier chunks remain valid because their backing buffers stay alive (C1).

`to_chunked_buffer()` automatically satisfies C2 in **compact mode** (`serialize_flags::none`) when both:

1. `buf_size` is large enough to hold the longest leaf together with its longest terminating chain (a few bytes for typical schemas; deep collapsing nesting adds one byte per level), **and**
2. that terminating chain is no longer than 16 bytes (`tail_buf[16]` is an internal staging buffer; ~15+ levels collapsing at the same point fall back to separate writes — pathologically deep nesting only).

Internally the writer collects each leaf and its trailing chain into a single span before deciding to flush, so a chunk boundary cannot split `<leaf><chain>` while both conditions above hold. If the leaf alone is larger than `buf_size`, the writer falls back to multiple `write()` calls and atomicity is sacrificed (long-leaf fallback). **Pretty mode does not provide the C2 guarantee at all** — its multi-byte separators (`,\n<indent>`) are emitted as three separate writes; if you need chunked `parse_m()` round-tripping, serialise compact.

The writer/reader pair on this library therefore round-trips trivially:

```cpp
std::vector<std::vector<char>> chunks;
char tmp[64];                          // > max leaf + framing -> C2 holds
jsonrefl::to_chunked_buffer(tmp, sizeof(tmp), src,
    [&](const void *data, std::size_t n) -> bool {
        chunks.emplace_back(static_cast<const char*>(data),
                            static_cast<const char*>(data) + n);
        return true;
    }
);

std::vector<kv> sink;
auto pp = jsonrefl::make_parser(&sink);
jsonrefl::state st = jsonrefl::state::incomplete;
for ( auto &c : chunks ) {
    st = pp.parse_m(c.data(), c.size()).status();  // each chunk is its own buffer
    // st == incomplete except for the last chunk where st == ok
}
```

If C2 is violated (a leaf value or key is split across two buffers), `parse_m()` does **not** silently corrupt subsequent chunks — it returns `state::sv_cross_chunk` immediately on the chunk where the split is detected, so you can see the protocol error and `reset()`.

`parse_m()` chunked return states: `ok`, `incomplete`, `invalid`, `record_end`, `sv_cross_chunk`. `no_buffer` is never returned.

### Sequential records (`parse_next` / `parse_next_m`)

Some protocols deliver multiple JSON documents concatenated in the same buffer or stream — NDJSON, JSON-seq, log pipelines, socket frames, etc. Each record must be an object or an array (see **Record types** below):

```
{"id":1,"msg":"hello"}{"id":2,"msg":"world"}
[1,2,3][4,5,6]
```

The flow is: call `parse()` / `parse_m()` once to read the first document, then keep calling `parse_next()` / `parse_next_m()` with the **same `jsonrefl::cursor`** until the cursor reports `state::ok` (or an error). The cursor carries the position of the next document, so you never track pointers yourself, and `parse_next*` resets the parser internally before each record (no manual `reset()`):

- `parse()` / `parse_m()` return a `cursor`. `cursor::status()` is `state::record_end` when more documents follow, or `state::ok` when this was the last one.
- `parse_next(cursor*)` and `parse_next_m(cursor*)` take the cursor **by pointer**, advance it in place, and return the new `state`. `parse_next_m` is the in-source (zero-copy) counterpart; it parses inside the original mutable buffer.
- `cursor::status()` may also be `state::incomplete` / `state::invalid`, with the same semantics as `parse()`.

`parse_next_m` requires that the buffer originally passed to `parse_m` was a writable `char` buffer (it decodes escapes in place).

**NDJSON loop:**

```cpp
struct event { int id; std::string msg; };
JSONREFL_METADATA(event, id, msg);

std::vector<event> log;
event obj{};
std::string accum;
auto pp = jsonrefl::make_parser(&obj, &accum);

auto cur = pp.parse(ndjson_buf, ndjson_len);
for ( ;; ) {
    if ( cur.status() != jsonrefl::state::ok && cur.status() != jsonrefl::state::record_end ) {
        /* incomplete / invalid */
        break;
    }
    log.push_back(obj);                 // consume obj before the next record
    if ( cur.status() == jsonrefl::state::ok ) { break; }
    obj = {};
    pp.parse_next(&cur);                // resets the parser and accum internally
}
```

`parse_next` clears the bound `accum`, so any `jsonrefl::string_view_t` members of `obj` must be consumed (or copied out) **before** the next `parse_next` call.

**In-source (zero-copy) variant:**

```cpp
char buf[] = R"({"id":1,"msg":"hi"}{"id":2,"msg":"bye"})";
event obj{};
auto pp = jsonrefl::make_parser(&obj);

auto cur = pp.parse_m(buf, sizeof(buf) - 1);
for ( ;; ) {
    if ( cur.status() != jsonrefl::state::ok && cur.status() != jsonrefl::state::record_end ) { break; }
    process(obj);
    if ( cur.status() == jsonrefl::state::ok ) { break; }
    obj = {};
    pp.parse_next_m(&cur);
}
```

**Record types.** Sequential parsing is restricted to **objects** and **arrays** — every record must be a JSON object (`{...}`) or array (`[...]`). This falls out of the parser root constraint: the root type is always an object (a struct registered with `JSONREFL_METADATA`, or `std::map` / `std::unordered_map`) or an array (`std::vector` / `std::list`); a bare primitive cannot be a parser root and will not compile. Primitive values (string, number, `true`/`false`/`null`) are therefore supported **only as leaves inside** a record (e.g. `std::vector<int>` → `[1,2,3]`), never as standalone top-level records.

| Record type | Adjacent example | Notes |
|---|---|---|
| object | `{...}{...}` | `}` ends the first record, `{` starts the next |
| array | `[...][...]` | `]` ends the first record, `[` starts the next |

Because objects and arrays are self-delimited by their braces/brackets, no separator is required between adjacent records; optional whitespace (and comments, with `flags::allow_comments`) is allowed: `{...}\n{...}`, `[...] [...]`. A single parser carries one fixed root type, so a stream is homogeneous — all objects or all arrays. To switch record type mid-stream, hand the same `cursor` to a second parser of the other type.

### Resetting the parser

After any terminal state (`invalid`, `sv_cross_chunk`) — or when you want to reuse a parser for a fresh document — call `reset()` (the sequential `parse_next*` calls do this for you):

```cpp
auto pp = jsonrefl::make_parser(&obj);
static constexpr char bad_json[] = R"(not json)";
static constexpr char good_json[] = R"({"i":1,"s":"x"})";  // illustration; match your `T`
auto st = pp.parse(bad_json, sizeof(bad_json) - 1);
// st.status() == jsonrefl::state::invalid

pp.reset();
obj = {};

st = pp.parse(good_json, sizeof(good_json) - 1);
// st.status() == jsonrefl::state::ok
```

### Root containers (no wrapping struct)

Standard containers can be parsed at the document root without a wrapping struct:

```cpp
std::vector<int> nums;
static constexpr char nums_doc[] = "[10, 20, 30]";
jsonrefl::make_parser(&nums).parse(nums_doc, sizeof(nums_doc) - 1);
// nums == {10, 20, 30}

std::map<std::string, std::string> kv;
static constexpr char kv_doc[] = R"({"key":"value"})";
jsonrefl::make_parser(&kv).parse(kv_doc, sizeof(kv_doc) - 1);
// kv == {{"key", "value"}}
```

## Serialization

Three entry points, in order of escalation: convenient → exact-size → streaming.

### `to_string()`

The convenient path. Returns `std::string` with JSON. Pass `serialize_flags::pretty` for indented output:

```cpp
auto json   = jsonrefl::to_string(obj);                                  // compact
auto pretty = jsonrefl::to_string(obj, jsonrefl::serialize_flags::pretty); // indented
```

### `required_bytes()` + `to_buffer()`

Zero-allocation path — compute the exact size, then write into your own buffer in one shot:

```cpp
const auto n = jsonrefl::required_bytes(obj);
std::string buf;
buf.resize(n);
char *end = jsonrefl::to_buffer(buf.data(), obj);
// end - buf.data() == n, guaranteed
```

### `to_chunked_buffer()`

Streaming serialization into a fixed-size buffer with a flush callback. Ideal for sockets and constrained memory:

```cpp
char chunk[1472];   // e.g. UDP MTU
jsonrefl::to_chunked_buffer(chunk, sizeof(chunk), obj,
    [](const void *data, std::size_t size) -> bool {
        ::send(fd, data, size, 0);   // write chunk
        return true;                 // return false to abort
    }
);
```

In **compact mode** this writer is the C2-compatible producer for chunked `parse_m()` — see [`parse_m()` chunked](#parse_m--chunked-under-contracts-c1--c2) for the exact conditions.

### Serialization flags

All four serializers accept a trailing `serialize_flags` argument (default `serialize_flags::none`).
The flags are a bitmask — combine them with `|`:

| Flag | Effect |
|---|---|
| `serialize_flags::none` | Compact, single-line output (default). |
| `serialize_flags::pretty` | Indented, multi-line output. |
| `serialize_flags::comments` | Emit field doc comments (see below). Only takes effect together with `pretty`. |

```cpp
using SF = jsonrefl::serialize_flags;
jsonrefl::to_string         (obj, SF::pretty);
jsonrefl::required_bytes    (obj, SF::pretty);
jsonrefl::to_buffer    (buf, obj, SF::pretty);
jsonrefl::to_chunked_buffer (chunk, sizeof(chunk), obj, cb, SF::pretty);

// pretty + doc comments
jsonrefl::to_string(obj, SF::pretty | SF::comments);
```

### Field doc comments

Fields can carry a documentation string that is emitted as a `//` line comment above the
field when serializing with `serialize_flags::pretty | serialize_flags::comments`. Declare the
doc text with the `_DOC` variants of the metadata macros, where each field is a parenthesized
tuple ending with a string literal:

```cpp
// existing struct: JSONREFL_METADATA_DOC(type, (field, "doc"), ...)
struct config {
    int  port;
    bool tls;
};
JSONREFL_METADATA_DOC(
     config
    ,(port, "listening port")
    ,(tls,  "enable TLS")
);

// declare + register in one shot: JSONREFL_STRUCT_DOC(type, (T, field, "doc"), ...)
JSONREFL_STRUCT_DOC(
     config2
    ,(int,  port, "listening port")
    ,(bool, tls,  "enable TLS")
);

config c{8080, true};
std::cout << jsonrefl::to_string(c, SF::pretty | SF::comments);
```

```json
{
   // listening port
   "port": 8080,
   // enable TLS
   "tls": true
}
```

Notes:

- Comments are only emitted in pretty mode; `comments` without `pretty` is ignored and the
  output stays compact (line comments cannot survive single-line JSON).
- A field with an empty doc string emits no comment line.
- The emitted document is **not** standard JSON. Parse it back with
  [`flags::allow_comments`](#allow_comments).

### Root containers (no wrapping struct)

Standard containers can be serialized at the document root, without a wrapping struct:

```cpp
std::vector<int>            v = {1, 2, 3};
std::map<std::string, int>  m = {{"a", 1}, {"b", 2}};

jsonrefl::to_string(v);   // [1,2,3]
jsonrefl::to_string(m);   // {"a":1,"b":2}
```

## Internals: hybrid member index

> *This section is implementation detail — useful for performance-conscious users and for understanding error messages, but not required to use the library.*

Resolving an incoming key (e.g. `"preventedMatchId"`) to the right setter happens on the parser's hot path, so `jsonrefl` invests in keeping that lookup branchless and cache-friendly. The mechanism is selected at compile time per struct, based on the number of `JSONREFL_METADATA(...)` fields `N`.

### Strategy by `N`

| `N` | Strategy | Storage | Lookup |
|---|---|---|---|
| `0 … 16` | `linear_index<N>` (true MPHF) | two parallel `std::array<…, N>` of `(hash, setter*)` | `for (i: 0..N) if (hashes[i] == h) return setters[i];` — auto-vectorises into `vpcmpeqd` + `vpmovmskb` + `tzcnt` |
| `17 … ∞` | `phf_index<N, M>` (sparse PHF) | `M = next_pow2(N*4)` slots of `(hash, setter*)` | `slot = phf_slot<M>(h, mult, seed); return used[slot] && table[slot].hash == h ? setter : nullptr;` |

Both paths key off the same compile-time FNV-1a hash and resolve to the same `setter_base*`, so the choice is purely about latency.

### Why two flavours

**`N ≤ 16` — linear scan wins on real CPUs.** A packed pair-of-arrays beats a sparse table for three reasons:

- **No modular index.** Slot `i` is literally the declaration position of the `i`-th field, so the index is a *minimal* perfect hash (`get_index(name) == i`). No `& (M - 1)`, no probe sequence, no indirection through `used[]`.
- **SIMD-friendly memory layout.** All hashes sit in one or two contiguous SIMD registers; the compiler reliably turns the loop into a vectorised compare:
  - AVX2: 8 `uint32_t` per `ymm` ⇒ ≤ 2 vector compares for `N ≤ 16`.
  - AVX-512: 16 `uint32_t` per `zmm` ⇒ 1 vector compare for `N ≤ 16`.
- **Branch-free reduction.** `vpmovmskb` + `tzcnt` recover the matching slot index without a misprediction-prone scalar loop.

Empirically this beats the sparse PHF up to roughly `N = 17` on AVX2 and `N = 32` on AVX-512. The threshold is set at **16** so the same binary stays optimal on AVX2 hardware without sacrificing AVX-512 systems.

**`N > 16` — sparse PHF wins on cache and on instructions.** Once the hash array no longer fits in one or two vector registers, the linear scan starts costing extra cache lines and extra compares. The sparse PHF instead does:

```
slot = ((hash * mult) >> shift) ^ seed) & (M - 1);
return used[slot] && table[slot].hash == hash ? table[slot].setter : nullptr;
```

Two multiplications/shifts and one masked load — independent of `N`. The cost is memory: `M = next_pow2(N*4)` slots (e.g. `N=20` ⇒ `M=128`).

### Hard-mode construction

Both flavours treat a name collision as a fatal construction error rather than a silent wrong-setter return:

- `linear_index<N>` — after copying `(hash, setter*)` pairs, the constructor runs an `O(N²)` pairwise check. Two equal hashes call `linear_index_collision_detected()`.
- `phf_index<N, M>` — the constructor brute-force-searches `(mult, seed)` pairs looking for an injective mapping into `M` slots. If the search space is exhausted, it calls `phf_perfect_search_failed()`.

Both `*_failed()` / `*_detected()` functions are intentionally **non-`constexpr`**. The effect depends on how the holder is instantiated:

| Instantiation site | What happens on collision |
|---|---|
| `JSONREFL_METADATA(...)` (expands to namespace-scope `inline constexpr auto __jsonrefl_meta_T = …`) — i.e. the standard usage | **Compile error** — calling a non-`constexpr` function from a constant-initialiser is ill-formed. The compiler points at the offending `__jsonrefl_meta_T` definition. |
| Runtime-built holder (e.g. `static const auto h = jsonrefl::object_holder(...)` from a factory) | **`std::abort()` at first construction** — a one-line message goes to `stderr` and the process dies. Acts as a backstop for code paths the compiler can't constant-fold. |

The result is the same in either path: a name collision can't slip through into runtime behaviour.

### Inspection

The chosen strategy is exposed for tests and tooling:

```cpp
const auto &meta = jsonrefl::metadata<my_struct>();
using meta_t = std::remove_reference_t<decltype(meta)>;

if constexpr (meta_t::uses_minimal_index()) {
    // small struct => linear_index<N>: meta.index().hashes[i], meta.index().setters[i]
} else {
    // large struct => phf_index<N, M>: meta.index().table[slot], meta.index().used[slot]
}
```

`meta.dump(std::ostream&)` walks both flavours and prints the populated entries.

## License

Apache License 2.0 — see [LICENSE](LICENSE) for details.
