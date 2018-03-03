#include "ProgramTree.hpp"

using namespace p4cl;
using namespace p4cl::ast;

ProgramTreeBuilder::ProgramTreeBuilder()
{
    _root = std::make_shared<Node>();
    _stack.push(_root.get());
}

std::shared_ptr<const Node> ProgramTreeBuilder::build()
{
    return _root;
}

lang::ProgramNodeType ProgramTreeBuilder::currentNodeType() const
{
    return _stack.top()->nodeType;
}

bool ProgramTreeBuilder::isInScriptNode() const
{
    return _stack.top()->nodeType == lang::ProgramNodeType::Script;
}

bool ProgramTreeBuilder::operator()(const ObjectBegin& obj)
{
    Node* node;
    if (lang::isScriptObject(obj.type.value)) {
        ScriptNode* scriptNode = new ScriptNode;
        scriptNode->position = obj;
        node = scriptNode;
    }
    else {
        ObjectNode* objectNode = new ObjectNode;
        objectNode->setTypeFrom(obj.type);
        objectNode->setIdFrom(obj.id);
        objectNode->position = obj;
        node = objectNode;
    }

    _stack.top()->add(node);
    _stack.push(node);
    return true;
}

bool ProgramTreeBuilder::operator()(const AnonymousObjectBegin& obj)
{
    Node* node;
    if (lang::isScriptObject(obj.type.value)) {
        ScriptNode* scriptNode = new ScriptNode;
        scriptNode->position = obj;
        node = scriptNode;
    }
    else {
        ObjectNode* objectNode = new ObjectNode;
        objectNode->setTypeFrom(obj.type);
        objectNode->position = obj;
        node = objectNode;
    }

    _stack.top()->add(node);
    _stack.push(node);
    return true;
}

bool ProgramTreeBuilder::operator()(const ObjectEnd& endStm)
{
    if (currentNodeType() != lang::ProgramNodeType::Object &&
        currentNodeType() != lang::ProgramNodeType::Script)
        return false;

    auto begin = _stack.top()->position.strView.begin();
    auto end = endStm.strView.end();
    _stack.top()->position.annotate(begin, end);
    _stack.pop();
    return true;
}

bool ProgramTreeBuilder::operator()(const Assignment& assignment)
{
    AssignementNode* assignementNode = new AssignementNode;
    assignementNode->setFrom(assignment.id);
    assignementNode->setFrom(assignment.expression);
    assignementNode->position = assignment;

    _stack.top()->add(assignementNode);
    return true;
}

void ProgramTreeBuilder::addScriptLine(std::string_view line)
{
    ScriptNode* scriptNode = _stack.top()->as<ScriptNode>();

    if (scriptNode->code.begin() == nullptr) {
        scriptNode->code = line;
    }
    else {
        scriptNode->code = std::string_view(scriptNode->code.begin(),
                                            std::distance(scriptNode->code.begin(), line.end()));
    }
}
