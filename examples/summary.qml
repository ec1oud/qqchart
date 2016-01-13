import QtQuick 2.5
import org.ecloud.charts 1.0

Rectangle {
    width: 480
    height: 320
    color: "black"

    Repeater {
        model: LmSensors.filtered(Sensor.Fan)
        LabeledSensorGraph {
            model: modelData
            color: Qt.rgba(0.3 + index * 0.03, 0.2 + index * 0.17, 1, 1)
            labelLine: index + 2
            lineWidth: 3
            visible: model.maxSampleValue > 0
        }
    }
    Repeater {
        model: LmSensors.filtered(Sensor.Temperature, "Core 0")
        LabeledSensorGraph {
            model: modelData
            color: "#F85"
            labelLine: 0
            minValue: 25
            maxValue: 85
        }
    }
    Repeater {
        model: LmSensors.filtered(Sensor.Cpu)
        LabeledSensorGraph {
            model: modelData
            color: "wheat"
            labelLine: 1
        }
    }

    // possible but unnecessary:
    // Component.onCompleted: LmSensors.setDownsampleInterval(2)
}
