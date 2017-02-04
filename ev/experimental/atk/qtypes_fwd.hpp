#pragma once


#define ATK_ALIAS_QTYPE(qtype,type) \
class qtype; \
namespace ev { namespace atk { using type = qtype; }  }


ATK_ALIAS_QTYPE(QObject,qobject)
ATK_ALIAS_QTYPE(QApplication,qapplication)
ATK_ALIAS_QTYPE(QWidget,qwidget)
ATK_ALIAS_QTYPE(QMainWindow,qmainwindow)
ATK_ALIAS_QTYPE(QAction,qaction)
ATK_ALIAS_QTYPE(QString,qstring)
ATK_ALIAS_QTYPE(QStringList,qtringlist)
ATK_ALIAS_QTYPE(QByteArray,qbytearray)
ATK_ALIAS_QTYPE(QVariant,qvariant)
ATK_ALIAS_QTYPE(QIcon,qicon)
ATK_ALIAS_QTYPE(QColor,qcolor)
ATK_ALIAS_QTYPE(QModelIndex,qmodelindex)


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
