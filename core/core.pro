TEMPLATE = lib

TARGET  = ev_core

include(../config.pri)

HEADERS += \
    macros.h \
    types.h \
    utils.h \
    variant.h \
    thread_pool.h \
    variant_serialization.h \
    event_processor.h \
    logging.h

SOURCES += \
    utils.cpp \
    thread_pool.cpp \
    variant_serialization.cpp \
    event_processor.cpp



