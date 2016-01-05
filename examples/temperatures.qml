import QtQuick 2.5
import org.ecloud.charts 1.0

Rectangle {
    width: 480
    height: 320
    color: "black"
    property var colors: ["grey", "cyan", "magenta", "violet", "steelblue", "wheat", "beige", "brown"]
    Repeater {
        id: list
        anchors.fill: parent
        anchors.margins: 6
        model: LmSensors.filtered(Sensor.Temperature)
        LineGraph {
            model: modelData
            anchors.fill: parent
            color: colors[index]
            Text {
                color: parent.color
                text: modelData.label + " (" + modelData.chipName + ")"
                y: index * font.pixelSize
            }
            Text {
                color: "white"
                text: modelData.currentValue.toFixed(2) + " " + modelData.unit
                anchors.horizontalCenter: parent.horizontalCenter
                y: index * font.pixelSize
            }
        }
    }
}
