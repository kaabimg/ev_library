#include "splitter.hpp"

#include <qpainter.h>

using namespace ev::atk;

class splitter_handle_t : public QSplitterHandle {
public:
    explicit splitter_handle_t(Qt::Orientation orientation, QSplitter* parent = 0)
        : QSplitterHandle(orientation, parent)
    {
        setMask(QRegion(contentsRect()));
        setAttribute(Qt::WA_MouseNoMask, true);
    }

protected:
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
        painter.fillRect(rect(), Qt::black);
    }
};

splitter_t::splitter_t(Qt::Orientation orientation, QWidget* parent)
    : QSplitter(orientation, parent)
{
    setHandleWidth(1);
}

QSplitterHandle* splitter_t::createHandle()
{
    return new splitter_handle_t(orientation(), this);
}
