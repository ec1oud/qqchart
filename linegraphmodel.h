#ifndef LINEGRAPHMODEL_H
#define LINEGRAPHMODEL_H

#include <QObject>
#include <QDateTime>
#include "linenode_p.h"

class LineGraphModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int timeSpan READ timeSpan WRITE setTimeSpan NOTIFY timeSpanChanged)
    Q_PROPERTY(qreal downsampleInterval READ downsampleInterval WRITE setDownsampleInterval NOTIFY downsampleIntervalChanged)
    Q_PROPERTY(DownsampleMethod downsampleMethod READ downsampleMethod WRITE setDownsampleMethod NOTIFY downsampleMethodChanged)
    Q_PROPERTY(qreal currentValue READ currentValue NOTIFY samplesChanged)
    Q_PROPERTY(qreal minValue READ minValue WRITE setMinValue NOTIFY minValueChanged)
    Q_PROPERTY(qreal maxValue READ maxValue WRITE setMaxValue NOTIFY maxValueChanged)
    Q_PROPERTY(qreal normalMinValue READ normalMinValue WRITE setNormalMinValue NOTIFY normalMinValueChanged)
    Q_PROPERTY(qreal normalMaxValue READ normalMaxValue WRITE setNormalMaxValue NOTIFY normalMaxValueChanged)
    Q_PROPERTY(qreal minSampleValue READ minSampleValue NOTIFY minSampleValueChanged)
    Q_PROPERTY(qreal maxSampleValue READ maxSampleValue NOTIFY maxSampleValueChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)

public:
    enum DownsampleMethod { NoDownsample = 0, LargestTriangleThreeBuckets };

    explicit LineGraphModel(QObject *parent = 0);

    int timeSpan() const { return m_timeSpan; }
    void setTimeSpan(int timeSpan);

    qreal downsampleInterval() const { return m_downsampleBucketInterval; }
    void setDownsampleInterval(qreal downsampleInterval);

    DownsampleMethod downsampleMethod() const { return m_downsampleMethod; }
    void setDownsampleMethod(DownsampleMethod downsampleMethod);

    qreal currentValue() const;

    qreal minValue() const { return m_minValue; }
    void setMinValue(qreal minValue);

    qreal maxValue() const { return m_maxValue; }
    void setMaxValue(qreal maxValue);

    qreal minSampleValue() const { return m_minSampleValue; }
    qreal maxSampleValue() const { return m_maxSampleValue; }

    QString label() const { return m_label; }
    void setLabel(QString label);

    Q_INVOKABLE int sampleIndexNearest(qint64 time);
    Q_INVOKABLE qint64 sampleTimeNearest(qint64 time);
    Q_INVOKABLE qreal sampleNearest(qint64 time);

    qreal normalMinValue() const { return m_normalMinValue; }
    void setNormalMinValue(qreal normalMinValue);

    qreal normalMaxValue() const { return m_normalMaxValue; }
    void setNormalMaxValue(qreal normalMaxValue);

signals:
    void timeSpanChanged();
    void downsampleIntervalChanged();
    void downsampleMethodChanged();
    void samplesChanged();
    void minSampleValueChanged();
    void maxSampleValueChanged();
    void minValueChanged();
    void maxValueChanged();
    void normalMinValueChanged();
    void normalMaxValueChanged();
    void labelChanged();

public slots:
    void appendSample(qreal value, qint64 timestamp = QDateTime::currentMSecsSinceEpoch());
    void appendSample(qreal value, QDateTime timestamp) { appendSample(value, timestamp.toMSecsSinceEpoch()); }
    void removeFirstSample();

protected:
    struct TimeValue {
        qreal time;
        qreal value;
    };

protected:
    virtual const QVector<LineNode::LineVertex> *vertices();
    void appendVertices(qreal time, qreal value);
    void modifyEndVertices(qreal time, qreal value, int fromLast = 0);
    TimeValue endVertex(int fromLast = 0);
    double triangleArea(const LineGraphModel::TimeValue &a, const LineGraphModel::TimeValue &b, const LineGraphModel::TimeValue &c);
    LineGraphModel::TimeValue largestTriangle(const TimeValue &previous, const QVector<TimeValue> &current, const TimeValue &next);
    LineGraphModel::TimeValue average(const QVector<TimeValue> &tvv);
    virtual void finagle(qreal &time, qreal &value) { Q_UNUSED(time); Q_UNUSED(value); }

protected:
    QVector<TimeValue> m_currentBucket;
    QVector<TimeValue> m_previousBucket;
    QVector<LineNode::LineVertex> m_vertices;
    int m_timeSpan = 1000; // in seconds
    DownsampleMethod m_downsampleMethod = LargestTriangleThreeBuckets;
    qreal m_downsampleBucketInterval = 0; // time in seconds: duration of one bucket
    qreal m_minValue = 0;
    qreal m_maxValue = 1;
    qreal m_normalMinValue = 0;
    qreal m_normalMaxValue = 1;
    qreal m_minSampleValue = qInf(); // seen for all time, even if it's no longer in m_vertices
    qreal m_maxSampleValue = -qInf();
    qint64 m_timeOffset = QDateTime::currentMSecsSinceEpoch();
    QString m_label;

    friend class LineGraph;
};

#endif // LINEGRAPHMODEL_H
