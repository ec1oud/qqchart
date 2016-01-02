QT       += qml quick
TARGET = org/ecloud/charts/chartsplugin
TEMPLATE = lib

SOURCES += plugin.cpp \
    LineGraph.cpp \
    LineNode.cpp \
    lmsensors.cpp \
    sensorgraph.cpp

HEADERS += \
    LineGraph.h \
    LineNode.h \
    lmsensors.h \
    sensorgraph.h

RESOURCES += graph.qrc

LIBS += -lsensors

QMAKE_CXXFLAGS += -std=c++11

OTHER_FILES += examples/*.qml org/ecloud/charts/qmldir
