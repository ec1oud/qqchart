import QtQuick 2.0
import QQChart 1.0

Item {
    id: root
    width: 640
    height: 320

    DataSequenceModel {
        id: tableModel
        csvSourceFile: "table.csv"
    }

    Column {
        anchors.fill: parent
        anchors.leftMargin: 10

        Rectangle {
            width: 10
            height: 10
        }

        Chart2D {
            model: tableModel
            x:50
            height: 50; width: 600
            //        clip: true
            Rectangle {
                border.color: "black"
                color: "transparent"
                anchors.fill: parent
            }
        }
        Rectangle {
            width: 10
            height: 10
        }
        Chart2D {
            model: tableModel
            height: root.height / 2; width: 600
            color: "blue"
            //        clip: true
            Rectangle {
                border.color: "cyan"
                color: "transparent"
                anchors.fill: parent
            }
        }
    }
}
