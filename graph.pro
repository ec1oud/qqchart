QT       += qml quick
TARGET = org/ecloud/charts/chartsplugin
TEMPLATE = lib

SOURCES += plugin.cpp \
    linegraph.cpp \
    linegraphmodel.cpp \
    linenode.cpp \
    lmsensors.cpp

HEADERS += \
    linegraph.h \
    linegraphmodel.h \
    linenode_p.h \
    lmsensors.h

RESOURCES += graph.qrc

LIBS += -lsensors

QMAKE_CXXFLAGS += -std=c++11

OTHER_FILES += examples/*.qml org/ecloud/charts/qmldir

target.path = $$[QT_INSTALL_QML]/org/ecloud/charts
qmldir.path = $$[QT_INSTALL_QML]/org/ecloud/charts
qmldir.files = org/ecloud/charts/qmldir
INSTALLS += target qmldir
