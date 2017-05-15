import QtQuick 2.5
import org.ecloud.charts 1.0

LineGraph {
    id: root
    anchors.fill: parent
    anchors.margins: 10
    property int labelLine: 0
    timeSpan: 600
    property int hoverX: -1
    property var hoverSample: null

    function updateHover() {
        if (hoverX < 0 || hoverX > width) {
            hoverSample = null
        } else {
            hoverSample = sampleNearestX(hoverX)
        }
    }

    onHoverXChanged: updateHover()
    onSamplesChanged: updateHover()

    Rectangle {
        id: pointFeedback
        x: hoverSample ? hoverSample.x - width / 2 : 0
        y: hoverSample ? hoverSample.y - height / 2 : 0
        color: "red" // root.color
        width: 7
        height: 7
        radius: width / 2
        border.color: "white"
        antialiasing: true
//        visible: hoverSample

        Rectangle {
            id: labelBackground
            color: "#55555533"
            border.color: root.color
            width: valueLabel.implicitWidth * 1.2
            height: valueLabel.implicitHeight * 1.5
            radius: height * 0.2
            anchors.right: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            Text {
                id: valueLabel
                anchors.centerIn: parent
                text: hoverSample ? hoverSample.value.toFixed(2) : ""
                color: "white"
            }
        }
    }
    Text {
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
            color: "grey"
            text: "(max " + Math.round(model.maxSampleValue) + " / " + Math.round(root.maxValue) + ")"
            y: labelLine * font.pixelSize
            font.weight: Font.Bold
        }
    }
}
