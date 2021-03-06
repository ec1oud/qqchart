#include <QDir>
#include <QJSEngine>
#include <QLoggingCategory>
#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <QtQml>

#include "influxdb.h"
#include "lmsensors.h"
#include "linegraph.h"

Q_LOGGING_CATEGORY(lcRegistration, "org.ecloud.charts.registration")

static const char *ModuleName = "org.ecloud.charts";

static QObject *LmSensorsSingleton(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    LmSensors *ret = new LmSensors();
    return ret;
}

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
        qmlRegisterType<LineGraphModel>(uri, 1, 0, "LineGraphModel");
        qmlRegisterSingletonType<LmSensors>(uri, 1, 0, "LmSensors", LmSensorsSingleton);
        qmlRegisterType<Sensor>(uri, 1, 0, "Sensor");
        qmlRegisterType<InfluxQuery>(uri, 1, 0, "InfluxQuery");
        qmlRegisterUncreatableType<InfluxValueSeries>(uri, 1, 0, "InfluxValueSeries", "InfluxValueSeries is only available from InfluxQuery");
//        qmlRegisterType<InfluxValueSeries>(uri, 1, 0, "InfluxValueSeries");
    }
};

QT_END_NAMESPACE

#include "plugin.moc"
