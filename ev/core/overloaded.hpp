#pragma once

#include <type_traits>

namespace ev {

template <class... T>
struct overloaded;

template <class F, class... Fs>
struct overloaded<F, Fs...> : std::decay_t<F>, overloaded<Fs...> {
    overloaded(F&& f, Fs&&... fs)
        : std::decay_t<F>(std::forward<F>(f)), overloaded<Fs...>(std::forward<Fs>(fs)...)
    {
    }

    using std::decay_t<F>::operator();
    using overloaded<Fs...>::operator();
};

template <class F>
struct overloaded<F> : std::decay_t<F> {
    overloaded(F&& f) : std::decay_t<F>(std::forward<F>(f))
    {
    }

    using std::decay_t<F>::operator();
};

template <class... Fs>
inline auto make_overloaded(Fs&&... fs)
{
    return overloaded<Fs...>(std::forward<Fs>(fs)...);
}
}
