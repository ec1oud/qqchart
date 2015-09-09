#include <QGuiApplication>
#include <QtQuick/QQuickView>

#include "chart2d.h"
#include "datasequencemodel.h"

bool multisample = false;

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Chart2D>("QQChart", 1, 0, "Chart2D");
    qmlRegisterType<DataSequenceModel>("QQChart", 1, 0, "DataSequenceModel");

    QQuickView view;
    // fake antialiasing with multisampling
    if (!app.arguments().contains(QLatin1String("-n"))) {
        QSurfaceFormat surfaceFormat = view.requestedFormat();
        surfaceFormat.setSamples(8);
        view.setFormat(surfaceFormat);
        multisample = true;
    }
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    if (app.arguments().contains(QLatin1String("-b")))
        view.setSource(QUrl("gridBenchmark.qml"));
    else
        view.setSource(QUrl("main.qml"));
    view.show();

    return app.exec();
}

