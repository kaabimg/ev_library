#pragma once

#include "common.hpp"
#include <ev/core/object.hpp>
#include <ev/core/flags.hpp>

namespace ev
{
namespace ogl
{
enum class event_type_e
{
    resize_event,
    mouse_event,
    key_event,
    close_event
};

class close_event_t : public ev::event_t
{
public:
    close_event_t(ev::object_t* target)
        : event_t(target, as_int(event_type_e::close_event))
    {
    }
};

class resize_event_t : public ev::event_t
{
public:
    resize_event_t(ev::object_t* target, const size_2d_t& s)
        : event_t(target, as_int(event_type_e::resize_event)), size{s}
    {
    }

    size_2d_t size;
};

class mouse_event_t : public ev::event_t
{
public:
};

class key_event_t : public ev::event_t
{
public:
    key_event_t(ev::object_t* target)
        : event_t(target, as_int(event_type_e::key_event))
    {
    }

    int key;
    int scan_code;
    int action;
    int mods;
};
}
}
