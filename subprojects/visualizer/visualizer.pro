#-------------------------------------------------
#
# Project created by QtCreator 2016-07-07T14:54:59
#
#-------------------------------------------------

include(../configure.pri)
#include(../warnings.pri)

QT += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = visualizer
TEMPLATE = app
QMAKE_LIBS +=-lreadline

INCLUDEPATH += ../../include/visualizer/ ../../include/interpreter/

SOURCES += \
	../../src/visualizer/infoform.cpp \
	../../src/visualizer/main.cpp \
	../../src/visualizer/mainwindow.cpp \
	../../src/visualizer/qcustomplot.cpp

HEADERS += \
	../../include/visualizer/infoform.hpp \
	../../include/visualizer/mainwindow.hpp \
	../../include/visualizer/qcustomplot.hpp

FORMS += \
	../../src/visualizer/infoform.ui \
	../../src/visualizer/mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../interpreter_lib/release/ -linterpreter_lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../interpreter_lib/debug/ -linterpreter_lib
else:unix: LIBS += -L$$OUT_PWD/../interpreter_lib/ -linterpreter_lib

INCLUDEPATH += $$PWD/../interpreter_lib
DEPENDPATH += $$PWD/../interpreter_lib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interpreter_lib/release/libinterpreter_lib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interpreter_lib/debug/libinterpreter_lib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interpreter_lib/release/interpreter_lib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interpreter_lib/debug/interpreter_lib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../interpreter_lib/libinterpreter_lib.a
