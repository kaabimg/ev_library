#pragma once

class QObject;
class QApplication;
class QWidget;
class QAction;

class QString;
class QByteArray;
class QVariant;
class QIcon;
class QColor;

template <typename T>
class QVector;

template <typename T>
class QList;

template <class Key, class T>
class QHash;

template <class Key, class T>
class QMap;

namespace ev {
namespace atk {

using qobject      = QObject;
using qwidget      = QWidget;
using qapplication = QApplication;
using qaction      = QAction;

using qstring    = QString;
using qbytearray = QByteArray;
using qvariant   = QVariant;
using qicon      = QIcon;
using qcolor     = QColor;

template <typename T>
using qvector = QVector<T>;

template <typename T>
using qlist = QList<T>;

template <class Key, class T>
using qhash = QHash<Key, T>;

template <class Key, class T>
using qmap = QMap<Key, T>;
}
}
