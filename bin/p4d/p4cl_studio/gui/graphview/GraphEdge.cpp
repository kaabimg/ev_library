#include "GraphEdge.hpp"
#include "GraphNode.hpp"

#include <QPen>

p4s::GraphEdge::GraphEdge(const p4s::GraphNode* from, const p4s::GraphNode* to)
    : QGraphicsPathItem(), _from(from), _to(to)
{
    adjustPosition();
    setZValue(-1);


    QColor color("#0088FF");
    color.setAlpha(100);
    QPen pen;
    pen.setWidth(2);
    pen.setBrush(color);
    setPen(pen);

}

void p4s::GraphEdge::adjustPosition()
{
    setPath(shape());
}

inline std::pair<QPointF, QPointF> controlPointsOf(const QPointF& fromPos, const QPointF& toPos)
{
    double xDistance = toPos.x() - fromPos.x();
    // double yDistance = _in.y() - _out.y() - 100;

    double defaultOffset = 200;

    double minimum = qMin(defaultOffset, std::abs(xDistance));

    double verticalOffset = 0;

    double ratio1 = 0.5;

    if (xDistance <= 0) {
        verticalOffset = -minimum;

        ratio1 = 1.0;
    }

    QPointF c1(fromPos.x() + minimum * ratio1, fromPos.y() + verticalOffset);

    QPointF c2(toPos.x() - minimum * ratio1, toPos.y() + verticalOffset);

    return std::make_pair(c1, c2);
}

QPainterPath p4s::GraphEdge::shape() const
{
    QPointF fromPos = _from->globalOutPortPosition();
    QPointF toPos = _to->globalInPortPosition();
    QPainterPath path(fromPos);

    auto controlPoints = controlPointsOf(fromPos, toPos);
    path.cubicTo(controlPoints.first, controlPoints.second, toPos);

    return path;
}
