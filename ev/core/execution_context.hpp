#pragma once

#include "logging.hpp"
#include <boost/container/stable_vector.hpp>

#include <memory>
#include <string>
#include <functional>
#include <variant>

namespace ev {

enum class execution_status { pending, running, stopped, finished, error };

enum class execution_event { status_changed, progress_changed, log_changed, new_sub_context };

class execution_context;
using execution_observer = std::function<void(execution_context&, execution_event)>;

namespace detail {
template <typename T>
struct stable_vector  /// one writer, multiple readers
{
    boost::container::stable_vector<T> data;
    size_t size = 0;

    void push_back(const T& t)
    {
        data.push_back(t);
        ++size;
    }

    void push_back(T&& t)
    {
        data.push_back(std::move(t));
        ++size;
    }
};
}

class execution_context {
public:
    struct subitem;

    execution_context();
    execution_context(execution_observer obs);

    operator bool() const;

    void set_name(std::string& n);
    const std::string& name() const;

    //// Write API: task side Thread safety condition: Max 1 writer, multiple readers
    execution_context push_sub_context();
    void log(const std::string& msg, log_category categroy);
    void set_status(execution_status s);
    void set_progress(uint8_t p);
    void stop();

    //// Read API: client side. Thread safety condition: Max 1 writer, multiple readers
    execution_context parent() const;

    size_t subitem_count() const;
    const subitem& subitem_at(size_t i) const;

    execution_status status() const;
    uint8_t progress() const;
    bool has_stop_request();

private:
    void notify(execution_event event);

private:
    struct data;
    std::shared_ptr<data> d;

private:
    execution_context(std::nullptr_t);
    execution_context(std::shared_ptr<data> d_);
};

struct execution_context::subitem {
    execution_context parent;
    std::variant<execution_context, log_message> payload;

    template <typename T>
    bool is() const;

    const execution_context& context() const;
    const log_message& message() const;
};

inline const execution_context &execution_context::subitem::context() const
{
    return std::get<execution_context>(payload);
}

inline const log_message &execution_context::subitem::message() const
{
    return std::get<log_message>(payload);
}

template <typename T>
inline bool execution_context::subitem::is() const
{
    return std::holds_alternative<T>(payload);
}

struct execution_context::data {
    std::shared_ptr<data> parent_data{nullptr};
    detail::stable_vector<subitem> subitems;
    execution_observer observer;
    std::string name;
    execution_status status = execution_status::pending;
    uint8_t progress = 0;
    bool stop_request = false;

    execution_observer* find_observer();
    bool has_stop_request();
};

using execution_context_subitem = execution_context::subitem;

inline execution_context::execution_context()
{
    d = std::make_shared<data>();
}

inline execution_context::execution_context(execution_observer obs) : execution_context()
{
    d->observer = std::move(obs);
}

inline void execution_context::set_name(std::string& n)
{
    d->name = n;
}

inline const std::string& execution_context::name() const
{
    return d->name;
}

inline execution_context::operator bool() const
{
    return d.get() != nullptr;
}

inline execution_context execution_context::parent() const
{
    if (d->parent_data) return execution_context{d->parent_data};
    return execution_context{nullptr};
}

inline execution_context execution_context::push_sub_context()
{
    execution_context ec;
    ec.d->parent_data = d;
    d->subitems.push_back({*this, ec});
    notify(execution_event::new_sub_context);
    return ec;
}

inline size_t execution_context::subitem_count() const
{
    return d->subitems.size;
}

inline const execution_context_subitem& execution_context::subitem_at(size_t i) const
{
    return d->subitems.data[i];
}

inline void execution_context::log(const std::string& msg, log_category categroy)
{
    d->subitems.push_back({*this, log_message{msg, categroy}});
    notify(execution_event::log_changed);
}

inline void execution_context::set_status(execution_status s)
{
    d->status = s;
    notify(execution_event::status_changed);
}

inline execution_status execution_context::status() const
{
    return d->status;
}

inline void execution_context::set_progress(uint8_t p)
{
    d->progress = p;
    notify(execution_event::progress_changed);
}

inline uint8_t execution_context::progress() const
{
    return d->progress;
}

inline void execution_context::stop()
{
    d->stop_request = true;
}

inline bool execution_context::has_stop_request()
{
    return d->has_stop_request();
}

inline void execution_context::notify(execution_event event)
{
    auto observer = d->find_observer();
    if (observer) (*observer)(*this, event);
}

inline execution_context::execution_context(std::nullptr_t)
{
}

inline execution_context::execution_context(std::shared_ptr<execution_context::data> d_) : d(d_)
{
}

inline execution_observer* execution_context::data::find_observer()
{
    if (observer) return &observer;
    if (parent_data) return parent_data->find_observer();
    return nullptr;
}

inline bool execution_context::data::has_stop_request()
{
    return stop_request || (parent_data && parent_data->has_stop_request());
}

/////////////////////////////////////////////////////////////////

template <>
struct logger<execution_context> {
    logger(execution_context& ec) : _ec(ec)
    {
    }
    void log(const log_message& msg)
    {
        _ec.log(msg.message, msg.category);
    }

private:
    execution_context& _ec;
};
}

namespace std {
inline std::string to_string(ev::execution_status s)
{
    switch (s) {
        case ev::execution_status::pending: return "Pending";
        case ev::execution_status::running: return "Running";
        case ev::execution_status::stopped: return "Stopped";
        case ev::execution_status::finished: return "Finished";
        case ev::execution_status::error: return "Error";
    }
}

inline std::string to_string(ev::execution_event e)
{
    switch (e) {
        case ev::execution_event::status_changed: return "Status changed";
        case ev::execution_event::progress_changed: return "Progress changed";
        case ev::execution_event::log_changed: return "Log changed";
        case ev::execution_event::new_sub_context: return "New sub context";
    }
}
}
