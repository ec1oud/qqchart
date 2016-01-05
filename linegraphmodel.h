#ifndef LINEGRAPHMODEL_H
#define LINEGRAPHMODEL_H

#include <QObject>
#include <QDateTime>
#include "linenode_p.h"

class LineGraphModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int maxSamples READ maxSamples WRITE setMaxSamples NOTIFY maxSamplesChanged)
    Q_PROPERTY(qreal currentValue READ currentValue NOTIFY samplesChanged)
    Q_PROPERTY(qreal minValue READ minValue WRITE setMinValue NOTIFY minValueChanged)
    Q_PROPERTY(qreal maxValue READ maxValue WRITE setMaxValue NOTIFY maxValueChanged)
    Q_PROPERTY(qreal normalMinValue READ normalMinValue WRITE setNormalMinValue NOTIFY normalMinValueChanged)
    Q_PROPERTY(qreal normalMaxValue READ normalMaxValue WRITE setNormalMaxValue NOTIFY normalMaxValueChanged)
    Q_PROPERTY(qreal minSampleValue READ minSampleValue NOTIFY minSampleValueChanged)
    Q_PROPERTY(qreal maxSampleValue READ maxSampleValue NOTIFY maxSampleValueChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)

public:
    explicit LineGraphModel(QObject *parent = 0);

    int maxSamples() const { return m_maxSamples; }
    void setMaxSamples(int maxSamples);

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
    void maxSamplesChanged();
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
    virtual const QVector<LineNode::LineVertex> *vertices();

protected:
    QVector<LineNode::LineVertex> m_vertices;
    int m_maxSamples = 1000;
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
