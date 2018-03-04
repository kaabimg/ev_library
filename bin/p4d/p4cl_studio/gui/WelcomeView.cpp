#include "WelcomeView.hpp"

#include <evt/gui/Style.hpp>

#include <QLabel>

namespace {

inline QString createWelcomeText()
{
    static const QString text = R"(
        <html><body style=color:%0; font-size:20px>
        <div align='center'>
        <div style=font-size:25px>Welcome to P4CL Studio</div>
        <table><tr><td>
        <hr/>
        <div style=margin-top: 5px>&bull; Open File or Directory </div>
        <div style=margin-top: 5px>&bull; Drag and drop files here</div>

        <div style=margin-top: 5px>&bull; p4cl editor</div>
        <div style=margin-left: 1em>- Copy/Cut/Paste : Ctrl+C/X/V</div>
        <div style=margin-left: 1em>- Undo/Redo    : Ctrl+Z/Y</div>
        <div style=margin-left: 1em>- Save : Ctrl+S</div>
        <div style=margin-left: 1em>- Completion     : Ctrl+Space</div>

        <div style=margin-left: 1em>- Create an new netlist : Ctrl+N</div>
        <div style=margin-left: 1em>- Open an existing p4cl file :
        Ctrl+O</div>

        <div style=margin-top: 5px>&bull; Run pipelines</div>

        </td></tr></table>
        </div>
        </body></html>
    )";

    return text;
}
}

struct p4s::WelcomeView::Impl {
    QLabel* label;
};
p4s::WelcomeView::WelcomeView(QObject* parent) : QObject(parent)
{
    d->label = new QLabel();

    QPalette palette = d->label->palette();
    palette.setColor(d->label->backgroundRole(), evt::appStyle().theme.background.dark);
    d->label->setAutoFillBackground(true);
    d->label->setPalette(palette);
    d->label->setText(createWelcomeText().arg(evt::appStyle().theme.foregroundColor.name()));
}

p4s::WelcomeView::~WelcomeView()
{
}

void p4s::WelcomeView::applyStyle(const evt::Style&)
{
}

evt::MainWindowView p4s::WelcomeView::view()
{
    return {evt::appStyle().icon(evt::IconType::Home), "Home", nullptr, d->label};
}
