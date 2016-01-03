#ifndef SENSORGRAPH_H
#define SENSORGRAPH_H

#include <QObject>
#include "LineGraph.h"
#include "lmsensors.h"

class SensorGraph : public LineGraph
{
    Q_OBJECT
    Q_PROPERTY(SensorItem * sensor READ sensor WRITE setSensor NOTIFY sensorChanged)

public:
    SensorGraph();
    SensorItem * sensor() const { return m_sensor; }
    void setSensor(SensorItem * sensor);

signals:
    void sensorChanged(SensorItem * sensor);

private slots:
    void onMinMaxChanged();

private:
    SensorItem *m_sensor = Q_NULLPTR;
};

#endif // SENSORGRAPH_H
