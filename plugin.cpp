#include <QDir>
#include <QJSEngine>
#include <QLoggingCategory>
#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <QtQml>

#include "lmsensors.h"
#include "LineGraph.h"

Q_LOGGING_CATEGORY(lcRegistration, "org.ecloud.charts.registration")

static const char *ModuleName = "org.ecloud.charts";

class ChartsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")

public:
    ChartsPlugin() : QQmlExtensionPlugin() { }

    virtual void initializeEngine(QQmlEngine *engine, const char * uri) {
        Q_UNUSED(engine)
        qCDebug(lcRegistration) << uri;
    }

    virtual void registerTypes(const char *uri) {
        qCDebug(lcRegistration) << uri;
        Q_ASSERT(uri == QLatin1String(ModuleName));
        qmlRegisterType<LineGraph>(uri, 1, 0, "LineGraph");
        qmlRegisterType<LmSensors>(uri, 1, 0, "LmSensors");
        qmlRegisterUncreatableType<SensorItem>(uri, 1, 0, "Sensor", "objects of type Sensor are provided by LmSensors");
        qmlRegisterUncreatableType<SensorSample>(uri, 1, 0, "SensorSample", "objects of type SensorSample are provided by SensorItem");
    }
};

QT_END_NAMESPACE

#include "plugin.moc"
