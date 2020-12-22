import QtQuick 2.5
import org.ecloud.charts 1.0

LineGraphWithHoverFeedback {
    id: root
    property int labelLine: 0
    timeSpan: 600
    height: 100
    x: 4
    width: parent.width - 8
    property alias lineHeight: label.font.pixelSize
    Text {
        id: label
        color: parent.color
        text: model.label + (model.chipName === "" ? "" : " (" + model.chipName + ")")
        y: labelLine * font.pixelSize
        font.weight: Font.Bold
    }
    Row {
        anchors.left: parent.horizontalCenter
        spacing: 10
        Text {
            color: "white"
            text: model.currentValue.toFixed(2) + " " + model.unit
            y: labelLine * font.pixelSize
            font.weight: Font.Bold
        }
        Text {
            color: "darkgrey"
            text: "(max " + Math.round(model.maxSampleValue) + " / " + Math.round(root.maxValue) + ")"
            y: labelLine * font.pixelSize
            font.weight: Font.Bold
        }
    }
}
