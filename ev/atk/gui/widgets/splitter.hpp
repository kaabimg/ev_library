#pragma once

#include <qsplitter.h>

namespace ev {
namespace atk {

using qsplitter = QSplitter;

class splitter_t : public qsplitter
{
    Q_OBJECT
public:
    explicit splitter_t(Qt::Orientation orientation, QWidget* parent = 0);

protected:
    QSplitterHandle* createHandle();

public slots:
};

}
}
