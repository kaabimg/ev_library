#pragma once
#include <ev/core/executor.hpp>
#include <ev/core/traits.hpp>
#include <ev/core/meta.hpp>

namespace ev {
namespace ef {

template <typename... T>
struct node_data;

template <typename R, typename... Args>
struct node_data<R(Args...)> {
    using continuation_action = std::function<void(R&)>;

    node_data(size_t n = 1) : _executor(n)
    {
    }

    void operator()(Args... args)
    {
        auto task = [argsn, &f] {
            if constexpr(ev_type(R) == ev_type(void))
            {
                _f(args);
            }
            else {
                _f(args);
            }
        }
    }

    void connect_to(auto node)
    {
        _connections.push_back([node](R& ret) { node(ret); });
    }

protected:
    void on_ready(R&)
    {
    }

private:
    ev::executor _executor;
    std::function<R(Args...)> _f;
    std::vector<continuation_action> _connections;
};
}
}
