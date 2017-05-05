TEMPLATE = subdirs
CONFIG -=c++11

QMAKE_CXXFLAGS +=-fopenmp -std=c++17 -g3
QMAKE_LIBS +=-lgomp

SUBDIRS += \
	subprojects/interpreter \
	#subprojects/interpreter_lib \
	#subprojects/visualizer

#interpreter_lib.depends = interpreter
#visualizer.depends = interpreter_lib

#CONFIG += ordered*/
