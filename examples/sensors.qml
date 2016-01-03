import QtQuick 2.5
import org.ecloud.charts 1.0

ListView {
    width: 200
    height: 200
    property LmSensors lmSensors: LmSensors { }
    model: lmSensors.items
    delegate: Text { text: label + ": " + value.toFixed(2) }
    Timer {
        interval: 500; running: true; repeat: true
        onTriggered: lmSensors.sampleAllValues()
    }
}
