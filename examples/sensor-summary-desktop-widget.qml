import QtQuick 2.5
import QtQuick.Window 2.1
import QtGraphicalEffects 1.0
import Qt.labs.settings 1.0
import org.ecloud.charts 1.0

Window {
    id: window
    width: 580
    height: 320
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnBottomHint
    visible: true

    Settings {
        category: "SensorSummaryDesktopWidget"
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }

    Rectangle {
        id: content
        anchors.fill: parent
        visible: false
        color: "transparent"
        border.color: "#6e4b4d"
        antialiasing: true
        radius: 20
        Repeater {
            id: batteries
            model: LmSensors.filtered(Sensor.Energy)
            LabeledSensorGraph {
                model: modelData
                color: "lightgreen"
                labelLine: index + 1
                lineWidth: 1
//                visible: modelData.maxSampleValue > 0
            }
        }
        Repeater {
            id: fans
            model: LmSensors.filtered(Sensor.Fan)
            LabeledSensorGraph {
                model: modelData
                color: Qt.rgba(0.3 + index * 0.03, 0.2 + index * 0.17, 1, 1)
                labelLine: batteries.count + index + 2
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
                labelLine: batteries.count + 1
            }
        }
        Repeater {
            model: LmSensors.filtered(Sensor.Memory, "Memory Used")
            LabeledSensorGraph {
                model: modelData
                color: "cyan"
                labelLine: batteries.count + fans.count + 2
                lineWidth: 2
            }
        }
        Repeater {
            model: LmSensors.filtered(Sensor.Memory, "Memory Cached")
            LabeledSensorGraph {
                model: modelData
                color: "darkgreen"
                labelLine: batteries.count + fans.count + 3
                lineWidth: 1
            }
        }
        Repeater {
            model: LmSensors.filtered(Sensor.Memory, "Swap Free")
            LabeledSensorGraph {
                model: modelData
                color: "midnightblue"
                labelLine: batteries.count + fans.count + 4
                lineWidth: 3
                visible: modelData.maxValue > 0
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
