QT = core network positioning gui
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

SOURCES += main.cpp \
    client.cpp \
    ewconn.cpp

HEADERS += \
    main.h \
    client.h \
    ewconn.h
