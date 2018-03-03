#include "Console.hpp"
#include "Style.hpp"
#include "Application.hpp"

#include "../core/ApplicationScriptEngine.hpp"
#include "../style/stylesheets/TreeView.hpp"
#include "../style/stylesheets/LineEdit.hpp"

#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>

using namespace evt;
struct Console::Impl {
    QLineEdit* commandLine;
    QListWidget* output;
    std::vector<QString> history;
    int currentCommandIndex = -1;
};

evt::Console::Console(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    d->output = new QListWidget(this);
    //_impl->output->setHeaderHidden(true);
    d->commandLine = new QLineEdit(this);

    layout->addWidget(d->output);
    layout->addWidget(d->commandLine);

    d->commandLine->setClearButtonEnabled(true);
    d->commandLine->addAction(appStyle().icon(IconType::ConsoleArrow),
                                  QLineEdit::LeadingPosition);

    d->commandLine->installEventFilter(this);

    connect(d->commandLine, &QLineEdit::returnPressed, [this] {
        QString str = d->commandLine->text();
        if (!str.isEmpty()) {
            d->commandLine->clear();
            d->history.push_back(str);
            d->currentCommandIndex = -1;
            Q_EMIT execRequest(str);
        }
    });

    //_impl->output->setAutoFillBackground(true);
    monitorStyleChangeFor(this);
    applyStyle(appStyle());

    QObject::connect(this, &evt::Console::execRequest, &app()->scriptEngine(),
                     &evt::ApplicationScriptEngine::evaluate);
    QObject::connect(&app()->scriptEngine(), &evt::ApplicationScriptEngine::resultReady, this,
                     &evt::Console::addOutput);
}

Console::~Console()
{
}

void evt::Console::applyStyle(const evt::Style& style)
{
    d->output->setStyleSheet(style.adaptStyleSheet(listViewStyleSheet));
    d->commandLine->setStyleSheet(style.adaptStyleSheet(lineEditStyleSheet));
}

void evt::Console::addOutput(const QString& cmd, const LogMessage& msg)
{
    static const int itemHeight = 25; // TODO: move to settings
    QListWidgetItem* item = new QListWidgetItem(d->output);
    item->setIcon(appStyle().icon(IconType::ConsoleArrow));
    item->setText(cmd);
    auto font = item->font();
    font.setBold(true);
    item->setFont(font);
    item->setSizeHint(QSize(item->sizeHint().width(), itemHeight));

    if (!msg.msg.empty()) {
        item = new QListWidgetItem(d->output);
        item->setText(msg.msg.c_str());
        item->setSizeHint(QSize(item->sizeHint().width(), itemHeight));
        item->setIcon(appStyle().icon(IconType::ConsoleOutputArrow));
    }

    d->output->setCurrentItem(item);
}

bool Console::eventFilter(QObject* target, QEvent* event)
{
    if (target == d->commandLine) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (d->history.size()) {
                if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down) {
                    if (d->currentCommandIndex == -1) {
                        d->currentCommandIndex = (int)d->history.size() - 1;
                    }
                    else if (keyEvent->key() == Qt::Key_Up) {
                        d->currentCommandIndex = std::max(0, d->currentCommandIndex - 1);
                    }
                    else {
                        d->currentCommandIndex = std::min((int)d->history.size() - 1,
                                                              d->currentCommandIndex + 1);
                    }

                    if (d->currentCommandIndex > -1 &&
                        d->currentCommandIndex < (int)d->history.size()) {
                        d->commandLine->setText(d->history[d->currentCommandIndex]);
                    }
                }
            }
        }
    }
    return QWidget::eventFilter(target, event);
}
