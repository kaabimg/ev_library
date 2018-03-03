#include "AppScriptObject.hpp"
#include "../gui/EditView.hpp"

#include <QFileInfo>
#include <QDir>

using namespace p4s;

QString AppScriptObject::set_working_dir(const QString& wd)
{
    if (QDir(wd).exists()) {
        editView->setWorkingDir(wd);
        return "";
    }
    else {
        return "Error: Failed to open '" + wd + "'";
    }
}
