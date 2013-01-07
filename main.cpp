#include <QGuiApplication>
#include <QtQuick/QQuickView>

#include "chart2d.h"
#include "datasequencemodel.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Chart2D>("QQChart", 1, 0, "Chart2D");
    qmlRegisterType<DataSequenceModel>("QQChart", 1, 0, "DataSequenceModel");

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("main.qml"));
    view.show();

    return app.exec();
}
