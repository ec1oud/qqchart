#ifndef LMSENSORS_H
#define LMSENSORS_H

#include <QObject>

#include <QTime>
#include <QDebug>
#include <QQmlListProperty>
#include <QPointF>
#include <QRectF>
#include <QFile>
#include <QStringList>
#include <QtQml>

// LM-Sensors Library Header
#include <sensors/sensors.h>
#include <sensors/error.h>
#include "linegraphmodel.h"

class Sensor : public LineGraphModel
{
    Q_OBJECT
    Q_PROPERTY(SensorType type READ type CONSTANT)
    Q_PROPERTY(QString adapter READ adapter CONSTANT)
    Q_PROPERTY(QString chipName READ chipName CONSTANT)
    Q_PROPERTY(int chipId READ chipId CONSTANT)
    Q_PROPERTY(QString unit READ unit CONSTANT)

public:
    enum SensorType { Unknown = 0, Cpu,
                      Memory = 0x100, MemoryFree, MemoryUsed, MemoryCache, MemoryTotal, SwapFree, SwapUsed, SwapTotal,
                      Input = 0x200, Fan, Temperature, Power, Energy, Current, Humidity, Vid, Intrusion, Connected, Frequency };
    Q_ENUM(SensorType)

    explicit Sensor(SensorType type = Unknown, QObject *parent = 0);

    Q_INVOKABLE qreal sample();

    SensorType type() { return m_type; }
    QString adapter() { return m_adapter; }
    QString chipName() { return m_chipName; }
    int chipId() { return m_chipId; }
    QString unit() { return m_unit; }

private:
    bool recordSample(qint64 timestamp);
    void getMemoryMetric(const char *metric, qreal &val);
    void getCPULoad(qreal &val);

private:
    int m_index = -1;
    int m_chipId = 0;
    SensorType m_type = SensorType::Unknown;
    const sensors_chip_name *m_chip = 0;
    const sensors_feature *m_feature = 0;
    const sensors_subfeature *m_subfeature = 0;
    qint64 m_totalJiffies = 0;
    qint64 m_workJiffies = 0;
    qreal m_scale = 1;
    QString m_adapter;
    QString m_chipName;
    QString m_unit;
    QFile *m_file = nullptr;

    friend class LmSensors;
};


class LmSensors : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool initialized READ initialized NOTIFY sensorsChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(QQmlListProperty<Sensor> sensors READ sensors NOTIFY sensorsChanged)
    Q_PROPERTY(int updateIntervalMs READ updateIntervalMs WRITE setUpdateIntervalMs NOTIFY updateIntervalMsChanged)

public:

    explicit LmSensors(QObject *parent = 0);

    Q_INVOKABLE bool sampleAllValues();
    Q_INVOKABLE qint64 timestamp() { return (QDateTime().currentDateTime().toMSecsSinceEpoch()); }
    Q_INVOKABLE void setDownsampleInterval(qreal downsampleInterval);

    bool initialized() { return m_initialized; }
    QString errorMessage() { return m_errorMessage; }

    int updateIntervalMs() const { return m_updateIntervalMs; }
    void setUpdateIntervalMs(int updateIntervalMs);

    QQmlListProperty<Sensor> sensors();

    Q_INVOKABLE QList<QObject*> filtered(int type, const QString substring = QString()); // int is really Sensor::SensorType
    static qreal readRealFile(const QString &path);

signals:
    void sensorsChanged();
    void errorMessageChanged();
    void updateIntervalMsChanged();

protected:
    void timerEvent(QTimerEvent *) Q_DECL_OVERRIDE { sampleAllValues(); }
    static QStringList find(const QString &dir, const QStringList &nameFilters);

private:
    bool init();

private:
    QList<Sensor *> m_sensors;
    QString m_errorMessage;
    bool m_initialized;
    int m_updateIntervalMs = 1000;
    int m_timerId = -1;
};

//QML_DECLARE_TYPE(LmSensors *)
//QML_DECLARE_TYPE(Sensor *)


#endif // LMSENSORS_H
