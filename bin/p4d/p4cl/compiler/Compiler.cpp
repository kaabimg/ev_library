#include "Compiler.hpp"
#include <stack>

#include <QQmlEngine>
#include <QDebug>

using namespace p4cl;

namespace {

QString toQString(std::string_view v)
{
    return QString::fromLatin1(v.begin(), v.size());
}

struct CompilationContext {
    CompilationContext(compiler::Result& r) : result(r)
    {
    }
    bool atProgramLevel() const
    {
        return objectStack.top() == program.get();
    }

    void addObject(const QString& name, QObject* obj)
    {
        engine.globalObject().setProperty(name, engine.newQObject(obj));
    }

    void addCompilationError(const ast::Node* node, std::string msg)
    {
        result.log->push_back(
            {p4cl::lang::CompilationMessage::CompilationError, node->position, std::move(msg)});
    }

    void addEvaluationError(const ast::Node* node, std::string msg)
    {
        result.log->push_back(
            {p4cl::lang::CompilationMessage::EvaluationError, node->position, std::move(msg)});
    }

    void addWarning(const ast::Node* node, std::string msg)
    {
        result.log->push_back(
            {p4cl::lang::CompilationMessage::Warning, node->position, std::move(msg)});
    }

public:
    compiler::Result& result;
    QQmlEngine engine;
    std::stack<compiler::Node*> objectStack;
    std::shared_ptr<compiler::ProgramNode> program;
    lang::CompilationMode mode;

    std::vector<compiler::TaskNode*> highLevelTaskNodes;
    std::vector<compiler::DataNode*> highLevelDataNodes;
};

void compileNode(const ast::Node* node, CompilationContext& context);

inline void compileDataNode(const ast::ObjectNode* object, CompilationContext& context)
{
    std::string_view type = object->type->position.strView;
    auto dataType = lang::findDataType(type);

    compiler::DataNode* data = new compiler::DataNode(context.objectStack.top());

    if (!context.objectStack.top()->addNode(data)) {
        context.addCompilationError(object, lang::errors::invalidHierarchy);
        delete data;
        return;
    }

    if (object->id) {
        data->setObjectName(object->id->id.value.c_str());
        context.addObject(object->id->id.value.c_str(), data);
    }

    data->astNode = object;
    data->dataType = dataType;

    if (context.atProgramLevel()) context.highLevelDataNodes.push_back(data);

    context.objectStack.push(data);
    for (auto n : object->children) compileNode(n, context);
    context.objectStack.pop();
}

inline void compileTaskNode(const ast::ObjectNode* object, CompilationContext& context)
{
    std::string_view type = object->type->position.strView;
    auto taskType = lang::findTaskType(type);

    compiler::TaskNode* task = new compiler::TaskNode(context.objectStack.top());

    if (!context.objectStack.top()->addNode(task)) {
        context.addCompilationError(object, lang::errors::invalidHierarchy);
        delete task;
        return;
    }

    if (object->id) {
        task->setObjectName(object->id->id.value.c_str());
        context.addObject(object->id->id.value.c_str(), task);
    }

    task->astNode = object;
    task->taskType = taskType;

    if (context.atProgramLevel()) context.highLevelTaskNodes.push_back(task);

    context.objectStack.push(task);
    for (auto n : object->children) compileNode(n, context);
    context.objectStack.pop();
}

inline void compileObject(const ast::ObjectNode* object, CompilationContext& context)
{
    std::string_view type = object->type->position.strView;

    if (lang::findDataType(type)) {
        compileDataNode(object, context);
    }
    else if (lang::findTaskType(type)) {
        compileTaskNode(object, context);
    }
    else {
        context.addCompilationError(object, lang::errors::unknownDataType);
    }
}

inline void compileAssignment(const ast::AssignementNode* assignement, CompilationContext& context)
{
    QJSValue value = context.engine.evaluate(toQString(assignement->expression->position.strView));

    if (value.isError()) {
        context.addEvaluationError(assignement, value.toString().toStdString());
        return;
    }

    if (context.atProgramLevel()) {
        context.engine.globalObject().setProperty(assignement->id->id.value.c_str(), value);
    }
    else {
        try {
            context.objectStack.top()->setProperty(assignement->id->id.value, value.toVariant());
        }
        catch (const std::string& error) {
            context.addCompilationError(assignement, error);
        }
        catch (const std::vector<std::string>& errors) {
            for (auto& error : errors) context.addCompilationError(assignement, error);
        }
    }
}

inline void compileScript(const ast::ScriptNode* script, CompilationContext& context)
{
    if (context.atProgramLevel()) {
        QJSValue value = context.engine.evaluate(toQString(script->code));
        if (value.isError()) {
            context.addEvaluationError(script, value.toString().toStdString());
        }
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << "TODO";
    }
}

inline void compileNode(const ast::Node* node, CompilationContext& context)
{
    switch (node->nodeType) {
        case lang::ProgramNodeType::Program: {
            for (auto n : node->children) compileNode(n, context);
            break;
        }
        case lang::ProgramNodeType::Object: {
            compileObject(node->as<ast::ObjectNode>(), context);
            break;
        }

        case lang::ProgramNodeType::Assignement: {
            compileAssignment(node->as<ast::AssignementNode>(), context);
            break;
        }

        case lang::ProgramNodeType::Script: {
            compileScript(node->as<ast::ScriptNode>(), context);
            break;
        }

        default: break;
    }
}

inline void guessPipelineConnections(CompilationContext& context)
{
    for (auto task : context.highLevelTaskNodes) {
        if (task->source.isNull()) {
            std::vector<compiler::DataNode*> dataNodes;
            std::vector<compiler::TaskNode*> taskNodes;

            for (auto d : context.highLevelDataNodes) {
                if (p4cl::lang::isValidConnection(d->dataType.type, task->taskType.type)) {
                    dataNodes.push_back(d);
                }
            }

            for (auto t : context.highLevelTaskNodes) {
                if (p4cl::lang::isValidConnection(t->taskType.type, task->taskType.type)) {
                    taskNodes.push_back(t);
                }
            }

            if (taskNodes.size() > 1)  // error
            {
                std::vector<std::string> messages;
                messages.push_back("Unable to determine source nodes");
                messages.push_back("Possible sources:");
                for (auto t : taskNodes) {
                    messages.push_back(" * " + t->objectNameOrAnonymous() + ":" +
                                       t->astNode->as<ast::ObjectNode>()->type->id.value);
                }

                for (auto& message : messages)
                    context.addWarning(task->astNode, std::move(message));
                return;
            }

            if (dataNodes.empty() && taskNodes.empty())  // warning
            {
                context.addWarning(task->astNode,
                                   "No source nodes found.\nStatement has no effect.");
                return;
            }

            for (auto d : dataNodes) task->sourceData.push_back(d);
            for (auto t : taskNodes) task->sourceTasks.push_back(t);
        }
    }
}

inline void debugNode(compiler::Node* node, int indent = 0)
{
    qDebug() << QByteArray(indent * 2, ' ').constData() << node;
    for (auto p : node->dynamicPropertyNames()) {
        qDebug() << QByteArray(indent * 2, ' ').constData() << "  *** property" << p << "="
                 << node->property(p);
    }

    for (auto c : node->children) debugNode(c, indent + 1);
}

inline void debug(const compiler::PipelineGraph* graph)
{
    for (auto node : graph->nodes()) qDebug() << "Node" << node << "rank =" << node->rank;

    for (auto c : graph->connections()) qDebug() << "Connection" << c.from << c.to;
}
}

compiler::Result compiler::compile(parser::Result pr, lang::CompilationMode mode)
{
    Result result;
    result.parserResult = pr;
    result.pipelineGraph = std::make_shared<PipelineGraph>();
    result.log = pr.log;

    CompilationContext context{result};
    context.program = std::make_shared<ProgramNode>();
    context.objectStack.push(context.program.get());
    context.mode = mode;

    compileNode(pr.rootNode.get(), context);
    guessPipelineConnections(context);

    result.program = context.program;

    result.pipelineGraph->buildFrom(result.program.get());

    return result;
}

compiler::Result compiler::compile(std::string str, lang::CompilationMode mode)
{
    return compile(parser::parse(std::move(str), mode), mode);
}
