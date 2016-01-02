#include "sensorgraph.h"

SensorGraph::SensorGraph()
{
    m_maxValue = 100;
    m_timeScale = 1;
}

void SensorGraph::setSensor(SensorItem *sensor)
{
    if (m_sensor == sensor)
        return;

    connect(sensor, SIGNAL(currentsampleChanged(const QVector<LineNode::LineVertex>*)),
            this, SLOT(setVertices(const QVector<LineNode::LineVertex>*)));

    m_sensor = sensor;
    emit sensorChanged(sensor);
}
