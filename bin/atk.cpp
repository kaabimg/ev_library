#include <ev/experimental/atk/core/application.hpp>
#include <ev/experimental/atk/gui/main_window.hpp>
#include <ev/experimental/atk/gui/widget.hpp>

#include <qtextedit.h>
#include <qicon.h>
#include <qlineedit.h>
#include <qapplication.h>
#include <qstyle.h>

using namespace ev;

int tab = 1;

struct view_t : atk::mainview_provider_t {


    QWidget* left_widget() const override
    {
        return new QTextEdit("Left Side "+QString::number(tab));
    }

    atk::widget_t* central_widget() const override
    {
        auto w = atk::widget_t::make_from(new QTextEdit("Content1"));
        w->set_title("tab"+QString::number(tab++));
        w->set_icon(QApplication::style()->standardIcon(QStyle::SP_DirHomeIcon));
        return w;
    }
};


int main(int argc, char* argv[])
{
    atk::application_t app{argc, argv};

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
