#include "application.hpp"
#include "window.hpp"
#include "glfw.hpp"

#include <ev/core/logging.hpp>
#include <ev/core/synchronized_data.hpp>

#include <boost/program_options.hpp>

#include <queue>

using namespace ev::ogl;
struct application_t::impl_t
{
    static application_t* instance;

    ev::synchronized_data_t<std::queue<ev::event_t*>> events;
    std::vector<window_t*> windows;
};

namespace events
{
void on_error(int, const char* description)
{
    throw std::runtime_error(description);
}
}

application_t* application_t::impl_t::instance = nullptr;

application_t::application_t(int argc, char** argv)
    : ev::object_t{nullptr}, m_impl{new impl_t}
{
    if (impl_t::instance)
        throw std::runtime_error("application_t instancieted twice");

    if (glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("Failed to init GLFW");
    }

    glfwSetErrorCallback(events::on_error);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    impl_t::instance = this;
}

application_t::~application_t()
{
    delete m_impl;
    glfwTerminate();
}

application_t& application_t::instance()
{
    if (!impl_t::instance)
        throw std::runtime_error("application_t not instancieted");

    return *impl_t::instance;
}

void application_t::post(ev::event_t* event)
{
    m_impl->events->push(event);
    glfwPostEmptyEvent();
}

int application_t::exec()
{
    while (true) {
        glfwPollEvents();

        event_t* event = nullptr;

        do
        {
            synchronized(events, m_impl->events)
            {
                if (!events.empty()) {
                    event = events.front();
                    events.pop();
                }
                else
                    event = nullptr;
            }
            if (event) {
                event->target()->process_event(event);
                delete event;
            }
        } while (event);
        if (m_impl->windows.empty()) break;
        for (auto window : m_impl->windows) window->render();
    }

    return EXIT_SUCCESS;
}

void application_t::register_window(window_t* w)
{
    auto pos = std::find(m_impl->windows.begin(), m_impl->windows.end(), w);
    if (pos == m_impl->windows.end()) m_impl->windows.push_back(w);
}

void application_t::unregister_window(window_t* w)
{
    auto pos = std::find(m_impl->windows.begin(), m_impl->windows.end(), w);
    if (pos != m_impl->windows.end()) m_impl->windows.erase(pos);
}
