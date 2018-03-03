#pragma once
#include <QObject>

namespace evt {

class ChangeNotifier : public QObject {
    Q_OBJECT
public:
    ChangeNotifier(QObject* parent = nullptr) : QObject(parent)
    {
    }

    inline void onChange(auto objPtr, auto&& f)
    {
        connect(this, &ChangeNotifier::changed, objPtr, f);
    }

Q_SIGNALS:
    void changed();
};
}
