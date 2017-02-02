#include "main_window.hpp"
#include "main_window/io_bar.hpp"
#include "main_window/tab_bar.hpp"
#include "widgets/splitter.hpp"
#include "widget.hpp"
#include "../preprocessor.hpp"

#include <qlayout.h>
#include <qtextedit.h>
#include <qstackedwidget.h>
#include <qfile.h>
#include <qapplication.h>

namespace ev {
namespace atk {
ATK_DECLARE_ENUM_QHASH(standard_icon_e)
}
}

using namespace ev::atk;

struct view_container_t {
    mainview_provider_t* provider = nullptr;
    qwidget* left_widget          = nullptr;
    widget_t* central_widget      = nullptr;
};


struct main_window_t::impl_t {
    QHBoxLayout* main_layout;
    QVBoxLayout* content_layout;
    tab_bar_t* tab_bar;
    io_bar_t* io_bar;
    splitter_t *horizontal_splitter, *vertical_splitter;

    QStackedWidget *left_side_widget_container, *content_widget_container, *io_pane_container;

    qvector<view_container_t> views;
    qvector<widget_t*> io_panes;
    main_window_settings_t settings;
    qhash<standard_icon_e, qicon> icons;
    static main_window_t* instance;
};

main_window_t* main_window_t::impl_t::instance = nullptr;

main_window_t::main_window_t(const main_window_settings_t& settings)
    : qwidget{nullptr}, d{new impl_t}
{
    impl_t::instance = this;

    d->settings = settings;
    load_icons();

    d->main_layout    = new QHBoxLayout(this);
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
    d->io_bar->setFixedHeight(window_sizes().header_height);
    d->content_layout->addWidget(d->io_bar);

    ////
    d->horizontal_splitter->addWidget(d->left_side_widget_container);
    d->vertical_splitter->addWidget(d->content_widget_container);
    d->vertical_splitter->addWidget(d->io_pane_container);
    d->horizontal_splitter->addWidget(d->vertical_splitter);

    ////
    d->io_pane_container->setVisible(false);

    ////

    QFile style_file(":/main_style.qss");
    style_file.open(QFile::ReadOnly);

    qstring ss          = style_file.readAll();
    const auto& palette = d->settings.palette;

    ss = ss.arg(palette.dark.name(), palette.dark_gray.name(), palette.light_gray.name(),
                palette.light.name(), palette.primary.name(), palette.secondary.name());

    qApp->setStyleSheet(ss);
    style_file.close();
    ////
    connect(ATK_SIGNAL(d->io_bar, show_request), ATK_SLOT(this, on_io_pane_show_request));
    connect(ATK_SIGNAL(d->io_bar, hide_request), ATK_SLOT(this, on_io_pane_hide_request));
}

main_window_t::~main_window_t()
{
    delete d;
}

void main_window_t::load_icons()
{
}

main_window_t* main_window_t::instance()
{
    return impl_t::instance;
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

    d->left_side_widget_container->addWidget(view.left_widget);



    d->content_widget_container->addWidget(view.central_widget);
    d->tab_bar->insertTab(view.central_widget);
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

void main_window_t::on_io_pane_show_request(widget_t* widget)
{
    d->io_pane_container->setVisible(true);
    d->io_pane_container->setCurrentWidget(widget);
}

void main_window_t::on_io_pane_hide_request()
{
    d->io_pane_container->setVisible(false);
}
