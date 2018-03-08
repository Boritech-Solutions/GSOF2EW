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

#QMAKE_CXXFLAGS += -m32 -Dlinux -D__i386 -D_LINUX -D_INTEL -D_USE_SCHED  -D_USE_PTHREADS -D_USE_TERMIOS -Wno-write-strings
#QMAKE_CFLAGS += -m32 -Dlinux -D__i386 -D_LINUX -D_INTEL -D_USE_SCHED  -D_USE_PTHREADS -D_USE_TERMIOS -Wno-write-strings

#unix:!macx: PRE_TARGETDEPS += $$(EW_HOME)/$$(EW_VERSION)/lib/libew.a $$(EW_HOME)/$$(EW_VERSION)/lib/dirops_ew.o

#INCLUDEPATH += /opt/earthworm/trunk/include
#DEPENDPATH += /opt/earthworm/trunk/include

#INCLUDEPATH += $$(EW_HOME)/$$(EW_VERSION)/include
#DEPENDPATH += $$(EW_HOME)/$$(EW_VERSION)/include

#LIBS += -L/opt/earthworm/trunk/lib -lew
#OBJECTS += /opt/earthworm/trunk/lib/dirops_ew.o #$$PWD/../../../lib/libew.a

#LIBS += -L$$(EW_HOME)/$$(EW_VERSION)/lib/ -lew
#OBJECTS += $$(EW_HOME)/$$(EW_VERSION)/lib/dirops_ew.o #$$PWD/../../../lib/libew.a
