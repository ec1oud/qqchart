import QtQuick 2.0
import QQChart 1.0

Item {

    width: 640
    height: 320

    DataSequenceModel {
        id: tableModel
        csvSourceFile: "table.csv"
    }

    Chart2D {
        model: tableModel
        anchors.fill: parent
        anchors.margins: 100
        anchors.topMargin: 50
//        clip: true
        Rectangle {
            border.color: "blue"
            color: "transparent"
            anchors.fill: parent
        }
    }
    Chart2D {
        model: tableModel
        anchors.fill: parent
        anchors.leftMargin: 100
        anchors.rightMargin: 100
        anchors.topMargin: 230
        anchors.bottomMargin: 3
        clip: true
        Rectangle {
            border.color: "cyan"
            color: "transparent"
            anchors.fill: parent
        }
    }
}
