#include "MainWindow.hpp"
#include "SplitWidget.hpp"
#include "OutputPaneWidget.hpp"
#include "Style.hpp"
#include "TabWidget.hpp"
#include "FileSystemTree.hpp"
#include "OutputPaneWidget.hpp"

#include "../style/stylesheets/MainWindow.hpp"
#include "../style/stylesheets/Button.hpp"

#include "../core/File.hpp"

#include <QToolBox>
#include <QDir>
#include <QToolBar>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QTextEdit>
#include <QStackedWidget>
#include <QStyle>
#include <QPushButton>

#include <QDebug>

using namespace evt;

struct MainWindow::Impl {
    struct View {
        uint id = 0;
        QWidget* left;
        QWidget* central;
    };

    uint viewId = 0;
    std::vector<View> views;

    QToolBar* mainBar;
    SplitWidget* centralWidget;
    QStatusBar* statusBar;

    struct ViewWidget {
        QStackedWidget* left;
        QStackedWidget* content;
    } currentView;

    OutputPaneWidget* ouptutWidget;
    QPushButton* enableCurrentViewLeftWidget;
    QPushButton* enableOutputPaneButton;
};

MainWindow::MainWindow()
{
    QWidget* centralWidgetContainer = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidgetContainer);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    d->mainBar = new QToolBar(centralWidgetContainer);
    d->mainBar->setOrientation(Qt::Vertical);

    SplitWidget* currentViewWidget = new SplitWidget(Qt::Horizontal, centralWidgetContainer);
    d->currentView.left = new QStackedWidget(currentViewWidget);
    d->centralWidget = new SplitWidget(Qt::Vertical, currentViewWidget);

    mainLayout->addWidget(d->mainBar);
    mainLayout->addWidget(currentViewWidget);

    d->currentView.content = new QStackedWidget(d->centralWidget);
    d->ouptutWidget = new OutputPaneWidget(d->centralWidget);

    setCentralWidget(centralWidgetContainer);

    d->statusBar = new QStatusBar(this);
    setStatusBar(d->statusBar);

    d->enableCurrentViewLeftWidget =
        new QPushButton(appStyle().icon(IconType::SideBar), "", d->statusBar);

    d->enableCurrentViewLeftWidget->setCheckable(true);
    d->enableCurrentViewLeftWidget->setChecked(true);
    connect(d->enableCurrentViewLeftWidget, &QPushButton::toggled, d->currentView.left,
            &QStackedWidget::setVisible);

    d->enableOutputPaneButton =
        new QPushButton(appStyle().icon(IconType::Message), "", d->statusBar);
    d->enableOutputPaneButton->setCheckable(true);
    d->enableOutputPaneButton->setChecked(true);

    connect(d->enableOutputPaneButton, &QPushButton::toggled, d->ouptutWidget,
            &QStackedWidget::setVisible);

    d->statusBar->addWidget(d->enableCurrentViewLeftWidget);
    d->statusBar->addWidget(d->enableOutputPaneButton);

    d->centralWidget->setSizes(QList<int>() << 1000 << 200);
    currentViewWidget->setSizes(QList<int>() << 200 << 1000);

    applyStyle(appStyle());
    monitorStyleChangeFor(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::applyStyle(const Style& style)
{
    setFont(style.theme.font);
    setStyleSheet(style.adaptStyleSheet(mainwindowStyleSheet));
    d->mainBar->setFixedWidth(style.sizes.mainBarWidth);
    d->mainBar->setIconSize(
        QSize(style.sizes.mainBarWidth * 0.7, style.sizes.mainBarWidth * 0.7));

    d->enableOutputPaneButton->setStyleSheet(style.adaptStyleSheet(statusBarButtonStyleSheet));
    d->enableOutputPaneButton->setIcon(style.icon(IconType::Message));

    d->enableCurrentViewLeftWidget->setStyleSheet(
        style.adaptStyleSheet(statusBarButtonStyleSheet));
    d->enableCurrentViewLeftWidget->setIcon(style.icon(IconType::SideBar));

    d->mainBar->setStyleSheet(style.adaptStyleSheet(toolBarStyleSheet));
    d->statusBar->setStyleSheet(style.adaptStyleSheet(statusBarStyleSheet));
}

void MainWindow::addView(const MainWindowView& view)
{
    uint id = d->views.size();

    d->mainBar->addAction(view.icon, view.label, this, [id, this] { setCurrentView(id); });
    d->views.push_back({id, view.left, view.central});

    if (view.left) d->currentView.left->addWidget(d->views[id].left);
    if (view.central) d->currentView.content->addWidget(d->views[id].central);

    setCurrentView(id);
}

void MainWindow::setCurrentView(uint id)
{
    if(id>= d->views.size())
        return;

    if (d->views[id].left) {
        d->currentView.left->setVisible(true);
        d->currentView.left->setCurrentWidget(d->views[id].left);
    }
    else {
        d->currentView.left->setVisible(false);
    }

    if (d->views[id].central) {
        d->currentView.content->setVisible(true);
        d->currentView.content->setCurrentWidget(d->views[id].central);
    }
    else {
        d->currentView.content->setVisible(false);
    }
}
