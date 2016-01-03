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

// LM-Sensors Library Header
#include <sensors/sensors.h>
#include <sensors/error.h>
#include "LineNode.h"

class SensorItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(QString adapter READ adapter CONSTANT)
    Q_PROPERTY(QString chipName READ chipName CONSTANT)
    Q_PROPERTY(int chipId READ chipId CONSTANT)
    Q_PROPERTY(QString unit READ unit CONSTANT)
    Q_PROPERTY(qreal value READ currentSample NOTIFY currentSampleChanged)
    Q_PROPERTY(qreal minValue READ minValue NOTIFY minValueChanged)
    Q_PROPERTY(qreal maxValue READ maxValue NOTIFY maxValueChanged)
    Q_PROPERTY(qint32 maxSamples READ maxSamples WRITE setMaxSamples NOTIFY maxSamplesChanged)
    Q_PROPERTY(qreal valueMin READ valueMin WRITE setValueMin NOTIFY valueMinChanged)
    Q_PROPERTY(qreal valueMax READ valueMax WRITE setValueMax NOTIFY valueMaxChanged)
    Q_PROPERTY(qreal normalMin READ normalMin WRITE setNormalMin NOTIFY normalMinChanged)
    Q_PROPERTY(qreal normalMax READ normalMax WRITE setNormalMax NOTIFY normalMaxChanged)

public:
    explicit SensorItem(QObject *parent = 0);

    Q_INVOKABLE qreal sample();
    Q_INVOKABLE qreal valueAt(const qint64 &timestamp);

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

    enum SensorType { CPU, LM };

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
    SensorType m_type = CPU;
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
    Q_PROPERTY(bool initialized READ initialized NOTIFY itemsChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(QQmlListProperty<SensorItem> items READ items NOTIFY itemsChanged)

public:
    explicit LmSensors(QObject *parent = 0);

    Q_INVOKABLE bool sampleAllValues();
    Q_INVOKABLE qint64 timestamp() { return (QDateTime().currentDateTime().toMSecsSinceEpoch()); }

    bool initialized() { return m_initialized; }
    QString errorMessage() { return m_errorMessage; }

    QQmlListProperty<SensorItem> items();

signals:
    void itemsChanged();
    void errorMessageChanged();

private:
    bool init();

private:
    QList<SensorItem *> m_sensorItems;
    QString m_errorMessage;
    bool m_initialized;
};

#endif // LMSENSORS_H
