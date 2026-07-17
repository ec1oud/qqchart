import QtQuick
import QtQuick.Effects
import org.ecloud.charts 1.0

Window {
    width: 480
    height: 320
    //x: 1920 - width - 4
    y: 1080 - height
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
            model: LmSensors.filtered(Sensor.Memory)
            LabeledSensorGraph {
                model: modelData
                color: Qt.rgba(0.3 + index * 0.33, 1 - index * 0.27, 0.7 + index * 0.11, 1)
                labelLine: index + 2
                lineWidth: 1
                visible: modelData.maxValue > 0
            }
        }
    }
    MultiEffect {
        source: content
        anchors.fill: content
        shadowEnabled: true
        shadowColor: "#80000000"
        shadowHorizontalOffset: 1
        shadowVerticalOffset: 1
        shadowBlur: 0.3
    }
}
