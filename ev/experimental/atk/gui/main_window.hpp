#pragma once

#include "../qtypes_fwd.hpp"
#include <ev/core/preprocessor.hpp>
#include <qwidget.h>
#include <qicon.h>

#define atk_main_window ::ev::atk::main_window_t::instance()
#define atk_window_palette ::ev::atk::main_window_t::instance()->window_palette()
namespace ev {
namespace atk {

class widget_t;

struct mainview_provider_t {
    virtual ~mainview_provider_t()
    {
    }
    virtual qwidget* left_widget() const     = 0;
    virtual widget_t* central_widget() const = 0;
};

struct ready_mainview_provider_t : mainview_provider_t {
    ready_mainview_provider_t(qwidget* left_widget, widget_t* central_widget):
        _left_widget(left_widget),
        _central_widget(central_widget)
    {}
    qwidget* _left_widget;
    widget_t* _central_widget;

    qwidget* left_widget() const
    {
        return _left_widget;
    }
    widget_t* central_widget() const
    {
        return _central_widget;
    }
};

inline mainview_provider_t* make_ready_mainview_provider(qwidget* left_widget, widget_t* central_widget)
{
    return new ready_mainview_provider_t{left_widget, central_widget};
}

struct window_palette_t {
    qcolor dark       = "#050505";
    qcolor dark_gray  = "#404244";
    qcolor light_gray = "#BFC1C2";
    qcolor light      = "#FFFFFF";
    qcolor primary    = "#00415A";
    qcolor secondary  = "#951C37";
};

struct window_sizes_t {
    int header_height = 27;
    int tab_bar_width = 70;
};

struct main_window_settings_t {
    qstring icons_path = ":/";
    window_palette_t palette;
    window_sizes_t sizes;
};

enum class standard_icon_e {

};

class main_window_t : public qwidget {
    Q_OBJECT
public:
    main_window_t(const main_window_settings_t& settings = main_window_settings_t());
    ~main_window_t();

    static main_window_t* instance();

    const window_palette_t& window_palette() const;
    const window_sizes_t& window_sizes() const;

    const qicon& std_icon(standard_icon_e) const;

    void add_view(mainview_provider_t*);
    void add_pane(widget_t*iopane);
    void add_status_widget(qwidget*);
protected Q_SLOTS:
    void on_io_pane_show_request(widget_t*);
    void on_io_pane_hide_request();

protected:
    void load_icons();

private:
    EV_IMPL(main_window_t)
};
}
}
