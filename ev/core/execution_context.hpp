#pragma once

#include "logging.hpp"
#include <boost/container/stable_vector.hpp>

#include <memory>
#include <string>
#include <functional>
#include <variant>
#include <any>

namespace ev {

enum class execution_status { pending, running, stopped, finished, error };

enum class execution_event { status_changed, progress_changed, subitems_changed };

class execution_context;
using execution_observer = std::function<void(const execution_context&, execution_event)>;

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

struct execution_cxt_data : std::enable_shared_from_this<execution_cxt_data> {
    struct item {
        const execution_cxt_data* parent = nullptr;
        std::variant<const log_message*, const execution_cxt_data*> payload;

        bool is_message() const;
        bool is_execution_context() const;
        const log_message& message() const;
        const execution_cxt_data& context_data() const;

        execution_context context() const;
    };

    execution_cxt_data* parent{nullptr};

    detail::stable_vector<item> subitems;
    detail::stable_vector<log_message> messages;
    detail::stable_vector<execution_context> sub_ctxs;

    execution_observer observer;
    std::string name;
    execution_status status = execution_status::pending;
    uint8_t progress = 0;
    bool stop_request = false;
    std::any user_data;

    execution_observer* find_observer();
    bool has_stop_request();
    execution_context to_context() const;
};

class execution_context {
public:
    using subitem = execution_cxt_data::item;

    execution_context(std::nullptr_t);
    execution_context();
    execution_context(std::shared_ptr<execution_cxt_data> d);

    const execution_cxt_data* data_ptr() const;
    bool is_root() const;
    execution_context parent() const;

    template <typename T>
    void set_user_data(T&&);

    template <typename T>
    const T& user_data() const;

    template <typename T>
    T& user_data();

    bool has_user_data() const;

    //// API
    operator bool() const;

    void set_name(const std::string& n);
    const std::string& name() const;

    void set_observer(execution_observer);

    //// Write API: task side Thread safety condition: Max 1 writer, multiple readers
    execution_context create_sub_ctx();
    void log(const std::string& msg, log_category categroy);
    void set_status(execution_status s);
    void set_progress(uint8_t p);
    void stop();

    //// Read API: client side. Thread safety condition: Max 1 writer, multiple readers

    size_t subitem_count() const;
    const subitem& subitem_at(size_t i) const;

    execution_status status() const;
    uint8_t progress() const;
    bool has_stop_request();

private:
    void notify(execution_event event);

private:
    std::shared_ptr<execution_cxt_data> d;
};

//////////////////////////////////

inline bool execution_cxt_data::item::is_message() const
{
    return std::holds_alternative<const log_message*>(payload);
}

inline bool execution_cxt_data::item::is_execution_context() const
{
    return std::holds_alternative<const execution_cxt_data*>(payload);
}

inline const log_message& execution_cxt_data::item::message() const
{
    return *std::get<const log_message*>(payload);
}

inline const execution_cxt_data& execution_cxt_data::item::context_data() const
{
    return *std::get<const execution_cxt_data*>(payload);
}

inline execution_context execution_cxt_data::item::context() const
{
    return std::get<const execution_cxt_data*>(payload)->to_context();
}

inline execution_observer* execution_cxt_data::find_observer()
{
    if (observer) return &observer;
    if (parent) return parent->find_observer();
    return nullptr;
}

inline bool execution_cxt_data::has_stop_request()
{
    return stop_request || (parent && parent->has_stop_request());
}

inline execution_context execution_cxt_data::to_context() const
{
    return execution_context(const_cast<execution_cxt_data*>(this)->shared_from_this());
}

//////////////////////////////////

inline execution_context::execution_context(std::nullptr_t)
{
}

inline execution_context::execution_context()
{
    d = std::make_shared<execution_cxt_data>();
}

inline execution_context::execution_context(std::shared_ptr<execution_cxt_data> d_)
{
    d = std::move(d_);
}

inline const execution_cxt_data* execution_context::data_ptr() const
{
    return d.get();
}

inline bool execution_context::is_root() const
{
    return d->parent == nullptr;
}

template <typename T>
inline void execution_context::set_user_data(T&& t)
{
    d->user_data = std::make_any<T>(std::forward<T>(t));
}

template <typename T>
inline const T& execution_context::user_data() const
{
    if (d->user_data) {
        return std::any_cast<const T&>(d->user_data);
    }
    else if (!is_root()) {
        const auto p = parent();
        return p.user_data<T>();
    }
    throw std::bad_any_cast();
}

template <typename T>
inline T& execution_context::user_data()
{
    if (d->user_data) {
        return std::any_cast<T&>(d->user_data);
    }
    else if (!is_root()) {
        auto p = parent();
        return p.user_data<T>();
    }
    throw std::bad_any_cast();
}

//// API
inline execution_context::operator bool() const
{
    return d.get() != nullptr;
}

inline void execution_context::set_name(const std::string& n)
{
    d->name = n;
}
inline const std::string& execution_context::name() const
{
    return d->name;
}

inline void execution_context::set_observer(execution_observer obs)
{
    d->observer = std::move(obs);
}

//// Write API: task side Thread safety condition: Max 1 writer, multiple readers
inline execution_context execution_context::create_sub_ctx()
{
    execution_context env;
    env.d->parent = d.get();
    d->sub_ctxs.push_back(env);
    subitem i;
    i.parent = d.get();
    i.payload = env.d.get();
    d->subitems.push_back(i);
    notify(execution_event::subitems_changed);
    return env;
}
inline void execution_context::log(const std::string& msg, log_category categroy)
{
    d->messages.push_back({msg, categroy});
    subitem i;
    i.parent = d.get();
    i.payload = &(*d->messages.data.rbegin());
    d->subitems.push_back(i);
    notify(execution_event::subitems_changed);
}
inline void execution_context::set_status(execution_status s)
{
    d->status = s;
    notify(execution_event::status_changed);
}
inline void execution_context::set_progress(uint8_t p)
{
    d->progress = p;
    notify(execution_event::progress_changed);
}
inline void execution_context::stop()
{
    d->stop_request = true;
}

//// Read API: client side. Thread safety condition: Max 1 writer, multiple readers
inline execution_context execution_context::parent() const
{
    if (d->parent) {
        return d->parent->to_context();
    }
    return execution_context{nullptr};
}

inline bool execution_context::has_user_data() const
{
    return d->user_data.has_value() || (!is_root() && parent().has_user_data());
}

inline size_t execution_context::subitem_count() const
{
    return d->subitems.size;
}

inline const execution_context::subitem& execution_context::subitem_at(size_t i) const
{
    return d->subitems.data[i];
}

inline execution_status execution_context::status() const
{
    return d->status;
}

inline uint8_t execution_context::progress() const
{
    return d->progress;
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
        case ev::execution_event::subitems_changed: return "Hierarchy changed";
        default: return "Unknown";
    }
}
}
