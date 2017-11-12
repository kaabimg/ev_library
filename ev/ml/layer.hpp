#pragma once

#include "transfert_function.hpp"

#include <vector>
#include <algorithm>

namespace ev {
namespace ml {

struct neuron_data {
    std::vector<double> weights;
    double bias = 0.0;
};

struct basic_layer {
    basic_layer(size_t count) : _values(count)
    {
    }

    size_t size() const
    {
        return _values.size();
    }

    const std::vector<double>& output() const
    {
        return _values;
    }

    inline double operator()(size_t i) const
    {
        return _values[i];
    }

    inline void update_from(const std::vector<double>& values)
    {
        const size_t size = this->size();
        for (size_t i = 0; i < size; ++i) {
            _values[i] = values[i];
        }
    }

protected:
    std::vector<double> _values;
};

template <typename TransferFunction>
struct layer : basic_layer {
    using transfert_function = TransferFunction;

    layer(size_t count) : basic_layer(count), _neurons_data(count)
    {
    }

    size_t size() const
    {
        return _neurons_data.size();
    }

    void apply(auto&& f)
    {
        for (size_t i = 0; i < size(); ++i) {
            f(_values[i], _neurons_data[i]);
        }
    }

    inline constexpr void update_from(const auto& l1, auto&& transfert_fn = identity)
    {
        const size_t size = l1.size();

        apply([size, &l1, &transfert_fn](auto& value, auto& neuron_data) {
            double wx = 0.0;
            for (size_t i = 0; i < size; ++i) {
                wx += neuron_data.weights[i] * l1(i);
            }
            value = transfert_fn(wx + neuron_data.bias);
        });
    }

    inline constexpr void update_from(const std::vector<double>& values)
    {
        _values = values;
    }

    inline constexpr void update_from(const std::vector<double>& values, auto&& transform)
    {
        const size_t size = this->size();
        for (size_t i = 0; i < size; ++i) {
            _values[i] = transform(values[i]);
        }
    }

private:
    std::vector<neuron_data> _neurons_data;
    transfert_function _tf;
};

template <typename Neuron>
inline constexpr auto make_layer(size_t count)
{
    return layer<Neuron>(count);
}

inline constexpr void connect(const auto& l1, auto& l2)
{
    const size_t size = l1.size();

    l2.apply([size](auto&, auto& neuron_data) { neuron_data.weights.resize(size); });
}

template <typename F, typename L0, typename L1, typename... Ls>
inline const void feed_forward(F&& f, L0&& l0, L1&& l1, Ls&&... ls)
{
    l1.update_from(l0, f);
    if
        constexpr(sizeof...(Ls))
        {
            feed_forward(std::forward<F>(f), std::forward<L1>(l1), std::forward<Ls>(ls)...);
        }
}
}
}
