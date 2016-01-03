#ifndef SENSORGRAPH_H
#define SENSORGRAPH_H

#include <QObject>
#include "LineGraph.h"
#include "lmsensors.h"

class SensorGraph : public LineGraph
{
    Q_OBJECT
    Q_PROPERTY(Sensor * sensor READ sensor WRITE setSensor NOTIFY sensorChanged)

public:
    SensorGraph();
    Sensor * sensor() const { return m_sensor; }
    void setSensor(Sensor * sensor);

signals:
    void sensorChanged(Sensor * sensor);

private slots:
    void onMinMaxChanged();

private:
    Sensor *m_sensor = Q_NULLPTR;
};

#endif // SENSORGRAPH_H
