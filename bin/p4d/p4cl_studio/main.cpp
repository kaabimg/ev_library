#include <evt/gui/MainWindow.hpp>
#include <evt/gui/Application.hpp>
#include <evt/gui/Style.hpp>

#include "gui/WelcomeView.hpp"
#include "gui/EditView.hpp"
#include "gui/Theme.hpp"

#include "app/AppScriptObject.hpp"

int main(int argc, char** argv)
{
    evt::Application app(argc, argv);

    evt::Style::instance.initCustomData(p4s::Theme::createDefault());

    evt::MainWindow mw;

    p4s::EditView ev;
    p4s::WelcomeView wv;

    p4s::AppScriptObject appScriptObject;
    app.scriptEngine().registerObject("app", &appScriptObject);
    appScriptObject.editView = &ev;

    mw.addView(wv.view());
    mw.addView(ev.view());
    mw.setCurrentView(0);
    mw.showMaximized();

    return app.exec();
}
