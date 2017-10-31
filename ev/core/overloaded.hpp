#pragma once

namespace ev {

template <class... T>
struct overloaded;

template <class F0, class... Fs>
struct overloaded<F0, Fs...> : F0, overloaded<Fs...> {
    overloaded(F0 f0, Fs&&... fs) : F0(f0), overloaded<Fs...>(std::forward<Fs>(fs)...)
    {
    }

    using F0::operator();
    using overloaded<Fs...>::operator();
};

template <class F0>
struct overloaded<F0> : F0 {
    overloaded(F0 f0) : F0(f0)
    {
    }

    using F0::operator();
};

template <class... Fs>
inline auto make_overloaded(Fs&&... fs)
{
    return overloaded<Fs...>(std::forward<Fs>(fs)...);
}
}
