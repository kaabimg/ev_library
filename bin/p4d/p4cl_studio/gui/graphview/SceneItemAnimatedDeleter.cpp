#include "SceneItemAnimatedDeleter.hpp"
#include "GraphEdge.hpp"
#include "GraphNode.hpp"

using namespace p4s;

SceneItemAnimatedDeleter::SceneItemAnimatedDeleter(QObject* parent) : QPropertyAnimation(parent)
{
    setTargetObject(this);
    setPropertyName("opacity");
    setDuration(1000);
    setEasingCurve(QEasingCurve::InOutCubic);
    connect(this, &SceneItemAnimatedDeleter::finished, this,
            &SceneItemAnimatedDeleter::deleteLater);
}

SceneItemAnimatedDeleter::~SceneItemAnimatedDeleter()
{
    if (_deleteItems) {
        for (auto i : _targets) {
            delete i;
        }
    }
}

qreal SceneItemAnimatedDeleter::opacity() const
{
    return _opacity;
}

void SceneItemAnimatedDeleter::setOpacity(qreal val)
{
    _opacity = val;
    for (auto t : _targets) t->setOpacity(val);
}

void SceneItemAnimatedDeleter::hideAndDelete(const std::vector<GraphEdge*>& targets)
{
    SceneItemAnimatedDeleter* animator = new SceneItemAnimatedDeleter;
    animator->setStartValue(1.);
    animator->setEndValue(0.0);
    for(auto i : targets)
        animator->_targets.push_back(i);
    animator->_deleteItems = true;
    animator->start();
}

void p4s::SceneItemAnimatedDeleter::hideAndDelete(const std::vector<p4s::GraphNode*>& targets)
{
    SceneItemAnimatedDeleter* animator = new SceneItemAnimatedDeleter;
    animator->setStartValue(1.);
    animator->setEndValue(0.0);
    for(auto i : targets)
        animator->_targets.push_back(i);
    animator->_deleteItems = true;
    animator->start();
}
