import QtQuick 2.0
import QtQuick.Controls 1.4

import LineGraph 1.0

Item {
    width: 800
    height: 400

    GraphStack {
        id: graphs
        anchors.fill: parent
        anchors.margins: 50
        anchors.bottomMargin: 250
        color: "lightsteelblue"

        Component.onCompleted: {
            for (var i=0; i<2; ++i)
                appendSampleToAll(newSample(i));
            appendSampleToAll(0.5);
            appendSampleToAll(0);
            appendSampleToAll(1);
            appendSampleToAll(0);
            appendSampleToAll(1);
            for (var i=2; i<20; ++i)
                appendSampleToAll(newSample(i));
        }

    }

    GraphStack {
        anchors.fill: graphs
        color: "red"

        Component.onCompleted: {
            for (var i=0; i<20; ++i)
                appendSampleToAll(newSample(i));
        }

    }

    GraphStack {
        anchors.fill: graphs
        color: "green"

        Component.onCompleted: {
            for (var i=0; i<100; ++i)
                appendSampleToAll(newSample(i));
        }

    }

    Rectangle {
        anchors.fill: graphs
        color: "transparent"
        border.color: "black"
        border.width: 2
        Row {
            anchors.top: parent.bottom
            width: parent.width
            Column {
                width: 300
                Slider {
                    id: widthSlider
                    width: 300
                    minimumValue: 1
                    maximumValue: 5
                    value: 3
                }
                Text {
//                    anchors.top: widthSlider.bottom
                    text: "line width " + widthSlider.value.toFixed(2)
                }
            }
            CheckBox {
                id: aaCb
                text: "antialiasing"
                checked: true
            }
            CheckBox {
                id: originalLineCb
                text: "actual samples"
//                checked: true
            }
            CheckBox {
                id: wireframeCb
                text: "wireframe"
//                checked: true
            }
            CheckBox {
                id: timerRun
                text: "periodic update"
            }
        }
    }
}
