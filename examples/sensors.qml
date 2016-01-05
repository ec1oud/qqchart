import QtQuick 2.5
import org.ecloud.charts 1.0

ListView {
    width: 200
    height: 200
    model: LmSensors.sensors
    delegate: Text { text: label + ": " + currentValue.toFixed(2) }
}
