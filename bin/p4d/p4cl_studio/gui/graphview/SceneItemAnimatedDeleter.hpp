#pragma once

#include <QPropertyAnimation>
#include <QGraphicsItem>

#include <vector>

namespace p4s {
class GraphEdge;
class GraphNode;

class SceneItemAnimatedDeleter : public QPropertyAnimation {
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
public:
    SceneItemAnimatedDeleter(QObject* parent = nullptr);
    ~SceneItemAnimatedDeleter();

    qreal opacity() const;
    void setOpacity(qreal);

    static void hideAndDelete(const std::vector<GraphEdge*>& targets);
    static void hideAndDelete(const std::vector<GraphNode*>& targets);

private:
    qreal _opacity = 1.0;
    std::vector<QGraphicsItem*> _targets;
    bool _deleteItems = false;
};
}
