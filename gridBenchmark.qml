import QtQuick 2.0
import QQChart 1.0
import QtQuick.Layouts 1.2

Rectangle {
    id: root
    color: "black"
    width: 640
    height: 480

    GridLayout {
        id: grid
        columns: 3
        rowSpacing: 8
        columnSpacing: 8
        anchors.fill: parent

        Repeater {
            model: 12

            Chart2D {
                id: chart
                model: tableModel
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: Qt.rgba(0.5+(48 - index)*Math.random()/48,
                               0.3+index*Math.random()/48,
                               0.3*Math.random(),
                               1.0)
                gridColor: "yellow"
                //        clip: true
                Rectangle {
                    border.color: "yellow"
                    color: "transparent"
                    anchors.fill: parent
                }
                Timer {
                    interval: 10 + index; running: true; repeat: true
                    onTriggered: {
                        horizontalZoom = Math.random() * 2 * defaultHorizontalZoom
//                        console.log("timer for index " + index + " gave us " + horizontalZoom)
                    }
                }
            }

        }
    }

    DataSequenceModel {
        id: tableModel
        csvSource: "aapl.csv"
    }
}
