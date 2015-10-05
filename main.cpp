#include <QGuiApplication>
#include <QQuickView>

#include "LineGraph.h"
#include <QtQml/QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    LineGraph::registerMetaType();

    QQuickView view;
    view.resize(1800, 900);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///main.qml"));
    view.show();

    return a.exec();
}
