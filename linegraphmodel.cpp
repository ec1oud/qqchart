#include "linegraphmodel.h"

LineGraphModel::LineGraphModel(QObject *parent) : QObject(parent)
{

}

void LineGraphModel::appendSample(qreal value, qint64 timestamp)
{
//    qDebug() << m_label << timestamp << value << "already have samples:" << m_vertices.size();
    // make time (x) values smaller; TODO why does this even matter? loss of precision?
    float time = (timestamp - m_timeOffset) / 1000.0;
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

    if (m_vertices.size() > m_maxSamples * LineNode::verticesPerSample)
        m_vertices.remove(0, m_vertices.size() - m_maxSamples * LineNode::verticesPerSample);
    emit samplesChanged();
}

int LineGraphModel::sampleIndexNearest(qint64 time)
{
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

void LineGraphModel::setMaxSamples(int maxSamples)
{
    if (m_maxSamples == maxSamples)
        return;

    m_maxSamples = maxSamples;
    emit maxSamplesChanged();
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

const QVector<LineNode::LineVertex> *LineGraphModel::vertices()
{
    return &m_vertices;
}
