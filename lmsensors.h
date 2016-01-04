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
#include "LineNode.h"

class Sensor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SensorType type READ type CONSTANT)
    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(QString adapter READ adapter CONSTANT)
    Q_PROPERTY(QString chipName READ chipName CONSTANT)
    Q_PROPERTY(int chipId READ chipId CONSTANT)
    Q_PROPERTY(QString unit READ unit CONSTANT)
    Q_PROPERTY(qreal value READ currentSample NOTIFY currentSampleChanged)
    Q_PROPERTY(qreal minValue READ minValue NOTIFY minValueChanged) // seen so far
    Q_PROPERTY(qreal maxValue READ maxValue NOTIFY maxValueChanged)
    Q_PROPERTY(qint32 maxSamples READ maxSamples WRITE setMaxSamples NOTIFY maxSamplesChanged)
    Q_PROPERTY(qreal valueMin READ valueMin WRITE setValueMin NOTIFY valueMinChanged) // limit
    Q_PROPERTY(qreal valueMax READ valueMax WRITE setValueMax NOTIFY valueMaxChanged)
    Q_PROPERTY(qreal normalMin READ normalMin WRITE setNormalMin NOTIFY normalMinChanged)
    Q_PROPERTY(qreal normalMax READ normalMax WRITE setNormalMax NOTIFY normalMaxChanged)

public:
    enum SensorType { Unknown = 0, Cpu,
                      Input = 0x100, Fan, Temperature, Power, Energy, Current, Humidity, Vid, Intrusion };
    Q_ENUM(SensorType)

    explicit Sensor(QObject *parent = 0);

    Q_INVOKABLE qreal sample();
    Q_INVOKABLE qreal valueAt(const qint64 &timestamp);

    SensorType type() { return m_type; }
    QString label() { return m_label; }
    QString adapter() { return m_adapter; }
    QString chipName() { return m_chipName; }
    int chipId() { return m_chipId; }
    QString unit() { return m_unit; }
    qint64 maxSamples() { return m_maxSamples; }
    void setMaxSamples(qint64 val) { m_maxSamples = val; }
    qreal valueMin() { return m_valueMin; }
    void setValueMin(qreal val);
    qreal valueMax() { return m_valueMax; }
    void setValueMax(qreal val);
    qreal normalMin() const { return m_normalMin; }
    void setNormalMin(qreal normalMin);
    qreal normalMax() const { return m_normalMax; }
    void setNormalMax(qreal normalMax);
    qreal currentSample();
    qreal minValue() { return m_minValue; }
    qreal maxValue() { return m_maxValue; }

    const QVector<LineNode::LineVertex> &samples() { return m_vertices; }


signals:
    void checkChanged();
    void currentSampleChanged(const QVector<LineNode::LineVertex> *v);
    void widthChanged();
    void maxSamplesChanged();
    void valueMinChanged();
    void valueMaxChanged();
    void minValueChanged();
    void maxValueChanged();
    void normalMinChanged();
    void normalMaxChanged();

private:
    bool recordSample(const qint64 &timestamp);
    void getCPULoad(qreal &val);

private:
    int m_index = -1;
    int m_chipId = 0;
    qint32 m_maxSamples = 1000;
    SensorType m_type = SensorType::Unknown;
    const sensors_chip_name *m_chip = 0;
    const sensors_feature *m_feature = 0;
    const sensors_subfeature *m_subfeature = 0;
    qreal m_valueMin = 0, m_valueMax = 100; // min/max value (y-axis limits)
    qreal m_normalMin = 0, m_normalMax = 100; // normal min/max value: outside this range is an alarm
    qreal m_minValue = qInf(), m_maxValue = 0; // min/max values that have actually been seen
    QString m_label;
    QString m_adapter;
    QString m_chipName;
    QString m_unit;
    QVector<LineNode::LineVertex> m_vertices;
    qint64 m_total_jiffies, m_work_jiffies;

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

    bool initialized() { return m_initialized; }
    QString errorMessage() { return m_errorMessage; }

    int updateIntervalMs() const { return m_updateIntervalMs; }
    void setUpdateIntervalMs(int updateIntervalMs);

    QQmlListProperty<Sensor> sensors();

    Q_INVOKABLE QList<QObject*> filtered(int type, const QString substring = QString()); // int is really Sensor::SensorType


signals:
    void sensorsChanged();
    void errorMessageChanged();
    void updateIntervalMsChanged();

protected:
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE { sampleAllValues(); }

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
