#include <ev/experimental/atk/core/application.hpp>
#include <ev/experimental/atk/core/session.hpp>

#include <ev/experimental/atk/gui/main_window.hpp>
#include <ev/experimental/atk/gui/widget.hpp>
#include <ev/experimental/atk/gui/object_tree_model.hpp>

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
        auto tree_view = new QListView;
        auto model     = new atk::object_model_t(atk_app->session(), tree_view);
        tree_view->setModel(model);
        return tree_view;
    }

    atk::widget_t* central_widget() const override
    {
        auto w = atk::widget_t::make_from(new QTextEdit("Content1"));
        w->set_title("tab" + QString::number(tab++));
        w->set_icon(QApplication::style()->standardIcon(QStyle::SP_DirHomeIcon));
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

    main_window.add_status_widget(new QLineEdit);
    return app.exec();
}
