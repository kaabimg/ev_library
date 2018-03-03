#pragma once

#include <p4cl/P4CLang.hpp>
#include <p4cl/parser/ProgramTree.hpp>

#include <QObject>
#include <QVariant>
#include <QJSValue>

namespace p4cl {
namespace compiler {

class Node : public QObject {
    Q_OBJECT
public:
    Node(Node* parent = nullptr) : QObject(parent)
    {
    }
    virtual bool addNode(Node*) = 0;
    virtual void setProperty(const std::string&, const QVariant&) = 0;
    virtual std::string objectNameOrAnonymous() const;

    const Node* parentNode() const;
    int indexInParent() const;

    void applyVisitor(auto&& f) const
    {
        f(this);
        for (auto c : children) {
            c->applyVisitor(f);
        }
    }

public:
    const ast::Node* astNode = nullptr;
    std::vector<Node*> children;
    mutable size_t rank = 0;

protected:
    bool addNodePrivate(Node* node)
    {
        children.push_back(node);
        node->setParent(this);
        return true;
    }
};

class ScriptNode : public Node {
    Q_OBJECT
public:
    ScriptNode(Node* parent = nullptr) : Node(parent)
    {
    }
    bool addNode(Node*) final
    {
        return false;
    }

    void setProperty(const std::string& p, const QVariant& val)
    {
        QObject::setProperty(p.c_str(), val);
    }
};

class OptionsNode : public Node {
    Q_OBJECT
public:
    OptionsNode(Node* parent = nullptr) : Node(parent)
    {
    }
    bool addNode(Node*) final
    {
        return false;
    }

    void setProperty(const std::string& p, const QVariant& val) final;
};

class DataNode : public Node {
    Q_OBJECT
public:
    DataNode(Node* parent = nullptr) : Node(parent)
    {
    }
    bool addNode(Node* astNode) final;
    void setProperty(const std::string& p, const QVariant& val) final;

    lang::DataType dataType;
};

class TaskNode : public Node {
    Q_OBJECT
    Q_PROPERTY(QVariant source MEMBER source)
public:
    TaskNode(Node* parent = nullptr) : Node(parent)
    {
    }
    bool addNode(Node* astNode) final;
    void setProperty(const std::string& p, const QVariant& val) final;

    lang::TaskType taskType;
    QVariant source;

    std::vector<const DataNode*> sourceData;
    std::vector<const TaskNode*> sourceTasks;
};

class ProgramNode : public Node {
    Q_OBJECT
public:
    ProgramNode(Node* parent = nullptr) : Node(parent)
    {
    }
    bool addNode(Node* astNode) final;
    void setProperty(const std::string& p, const QVariant& val) final;
};

struct Connection {
    const Node* from;
    const Node* to;
};

class PipelineGraph {
public:
    void buildFrom(const ProgramNode*);

    const std::vector<const Node*>& nodes() const;
    const std::vector<Connection>& connections() const;

    std::vector<Connection> inConnectionsOf(const Node*) const;
    std::vector<Connection> outConnectionsOf(const Node*) const;

private:
    size_t rankOf(const Node*);
    void guessSourcesOf(const TaskNode*);

private:
    std::vector<const Node*> _nodes;
    std::vector<Connection> _connections;
};
}
}
