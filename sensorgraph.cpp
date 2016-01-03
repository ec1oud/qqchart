#include "sensorgraph.h"

SensorGraph::SensorGraph()
{
    m_maxValue = 100;
    m_timeScale = 1;
}

void SensorGraph::setSensor(SensorItem *sensor)
{
    if (m_sensor == sensor || !sensor)
        return;

    connect(sensor, SIGNAL(currentSampleChanged(const QVector<LineNode::LineVertex>*)),
            this, SLOT(setVertices(const QVector<LineNode::LineVertex>*)));
    setMinValue(qMin(sensor->valueMin(), sensor->minValue()));
    setMaxValue(qMax(sensor->valueMax(), sensor->maxValue()));
    connect(sensor, SIGNAL(minValueChanged()), this, SLOT(onMinMaxChanged()));
    connect(sensor, SIGNAL(maxValueChanged()), this, SLOT(onMinMaxChanged()));

    m_sensor = sensor;
    emit sensorChanged(sensor);
}

void SensorGraph::onMinMaxChanged()
{
    setMinValue(qMin(m_minValue, m_sensor->minValue()));
    setMaxValue(qMax(m_maxValue, m_sensor->maxValue()));
}
