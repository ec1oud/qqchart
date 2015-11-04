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
        anchors.bottomMargin: 180
        anchors.topMargin: 180
        color: "lightsteelblue"

        Component.onCompleted: {
            for (var i = 0; i < 2; ++i)
                appendSampleToAll(newSample(i));
            appendSampleToAll(0.5);
            appendSampleToAll(0);
            appendSampleToAll(1);
            appendSampleToAll(0);
            appendSampleToAll(1);
            appendSampleToAll(0.8);
            appendSampleToAll(0.6);
            appendSampleToAll(0.4);
            appendSampleToAll(0.2);
            appendSampleToAll(0);
            appendSampleToAll(0.01);
            appendSampleToAll(0.02);
            appendSampleToAll(0.03);
            appendSampleToAll(0.04);
            appendSampleToAll(0.05);
            appendSampleToAll(0.06);
            appendSampleToAll(0.07);
            appendSampleToAll(0.08);
            appendSampleToAll(0.09);
            appendSampleToAll(0.1);
            appendSampleToAll(0.08);
            appendSampleToAll(0.06);
            appendSampleToAll(0.04);
            appendSampleToAll(0.02);
            appendSampleToAll(0);
        }

    }

//    GraphStack {
//        anchors.fill: graphs
//        color: "#DDFFFF"

//        Component.onCompleted: {
//            for (var i = 0; i < 20; ++i)
//                appendSampleToAll(newSample(i));
//        }

//    }

//    GraphStack {
//        anchors.fill: graphs
//        color: "#DDFFDD"

//        Component.onCompleted: {
//            for (var i = 0; i < 100; ++i)
//                appendSampleToAll(newSample(i));
//        }

//    }

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
                    maximumValue: 50
                    value: 35
                }
                Text {
//                    anchors.top: widthSlider.bottom
                    text: "line width " + widthSlider.value.toFixed(2)
                }
            }
            CheckBox {
                id: fillCb
                text: "fill"
//                checked: true
            }
            CheckBox {
                id: aaCb
                text: "antialiasing"
//                checked: true
            }
            CheckBox {
                id: originalLineCb
                text: "actual samples"
                checked: true
            }
            CheckBox {
                id: wireframeCb
                text: "wireframe"
                checked: true
            }
            CheckBox {
                id: timerRun
                text: "periodic update"
            }
        }
    }
}
