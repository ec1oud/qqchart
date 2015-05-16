import QtQuick 2.0
import QQChart 1.0

Rectangle {
    id: root
    color: "black"
    width: chart.width + 12
    height: chart.height + 12

    DataSequenceModel {
        id: tableModel
        csvSourceFile: "aapl.csv"
    }

    Chart2D {
        id: chart
        model: tableModel
        width: 815.6; height: 702.41
        anchors.centerIn: parent
        color: "cyan"
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
