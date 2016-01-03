import QtQuick 2.5
import org.ecloud.charts 1.0

Rectangle {
    width: 600
    height: 200
    color: "black"
    property var colors: ["grey", "cyan", "magenta", "violet", "steelblue", "wheat", "beige", "brown"]
    Repeater {
        id: list
        anchors.fill: parent
        anchors.margins: 6
        model: lmSensors.byType(Sensor.Temperature)
        SensorGraph {
            sensor: modelData
            anchors.fill: parent
            color: colors[index]
        }
    }
    property LmSensors lmSensors: LmSensors { }
    Timer {
        interval: 1000; running: true; repeat: true
        onTriggered: lmSensors.sampleAllValues()
    }
}
