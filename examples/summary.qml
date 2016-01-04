import QtQuick 2.5
import org.ecloud.charts 1.0

Rectangle {
    width: 480
    height: 320
    color: "black"

    Repeater {
        model: LmSensors.filtered(Sensor.Fan)
        LabeledSensorGraph {
            sensor: modelData
            color: Qt.rgba(0.3 + index * 0.03, 0.2 + index * 0.17, 1, 1)
            labelLine: index + 2
            alertMaxValue: 5500
            maxValue: sensor.valueMax
            lineWidth: 3
            visible: sensor.maxValue > 0
        }
    }
    Repeater {
        model: LmSensors.filtered(Sensor.Temperature, "Core 0")
        LabeledSensorGraph {
            sensor: modelData
            color: "#F85"
            labelLine: 0
            minValue: 25
            maxValue: 85
        }
    }
    Repeater {
        model: LmSensors.filtered(Sensor.Cpu)
        LabeledSensorGraph {
            sensor: modelData
            color: "wheat"
            labelLine: 1
        }
    }
}
