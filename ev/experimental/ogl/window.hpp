#pragma once

#include "events.hpp"
#include <ev/core/object.hpp>
#include <memory>

namespace ev {
namespace ogl {

class window_t : public ev::object_t {
public:
    using paint_func_t = std::function<void(window_t&)>;

    window_t(const size_2d_t& size,
             const std::string& title = std::string(),
             ev::object_t* parent     = nullptr);
    ~window_t();

    void show();
    void hide();
    bool is_visible() const;
    void close();

    size_2d_t size() const;
    void resize(const size_2d_t& size);

    position_t position() const;
    void set_position(const position_t& size);

    void set_paint_func(paint_func_t&& func);
    void render();

    void make_current();

    bool process_event(event_t*) override;

protected:
    void init(const size_2d_t& size, const std::string& title);
    virtual void paint();
    void set_need_update(bool val = true);

    virtual bool on_key_event(key_event_t*);
    virtual bool on_close_event(close_event_t*);
    virtual bool on_resize_event(resize_event_t*);

private:
    EV_IMPL(window_t);
};

using window_paint_func_t = window_t::paint_func_t;
}
}
