#ifndef QLMSENSORS_H
#define QLMSENSORS_H

#include <QObject>

#include <QTime>
#include <QDebug>
#include <QQmlListProperty>
#include <QPointF>
#include <QRectF>
#include <QFile>
#include <QStringList>

// LM-Sensors Library Header
#include <sensors/sensors.h>/* Library initialization and clean-up */
#include <sensors/error.h>/* Library initialization and clean-up */



class QSensorSample : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 time READ time CONSTANT)
    Q_PROPERTY(float value READ value CONSTANT)
public:
    explicit QSensorSample(QObject *parent = 0):QObject(parent){m_time=0;m_value=0;};
    explicit QSensorSample(qint64 t, float v, QObject *parent = 0):QObject(parent){m_time=t;m_value=v;};

    qint64 time() {return(m_time);}
    float value() {return(m_value);}

signals:

public slots:

private:
    qint64 m_time;
    float m_value;
};



class QSensorItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString label READ getlabel)
    Q_PROPERTY(QString adapter READ getadapter)
    Q_PROPERTY(QString chipname READ getchipname CONSTANT)
    Q_PROPERTY(int chipid READ getchipid CONSTANT)
    Q_PROPERTY(QString unit READ getunit)
    Q_PROPERTY(float value READ currentsample NOTIFY currentsampleChanged)
    Q_PROPERTY(float minval READ getminval)
    Q_PROPERTY(float maxval READ getmaxval)
    Q_PROPERTY(qint64 tmin READ gettmin WRITE settmin NOTIFY tminChanged)
    Q_PROPERTY(qint64 tmax READ gettmax)
    Q_PROPERTY(qint32 max_samples READ getmax_samples WRITE setmax_samples NOTIFY max_samplesChanged)
    Q_PROPERTY(float ymin READ getymin WRITE setymin NOTIFY yminchanged)
    Q_PROPERTY(float ymax READ getymax WRITE setymax NOTIFY ymaxchanged)
    Q_PROPERTY(float width READ getwidth WRITE setwidth NOTIFY widthChanged)
    Q_PROPERTY(QString color READ getcolor WRITE setcolor NOTIFY colorChanged)
    Q_PROPERTY(bool checked READ getchecked WRITE setchecked NOTIFY checkChanged)
    Q_PROPERTY(QQmlListProperty<QSensorSample> samples READ getSamples CONSTANT)

public:
    explicit QSensorItem(QObject *parent = 0);

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
    QString getcolor(){return color;};
    void setcolor(const QString &newcol){color=newcol; emit colorChanged();};
    float getwidth(){return linewidth;};
    void setwidth(const float &newwidth){linewidth=newwidth; emit widthChanged();};
    bool getchecked(){return checked;};
    void setchecked(const bool &newcheck){if(checked!=newcheck) {checked=newcheck; emit checkChanged();} };
    float currentsample(){if(m_samples.length()) return m_samples.last()->value(); else return 0;}
    float getminval(){return minval;};
    float getmaxval(){return maxval;};


    QQmlListProperty<QSensorSample> getSamples();
    QList<QSensorSample*> samples(){return m_samples;};


    bool do_sample(const qint64 &timestamp);
    void getCPULoad(double &val);

    int index;
    enum SensorType { CPU, LM };
    SensorType type;
    const sensors_chip_name *chip;
    const sensors_feature *feature;
    const sensors_subfeature *sub;
    qint64 tmin, tmax;    // visible range in ms
    float ymin, ymax;   // min/max value y-axis
    float minval, maxval;   // min/max values of the signal
    QString label;
    QString adapter;
    int chipid;
    QString chipname;
    QString color;
    QString unit;
    float linewidth;
    float offset, scale;
    qint32 max_samples;
    bool checked;

signals:
//    void updateSamples();
    void checkChanged();
    void colorChanged();
    void currentsampleChanged();
    void widthChanged();
    void max_samplesChanged();
    void yminchanged();
    void ymaxchanged();
    void tminChanged();

public slots:

private:
    QList<QSensorSample*> m_samples;

    qint64 m_total_jiffies, m_work_jiffies;

};



class QLmSensors : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 timestamp READ timestamp)
    Q_PROPERTY(bool initialized READ initialized)
    Q_PROPERTY(QString errorMessage READ errorMessage)
    Q_PROPERTY(QQmlListProperty<QSensorItem> items READ getItems NOTIFY updateItems)

public:
    explicit QLmSensors(QObject *parent = 0);

    Q_INVOKABLE bool do_sampleValues();

    qint64 timestamp(){return(QDateTime().currentDateTime().toMSecsSinceEpoch());};
    bool initialized(){return m_initialized;};
    QString errorMessage(){return m_errorMessage;};

    QList<QSensorItem*> items(){return m_sensorItems;};

    QQmlListProperty<QSensorItem> getItems();

    QStringList palette;


signals:
    void updateItems();

public slots:

private:
    bool Init();

    QList<QSensorItem*> m_sensorItems;

    QString m_errorMessage;
    bool m_initialized;
};

#endif // QLMSENSORS_H
