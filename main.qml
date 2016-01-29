import QtQuick 2.0
import QQChart 1.0

Rectangle {
    id: root
    color: "black"
    width: 1900
    height: 1000

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
        horizontalZoom: defaultHorizontalZoom
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
