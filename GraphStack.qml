import QtQuick 2.0
import QtQuick.Controls 1.4

import LineGraph 1.0

Item {
    property alias color: graph.color

    function appendSampleToAll(s) {
        graph.appendSample(s);
        wireframe.appendSample(s);
        plainLine.appendSample(s);
    }

    function newSample(i) {
        return (Math.sin(i / 20.0 * Math.PI * 2) + 1) * 0.4 + Math.random() * 0.05;
    }

    LineGraph {
        id: graph
        anchors.fill: parent
        lineWidth: widthSlider.value
        color: "lightsteelblue"
        wireframe: false
        antialiasing: aaCb.checked
        visible: fillCb.checked
        opacity: 0.8
        timeScale: width / 30
//        clip: true

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
        lineWidth: 0
        color: "red"
        wireframe: true
        timeScale: graph.timeScale
        visible: originalLineCb.checked
    }

    Timer {
        id: timer
        interval: 500
        repeat: true
        running: timerRun.checked
        onTriggered: {
            graph.removeFirstSample();
            wireframe.removeFirstSample();
            plainLine.removeFirstSample();
            graph.appendSampleToAll(graph.newSample(++graph.offset));
        }
    }
}