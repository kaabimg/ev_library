#pragma once

#include <p4cl/parser/Parser.hpp>

#include <QStringList>
#include <QIcon>

namespace p4s {

struct CompletionString {
    QString string;
    p4cl::TextPosition substitution;
};

struct AssistItem {
    QString label;
    QStringList completion;
    QString substitutionLabel;
    QIcon icon;

public:
    CompletionString generate(const QString& indent) const;

    static AssistItem fromVariable(std::pair<std::string_view,std::string_view>);
    static AssistItem fromType(const p4cl::lang::TypeProperties&);
    static AssistItem fromProperty(const p4cl::lang::PropertyInfo&);

};
}
