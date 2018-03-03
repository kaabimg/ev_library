#pragma once

#include <p4cl/P4CLang.hpp>
#include <p4cl/parser/Ast.hpp>

#include <vector>
#include <stack>
#include <memory>

namespace p4cl {
namespace ast {

struct Node {
    virtual ~Node()
    {
        for (auto c : children) delete c;
    }

public:
    Node* parent = nullptr;
    lang::ProgramNodeType nodeType = lang::ProgramNodeType::Program;
    p4cl::TaggedPosition position;
    std::vector<Node*> children;

public:
    void add(Node* n)
    {
        children.push_back(n);
        n->parent = this;
    }

    template <typename T>
    const T* as() const
    {
        return reinterpret_cast<const T*>(this);
    }

    template <typename T>
    T* as()
    {
        return reinterpret_cast<T*>(this);
    }
};

struct IdNode : Node {
    IdNode()
    {
        nodeType = lang::ProgramNodeType::Identifier;
    }

    void setFrom(const ast::Identifier& s)
    {
        position = (id = s);
    }

public:
    ast::Identifier id;
};

struct ExpressionNode : Node {
    ExpressionNode()
    {
        nodeType = lang::ProgramNodeType::Expression;
    }

    void setFrom(const ast::Expression& s)
    {
        position = (expression = s);
    }

public:
    ast::Expression expression;
};

struct AssignementNode : Node {
    AssignementNode()
    {
        nodeType = lang::ProgramNodeType::Assignement;
    }

    void setFrom(const ast::Identifier& s)
    {
        id = std::make_unique<IdNode>();
        id->parent = this;
        id->setFrom(s);
    }

    void setFrom(const ast::Expression& s)
    {
        expression = std::make_unique<ExpressionNode>();
        expression->parent = this;
        expression->setFrom(s);
    }

    std::unique_ptr<IdNode> id;
    std::unique_ptr<ExpressionNode> expression;
};

struct ObjectNode : Node {
    ObjectNode()
    {
        nodeType = lang::ProgramNodeType::Object;
    }

    void setIdFrom(const ast::Identifier& s)
    {
        id = std::make_unique<IdNode>();
        id->parent = this;
        id->setFrom(s);
    }
    void setTypeFrom(const ast::Identifier& s)
    {
        type = std::make_unique<IdNode>();
        type->parent = this;
        type->setFrom(s);
    }

public:
    std::unique_ptr<IdNode> type;
    std::unique_ptr<IdNode> id;
};

struct ScriptNode : Node {
    ScriptNode()
    {
        nodeType = lang::ProgramNodeType::Script;
    }
    std::string_view code;
};

class ProgramTreeBuilder : public boost::static_visitor<bool> {
public:
    ProgramTreeBuilder();

    std::shared_ptr<const Node> build();

    lang::ProgramNodeType currentNodeType() const;
    bool isInScriptNode() const;

    bool operator()(const ast::ObjectBegin&);
    bool operator()(const ast::AnonymousObjectBegin& obj);
    bool operator()(const ast::ObjectEnd&);
    bool operator()(const ast::Assignment&);

    void addScriptLine(std::string_view line);

private:
    std::shared_ptr<Node> _root;
    std::stack<Node*> _stack;
};
}
}
