import QtQuick 2.5
import QtQuick.Window 2.1
import org.ecloud.charts 1.0

Window {
    width: 480
    height: 320
    color: "#555"
    visible: true
    title: lg.model.label + " " + lg.model.currentValue.toFixed(3)

    LineGraph {
        id: lg
        width: 480; height: 320
        model: LmSensors.filtered(Sensor.CPU)[0]
        color: "green"
        lineWidth: 1
        visible: model.maxValue > 0
    }
}
