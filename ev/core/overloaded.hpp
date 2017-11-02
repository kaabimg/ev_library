#pragma once

namespace ev {

template <class... T>
struct overloaded;

template <class F, class... Fs>
struct overloaded<F, Fs...> : F, overloaded<Fs...> {
    overloaded(F&& f, Fs&&... fs) : F(std::forward<F>(f)),
        overloaded<Fs...>(std::forward<Fs>(fs)...)
    {
    }

    using F::operator();
    using overloaded<Fs...>::operator();
};

template <class F>
struct overloaded<F> : F {
    overloaded(F&& f) : F(std::forward<F>(f))
    {
    }

    using F::operator();
};

template <class... Fs>
inline auto make_overloaded(Fs&&... fs)
{
    return overloaded<Fs...>(std::forward<Fs>(fs)...);
}
}
