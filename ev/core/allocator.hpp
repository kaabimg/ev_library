#pragma once

#include <memory>

namespace ev {
namespace experimental {

template <typename Primary, typename Fallback>
struct fallback_allocator : protected Primary, protected Fallback {
    void* allocate(size_t byte_count)
    {
        auto ptr = Primary::allocate(byte_count);
        if (ptr)
            return ptr;
        else
            return Fallback::allocate(byte_count);
    }
    void deallocate(void* ptr)
    {
        if (Primary::owns(ptr))
            Primary::deallocate(ptr);
        else
            Fallback::deallocate(ptr);
    }

    bool owns(const void* ptr)
    {
        return Primary::owns(ptr) || Fallback::owns(ptr);
    }
};
}

template <typename T, size_t element_count, std::size_t alignment = alignof(T)>
struct arena {
    static constexpr size_t count = element_count;
    static constexpr size_t size = sizeof(T) * count;

    T* begin() noexcept
    {
        return reinterpret_cast<T*>(_data);
    }
    T* end() noexcept
    {
        return begin() + count;
    }

    const T* begin() const noexcept
    {
        return reinterpret_cast<const T*>(_data);
    }
    const T* end() const noexcept
    {
        return begin() + count;
    }

    bool contains(const T* ptr) const noexcept
    {
        return ptr >= begin() && ptr < end();
    }

private:
    alignas(alignment) char _data[size];
};

template <class T, std::size_t N, class Allocator = std::allocator<T>>
class stack_allocator {
public:
    typedef typename std::allocator_traits<Allocator>::value_type value_type;
    typedef typename std::allocator_traits<Allocator>::pointer pointer;
    typedef typename std::allocator_traits<Allocator>::const_pointer const_pointer;
    typedef typename Allocator::reference reference;
    typedef typename Allocator::const_reference const_reference;
    typedef typename std::allocator_traits<Allocator>::size_type size_type;
    typedef typename std::allocator_traits<Allocator>::const_void_pointer const_void_pointer;
    typedef Allocator fallback_allocator_type;

public:
    explicit stack_allocator(const fallback_allocator_type& alloc = fallback_allocator_type())
        : m_fallback_allocator(alloc), m_begin(nullptr), m_end(nullptr), m_stack_pointer(nullptr)
    {
    }

    explicit stack_allocator(pointer buffer,
                             const fallback_allocator_type& alloc = fallback_allocator_type())
        : m_fallback_allocator(alloc), m_begin(buffer), m_end(buffer + N), m_stack_pointer(buffer)
    {
    }

    template <class U>
    stack_allocator(const stack_allocator<U, N, Allocator>& other)
        : m_fallback_allocator(other.m_fallback_allocator),
          m_begin(other.m_begin),
          m_end(other.m_end),
          m_stack_pointer(other.m_stack_pointer)
    {
    }

    template <size_t count>
    stack_allocator(arena<T, count>& arena,
                    const fallback_allocator_type& alloc = fallback_allocator_type())
        : m_fallback_allocator(alloc),
          m_begin(arena.begin()),
          m_end(arena.begin() + N),
          m_stack_pointer(m_begin)
    {
        static_assert(N <= count, "Arena is smaller than needed");
    }

    constexpr static size_type capacity()
    {
        return N;
    }

    pointer allocate(size_type n, const_void_pointer hint = const_void_pointer())
    {
        if (n <= size_type(std::distance(m_stack_pointer, m_end))) {
            pointer result = m_stack_pointer;
            m_stack_pointer += n;
            return result;
        }

        return m_fallback_allocator.allocate(n, hint);
    }

    void deallocate(pointer p, size_type n)
    {
        if (owns(p)) {
            m_stack_pointer -= n;
        }
        else
            m_fallback_allocator.deallocate(p, n);
    }

    size_type max_size() const noexcept
    {
        return m_fallback_allocator.max_size();
    }

    template <class U, class... Args>
    void construct(U* p, Args&&... args)
    {
        m_fallback_allocator.construct(p, std::forward<Args>(args)...);
    }

    template <class U>
    void destroy(U* p)
    {
        m_fallback_allocator.destroy(p);
    }

    pointer address(reference x) const noexcept
    {
        if (owns(std::addressof(x))) {
            return std::addressof(x);
        }

        return m_fallback_allocator.address(x);
    }

    const_pointer address(const_reference x) const noexcept
    {
        if (owns(std::addressof(x))) {
            return std::addressof(x);
        }

        return m_fallback_allocator.address(x);
    }

    template <class U>
    struct rebind {
        typedef stack_allocator<U, N, fallback_allocator_type> other;
    };

    pointer buffer() const noexcept
    {
        return m_begin;
    }

private:
    bool owns(const_pointer p) const
    {
        return (!(std::less<const_pointer>()(p, m_begin)) &&
                (std::less<const_pointer>()(p, m_end)));
    }

    fallback_allocator_type m_fallback_allocator;
    pointer m_begin;
    pointer m_end;
    pointer m_stack_pointer;
};

template <class T1, std::size_t N, class Allocator, class T2>
bool operator==(const stack_allocator<T1, N, Allocator>& lhs,
                const stack_allocator<T2, N, Allocator>& rhs) noexcept
{
    return lhs.buffer() == rhs.buffer();
}

template <class T1, std::size_t N, class Allocator, class T2>
bool operator!=(const stack_allocator<T1, N, Allocator>& lhs,
                const stack_allocator<T2, N, Allocator>& rhs) noexcept
{
    return !(lhs == rhs);
}
}
