#include "main_window.hpp"
#include "main_window/io_bar.hpp"
#include "main_window/tab_bar.hpp"
#include "main_window/message_pane.hpp"
#include "widgets/splitter.hpp"
#include "widget.hpp"
#include "../preprocessor.hpp"
#include "../core/application.hpp"

#include <qlayout.h>
#include <qtextedit.h>
#include <qstackedwidget.h>
#include <qfile.h>
#include <qapplication.h>
#include <qdebug.h>

namespace ev {
namespace atk {
ATK_DECLARE_ENUM_QHASH(standard_icon_e)
}
}

using namespace ev::atk;


window_sizes_t::window_sizes_t()
{
    toolbar_height     = 25;
    io_pane_height     = 35;
    main_tab_bar_width = 70;
}

window_palette_t::window_palette_t()
{
    dark       = "#050505";
    dark_gray  = "#404244";
    light      = "#FFFFFF";
    primary    = "#00415A";
    secondary  = "#951C37";
}

qcolor window_palette_t::background_color() const
{
    return dark_gray;
}

qcolor window_palette_t::highlight_color() const
{
    return dark_gray.lighter();
}

qcolor window_palette_t::selected_color() const
{
    return dark_gray.darker();
}

qcolor window_palette_t::text_color() const
{
    return light;
}

struct view_container_t {
    mainview_provider_t* provider = nullptr;
    qwidget* left_widget          = nullptr;
    widget_t* central_widget      = nullptr;
};

struct main_window_t::impl {
    QHBoxLayout* main_layout;
    QVBoxLayout* content_layout;
    tab_bar_t* tab_bar;
    io_bar_t* io_bar;
    splitter_t *horizontal_splitter, *vertical_splitter;

    QStackedWidget *left_side_widget_container, *content_widget_container, *io_pane_container;
    message_pane_t* message_pane;

    qvector<view_container_t> views;
    qvector<widget_t*> io_panes;
    main_window_settings_t settings;
    qhash<standard_icon_e, qicon> icons;
    static main_window_t* instance;
};

main_window_t* main_window_t::impl::instance = nullptr;

main_window_t::main_window_t(const main_window_settings_t& settings)
    : qmainwindow{nullptr}, d{new impl}
{
    impl::instance = this;

    d->settings = settings;
    load_icons();

    qwidget* central_widget = new qwidget(this);
    d->main_layout          = new QHBoxLayout(central_widget);
    setCentralWidget(central_widget);

    d->content_layout = new QVBoxLayout;

    d->main_layout->setContentsMargins(0, 0, 0, 0);
    d->main_layout->setMargin(0);
    d->main_layout->setSpacing(0);

    d->content_layout->setContentsMargins(0, 0, 0, 0);
    d->content_layout->setMargin(0);
    d->content_layout->setSpacing(0);

    d->horizontal_splitter        = new splitter_t(Qt::Horizontal, this);
    d->vertical_splitter          = new splitter_t(Qt::Vertical, this);
    d->io_bar                     = new io_bar_t(this);
    d->tab_bar                    = new tab_bar_t(this);
    d->left_side_widget_container = new QStackedWidget(this);
    d->content_widget_container   = new QStackedWidget(this);
    d->io_pane_container          = new QStackedWidget(this);

    /// tab_bar
    //    d->tab_bar->setFixedWidth(window_sizes().tab_bar_width);
    d->main_layout->addWidget(d->tab_bar);

    ///
    d->main_layout->addLayout(d->content_layout);
    d->content_layout->addWidget(d->horizontal_splitter);

    //// io_bar
    d->content_layout->addWidget(d->io_bar);

    ////
    d->horizontal_splitter->addWidget(d->left_side_widget_container);
    d->vertical_splitter->addWidget(d->content_widget_container);
    d->vertical_splitter->addWidget(d->io_pane_container);
    d->horizontal_splitter->addWidget(d->vertical_splitter);

    ////
    d->io_pane_container->setVisible(false);

    ////

    //    QFile style_file(":/main_style.qss");
    //    style_file.open(QFile::ReadOnly);

    //    qstring ss          = style_file.readAll();
    //    const auto& palette = d->settings.palette;

    //    ss = ss.arg(palette.dark.name(), palette.dark_gray.name(), palette.light_gray.name(),
    //                palette.light.name(), palette.primary.name(), palette.secondary.name());

    //    qApp->setStyleSheet(ss);
    //    style_file.close();
    ////
    connect(ATK_SIGNAL(d->io_bar, show_request), ATK_SLOT(this, on_io_pane_show_request));
    connect(ATK_SIGNAL(d->io_bar, hide_request), ATK_SLOT(this, on_io_pane_hide_request));
    connect(ATK_SIGNAL(d->tab_bar, show_request), ATK_SLOT(this, on_view_show_request));

    ////
    d->message_pane = new message_pane_t;

    auto pane = widget_t::make_from(d->message_pane);
    pane->set_title("Messages");
    add_pane(pane);

    atk_app->set_system_interface(this);
}

main_window_t::~main_window_t()
{
    delete d;
}

main_window_t* main_window_t::instance()
{
    return impl::instance;
}

const window_sizes_t& main_window_t::window_sizes() const
{
    return d->settings.sizes;
}

const qicon& main_window_t::std_icon(standard_icon_e icon) const
{
    static qicon empty;
    if (d->icons.contains(icon)) return d->icons[icon];
    return empty;
}

const window_palette_t& main_window_t::window_palette() const
{
    return d->settings.palette;
}

void main_window_t::add_view(mainview_provider_t* provider)
{
    view_container_t view;
    view.provider       = provider;
    view.left_widget    = provider->left_widget();
    view.central_widget = provider->central_widget();

    d->views << view;

    d->left_side_widget_container->addWidget(view.left_widget);

    d->content_widget_container->addWidget(view.central_widget);
    d->tab_bar->add_tab(view.central_widget);

    set_current_view(d->views.size() - 1);
}

void main_window_t::add_pane(widget_t* iopane)
{
    d->io_bar->add_pane(iopane->title(), iopane);
    d->io_panes << iopane;

    iopane->setParent(d->io_pane_container);
    d->io_pane_container->addWidget(iopane);
}

void main_window_t::add_status_widget(qwidget* widget)
{
    d->io_bar->add_widget(widget);
}

void main_window_t::set_current_view(int index)
{
    d->tab_bar->set_current_index(index);
}

void main_window_t::on_io_pane_show_request(widget_t* widget)
{
    d->io_pane_container->setVisible(true);
    d->io_pane_container->setCurrentWidget(widget);
}

void main_window_t::on_io_pane_hide_request()
{
    d->io_pane_container->setVisible(false);
}

void main_window_t::on_view_show_request(widget_t* w)
{
    auto iter = std::find_if(d->views.begin(), d->views.end(),
                             [w](const view_container_t& c) { return c.central_widget == w; });

    if (iter != d->views.end()) {
        if (iter->left_widget) {
            d->left_side_widget_container->setVisible(true);
            d->left_side_widget_container->setCurrentWidget(iter->left_widget);
        }
        else
            d->left_side_widget_container->setVisible(false);

        d->content_widget_container->setCurrentWidget(w);
    }
}

void main_window_t::info(const qstring& message)
{
    d->message_pane->add_info(message);
}

void main_window_t::warning(const qstring& message)
{
    d->message_pane->add_warning(message);
}

void main_window_t::error(const qstring& message)
{
    d->message_pane->add_error(message);
}

#define LOAD_ICON(name) load_icon(standard_icon_e::name, #name ".png")

void main_window_t::load_icons()
{
    LOAD_ICON(add);
    LOAD_ICON(clear);
    LOAD_ICON(close);
    LOAD_ICON(copy);
    LOAD_ICON(cut);
    LOAD_ICON(database);
    LOAD_ICON(down_indicator);
    LOAD_ICON(edit);
    LOAD_ICON(error);
    load_icon(standard_icon_e::export_, "export.png");
    LOAD_ICON(home);
    LOAD_ICON(import);
    LOAD_ICON(information);
    LOAD_ICON(left_indicator);
    LOAD_ICON(lock);
    LOAD_ICON(play);
    LOAD_ICON(redo);
    LOAD_ICON(refresh);
    LOAD_ICON(remove);
    LOAD_ICON(right_arrow);
    LOAD_ICON(right_indicator);
    LOAD_ICON(search);
    LOAD_ICON(settings);
    LOAD_ICON(stop);
    LOAD_ICON(trash);
    LOAD_ICON(undo);
    LOAD_ICON(unlock);
    LOAD_ICON(up_indicator);
    LOAD_ICON(warning);
}

void main_window_t::load_icon(standard_icon_e type, const char* file_name)
{
    QFile file(d->settings.icons_path + "/" + file_name);

    if (file.exists()) {
        d->icons.insert(type, QIcon(file.fileName()));
    }
}
