#-------------------------------------------------
#
# Project created by QtCreator 2013-06-11T13:13:18
#
#-------------------------------------------------

QT       += core gui quick

TARGET = graph

TEMPLATE = app

SOURCES += main.cpp \
    LineGraph.cpp \
    LineNode.cpp

HEADERS += \
    LineGraph.h \
    LineNode.h

RESOURCES += \
    graph.qrc

OTHER_FILES += \
    main.qml \
    shaders/noisy.vsh \
    shaders/noisy.fsh \
    shaders/line.fsh \
    shaders/line.vsh

