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

target.path = $$[QT_INSTALL_QML]/org/ecloud/charts
qmldir.path = $$[QT_INSTALL_QML]/org/ecloud/charts
qmldir.files = org/ecloud/charts/qmldir
INSTALLS += target qmldir
