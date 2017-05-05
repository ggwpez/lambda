include(../configure.pri)
#include(../warnings.pri)

QT -= gui core

TARGET = interpreter
CONFIG += console
CONFIG -= app_bundle c++11 C++11

TEMPLATE = app
LIBS += -lreadline

INCLUDEPATH += ../../include/interpreter/

SOURCES += \
    ../../src/interpreter/ast.cpp \
    ../../src/interpreter/evaled_exp.cpp \
    ../../src/interpreter/interpreter.cpp \
    ../../src/interpreter/io.cpp \
    ../../src/interpreter/lexer.cpp \
    ../../src/interpreter/line_interpreter.cpp \
    ../../src/interpreter/main.cpp \
    ../../src/interpreter/parser.cpp \
    ../../src/interpreter/split_string.cpp \
    ../../src/interpreter/tok.cpp \
    ../../src/interpreter/traits_allocator.cpp \
    ../../src/interpreter/scope.cpp

HEADERS += \
    ../../include/interpreter/ast.hpp \
    ../../include/interpreter/defines.hpp \
    ../../include/interpreter/evaled_exp.hpp \
    ../../include/interpreter/inter_info.hpp \
    ../../include/interpreter/interpreter.hpp \
    ../../include/interpreter/io.h \
    ../../include/interpreter/lexer.hpp \
    ../../include/interpreter/line_interpreter.hpp \
    ../../include/interpreter/object_pool.hpp \
    ../../include/interpreter/parser.hpp \
    ../../include/interpreter/split_string.hpp \
    ../../include/interpreter/tok.hpp \
    ../../include/interpreter/traits_allocator.hpp \
    ../../include/interpreter/scope.hpp

