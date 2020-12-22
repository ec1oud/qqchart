import QtQuick 2.5
import org.ecloud.charts 1.0

LineGraph {
    id: root
    property int hoverX: -1
    property var hoverSample: null

    function updateHover() {
        if (hoverX < 0) {
            hoverSample = null
        } else {
            hoverSample = sampleNearestX(hoverX)
        }
    }

    onHoverXChanged: updateHover()
    onSamplesChanged: updateHover()

    Rectangle {
        id: pointFeedback
        x: hoverSample ? hoverSample.x - width / 2 : -100
        y: hoverSample ? hoverSample.y - height / 2 : -100
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
}
