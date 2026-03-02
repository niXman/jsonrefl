# jsonrefl

Header-only C++17 library for compile-time reflection and JSON serialization/deserialization.
No external dependencies. No external code generation. Just one header.

## Key Features

- **Header-only** — single file `jsonrefl.hpp`, drop it into your project and go
- **Two macros, zero boilerplate**:
  - `JSONREFL_METADATA(type, members...)` — generate metadata for an existing struct
  - `JSONREFL_STRUCT(type, (type, name)...)` — declare a struct and generate metadata in one shot
- **Compile-time sorted hash index** — FNV-1a hash + binary search for O(log N) member lookup, no string comparisons at runtime
- **Just one-allocation serialization** — `required_bytes()` computes exact size, `to_buffer()` writes directly into a pre-allocated buffer
- **Streaming serialization** — `to_chunked_buffer()` writes into a fixed-size buffer with a flush callback, ideal for sockets and constrained memory
- **Zero-copy deserialization** — `std::string_view` members point directly into the input buffer, no string copying
- **Streaming deserialization** — `make_parser()` + `parse()` accepts data chunk by chunk as it arrives, returning `state` (`ok`, `incomplete`, `invalid`, `extra_data`, `no_buffer`)
- **Pretty-print** — all serialization functions accept a `pretty` flag for indented output
- **Rich type support** — nested structs, `std::vector`, `std::list`, `std::map`, `std::unordered_map`, `std::optional`, `std::string`, `std::string_view`, `bool`, integers, floats
- **Runtime introspection** — query struct name, member count, member types by name

## Quick Start

### Using `JSONREFL_METADATA`

Define your struct as usual, then register it:

```cpp
#include <jsonrefl/jsonrefl.hpp>
#include <iostream>

struct point {
    double x;
    double y;
};
JSONREFL_METADATA(point, x, y);

int main() {
    point p{3.14, 2.71};
    std::cout << jsonrefl::to_string(p) << std::endl;
    // {"x":3.140000,"y":2.710000}

    std::cout << jsonrefl::to_string(p, true) << std::endl;
    // {
    //    "x": 3.140000,
    //    "y": 2.710000
    // }
}
```

### Using `JSONREFL_STRUCT`

Declare the struct and metadata in a single macro:

```cpp
#include <jsonrefl/jsonrefl.hpp>
#include <iostream>

JSONREFL_STRUCT(point,
    (double, x),
    (double, y)
);

int main() {
    point p{3.14, 2.71};
    std::cout << jsonrefl::to_string(p) << std::endl;
    // {"x":3.140000,"y":2.710000}
}
```

### Nested Structs and Containers

```cpp
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

int main() {
    palette p;
    p.title = "Sunset";
    p.colors = {
        {"Coral",   255, 127, 80},
        {"Gold",    255, 215,  0},
        {"Crimson", 220,  20, 60},
    };
    std::cout << jsonrefl::to_string(p, true) << std::endl;
}
```

Output:

```json
{
   "title": "Sunset",
   "colors": [
      {
         "name": "Coral",
         "r": 255,
         "g": 127,
         "b": 80
      },
      {
         "name": "Gold",
         "r": 255,
         "g": 215,
         "b": 0
      },
      {
         "name": "Crimson",
         "r": 220,
         "g": 20,
         "b": 60
      }
   ]
}
```

## Serialization API

### `to_string`

Returns `std::string` with JSON. Pass `true` for pretty-printed output:

```cpp
auto json    = jsonrefl::to_string(obj);        // compact
auto pretty  = jsonrefl::to_string(obj, true);  // indented
```

### `required_bytes` + `to_buffer`

Zero-allocation path — compute exact size, then write into your own buffer:

```cpp
auto n = jsonrefl::required_bytes(obj);
auto buf = std::make_unique<char[]>(n);
char *end = jsonrefl::to_buffer(buf.get(), obj);
// end - buf.get() == n, guaranteed
```

### `to_chunked_buffer`

Streaming serialization into a fixed-size buffer with a flush callback. The callback is invoked each time the buffer fills up:

```cpp
char chunk[1472]; // for UDP streaming
jsonrefl::to_chunked_buffer(chunk, sizeof(chunk), obj,
    [](const void *data, std::size_t size) -> bool {
        send(fd, data, size, 0);  // write chunk to socket
        return true;              // return false to abort
    }
);
```

### Root Containers

Standard containers can be serialized directly, without a struct wrapper:

```cpp
std::vector<int> v = {1, 2, 3};
jsonrefl::to_string(v);  // [1,2,3]

std::map<std::string, int> m = {{"a", 1}, {"b", 2}};
jsonrefl::to_string(m);  // {"a":1,"b":2}
```

## Deserialization API

### Basic Parsing

`parse()` returns `jsonrefl::state`:

| Value | Meaning |
|---|---|
| `ok` | Document is complete |
| `incomplete` | More data needed (chunked input) |
| `invalid` | JSON is malformed or cannot be deserialized |
| `extra_data` | Non-whitespace data after a complete document |
| `no_buffer` | Accumulation needed but no buffer provided |

```cpp
struct config {
    std::string host;
    int port;
};
JSONREFL_METADATA(config, host, port);

config cfg{};
auto p = jsonrefl::make_parser(&cfg);

auto state = p.parse(R"({"host":"localhost","port":8080})");
// state == jsonrefl::state::ok
// cfg.host == "localhost", cfg.port == 8080
```

### Streaming (Chunked) Parsing

Feed data chunk by chunk as it arrives from the network. When a string or number value may be split across chunks, pass a `std::string*` accumulation buffer:

```cpp
config cfg{};
std::string accum;
auto p = jsonrefl::make_parser(&cfg);

auto s1 = p.parse(R"({"host":"local)", &accum);
// s1 == jsonrefl::state::incomplete

auto s2 = p.parse(R"(host","port":8080})", &accum);
// s2 == jsonrefl::state::ok
```

If `accum` is `nullptr` (default) and the parser needs to accumulate across chunk boundaries, `parse()` returns `state::no_buffer`.

### Parser Reset

After an error, `reset()` restores the parser to its initial state for reuse:

```cpp
auto p = jsonrefl::make_parser(&obj);
auto state = p.parse(bad_json);
// state == jsonrefl::state::invalid

p.reset();          // back to initial state
obj = {};           // clear the target object

state = p.parse(good_json);
// state == jsonrefl::state::ok
```

### Root Container Parsing

Standard containers can be parsed directly:

```cpp
std::vector<int> nums;
auto p = jsonrefl::make_parser(&nums);
p.parse("[10, 20, 30]");
// nums == {10, 20, 30}

std::map<std::string, std::string> kv;
auto p2 = jsonrefl::make_parser(&kv);
p2.parse(R"({"key":"value"})");
// kv == {{"key", "value"}}
```

## Zero-Copy Deserialization

Members of type `std::string_view` point directly into the input buffer — no string allocation or copying:

```cpp
struct message {
    std::string_view sender;
    std::string_view text;
};
JSONREFL_METADATA(message, sender, text);

const char *json = R"({"sender":"alice","text":"hello"})";

message msg{};
auto p = jsonrefl::make_parser(&msg);
p.parse(json);

// msg.sender and msg.text point into json buffer — zero copy
// json buffer must outlive msg
```

### Limitations

- **Buffer lifetime** — the input buffer must outlive all `std::string_view` members, since they point directly into it.
- **Chunked parsing** — if a `std::string_view` member's value is split across two chunks, the parser returns `state::invalid`, because there is no contiguous buffer to point into. Use `std::string` for members that may be split across chunk boundaries.
- **Escape sequences** — `std::string_view` members receive the raw (unescaped) slice from the buffer. Use `std::string` if you need decoded escape sequences (`\n`, `\uXXXX`, etc.).

## Supported Types

| C++ Type | JSON Representation |
|---|---|
| `bool` | `true` / `false` |
| `int`, `int64_t`, `size_t`, ... | number |
| `double`, `float` | number |
| `std::string` | string |
| `std::string_view` | string (zero-copy) |
| `std::optional<T>` | value or `null` |
| `std::vector<T>` | array |
| `std::list<T>` | array |
| `std::map<K, V>` | object |
| `std::unordered_map<K, V>` | object |
| struct with `JSONREFL_METADATA` | object |
| nested combinations of the above | nested JSON |

## Integration

jsonrefl is a single header. Copy `include/jsonrefl/jsonrefl.hpp` into your project, or add the include path.

### CMake

```cmake
cmake_minimum_required(VERSION 3.5)
project(myapp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(path/to/jsonrefl/include)

add_executable(myapp main.cpp)
```

### Direct Include

```bash
cp jsonrefl/include/jsonrefl/jsonrefl.hpp /your/project/include/jsonrefl/
```

```cpp
#include <jsonrefl/jsonrefl.hpp>
```

## License

Apache License 2.0 — see [LICENSE](LICENSE) for details.
