import QtQuick 2.5
import org.ecloud.charts 1.0

LineGraph {
    id: root
    anchors.fill: parent
    anchors.margins: 6
    property int labelLine: 0
    timeSpan: width / 0.5
    Text {
        color: parent.color
        text: model.label + " (" + model.chipName + ")"
        y: labelLine * font.pixelSize
    }
    Row {
        anchors.left: parent.horizontalCenter
        spacing: 10
        Text {
            color: "white"
            text: model.currentValue.toFixed(2) + " " + model.unit
            y: labelLine * font.pixelSize
        }
        Text {
            color: "grey"
            text: "(max " + Math.round(model.maxSampleValue) + " / " + Math.round(root.maxValue) + ")"
            y: labelLine * font.pixelSize
        }
    }
}
