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
    Q_PROPERTY(QString label READ getlabel CONSTANT)
    Q_PROPERTY(QString adapter READ getadapter CONSTANT)
    Q_PROPERTY(QString chipname READ getchipname CONSTANT)
    Q_PROPERTY(int chipid READ getchipid CONSTANT)
    Q_PROPERTY(QString unit READ getunit CONSTANT)
    Q_PROPERTY(float value READ currentsample NOTIFY currentsampleChanged)
    Q_PROPERTY(float minval READ getminval CONSTANT)
    Q_PROPERTY(float maxval READ getmaxval CONSTANT)
    Q_PROPERTY(qint64 tmin READ gettmin WRITE settmin NOTIFY tminChanged)
    Q_PROPERTY(qint64 tmax READ gettmax CONSTANT)
    Q_PROPERTY(qint32 max_samples READ getmax_samples WRITE setmax_samples NOTIFY max_samplesChanged)
    Q_PROPERTY(float ymin READ getymin WRITE setymin NOTIFY yminchanged)
    Q_PROPERTY(float ymax READ getymax WRITE setymax NOTIFY ymaxchanged)

public:
    explicit SensorItem(QObject *parent = 0);

    Q_INVOKABLE float getvalue();
    Q_INVOKABLE float valueAt(const qint64 &timestamp);
    Q_INVOKABLE QPointF map2canvas(const QRectF &bounds, const qint64 &timestamp, const float &val);

    QString getlabel(){return label;};
    QString getadapter(){return adapter;};
    QString getchipname(){return chipname;};
    int getchipid(){return chipid;};
    QString getunit(){return unit;};
    qint64 gettmin(){return tmin;};
    void settmin(const qint64 &val){tmin=val; emit tminChanged();}
    qint64 gettmax(){return tmax;};
    qint64 getmax_samples(){return max_samples;};
    void setmax_samples(qint64 val){max_samples=val;};
    float getymin(){return ymin;};
    void setymin(float val){ymin=val; emit yminchanged();};
    float getymax(){return ymax;};
    void setymax(float val){ymax=val; emit ymaxchanged();};
    float currentsample(){if(m_vertices.length()) return m_vertices.last().y; else return 0;}
    float getminval(){return minval;};
    float getmaxval(){return maxval;};


    const QVector<LineNode::LineVertex> &samples() { return m_vertices; }


    bool do_sample(const qint64 &timestamp);
    void getCPULoad(double &val);

    enum SensorType { CPU, LM };

    int index = -1;
    int chipid = 0;
    qint32 max_samples = 32;
    SensorType type = CPU;
    const sensors_chip_name *chip = 0;
    const sensors_feature *feature = 0;
    const sensors_subfeature *sub = 0;
    qint64 tmin = 0, tmax = 0;    // visible range in ms
    float ymin = 0, ymax = 0;   // min/max value y-axis
    float minval = 0, maxval = 0;   // min/max values of the signal
    QString label;
    QString adapter;
    QString chipname;
    QString unit;

signals:
    void checkChanged();
    void currentsampleChanged(const QVector<LineNode::LineVertex> * v);
    void widthChanged();
    void max_samplesChanged();
    void yminchanged();
    void ymaxchanged();
    void tminChanged();

public slots:

private:
    QVector<LineNode::LineVertex> m_vertices;
    qint64 m_total_jiffies, m_work_jiffies;
};



class LmSensors : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 timestamp READ timestamp)
    Q_PROPERTY(bool initialized READ initialized)
    Q_PROPERTY(QString errorMessage READ errorMessage)
    Q_PROPERTY(QQmlListProperty<SensorItem> items READ getItems NOTIFY updateItems)

public:
    explicit LmSensors(QObject *parent = 0);

    Q_INVOKABLE bool do_sampleValues();

    qint64 timestamp(){return(QDateTime().currentDateTime().toMSecsSinceEpoch());};
    bool initialized(){return m_initialized;};
    QString errorMessage(){return m_errorMessage;};

    QList<SensorItem*> items(){return m_sensorItems;};

    QQmlListProperty<SensorItem> getItems();

signals:
    void updateItems();

public slots:

private:
    bool init();

    QList<SensorItem*> m_sensorItems;

    QString m_errorMessage;
    bool m_initialized;
};

#endif // LMSENSORS_H
