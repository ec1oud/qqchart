import QtQuick 2.0
import QtQuick.Controls 1.4

import LineGraph 1.0

Item {
    id: root
    property alias color: graph.color

    function appendSampleToAll(s) {
        graph.appendSample(s);
        wireframe.appendSample(s);
        plainLine.appendSample(s);
    }

    function newSample(i) {
//        return (Math.sin(i / 20.0 * Math.PI * 2) + 1) * 0.4;
        return (Math.sin(i / 20.0 * Math.PI * 2) + 1) * 0.4 + Math.random() * 0.05;
    }

    property int offset: 25;

    LineGraph {
        id: graph
        anchors.fill: parent
        lineWidth: widthSlider.value
        color: "lightsteelblue"
        alertMinColor: "yellow"
        alertMaxColor: "orange"
        wireframe: false
        antialiasing: aaCb.checked
        visible: fillCb.checked
        opacity: 0.8
        timeScale: width / 30
    }

    LineGraph {
        id: wireframe
        anchors.fill: graph
        lineWidth: widthSlider.value
        color: graph.visible ? "black" : "white"
        alertMinColor: "black"
        alertMaxColor: "black"
        wireframe: true
        timeScale: graph.timeScale
        visible: wireframeCb.checked
    }

    LineGraph {
        id: plainLine
        anchors.fill: graph
        lineWidth: 0
        color: "red"
        alertMinColor: "red"
        alertMaxColor: "red"
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
            root.appendSampleToAll(root.newSample(++root.offset));
        }
    }
}
