#pragma once
#include <cmath>
namespace ev {
namespace ml {

inline constexpr double identity(double v)
{
    return v;
}

inline constexpr double sigmoid(double v)
{
    return 1. / (1. + std::exp(-v));
}

inline constexpr double relu(double v)
{
    if(v < 0)
        return 0.0;
    return v;
}

using identity_t = decltype(identity);
using sigmoid_t = decltype(sigmoid);
using relu_t = decltype(relu);

}
}
