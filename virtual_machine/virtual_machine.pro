TEMPLATE = app
TARGET = virtual_machine
EV += core

include(../config.pri)


SOURCES += main.cpp \
    parser.cpp \
    compiler.cpp \
    jit_types.cpp \
    jit_debug.cpp \
    virtual_machine.cpp \
    vm_object.cpp

HEADERS += \
    ast.h \
    parser.h \
    compiler.h \
    jit_types.h \
    jit_debug.h \
    virtual_machine.h \
    vm_object.h




## libjit

LIBJIT_INSTALL_DIR = /home/ghaith/data/development/installs/libjit/install

INCLUDEPATH += $$LIBJIT_INSTALL_DIR/include
LIBS += -L$$LIBJIT_INSTALL_DIR/lib/ -ljitplus -ljit -ljitdynamic




