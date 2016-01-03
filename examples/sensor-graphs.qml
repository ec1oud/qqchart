import QtQuick 2.5
import org.ecloud.charts 1.0

Rectangle {
    width: 600
    height: 640
    color: "black"
    ListView {
        id: list
        anchors.fill: parent
        anchors.margins: 6
        model: LmSensors.sensors
        delegate: Rectangle {
            width: parent.width
            height: 75
            border.color: "#111"
            color: "transparent"
            SensorGraph {
                id: graph
                sensor: modelData
                anchors.fill: parent
                Text {
                    text: label + " (" + chipName + "):\nscale " + parent.timeScale +
                          " min " + minValue.toFixed(2) + " max " + maxValue.toFixed(2) +
                          " norm " + normalMin.toFixed(2) + ".." + normalMax.toFixed(2)
                    color: "grey"
                }
                Text {
                    text: value.toFixed(2) + " " + unit
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: "white"
                }
                Text {
                    anchors {
                        right: parent.right
                        top: parent.top
                    }
                    horizontalAlignment: Text.Right
                    text: graph.maxValue
                    color: "grey"
                }
                Text {
                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                    }
                    horizontalAlignment: Text.Right
                    text: graph.minValue
                    color: "grey"
                }
            }
        }
    }
}
