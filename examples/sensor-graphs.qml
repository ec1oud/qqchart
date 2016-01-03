import QtQuick 2.5
import org.ecloud.charts 1.0

Rectangle {
    width: 420
    height: 640
    color: "black"
    ListView {
        id: list
        anchors.fill: parent
        anchors.margins: 6
        model: lmSensors.items
        delegate: Rectangle {
            width: parent.width
            height: 75
            border.color: "#111"
            color: "transparent"
            SensorGraph {
                id: graph
                sensor: modelData
                anchors.fill: parent
                timeScale: 1
                color: "green"
                alertMinColor: "red"
                alertMaxColor: "red"
                alertMaxValue: valueMax // TODO find warning value
                alertMinValue: valueMin
//                MouseArea {
//                    anchors.fill: parent
//                    onWheel: {
//                        if (wheel.angleDelta.y > 0)
//                            parent.timeScale *= 10;
//                        else
//                            parent.timeScale /= 10;
//                    }
//                }
                Text {
                    text: label + ": " + value.toFixed(2) + " " + unit + " scale " + parent.timeScale +
                          " min " + minValue.toFixed(2) + " max " + maxValue.toFixed(2)
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
    property LmSensors lmSensors: LmSensors { }
    Timer {
        interval: 1000; running: true; repeat: true
        onTriggered: lmSensors.sampleAllValues()
    }
}
