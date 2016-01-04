import QtQuick 2.5
import org.ecloud.charts 1.0

SensorGraph {
    id: root
    anchors.fill: parent
    anchors.margins: 6
    property int labelLine: 0
    Text {
        color: parent.color
        text: root.sensor.label + " (" + root.sensor.chipName + ")"
        y: labelLine * font.pixelSize
    }
    Row {
        anchors.left: parent.horizontalCenter
        spacing: 10
        Text {
            color: "white"
            text: root.sensor.value.toFixed(2) + " " + root.sensor.unit
            y: labelLine * font.pixelSize
        }
        Text {
            color: "grey"
            text: "(max " + Math.round(root.sensor.maxValue) + " / " + Math.round(root.maxValue) + ")"
            y: labelLine * font.pixelSize
        }
    }
}
