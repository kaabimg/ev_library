#pragma once

#include <ev/core/pimpl.hpp>
#include <p4cl/compiler/Compiler.hpp>
#include <QGraphicsView>

namespace evt {
class Style;
}

namespace p4cl {
namespace ast {
class Node;
}
}

namespace p4s {

class GraphView : public QGraphicsView {
    Q_OBJECT
    Q_PROPERTY(QPointF center READ center WRITE centerOn)
public:
    GraphView(QWidget* parent = nullptr);
    ~GraphView();

    void build(p4cl::compiler::Result r);
    void applyStyle(const evt::Style&);

    QPointF center() const;

Q_SIGNALS:
    void gotoNodeRequest(const p4cl::ast::Node*);

private:
    void animateCenter(QPointF, QPointF);
    void wheelEvent(QWheelEvent* event);

private:
    class Impl;
    ev::pimpl<Impl> _impl;
};
}
