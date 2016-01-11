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
            LineGraph {
                id: graph
                model: modelData
                anchors.fill: parent
                timeSpan: width
            }
            Text {
                text: label + " (" + chipName + "):\nscale " + parent.width / parent.timeSpan +
                      " min " + minSampleValue.toFixed(2) + " max " + maxSampleValue.toFixed(2) +
                      " norm " + normalMinValue.toFixed(2) + ".." + normalMaxValue.toFixed(2)
                color: "grey"
            }
            Text {
                text: currentValue.toFixed(2) + " " + unit
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
            }
            Text {
                anchors {
                    right: parent.right
                    top: parent.top
                }
                horizontalAlignment: Text.Right
                text: maxValue
                color: "grey"
            }
            Text {
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                }
                horizontalAlignment: Text.Right
                text: minValue
                color: "grey"
            }
        }
    }
}
