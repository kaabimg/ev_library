#pragma once

#include <ev/core/object.hpp>

#include <memory>

namespace ev
{
namespace ogl
{
class window_t;

class application_t : public ev::object_t
{
public:
    application_t(int argc = 0, char** argv = nullptr);
    ~application_t();

    static application_t& instance();

    void post(ev::event_t*);
    int exec();

protected:
    friend class window_t;
    void register_window(window_t*);
    void unregister_window(window_t*);

private:
    EV_IMPL(application_t)
};
}
}
