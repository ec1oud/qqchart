QT       += qml quick
TARGET = org/ecloud/charts/chartsplugin
TEMPLATE = lib

SOURCES += plugin.cpp \
    LineGraph.cpp \
    LineNode.cpp \
    lmsensors.cpp

HEADERS += \
    LineGraph.h \
    LineNode.h \
    lmsensors.h

RESOURCES += graph.qrc

LIBS += -lsensors

OTHER_FILES += examples/*.qml org/ecloud/charts/qmldir
