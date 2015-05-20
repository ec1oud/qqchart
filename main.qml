import QtQuick 2.0
import QQChart 1.0

Rectangle {
    id: root
    color: "black"
    width: 640
    height: 480

    DataSequenceModel {
        id: tableModel
        csvSource: "aapl.csv"
    }

    Chart2D {
        id: chart
        model: tableModel
        anchors.fill: parent
        anchors.margins: 6
        color: "cyan"
        gridColor: "yellow"
        //        clip: true
        Rectangle {
            border.color: "yellow"
            color: "transparent"
            anchors.fill: parent
        }
        MouseArea {
            anchors.fill: parent
            onWheel: chart.horizontalZoom += chart.horizontalZoom * (wheel.angleDelta.y / 120 * 0.1)
        }
    }
}
