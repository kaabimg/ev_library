#pragma once

namespace ev {

template <typename F1, typename F2>
struct visitor : F1, F2 {
    visitor(F1&& f1, F2&& f2) : F1(std::forward<F1>(f1)), F2(std::forward<F2>(f2))
    {
    }

    using F1::operator();
    using F2::operator();
};

template <typename F1, typename F2>
inline visitor<F1, F2> make_visitor(F1&& f1, F2&& f2)
{
    return visitor{std::forward<F1>(f1), std::forward<F2>(f2)};
}

template <typename F1, typename... F>
inline auto make_visitor(F1&& f1, F&&... f)
    -> visitor<F1, decltype(make_visitor(std::declval<F>()...))>
{
    return {std::forward<F1>(f1), visitor{std::forward<F>(f)...}};
}
}
