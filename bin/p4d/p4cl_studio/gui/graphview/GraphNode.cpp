#include "GraphNode.hpp"
#include "GraphEdge.hpp"
#include "GraphView.hpp"

#include "../UiUtils.hpp"

#include <QPainter>
#include <QDebug>

using namespace p4s;

GraphNode::GraphNode(GraphView* view) : QObject(),_view(view)
{
    _isPressed = false;
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);

    _positionAnimation = new QPropertyAnimation(this, "position", this);
    _positionAnimation->setDuration(1500);
    _positionAnimation->setEasingCurve(QEasingCurve::InOutCubic);
}

QRectF GraphNode::boundingRect() const
{
    return QRectF(0, 0, _size.width() + _portRadius, _size.height() + _portRadius);
}

QPointF GraphNode::globalInPortPosition() const
{
    return mapToScene(_portRadius, boundingRect().height() / 2);
}

QPointF GraphNode::globalOutPortPosition() const
{
    return mapToScene(boundingRect().width() - _portRadius, boundingRect().height() / 2);
}

void GraphNode::animatePositionTo(QPointF pos)
{
    _positionAnimation->setStartValue(this->pos());
    _positionAnimation->setEndValue(pos);
    _positionAnimation->start();
}

void GraphNode::setCompilerNode(const p4cl::compiler::Node* node)
{
    _compilerNode = node;
    _text = ui::nodeType(node->astNode) + "\n" + ui::nodeName(node->astNode);
    _backgroundColor = ui::nodeColor(node);
}

void GraphNode::setSize(const QSizeF& s)
{
    _size = s;
}

QPointF GraphNode::inPortPosition() const
{
    return {0, boundingRect().height() / 2 - _portRadius};
}

QPointF GraphNode::outPortPosition() const
{
    return {boundingRect().width() - 2 * _portRadius, boundingRect().height() / 2 - _portRadius};
}

void GraphNode::paint(QPainter* painter,
                      const QStyleOptionGraphicsItem* /*option*/,
                      QWidget* /*widget*/)
{
    QRectF rect = boundingRect();
    rect.adjust(_portRadius, _portRadius, -_portRadius, -_portRadius);

    painter->setRenderHint(QPainter::HighQualityAntialiasing);

    {
        QPainterPath path;
        path.addRoundedRect(rect, 5, 5);
        auto color = _backgroundColor;
        color.setAlpha(100);
        painter->fillPath(path, color);

        if (_isPressed) {
            QPen pen(_borderColor, 2);
            painter->setPen(pen);
            painter->drawPath(path);
        }
    }

    {
        QPainterPath path;
        path.addRoundedRect(QRectF{0, 0, _portRadius * 2, _portRadius * 2}, _portRadius,
                            _portRadius);

        if (!inConnections.empty()) {
            painter->save();
            painter->translate(inPortPosition());
            painter->fillPath(path, _portColor);
            painter->restore();
        }
        if (!outConnections.empty()) {
            painter->save();
            painter->translate(outPortPosition());
            painter->fillPath(path, _portColor);
            painter->restore();
        }
    }

    rect.adjust(_portRadius, _portRadius, -_portRadius, -_portRadius);

    QPen pen;
    pen.setBrush(ui::codeEditorTextColor());

    auto flags = Qt::AlignVCenter | Qt::AlignHCenter;
    QFont font = ui::codeEditorFont();
    QRect fontBoundRect;
    fontBoundRect = painter->fontMetrics().boundingRect(rect.toRect(), flags, _text);
    while (rect.width() <= fontBoundRect.width() || rect.height() <= fontBoundRect.height()) {
        font.setPointSizeF(font.pointSizeF() * 0.95);
        painter->setFont(font);
        fontBoundRect = painter->fontMetrics().boundingRect(rect.toRect(), flags, _text);
    }
    painter->setPen(pen);
    painter->drawText(rect, flags, _text);
}

void GraphNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    _isPressed = true;
    update();
    QGraphicsItem::mousePressEvent(event);
}

void GraphNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    _isPressed = false;
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void GraphNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseDoubleClickEvent(event);
    _view->gotoNodeRequest(_compilerNode->astNode);
}

QVariant GraphNode::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged) {
        for (auto c : inConnections) c->adjustPosition();
        for (auto c : outConnections) c->adjustPosition();
    }
    return QGraphicsItem::itemChange(change, value);
}
