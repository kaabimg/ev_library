#pragma once

#include <QGraphicsItem>
#include <QPropertyAnimation>

class QPainter;

namespace p4cl {
namespace compiler {
class Node;
}
}

namespace p4s {

class GraphEdge;
class GraphView;

class GraphNode : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_PROPERTY(QPointF position READ pos WRITE setPos)
public:
    GraphNode(GraphView* view);

    QRectF boundingRect() const;

    QPointF globalInPortPosition() const;
    QPointF globalOutPortPosition() const;

    void animatePositionTo(QPointF);

    std::vector<GraphEdge*> inConnections, outConnections;

    void setCompilerNode(const p4cl::compiler::Node*);
    void setSize(const QSizeF& s);

private:
    const p4cl::compiler::Node* _compilerNode;
    QColor _backgroundColor = "#363636";
    QColor _portColor = "#3b73cb";
    QString _text;

    QColor _borderColor = "#bcbbbc";
    QSizeF _size = {100, 65};
    qreal _portRadius = 5;

private:
    QPointF inPortPosition() const;
    QPointF outPortPosition() const;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);

private:
    bool _isPressed;
    QPropertyAnimation* _positionAnimation;
    GraphView* _view;
};
}
