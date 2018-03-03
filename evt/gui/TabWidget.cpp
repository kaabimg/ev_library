#include "TabWidget.hpp"
#include "Style.hpp"
#include "../style/stylesheets/TabWidget.hpp"

#include <QTabBar>

evt::TabWidget::TabWidget(QWidget* parent) : QTabWidget(parent)
{
    setAutoFillBackground(true);
    monitorStyleChangeFor(this);
    applyStyle(appStyle());
}

void evt::TabWidget::applyStyle(const evt::Style& style)
{
    QPalette p = palette();
    p.setColor(QPalette::Background,style.theme.background.normal);
    setPalette(p);
    tabBar()->setPalette(p);
    setStyleSheet(style.adaptStyleSheet(tabWidgetStyleSheet));
}
