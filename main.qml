import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import LineGraph 1.0

Rectangle {
    width: 800
    height: 400
    color: "black"

    GraphStack {
        id: graphs
        anchors.fill: parent
        anchors.margins: 50
        anchors.bottomMargin: 180
        anchors.topMargin: 180
        color: "lightsteelblue"

        Component.onCompleted: {
            appendSampleToAll(0.3); // 0
            appendSampleToAll(0.5); // 1
            appendSampleToAll(0);   // 2
            appendSampleToAll(1);   // 3
            appendSampleToAll(0);   // 4
            appendSampleToAll(1);   // 5
            appendSampleToAll(0);   // 6
            appendSampleToAll(1);   // 7
            appendSampleToAll(0.9); // 8
            appendSampleToAll(1.01);
            appendSampleToAll(0.98);
            appendSampleToAll(0.9);
            appendSampleToAll(0.2);
            appendSampleToAll(0.02);
            appendSampleToAll(0);
            appendSampleToAll(0.01);
            appendSampleToAll(0.03);
            appendSampleToAll(0.02);
            appendSampleToAll(0.04);
            appendSampleToAll(0.07);
            appendSampleToAll(0.05);
            appendSampleToAll(0.06);
            appendSampleToAll(0.18);
            appendSampleToAll(0.09);
            appendSampleToAll(0.1);
            appendSampleToAll(0.08);
            appendSampleToAll(-0.06);
            appendSampleToAll(0.04);
            appendSampleToAll(0.02);
            appendSampleToAll(0);
            appendSampleToAll(0.1);
        }

    }

    Rectangle {
        anchors.fill: graphs
        color: "transparent"
        border.color: "#40444444"
        border.width: 2
        Row {
            anchors.top: parent.bottom
            anchors.margins: 50
            width: parent.width
            spacing: 20
            opacity: 0.5
            Column {
                width: 300
                Slider {
                    id: widthSlider
                    width: 300
                    minimumValue: 1.5
                    maximumValue: 50
                    value: 1.75
                }
                Text {
//                    anchors.top: widthSlider.bottom
                    text: "line width " + widthSlider.value.toFixed(2)
                    color: "white"
                }
            }
            Component {
                id: whiteCheckboxStyle
                CheckBoxStyle {
                    label: Text {
                        text: control.text
                        color: "white"
                    }
                }
            }

            CheckBox {
                id: fillCb
                text: "fill"
                style: whiteCheckboxStyle
                checked: true
            }
            CheckBox {
                id: aaCb
                text: "antialiasing"
                style: whiteCheckboxStyle
                checked: true
            }
            CheckBox {
                id: originalLineCb
                text: "actual samples"
                style: whiteCheckboxStyle
//                checked: true
            }
            CheckBox {
                id: wireframeCb
                text: "wireframe"
                style: whiteCheckboxStyle
//                checked: true
            }
            CheckBox {
                id: timerRun
                text: "periodic update"
                style: whiteCheckboxStyle
            }
        }
    }
}
