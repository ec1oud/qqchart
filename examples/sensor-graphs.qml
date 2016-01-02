import QtQuick 2.5
import org.ecloud.charts 1.0

Rectangle {
    width: 212
    height: 320
    color: "black"
    ListView {
        anchors.fill: parent
        anchors.margins: 6
        model: lmSensors.items
        delegate: Rectangle {
            width: 200
            height: 75
            border.color: "#111"
            color: "transparent"
            SensorGraph {
                sensor: modelData
                anchors.fill: parent
                timeScale: 1
                color: "green"
                alertMaxColor: "red"
                alertMaxValue: 100
                antialiasing: true
                MouseArea {
                    anchors.fill: parent
                    onWheel: {
                        if (wheel.angleDelta.y > 0)
                            parent.timeScale *= 10;
                        else
                            parent.timeScale /= 10;
                    }
                }
                Text {
                    text: label + ": " + value.toFixed(2) + " scale " + parent.timeScale
                    color: "white"
                }
            }
        }
    }
    property LmSensors lmSensors: LmSensors { }
    Timer {
        interval: 1000; running: true; repeat: true
        onTriggered: lmSensors.do_sampleValues()
    }
}
