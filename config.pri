

CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++14

LIBS += -pthread


ROOT_DIR = $$PWD/..
BUILD_DIR = $$ROOT_DIR/build

BIN_DIR = $$BUILD_DIR/bin
LIB_DIR = $$BUILD_DIR/lib
TEST_DIR = $$BUILD_DIR/test


INCLUDEPATH += $$ROOT_DIR/ev_library


equals(TEMPLATE, app) {
    contains(CONFIG,test) {
        DESTDIR = $$TEST_DIR
    }
    else {
        DESTDIR = $$BIN_DIR
    }

}
equals (TEMPLATE, lib){
    DESTDIR = $$LIB_DIR
}


OBJECTS_DIR = $$DESTDIR/.obj$$TARGET


#### Libraries

## boost

BOOST_DIR = /home/ghaith/data/development/installs/boost/boost_1_60_0

INCLUDEPATH += $$BOOST_DIR





## ev

contains (EV,core) {
    LIBS += -L$$LIB_DIR -lev_core
    DEPENDPATH += $$LIB_DIR
}

contains (EV,vm) {
    LIBS += -L$$LIB_DIR -lev_vm
    DEPENDPATH += $$LIB_DIR
}
