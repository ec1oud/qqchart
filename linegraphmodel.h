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
    Q_PROPERTY(bool clipValues READ clipValues WRITE setClipValues NOTIFY clipValuesChanged)
    Q_PROPERTY(qreal normalMinValue READ normalMinValue WRITE setNormalMinValue NOTIFY normalMinValueChanged)
    Q_PROPERTY(qreal normalMaxValue READ normalMaxValue WRITE setNormalMaxValue NOTIFY normalMaxValueChanged)
    Q_PROPERTY(qreal minSampleValue READ minSampleValue NOTIFY minSampleValueChanged)
    Q_PROPERTY(qreal maxSampleValue READ maxSampleValue NOTIFY maxSampleValueChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(QString unit READ unit WRITE setUnit NOTIFY unitChanged)

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

    bool clipValues() const { return m_clipValues; }
    void setClipValues(bool clipValues);

    qreal minSampleValue() const { return m_minSampleValue; }
    qreal maxSampleValue() const { return m_maxSampleValue; }
    qreal minSampleTime() const { return m_vertices.isEmpty() ? -1 : m_vertices.first().x; } // in seconds
    qreal maxSampleTime() const { return m_vertices.isEmpty() ? -1 : m_vertices.last().x; }

    QString label() const { return m_label; }
    void setLabel(QString label);

    QString unit() { return m_unit; }
    void setUnit(QString unit);

    void setMultiplier(qreal m) { m_multiplier = m; }

    Q_INVOKABLE LineNode::LineVertex sampleNearest(qreal time);
    Q_INVOKABLE void autoScale();

    qreal normalMinValue() const { return m_normalMinValue; }
    void setNormalMinValue(qreal normalMinValue);

    qreal normalMaxValue() const { return m_normalMaxValue; }
    void setNormalMaxValue(qreal normalMaxValue);

    static qreal niceNum(qreal range, bool round);

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
    void unitChanged();
    void clipValuesChanged();

public slots:
    void appendSample(qreal value, qreal time); // time in seconds
    void appendSampleMs(qreal value, qint64 timestamp = timeNowMs());
    void appendSampleMs(qreal value, QDateTime timestamp) { appendSampleMs(value, timestamp.toMSecsSinceEpoch()); }
    void removeFirstSample();

protected:
    struct TimeValue {
        qreal time;
        qreal value;
    };

    static qint64 timeNowMs() { return QDateTime::currentMSecsSinceEpoch(); }

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
    bool m_clipValues = true;
    int m_maxTicks = 10; // TODO adjust based on view size
    qreal m_downsampleBucketInterval = 0; // time in seconds: duration of one bucket
    qreal m_minValue = 0;
    qreal m_maxValue = 1;
    qreal m_normalMinValue = 0;
    qreal m_normalMaxValue = 1;
    qreal m_minSampleValue = qInf(); // seen for all time, even if it's no longer in m_vertices
    qreal m_maxSampleValue = -qInf();
    qreal m_multiplier = 1;
    QString m_label;
    QString m_unit;

    static const qint64 m_timeOffset;

    friend class LineGraph;
};

#endif // LINEGRAPHMODEL_H
