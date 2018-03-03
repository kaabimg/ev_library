#include "AssistItem.hpp"
#include "../UiUtils.hpp"

using namespace p4s;

CompletionString p4s::AssistItem::generate(const QString& indent) const
{
    CompletionString str;
    QStringList tmp = completion;

    for (int i = 1; i < tmp.size(); ++i) {
        tmp[i].prepend(indent);
    }

    str.string = tmp.join("\n");

    if (!substitutionLabel.isEmpty()) {
        str.substitution.begin = str.string.indexOf(substitutionLabel);
        str.substitution.end = str.substitution.begin + substitutionLabel.size();
    }

    return str;
}

AssistItem AssistItem::fromVariable(std::pair<std::string_view, std::string_view> var)
{
    AssistItem item;
    item.label = QString::fromStdString(std::string(var.first)) + " : " +
                 QString::fromStdString(std::string(var.second));

    item.completion << QString::fromStdString(std::string(var.first));

    if (p4cl::lang::findDataType(var.second)) {
        item.icon = ui::dataIcon(ui::IconShape::Round);
    }
    else if (auto task = p4cl::lang::findTaskType(var.second)) {
        if (task.category() == p4cl::lang::TaskCategory::Export)
            item.icon = ui::exportIcon(ui::IconShape::Round);
        else if (task.category() == p4cl::lang::TaskCategory::Processing)
            item.icon = ui::processingIcon(ui::IconShape::Round);
    }

    return item;
}

AssistItem AssistItem::fromType(const p4cl::lang::TypeProperties& type)
{
    AssistItem item;

    item.label = QString(type.typeName);

    item.completion << QString(type.typeName) + " name:";
    for (auto p : type.properties) {
        if (!p.optional)
            item.completion << "\t" + QString(p.name) + " = " + QString(p.defaultValue);
    }
    item.completion << "end";

    item.substitutionLabel = "name";

    if (p4cl::lang::findDataType(type.typeName)) {
        item.icon = ui::dataIcon();
    }
    else if (auto task = p4cl::lang::findTaskType(type.typeName)) {
        if (task.category() == p4cl::lang::TaskCategory::Export)
            item.icon = ui::exportIcon();
        else if (task.category() == p4cl::lang::TaskCategory::Processing)
            item.icon = ui::processingIcon();
    }

    return item;
}

AssistItem AssistItem::fromProperty(const p4cl::lang::PropertyInfo& property)
{
    AssistItem item;
    item.label = property.name;
    item.completion << QString(property.name).append(" = ").append(property.defaultValue);
    item.substitutionLabel = property.defaultValue;
    return item;
}
