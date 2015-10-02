import QtQuick 2.0
import QtQuick.Controls 1.4

import LineGraph 1.0

Item {
    width: 800
    height: 400

    LineGraph {
        id: graph
        anchors.fill: parent
        anchors.margins: 50
        anchors.bottomMargin: 250
        lineWidth: widthSlider.value
        color: "lightsteelblue"
        wireframe: false
        antialiasing: aaCb.checked
        opacity: 0.8
        timeScale: width / 30
//        clip: true

        function newSample(i) {
            return (Math.sin(i / 100.0 * Math.PI * 2) + 1) * 0.4 + Math.random() * 0.05;
        }

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

        function appendSampleToAll(s) {
            appendSample(s);
            wireframe.appendSample(s);
            plainLine.appendSample(s);
        }

        property int offset: 100;
    }

    LineGraph {
        id: wireframe
        anchors.fill: graph
        lineWidth: widthSlider.value
        color: "black"
        wireframe: true
        timeScale: graph.timeScale
        visible: wireframeCb.checked
    }

    LineGraph {
        id: plainLine
        anchors.fill: graph
        lineWidth: 1
        color: "red"
        wireframe: true
        timeScale: graph.timeScale
        visible: originalLineCb.checked
    }

    Timer {
        id: timer
        interval: 500
        repeat: true
//        running: true
        onTriggered: {
            graph.removeFirstSample();
            graph.appendSample(graph.newSample(++graph.offset));
        }
    }

    Rectangle {
        anchors.fill: graph
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
                    maximumValue: 100
                    value: 50
                }
                Text {
//                    anchors.top: widthSlider.bottom
                    text: "line width " + Math.round(widthSlider.value)
                }
            }
            CheckBox {
                id: aaCb
                checked: true
                text: "antialiasing"
            }
            CheckBox {
                id: originalLineCb
                text: "actual samples"
            }
            CheckBox {
                id: wireframeCb
                text: "wireframe"
            }
        }
    }
}
