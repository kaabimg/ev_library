#pragma once

#include <ev/core/executor.hpp>
#include <ev/core/traits.hpp>
#include <ev/core/meta.hpp>

namespace ev {

template <typename... T>
struct execution_node;

class execution_node_base;
class execution_graph {
public:
    execution_graph& operator<<(execution_node_base& n);
    void wait_all();

private:
    std::vector<execution_node_base*> _nodes;
};

struct execution_node_base {
    execution_node_base(execution_graph& graph, size_t thread_count);

    void wait();

protected:
    ev::executor _executor;
    friend class execution_graph;
};

template <typename R>
struct execution_graph_connection {
    using action = std::function<void(R&)>;
    using condition = std::function<bool(R&)>;

    action act;
    condition cond = [](R&) { return true; };
};

template <>
struct execution_graph_connection<void> {
    using action = std::function<void()>;
    using condition = std::function<bool()>;
    action act;
    condition cond = []() { return true; };
};

template <typename R, typename... Args>
struct execution_node<R(Args...)> : protected execution_node_base {
    execution_node(execution_graph& graph, size_t thread_count = 1);

    template <typename F>
    void set_task(F&& f);

    auto& operator>>(auto& node);
    void operator()(Args... args);

private:
    std::function<R(Args...)> _f;
    std::vector<execution_graph_connection<R>> _connections;
};

inline execution_graph& execution_graph::operator<<(execution_node_base& n)
{
    _nodes.push_back(&n);
}

inline void execution_graph::wait_all()
{
    for (auto n : _nodes) n->wait();
}

inline execution_node_base::execution_node_base(execution_graph& graph, size_t thread_count)
    : _executor(thread_count)
{
    graph << *this;
}

inline void execution_node_base::wait()
{
    _executor.wait();
}

template <typename R, typename... Args>
inline execution_node<R(Args...)>::execution_node(execution_graph& graph, size_t thread_count)
    : execution_node_base(graph, thread_count)
{
}

template <typename R, typename... Args>
template <typename F>
inline void execution_node<R(Args...)>::set_task(F&& f)
{
    _f = std::forward<F>(f);
}

template <typename R, typename... Args>
inline auto& execution_node<R(Args...)>::operator>>(auto& node)
{
    _connections.push_back({[&node](R& ret) { node(ret); }});
    return node;
}

template <typename R, typename... Args>
inline void execution_node<R(Args...)>::operator()(Args... args)
{
    auto task = [this, args...] {
        if constexpr (ev_type(R) == ev_type(void)) {
            _f(args...);
            for (auto& c : _connections)
                if (c.cond()) c.act();
        }
        else {
            auto ret = _f(args...);
            for (auto& c : _connections)
                if (c.cond(ret)) c.act(ret);
        }
    };
    _executor << task;
}

struct dipatch_node;
struct split_node;
struct join_node;
}
