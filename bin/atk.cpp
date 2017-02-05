#include <ev/experimental/atk/core/application.hpp>
#include <ev/experimental/atk/core/session.hpp>

#include <ev/experimental/atk/gui/main_window.hpp>
#include <ev/experimental/atk/gui/widget.hpp>
#include <ev/experimental/atk/gui/object_tree_view.hpp>

#include <qtextedit.h>
#include <qicon.h>
#include <qlineedit.h>
#include <qapplication.h>
#include <qstyle.h>
#include <qtreeview.h>
#include <qlistview.h>

using namespace ev;

int tab = 1;

struct view_t : atk::mainview_provider_t {
    QWidget* left_widget() const override
    {
        auto view = atk::widget_t::make_from(new atk::object_tree_view_t(atk_app->session()));
        view->set_title("Objects");
        view->enable_toolbar(true);

        return view;
    }

    atk::widget_t* central_widget() const override
    {
        auto w = atk::widget_t::make_from(new QTextEdit("Content1"));
        w->set_title("tab" + QString::number(tab++));
        w->set_icon(QApplication::style()->standardIcon(QStyle::SP_DirHomeIcon));
        w->enable_toolbar(true);
        return w;
    }
};

int main(int argc, char* argv[])
{
    atk::application_t app{argc, argv};

    auto obj = new atk::object_t(app.session());
    obj->set_name("c1");

    obj = new atk::object_t(obj);
    obj->set_name("c11");

    obj = new atk::object_t(obj);
    obj->set_name("c111");

    obj = new atk::object_t(app.session());
    obj->set_name("c3");

    obj = new atk::object_t(app.session());
    obj->set_name("c2");

    atk::main_window_t main_window;
    main_window.resize(800, 600);
    main_window.show();

    main_window.add_view(new view_t);
    main_window.add_view(new view_t);

    auto pane = atk::widget_t::make_from(new QTextEdit("IO widget"));
    pane->set_title("console");
    main_window.add_pane(pane);


    app.system_interface()->info("info");
    app.system_interface()->warning("warning");
    app.system_interface()->error("error");




    return app.exec();
}
