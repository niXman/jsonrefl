
#pragma once

#include <jsonrefl/jsonrefl.hpp>

#include <cstddef>
#include <iterator>
#include <list>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace chunked_array_feed {

/*************************************************************************************************/

template<typename T>
struct is_sequence_container : std::false_type {};

template<typename T, typename A>
struct is_sequence_container<std::vector<T, A>> : std::true_type {};

template<typename T, typename A>
struct is_sequence_container<std::list<T, A>> : std::true_type {};

template<typename Container>
inline constexpr bool is_sequence_container_v = is_sequence_container<Container>::value;

/*************************************************************************************************/

template<typename T>
struct needs_parse_accum : std::false_type {};

template<>
struct needs_parse_accum<std::string> : std::true_type {};

template<>
struct needs_parse_accum<jsonrefl::value_t> : std::true_type {};

template<typename T>
inline constexpr bool needs_parse_accum_v = needs_parse_accum<T>::value;

/*************************************************************************************************/

template<typename Container>
class parser_holder {
public:
    using container_type = Container;
    using element_type = typename Container::value_type;
    using parser_type = jsonrefl::parser<Container>;

    explicit parser_holder(Container *window, jsonrefl::flags fl = jsonrefl::flags::none)
        :m_window{window}
        ,m_parser{make_parser(window, accum_ptr(), fl)}
    {}

    parser_type &parser() noexcept { return m_parser; }
    const parser_type &parser() const noexcept { return m_parser; }

private:
    Container *m_window;
    std::string m_accum;
    parser_type m_parser;

    std::string *accum_ptr()
    { return needs_parse_accum_v<element_type> ? &m_accum : nullptr; }

    template<typename C>
    static parser_type make_parser(C *window, std::string *accum, jsonrefl::flags fl)
    { return jsonrefl::make_parser(window, accum, fl); }
};

/*************************************************************************************************/
// Feed one JSON chunk into a root array parser.
// `window` keeps only elements fully parsed in the *latest* chunk after the call.
// `on_batch(first, last)` fires for elements newly parsed in this chunk (before trim).
// Returns count of elements in `on_batch` (0 if none).

template<typename Container, typename OnBatch>
std::size_t feed_array_chunk(
     jsonrefl::parser<Container> &pp
    ,Container &window
    ,const char *data
    ,std::size_t size
    ,jsonrefl::state &last_st
    ,OnBatch &&on_batch)
{
    static_assert(is_sequence_container_v<Container>, "Container must be std::vector or std::list");

    const std::size_t prev_size = window.size();
    last_st = pp.parse(data, size).status();

    if ( window.size() <= prev_size ) {
        return 0;
    }

    auto batch_begin = window.begin();
    std::advance(batch_begin, static_cast<std::ptrdiff_t>(prev_size));

    const std::size_t count = static_cast<std::size_t>(std::distance(batch_begin, window.end()));
    on_batch(batch_begin, window.end());
    window.erase(window.begin(), batch_begin);

    return count;
}

} // namespace chunked_array_feed
