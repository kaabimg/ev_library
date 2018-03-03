#include "PipelineGraph.hpp"
#include <QVariant>

using namespace p4cl::compiler;

std::string Node::objectNameOrAnonymous() const
{
    QString name = objectName();
    if (name.isEmpty())
        return "<anonymous>";
    else
        return name.toStdString();
}

const Node* Node::parentNode() const
{
    return qobject_cast<const Node*>(parent());
}

int Node::indexInParent() const
{
    if (auto p = qobject_cast<Node*>(parent())) {
        auto iter = std::find(p->children.begin(), p->children.end(), this);
        if (iter != p->children.end()) {
            return std::distance(p->children.begin(), iter);
        }
    }
    return -1;
}

bool ProgramNode::addNode(Node* node)
{
    return addNodePrivate(node);
}

void ProgramNode::setProperty(const std::string& p, const QVariant& val)
{
    QObject::setProperty(p.c_str(), val);
}

void OptionsNode::setProperty(const std::string& p, const QVariant& val)
{
    QObject::setProperty(p.c_str(), val);
}

bool DataNode::addNode(Node* node)
{
    if (qobject_cast<DataNode*>(node)) {
        return addNodePrivate(node);
    }

    return false;
}

void DataNode::setProperty(const std::string& p, const QVariant& val)
{
    QObject::setProperty(p.c_str(), val);
}

bool TaskNode::addNode(Node* node)
{
    if (qobject_cast<DataNode*>(node)) {
        return addNodePrivate(node);
    }
    return false;
}

void TaskNode::setProperty(const std::string& p, const QVariant& val)
{
    if (p == "source") {
        source = val;
        static const auto variantToSupportedNode = [](const QVariant& var) -> Node* {
            if (auto obj = var.value<Node*>()) {
                if (auto node = qobject_cast<TaskNode*>(obj)) {
                    return node;
                }
                else if (auto node = qobject_cast<DataNode*>(obj)) {
                    return node;
                }
                return nullptr;
            }
        };

        std::vector<std::string> errors;

        if (qvariant_cast<QObject*>(source)) {
            auto node = variantToSupportedNode(source);

            if (auto task = qobject_cast<TaskNode*>(node)) {
                if (p4cl::lang::isValidConnection(task->taskType.type, this->taskType.type))
                    sourceTasks.push_back(task);
                else
                    errors.push_back(std::string("Incompatible source task ")
                                         .append(task->taskType.label)
                                         .append(":")
                                         .append(task->objectNameOrAnonymous()));
            }
            else if (auto data = qobject_cast<DataNode*>(node)) {
                if (p4cl::lang::isValidConnection(data->dataType.type, this->taskType.type))
                    sourceData.push_back(data);
                else
                    errors.push_back(std::string("Incompatible source data ")
                                         .append(data->dataType.label)
                                         .append(":")
                                         .append(data->objectNameOrAnonymous()));
            }
            else {
                errors.push_back("Incompatible source type");
            }
        }
        else if (source.type() == QVariant::List) {
            QVariantList list = source.toList();

            for (auto& li : list) {
                auto node = variantToSupportedNode(li);

                if (auto task = qobject_cast<TaskNode*>(node)) {
                    if (p4cl::lang::isValidConnection(task->taskType.type, this->taskType.type))
                        sourceTasks.push_back(task);
                    else
                        errors.push_back(std::string("Incompatible source task ")
                                             .append(task->taskType.label)
                                             .append(":")
                                             .append(task->objectNameOrAnonymous()));
                }
                else if (auto data = qobject_cast<DataNode*>(node)) {
                    if (p4cl::lang::isValidConnection(data->dataType.type, this->taskType.type))
                        sourceData.push_back(data);
                    else
                        errors.push_back(std::string("Incompatible source data ")
                                             .append(data->dataType.label)
                                             .append(":")
                                             .append(data->objectNameOrAnonymous()));
                }
                else {
                    errors.push_back("Incompatible source type");
                }
            }
        }
        else {
            errors.push_back("Incompatible source type");
        }

        if (errors.size()) throw errors;
    }
    else
        throw "Unknwon property";
}

/// Clearly not optimal. But fine for a small graph
size_t PipelineGraph::rankOf(const Node* node)
{
    size_t r = 0;
    for (auto n : inConnectionsOf(node)) {
        r = std::max(r, 1 + rankOf(n.from));
    }
    return r;
}

void PipelineGraph::guessSourcesOf(const TaskNode* task)
{
}

void PipelineGraph::buildFrom(const ProgramNode* program)
{
    for (auto node : program->children) {
        if (auto data = qobject_cast<DataNode*>(node)) {
            _nodes.push_back(data);
        }
        else if (auto task = qobject_cast<TaskNode*>(node)) {
            _nodes.push_back(task);
            for (auto s : task->sourceData) {
                _connections.push_back({s, task});
            }
            for (auto s : task->sourceTasks) {
                _connections.push_back({s, task});
            }
        }
    }

    for (auto& n : _nodes) {
        n->rank = rankOf(n);
    }
}

const std::vector<const Node*>& PipelineGraph::nodes() const
{
    return _nodes;
}

const std::vector<Connection>& PipelineGraph::connections() const
{
    return _connections;
}

std::vector<Connection> PipelineGraph::inConnectionsOf(const Node* node) const
{
    std::vector<Connection> connections;
    for (auto c : _connections) {
        if (c.to == node) connections.push_back(c);
    }
    return connections;
}

std::vector<Connection> PipelineGraph::outConnectionsOf(const Node* node) const
{
    std::vector<Connection> connections;
    for (auto c : _connections) {
        if (c.from == node) connections.push_back(c);
    }
    return connections;
}
