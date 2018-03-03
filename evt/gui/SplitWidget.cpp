#include "SplitWidget.hpp"
#include "Style.hpp"

#include <QPainter>

namespace {
class SplitterHandle : public QSplitterHandle {
public:
    explicit SplitterHandle(Qt::Orientation orientation, QSplitter* parent = nullptr)
        : QSplitterHandle(orientation, parent)
    {
        setMask(QRegion(contentsRect()));
        setAttribute(Qt::WA_MouseNoMask, true);
    }

    void resizeEvent(QResizeEvent* event)
    {
        if (orientation() == Qt::Horizontal)
            setContentsMargins(2, 0, 2, 0);
        else
            setContentsMargins(0, 2, 0, 2);
        setMask(QRegion(contentsRect()));

        QSplitterHandle::resizeEvent(event);
    }

    void paintEvent(QPaintEvent*)
    {
        QPainter painter(this);
        painter.fillRect(rect(), color);
    }
    QColor color = Qt::yellow;
};
}

evt::SplitWidget::SplitWidget(Qt::Orientation orientation, QWidget* parent)
    : QSplitter(orientation, parent)
{
    setHandleWidth(1);

    monitorStyleChangeFor(this);
    applyStyle(appStyle());
}

QSplitterHandle* evt::SplitWidget::createHandle()
{
    auto h = new SplitterHandle(orientation(), this);
    h->color = appStyle().theme.seperatorColor;
    return h;
}

void evt::SplitWidget::applyStyle(const evt::Style& style)
{
    for (int i = 0; i < count(); ++i) {
        auto h = reinterpret_cast<SplitterHandle*>(handle(i));
        h->color = style.theme.seperatorColor;
        h->update();
    }
}
