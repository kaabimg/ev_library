#pragma once
#include <vector>

namespace ev
{
using address_t    = void*;
using byte_t       = unsigned char;
using byte_array_t = std::vector<byte_t>;

struct mem_block_t
{
    address_t data;
    unsigned int size;
};

struct construct_inplace_t
{
};
static const construct_inplace_t construct_inplace;
struct empty_t
{
};

class non_copyable_t
{
protected:
    non_copyable_t()                      = default;
    ~non_copyable_t()                     = default;
    non_copyable_t(const non_copyable_t&) = delete;
    non_copyable_t& operator=(const non_copyable_t&) = delete;
};

template <typename T = empty_t, bool cond = true>
struct loop_scoped_variable_t
{
    template <typename... A>
    loop_scoped_variable_t(A&&... d) : data(std::forward<A>(d)...)
    {
    }
    operator bool() const { return cond; }
    T data;
};
}
