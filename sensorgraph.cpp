#include "sensorgraph.h"

SensorGraph::SensorGraph()
{
    m_maxValue = 100;
    m_timeScale = 1;
}

void SensorGraph::setSensor(Sensor *sensor)
{
    if (m_sensor == sensor || !sensor)
        return;

    connect(sensor, SIGNAL(currentSampleChanged(const QVector<LineNode::LineVertex>*)),
            this, SLOT(setVertices(const QVector<LineNode::LineVertex>*)));
    // Don't override min/max if the user has set them
    if (m_minValue == 0)
        setMinValue(qMin(sensor->valueMin(), sensor->minValue()));
    if (m_maxValue == 1)
        setMaxValue(qMax(sensor->valueMax(), sensor->maxValue()));
    if (m_alertMaxValue == 0)
        setAlertMinValue(sensor->normalMin());
    if (m_alertMaxValue == 1)
        setAlertMaxValue(sensor->normalMax());
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
