import QtQuick 2.5
import QtQuick.Window 2.1
import QtGraphicalEffects 1.0
import org.ecloud.charts 1.0

Window {
    width: 480
    height: 320
    x: 1920 - width - 4
    y: 4
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnBottomHint
    visible: true

    Rectangle {
        id: content
        anchors.fill: parent
        visible: false
        color: "transparent"
        border.color: "#6e4b4d"
        antialiasing: true
        radius: 20
        Repeater {
            model: LmSensors.filtered(Sensor.Fan)
            LabeledSensorGraph {
                model: modelData
                color: Qt.rgba(0.3 + index * 0.03, 0.2 + index * 0.17, 1, 1)
                labelLine: index + 2
                lineWidth: 3
                //visible: modelData.maxSampleValue > 0
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
    }
    DropShadow {
        source: content
        anchors.fill: content
        horizontalOffset: 1
        verticalOffset: 1
        radius: 8.0
        samples: 7
        color: "#80000000"
    }
}
