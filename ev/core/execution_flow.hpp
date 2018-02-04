#pragma once

#include <ev/core/executor.hpp>
#include <ev/core/traits.hpp>
#include <ev/core/meta.hpp>
#include <ev/core/algorithm.hpp>

namespace ev {
namespace exf {

template <typename... T>
struct node;

template <typename... Sig>
struct column;

class graph {
public:
    struct node_data {
        node_data(size_t tc) : executor(tc)
        {
        }
        virtual ~node_data() = default;

        ev::executor executor;
    };

public:
    graph& operator<<(std::shared_ptr<node_data> n);
    void wait_all();

private:
    std::vector<std::shared_ptr<node_data>> _nodes;
};

template <typename R>
struct connection {
    using action = std::function<void(R&)>;
    using condition = std::function<bool(const R&)>;

    action act;
    condition cond = [](const R&) { return true; };
};

template <>
struct connection<void> {
    using action = std::function<void()>;
    using condition = std::function<bool()>;
    action act;
    condition cond = []() { return true; };
};

template <typename R, typename... Args>
struct node<R(Args...)> {
    node(graph& g, size_t thread_count = 1);

    template <typename F>
    void set_task(F&& f);

    template <typename Sig>
    auto& operator>>(node<Sig>& node);

    template <typename... Sig>
    auto& operator>>(column<Sig...>& col);

    template <typename... Sig>
    auto operator>>(column<Sig...>&& col);

    void operator()(Args... args);

private:
    struct data : graph::node_data {
        data(size_t tc) : graph::node_data(tc)
        {
        }
        std::function<R(Args...)> f;
        std::vector<connection<R>> connections;
    };

private:
    std::shared_ptr<data> _data;
};

template <typename... Sigs>
struct column {
    column(node<Sigs>&... nodes) : _nodes(nodes...)
    {
    }

    column(const column&) = default;
    column(column&&) = default;

    std::tuple<node<Sigs>...>& nodes()
    {
        return _nodes;
    }

    template <typename Sig>
    auto& operator>>(node<Sig>& node);

    template <typename... Sig>
    auto& operator>>(column<Sig...>& col);

    template <typename... Sig>
    auto operator>>(column<Sig...>&& col);

private:
    std::tuple<node<Sigs>...> _nodes;
};

inline graph& graph::operator<<(std::shared_ptr<node_data> n)
{
    _nodes.emplace_back(std::move(n));
}

inline void graph::wait_all()
{
    for (auto n : _nodes) n->executor.wait();
}

template <typename R, typename... Args>
inline node<R(Args...)>::node(graph& g, size_t thread_count)
{
    _data = std::make_shared<data>(thread_count);
    g << _data;
}

template <typename R, typename... Args>
template <typename F>
inline void node<R(Args...)>::set_task(F&& f)
{
    _data->f = std::forward<F>(f);
}

template <typename R, typename... Args>
template <typename Sig>
inline auto& node<R(Args...)>::operator>>(node<Sig>& node)
{
    _data->connections.push_back({[&node](R& ret) { node(ret); }});
    return node;
}

template <typename R, typename... Args>
template <typename... Sig>
auto& node<R(Args...)>::operator>>(column<Sig...>& col)
{
    ev::for_each(col.nodes(), [this, &col](auto& n) { *this >> n; });
    return col;
}

template <typename R, typename... Args>
template <typename... Sig>
auto node<R(Args...)>::operator>>(column<Sig...>&& col)
{
    column<Sig...> col_c = std::move(col);
    ev::for_each(col_c.nodes(), [this](auto& n) { *this >> n; });
    return col_c;
}

template <typename R, typename... Args>
inline void node<R(Args...)>::operator()(Args... args)
{
    auto task = [ data = _data, args... ]
    {
        if constexpr (ev_type(R) == ev_type(void)) {
            data->f(args...);
            for (auto& c : data->connections)
                if (c.cond()) c.act();
        }
        else {
            auto ret = data->f(args...);
            for (auto& c : data->connections)
                if (c.cond(ret)) c.act(ret);
        }
    };
    _data->executor << task;
}

template <typename... Sigs>
template <typename Sig>
auto& column<Sigs...>::operator>>(node<Sig>& node)
{
    ev::for_each(_nodes, [&node](auto& n) { n >> node; });
    return node;
}

template <typename... Sigs>
template <typename... Sig>
auto& column<Sigs...>::operator>>(column<Sig...>& col)
{
    ev::for_each(_nodes, [&col](auto& n) { n >> col; });
    return col;
}

template <typename... Sigs>
template <typename... Sig>
auto column<Sigs...>::operator>>(column<Sig...>&& col)
{
    column<Sig...> col_c = std::move(col);
    ev::for_each(_nodes, [&col_c](auto& n) { n >> col_c; });
    return col_c;
}
}
}
