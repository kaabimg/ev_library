#pragma once
#include <QGraphicsPathItem>

class QPainter;

namespace p4s {
class GraphNode;

class GraphEdge : public QGraphicsPathItem {
public:
    GraphEdge(const GraphNode* from, const GraphNode* to);

    void adjustPosition();

    QPainterPath shape() const;

private:
    const GraphNode* _from;
    const GraphNode* _to;
};
}
