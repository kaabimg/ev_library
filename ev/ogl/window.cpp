#include "window.hpp"
#include "application.hpp"
#include "glfw.hpp"

#include <ev/core/logging.hpp>
#include <ev/core/flags.hpp>

using namespace ev::ogl;

struct window_t::impl_t {
    ~impl_t()
    {
        if (window) glfwDestroyWindow(window);
    }

    GLFWwindow* window             = nullptr;
    window_paint_func_t paint_func = nullptr;
    bool need_update               = true;
};

inline window_t* get_window(GLFWwindow* window)
{
    return static_cast<window_t*>(glfwGetWindowUserPointer(window));
}

namespace events {
inline void key_callback(GLFWwindow* window,
                         int key,
                         int scancode,
                         int action,
                         int mods)
{
    auto event       = new key_event_t(get_window(window));
    event->key       = key;
    event->scan_code = scancode;
    event->action    = action;
    event->mods      = mods;
    application_t::instance().post(event);
}

inline void mouse_callback(GLFWwindow*, int, int, int)
{
}

inline void window_size_callback(GLFWwindow* window,
                                 int /*width*/,
                                 int /*height*/)
{
    window_t* w = get_window(window);
    application_t::instance().post(new resize_event_t{w, w->size()});
}
}

window_t::window_t(const size_2d_t& size,
                   const std::string& title,
                   ev::object_t* parent)
    : ev::object_t{parent}, m_impl{new impl_t}
{
    init(size, title);
}

void window_t::init(const size_2d_t& size, const std::string& title)
{
    m_impl->window = glfwCreateWindow(size.width, size.height, title.c_str(),
                                      nullptr, nullptr);

    if (!m_impl->window) {
        throw std::runtime_error("Failed to create Window");
    }
    glfwSetWindowUserPointer(m_impl->window, this);

    glfwSetKeyCallback(m_impl->window, events::key_callback);
    glfwSetMouseButtonCallback(m_impl->window, events::mouse_callback);
    glfwSetWindowSizeCallback(m_impl->window, events::window_size_callback);

    application_t::instance().register_window(this);

    make_current();
}

window_t::~window_t()
{
    application_t::instance().unregister_window(this);
    if (m_impl) {
        delete m_impl;
        m_impl = nullptr;
    }
}

void window_t::show()
{
    glfwShowWindow(m_impl->window);
    set_need_update();
}

void window_t::hide()
{
    glfwHideWindow(m_impl->window);
    set_need_update();
}

bool window_t::is_visible() const
{
    int visible = glfwGetWindowAttrib(m_impl->window, GLFW_VISIBLE);
    return visible == GLFW_TRUE;
}

void window_t::close()
{
    set_need_update();
    application_t::instance().post(new close_event_t{this});
}

size_2d_t window_t::size() const
{
    size_2d_t size;
    glfwGetWindowSize(m_impl->window, &size.width, &size.height);
    return size;
}

void window_t::resize(const size_2d_t& size)
{
    glfwSetWindowSize(m_impl->window, size.width, size.height);
}

position_t window_t::position() const
{
    position_t pos;
    glfwGetWindowPos(m_impl->window, &pos.x, &pos.y);
    return pos;
}
void window_t::set_position(const position_t& pos)
{
    glfwSetWindowPos(m_impl->window, pos.x, pos.y);
}

void window_t::set_paint_func(window_t::paint_func_t&& func)
{
    m_impl->paint_func = std::move(func);
}

void window_t::paint()
{
    if (m_impl->paint_func) m_impl->paint_func(*this);
}

void window_t::set_need_update(bool val)
{
    m_impl->need_update = val;
}

void window_t::make_current()
{
    glfwMakeContextCurrent(m_impl->window);
}

void window_t::render()
{
    if (m_impl->need_update) {
        make_current();
        paint();
        glfwSwapBuffers(m_impl->window);
        m_impl->need_update = false;
    }
}

bool window_t::process_event(ev::event_t* event)
{
    switch (event->type()) {
        case as_int(event_type_e::key_event):
            return on_key_event(event->as<key_event_t>());
        case as_int(event_type_e::close_event):
            return on_close_event(event->as<close_event_t>());
        case as_int(event_type_e::resize_event):
            return on_resize_event(event->as<resize_event_t>());
        default:
            ev::debug() << "unhandled event" << event->type();
            return false;
    }
}

bool window_t::on_key_event(key_event_t* event)
{
    switch (event->key) {
        case GLFW_KEY_ESCAPE:
            if (event->action == GLFW_PRESS) close();
            break;
        default: break;
    }
}

bool window_t::on_close_event(close_event_t*)
{
    glfwSetWindowShouldClose(m_impl->window, GLFW_TRUE);
    application_t::instance().unregister_window(this);
    return true;
}

bool window_t::on_resize_event(resize_event_t* e)
{
    set_need_update();
}
