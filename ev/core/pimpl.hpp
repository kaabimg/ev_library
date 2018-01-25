#pragma once
#include <memory>

namespace ev {

template <typename T>
class pimpl {
public:
    template <typename... Args>
    pimpl(std::in_place_t = std::in_place, Args&&... args)
    {
        _ptr = std::make_unique<T>(std::forward<Args>(args)...);
    }
    pimpl(const pimpl& rhs)
    {
        _ptr = std::make_unique<T>(*rhs._ptr.get());
    }
    pimpl(pimpl&& rhs)
    {
        std::swap(_ptr, rhs._ptr);
    }

    pimpl& operator=(const pimpl& rhs)
    {
        _ptr->operator=(*rhs._ptr.get());
        return *this;
    }

    pimpl& operator=(pimpl&& rhs)
    {
        std::swap(_ptr, rhs._ptr);
        return *this;
    }

    T* operator->()
    {
        return _ptr.get();
    }
    const T* operator->() const
    {
        return _ptr.get();
    }

private:
    std::unique_ptr<T> _ptr;
};

namespace detail {

template <size_t StogateSize, size_t ClassSize>
inline void check_storage_size()
{
    static_assert(StogateSize == ClassSize, "Invalid storage size");
}

template <size_t StogateAlignment, size_t ClassAlignment>
inline void check_storage_alignment()
{
    static_assert(StogateAlignment == ClassAlignment, "Invalid storage alignment");
}

}
template <typename T, size_t Size,size_t Alignment, bool IsTriviallyDestructible = false>
class bimpl {
public:
    /////////////
    static void check_size_validity()
    {
        static_assert(!IsTriviallyDestructible ||
                          IsTriviallyDestructible == std::is_trivially_destructible<T>::value,
                      "Invalid IsTriviallyDestructible parameter");
        detail::check_storage_size<Size, sizeof(T)>();
        detail::check_storage_alignment<Alignment, alignof(T)>();
    }

    T* operator->()
    {
        check_size_validity();
        return reinterpret_cast<T*>(&_storage);
    }

    const T* operator->() const
    {
        check_size_validity();
        return reinterpret_cast<const T*>(&_storage);
    }

    /////////////

    template <typename... Args>
    bimpl(std::in_place_t = std::in_place, Args&&... args)
    {
        check_size_validity();
        construct(std::forward<Args>(args)...);
    }

    ~bimpl()
    {
        if constexpr (!IsTriviallyDestructible) {
            check_size_validity();
            self().~T();
        }
    }

    bimpl(const bimpl& rhs)
    {
        check_size_validity();
        construct(rhs.self());
    }

    bimpl(bimpl&& rhs)
    {
        check_size_validity();
        construct(std::move(rhs.self()));
    }

    bimpl& operator=(const bimpl& rhs)
    {
        check_size_validity();
        self() = rhs.self();
        return *this;
    }

    bimpl& operator=(bimpl&& rhs)
    {
        check_size_validity();
        self() = std::move(rhs.self());
        return *this;
    }

private:
    T& self()
    {
        return reinterpret_cast<T&>(_storage);
    }

    const T& self() const
    {
        return reinterpret_cast<const T&>(_storage);
    }

    template <typename... Args>
    void construct(Args&&... args)
    {
        new (this) T(std::forward<Args>(args)...);
    }

private:
    typename std::aligned_storage<Size,Alignment>::type _storage;
};
}
