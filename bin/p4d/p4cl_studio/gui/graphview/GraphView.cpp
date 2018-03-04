#include "GraphView.hpp"

#include "GraphEdge.hpp"
#include "GraphNode.hpp"
#include "SceneItemAnimatedDeleter.hpp"

#include <evt/gui/Style.hpp>

#include <QDebug>
#include <QKeyEvent>

using namespace p4s;

struct GraphView::Impl {
    p4cl::compiler::Result cr;
    QGraphicsScene scene;

    std::vector<GraphNode*> nodes;
    std::vector<GraphEdge*> edges;
    QPropertyAnimation centerAnimation;

    qreal nodeWidth = 80 * 1.6, nodeHeight = 80;
};

GraphView::GraphView(QWidget* parent) : QGraphicsView(parent)
{
    setScene(&d->scene);
    applyStyle(evt::appStyle());
    setRenderHint(QPainter::HighQualityAntialiasing);

    d->centerAnimation.setTargetObject(this);
    d->centerAnimation.setPropertyName("center");
    d->centerAnimation.setDuration(1000);
    d->centerAnimation.setEasingCurve(QEasingCurve::InOutCubic);
}

GraphView::~GraphView()
{
}

void GraphView::build(p4cl::compiler::Result r)
{
    const size_t nodeCount = r.pipelineGraph->nodes().size();
    if (nodeCount == 0) return;

    for (auto node : d->nodes) {
        node->inConnections.clear();
        node->outConnections.clear();
    }

    SceneItemAnimatedDeleter::hideAndDelete(d->edges);
    d->edges.clear();

    const size_t currentNodeCount = d->nodes.size();
    if (nodeCount > d->nodes.size()) {
        for (size_t i = 0; i < (nodeCount - currentNodeCount); ++i) {
            auto node = new GraphNode(this);
            node->setSize({d->nodeWidth, d->nodeHeight});
            d->nodes.push_back(node);
            d->scene.addItem(node);
        }
    }
    else if (nodeCount < d->nodes.size()) {
        std::vector<GraphNode*> nodesToDelete;
        for (size_t i = 0; i < (currentNodeCount - nodeCount); ++i) {
            auto node = d->nodes.rbegin();
            nodesToDelete.push_back(*node);
            (*node)->inConnections.clear();
            (*node)->outConnections.clear();
            d->nodes.pop_back();
        }
        SceneItemAnimatedDeleter::hideAndDelete(nodesToDelete);
    }

    std::map<size_t, std::vector<const p4cl::compiler::Node*>> nodesByRank;
    size_t maxRank = 0;
    for (auto n : r.pipelineGraph->nodes()) {
        nodesByRank[n->rank].push_back(n);
        maxRank = std::max(maxRank, n->rank);
    }

    std::map<const p4cl::compiler::Node*, GraphNode*> nodes;

    size_t index = 0;
    size_t maxPerColumn = 0;
    size_t columnCount = maxRank + 1;

    std::vector<size_t> rowCounts;
    rowCounts.resize(columnCount);
    for (size_t i = 0; i <= maxRank; ++i) {
        rowCounts[i] = nodesByRank[i].size();
        maxPerColumn = std::max(maxPerColumn, rowCounts[i]);
    }

    const qreal horizontalSpacing = d->nodeWidth * 1.5;
    const qreal sceneWidth = columnCount * horizontalSpacing;
    const qreal sceneHeight = maxPerColumn * d->nodeHeight * 2;

    QRectF sceneRecangle{-sceneWidth / 2, -sceneHeight / 2, sceneWidth, sceneHeight};

    auto posToGlobal = [sceneWidth, sceneHeight](QPointF pos) -> QPointF {
        return {pos.x() - sceneWidth / 2, pos.y() - sceneHeight / 2};
    };

    for (size_t i = 0; i <= maxRank; ++i) {
        auto& vec = nodesByRank[i];
        size_t count = vec.size();
        const qreal verticalSpacing = sceneHeight / (count + 1);

        const qreal centerX = i * horizontalSpacing + horizontalSpacing / 2;

        int j = 0;
        for (auto nodeData : vec) {
            auto node = d->nodes[index++];
            nodes[nodeData] = node;
            node->setCompilerNode(nodeData);
            const qreal centerY = verticalSpacing + j * verticalSpacing;
            const QPointF position = {centerX - d->nodeWidth / 2,
                                      centerY - d->nodeHeight / 2};

            node->animatePositionTo(posToGlobal(position));

            ++j;
        }
    }

    for (auto c : r.pipelineGraph->connections()) {
        auto from = nodes[c.from];
        auto to = nodes[c.to];
        auto con = new GraphEdge(from, to);

        from->outConnections.push_back(con);
        to->inConnections.push_back(con);

        d->scene.addItem(con);
        d->edges.push_back(con);
    }

    //setSceneRect(sceneRecangle);
    fitInView(sceneRecangle, Qt::KeepAspectRatio);

    d->cr = r;
}

void GraphView::applyStyle(const evt::Style& s)
{
    setBackgroundBrush(s.theme.background.dark);
}

QPointF GraphView::center() const
{
    return sceneRect().center();
}

void GraphView::animateCenter(QPointF from, QPointF to)
{
    d->centerAnimation.setStartValue(from);
    d->centerAnimation.setEndValue(to);
    d->centerAnimation.start();
}

void GraphView::wheelEvent(QWheelEvent* event)
{
    QGraphicsView::wheelEvent(event);

    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        static const qreal zoomInFactor = 1.02;
        static const qreal zoomOutFactor = 1. / zoomInFactor;
        if (event->delta() > 0)
            scale(zoomInFactor, zoomInFactor);
        else
            scale(zoomOutFactor, zoomOutFactor);
    }
}
