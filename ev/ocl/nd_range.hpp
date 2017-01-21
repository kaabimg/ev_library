#pragma once

#include <stddef.h>

namespace ev
{
namespace ocl
{
struct nd_range_t
{
    size_t dimension = 0;
    size_t size[3]   = {0, 0, 0};

    nd_range_t() {}
    nd_range_t(size_t s)
    {
        size[0]   = s;
        dimension = 1;
    }
    nd_range_t(size_t s1, size_t s2)
    {
        size[0]   = s1;
        size[1]   = s2;
        dimension = 2;
    }
    nd_range_t(size_t s1, size_t s2, size_t s3)
    {
        size[0]   = s1;
        size[1]   = s2;
        size[2]   = s3;
        dimension = 2;
    }
};
}
}
