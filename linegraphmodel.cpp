#include "linegraphmodel.h"
#include <cmath>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcLineGraphModel, "org.ecloud.charts.model")

LineGraphModel::LineGraphModel(QObject *parent) : QObject(parent)
{

}

double LineGraphModel::triangleArea(
        const LineGraphModel::TimeValue &a, const LineGraphModel::TimeValue &b, const LineGraphModel::TimeValue &c)
{
    // www.mathopenref.com/coordtrianglearea.html
    return qAbs( (a.time * (b.value - c.value) +
                  b.time * (c.value - a.value) +
                  c.time * (a.value - b.value)) / 2);
}

LineGraphModel::TimeValue LineGraphModel::largestTriangle(const TimeValue &previous, const QVector<TimeValue> &current, const TimeValue &next)
{
    if (current.count() == 1)
        return current.first();

    int chosen = 0;
    double chosenArea = triangleArea(previous, current.first(), next);
    for (int i = 1; i < current.size(); ++i) {
        if (triangleArea(previous, current[i], next) > chosenArea)
            chosen = i;
    }

    return current[chosen];
}

LineGraphModel::TimeValue LineGraphModel::average(const QVector<TimeValue> &tvv)
{
    TimeValue ret = {0, 0};
    for (const TimeValue &tv : tvv) {
        ret.time += tv.time;
        ret.value += tv.value;
    }
    ret.time /= tvv.count();
    ret.value /= tvv.count();
    return ret;
}


LineGraphModel::TimeValue LineGraphModel::endVertex(int fromLast)
{
    int i = (m_vertices.count() / LineNode::verticesPerSample - fromLast) * LineNode::verticesPerSample;
    TimeValue ret;
    ret.time = m_vertices[i].x /* + m_timeOffset */;
    ret.value = m_vertices[i].y;
    return ret;
}

void LineGraphModel::appendSample(qreal value, qint64 timestamp)
{
    // make time (x) values smaller; TODO why does this even matter? loss of precision?
    float time = (timestamp - m_timeOffset) / 1000.0;
    TimeValue tv { time, value };
    // On currentBucket overflow:
    // finalize the vertex from m_previousBucket
    // current becomes previous
    bool modify = !m_currentBucket.isEmpty();
    if (m_downsampleMethod == LargestTriangleThreeBuckets) {
        if (!m_currentBucket.isEmpty() && time > m_currentBucket.first().time + m_downsampleBucketInterval) {
            if (m_vertices.count() > LineNode::verticesPerSample * 2) {
                TimeValue previousVtx = largestTriangle(endVertex(2), m_previousBucket, tv);
                modifyEndVertices(previousVtx.time, previousVtx.value, 1);
            }
            m_previousBucket = m_currentBucket;
            m_currentBucket.clear();
            TimeValue previousVtx = average(m_previousBucket);
            modifyEndVertices(previousVtx.time, previousVtx.value);
            modify = false;
        }
        m_currentBucket.append(tv);
        qCDebug(lcLineGraphModel) << "buckets" << m_previousBucket.count() << m_currentBucket.count();
    } else {
        modify = false;
    }
    if (modify)
        modifyEndVertices(time, value);
    else
        appendVertices(time, value);
}

int LineGraphModel::sampleIndexNearest(qint64 time)
{
    Q_UNUSED(time);
    return 0; // TODO binary search
}

qint64 LineGraphModel::sampleTimeNearest(qint64 time)
{
    int idx = sampleIndexNearest(time);
    if (idx >= 0)
        return m_vertices[idx].x + m_timeOffset;
    return -1;
}

qreal LineGraphModel::sampleNearest(qint64 time)
{
    int idx = sampleIndexNearest(time);
    if (idx >= 0)
        return m_vertices[idx].y;
    return qQNaN();
}

/**
* Returns a "nice" number approximately equal to range
  Rounds the number if round = true Takes the ceiling if round = false.

  From http://stackoverflow.com/questions/8506881/nice-label-algorithm-for-charts-with-minimum-ticks
*
* @param range the data range
* @param round whether to round the result
* @return a "nice" number to be used for the data range
*/
qreal LineGraphModel::niceNum(qreal range, bool round)
{
    qreal exponent; /** exponent of range */
    qreal fraction; /** fractional part of range */
    qreal niceFraction; /** nice, rounded fraction */

    exponent = floor(log10(range));
    fraction = range / pow(10.f, exponent);

    if (round) {
        if (fraction < 1.5)
            niceFraction = 1;
        else if (fraction < 3)
            niceFraction = 2;
        else if (fraction < 7)
            niceFraction = 5;
        else
            niceFraction = 10;
    } else {
        if (fraction <= 1)
            niceFraction = 1;
        else if (fraction <= 2)
            niceFraction = 2;
        else if (fraction <= 5)
            niceFraction = 5;
        else
            niceFraction = 10;
    }

    return niceFraction * pow(10, exponent);
}

void LineGraphModel::autoScale()
{
    qreal range = niceNum(m_maxSampleValue - m_minSampleValue, false);
    qreal tickSpacing = niceNum(range / (m_maxTicks - 1), true);
    setMinValue(floor(m_minSampleValue / tickSpacing) * tickSpacing);
    setMaxValue(ceil(m_maxSampleValue / tickSpacing) * tickSpacing);
    qCDebug(lcLineGraphModel) << Q_FUNC_INFO << m_minSampleValue << m_maxSampleValue
                              << "tickSpacing" << tickSpacing << "autoscaled:" << minValue() << maxValue();
}

void LineGraphModel::setNormalMinValue(qreal normalMinValue)
{
    if (m_normalMinValue == normalMinValue)
        return;

    m_normalMinValue = normalMinValue;
    emit normalMinValueChanged();
}

void LineGraphModel::setNormalMaxValue(qreal normalMaxValue)
{
    if (m_normalMaxValue == normalMaxValue)
        return;

    m_normalMaxValue = normalMaxValue;
    emit normalMaxValueChanged();
}

void LineGraphModel::setLabel(QString label)
{
    if (m_label == label)
        return;

    m_label = label;
    emit labelChanged();
}

void LineGraphModel::setTimeSpan(int timeSpan)
{
    if (m_timeSpan == timeSpan)
        return;

    m_timeSpan = timeSpan;
    emit timeSpanChanged();
}

void LineGraphModel::setDownsampleInterval(qreal downsampleInterval)
{
    if (m_downsampleBucketInterval == downsampleInterval)
        return;

    m_downsampleBucketInterval = downsampleInterval;
    emit downsampleIntervalChanged();
}

void LineGraphModel::setDownsampleMethod(LineGraphModel::DownsampleMethod downsampleMethod)
{
    if (m_downsampleMethod == downsampleMethod)
        return;

    m_downsampleMethod = downsampleMethod;
    emit downsampleMethodChanged();
}

qreal LineGraphModel::currentValue() const
{
    if (m_vertices.length())
        return m_vertices.last().y;
    else
        return 0;
}

void LineGraphModel::setMinValue(qreal minValue)
{
    if (m_minValue == minValue)
        return;

    m_minValue = minValue;
    emit minValueChanged();
}

void LineGraphModel::setMaxValue(qreal maxValue)
{
    if (m_maxValue == maxValue)
        return;

    m_maxValue = maxValue;
    emit maxValueChanged();
}

void LineGraphModel::setClipValues(bool clipValues)
{
    if (m_clipValues == clipValues)
        return;

    m_clipValues = clipValues;
    emit clipValuesChanged();
}

const QVector<LineNode::LineVertex> *LineGraphModel::vertices()
{
    return &m_vertices;
}

void LineGraphModel::appendVertices(qreal time, qreal value)
{
//    qDebug() << m_label << time << value << "already have samples:" << m_vertices.size();
    if (m_clipValues) {
        if (value < m_minValue)
            value = m_minValue;
        if (value > m_maxValue)
            value = m_maxValue;
    }
    finagle(time, value);
    if (value > m_maxSampleValue) {
        m_maxSampleValue = value;
        emit maxSampleValueChanged();
    }
    if (value < m_minSampleValue) {
        m_minSampleValue = value;
        emit minSampleValueChanged();
    }
    int i = m_vertices.size() - LineNode::verticesPerSample;
    if (i >= 0) {
        m_vertices[i].nextX = time;
        m_vertices[i++].nextY = value;
        m_vertices[i].nextX = time;
        m_vertices[i++].nextY = value;
        m_vertices[i].nextX = time;
        m_vertices[i++].nextY = value;
        m_vertices[i].nextX = time;
        m_vertices[i++].nextY = value;
    }
    i = m_vertices.size();
    m_vertices.resize(i + LineNode::verticesPerSample);
    float tp = i > 0 ? m_vertices[i - 1].x : time;
    float samplePrev = i > 0 ? m_vertices[i - 1].y : value;
    m_vertices[i++].set(0, -1, time, value, tp, samplePrev, time + 0.01, value);
    m_vertices[i++].set(1,  1, time, value, tp, samplePrev, time + 0.01, value);
    m_vertices[i++].set(2, -1, time, value, tp, samplePrev, time + 0.01, value);
    m_vertices[i++].set(3,  1, time, value, tp, samplePrev, time + 0.01, value);
    Q_ASSERT(i == m_vertices.size());

    qint64 earliestTimeAllowed = m_vertices.last().x - m_timeSpan;
    while (m_vertices.size() > 2 && m_vertices[1].x < earliestTimeAllowed)
        m_vertices.remove(0, 4);
    emit samplesChanged();
}

void LineGraphModel::modifyEndVertices(qreal time, qreal value, int fromLast)
{
//    qDebug() << m_label << time << value << "already have samples:" << m_vertices.size();
    finagle(time, value);
    int i = (m_vertices.length() / LineNode::verticesPerSample - fromLast - 1) * LineNode::verticesPerSample;
    Q_ASSERT(i >= 0);
    qreal tp = time - 0.01;
    qreal samplePrev = value;
    if (i >= LineNode::verticesPerSample) {
        int j = i - LineNode::verticesPerSample;
        tp = m_vertices[j].x;
        samplePrev = m_vertices[j].y;
        m_vertices[j].nextX = time;
        m_vertices[j++].nextY = value;
        m_vertices[j].nextX = time;
        m_vertices[j++].nextY = value;
        m_vertices[j].nextX = time;
        m_vertices[j++].nextY = value;
        m_vertices[j].nextX = time;
        m_vertices[j++].nextY = value;
    }
    qreal tn = time + 0.001;
    qreal sampleNext = value;
    if (fromLast > 0) {
        int j = i + LineNode::verticesPerSample;
        tn = m_vertices[j].x;
        sampleNext = m_vertices[j].y;
        m_vertices[j].prevX = time;
        m_vertices[j++].prevY = value;
        m_vertices[j].prevX = time;
        m_vertices[j++].prevY = value;
        m_vertices[j].prevX = time;
        m_vertices[j++].prevY = value;
        m_vertices[j].prevX = time;
        m_vertices[j++].prevY = value;
    }
    m_vertices[i++].set(0, -1, time, value, tp, samplePrev, tn, sampleNext);
    m_vertices[i++].set(1,  1, time, value, tp, samplePrev, tn, sampleNext);
    m_vertices[i++].set(2, -1, time, value, tp, samplePrev, tn, sampleNext);
    m_vertices[i++].set(3,  1, time, value, tp, samplePrev, tn, sampleNext);
    emit samplesChanged();
}
