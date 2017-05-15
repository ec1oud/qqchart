QT       += qml quick network
TARGET = org/ecloud/charts/chartsplugin
TEMPLATE = lib

SOURCES += plugin.cpp \
    influxdb.cpp \
    linegraph.cpp \
    linegraphmodel.cpp \
    linenode.cpp \
    lmsensors.cpp

HEADERS += \
    influxdb.h \
    linegraph.h \
    linegraphmodel.h \
    linenode_p.h \
    lmsensors.h

RESOURCES += graph.qrc

LIBS += -lsensors

QMAKE_CXXFLAGS += -std=c++14

OTHER_FILES += examples/*.qml org/ecloud/charts/qmldir

target.path = $$[QT_INSTALL_QML]/org/ecloud/charts
qmldir.path = $$[QT_INSTALL_QML]/org/ecloud/charts
qmldir.files = org/ecloud/charts/qmldir
INSTALLS += target qmldir
