import QtQuick 2.0
import QtQuick.Controls 1.4

import org.ecloud.charts 1.0

Item {
    id: root
    property alias color: graph.color
    property alias antialiasing: graph.antialiasing
    property LineGraphModel model: null

    function newSample(i) {
//        return (Math.sin(i / 20.0 * Math.PI * 2) + 1) * 0.4;
//        return (Math.sin(i / 20.0 * Math.PI * 2) + 1) * 0.4 + Math.random() * 0.05;
        return Math.sin(i / 20.0 * Math.PI * 2) * 0.95 + Math.random() * 0.05;
    }

    property int offset: 25;

    LineGraph {
        id: graph
        anchors.fill: parent
        model: root.model
        minValue: -1 + offsetSlider.value
        maxValue: 1 + offsetSlider.value
        lineWidth: widthSlider.value
        color: "lightsteelblue"
        fillColorBelow: fillBelowCb.checked ? "#22FF2222" : "transparent"
        fillColorAbove: fillAboveCb.checked ? "#222222FF" : "transparent"
        warningMinColor: "yellow"
        warningMaxColor: "orange"
        wireframe: false
        visible: fillCb.checked
        opacity: 0.8
        timeScale: width / 30
    }

    LineGraph {
        id: wireframe
        anchors.fill: graph
        model: root.model
        minValue: -1 + offsetSlider.value
        maxValue: 1 + offsetSlider.value
        lineWidth: widthSlider.value
        color: graph.visible ? "black" : "white"
        warningMinColor: "black"
        warningMaxColor: "black"
        wireframe: true
        timeScale: graph.timeScale
        visible: wireframeCb.checked
    }

    LineGraph {
        id: plainLine
        anchors.fill: graph
        model: root.model
        minValue: -1 + offsetSlider.value
        maxValue: 1 + offsetSlider.value
        lineWidth: 0
        color: "red"
        warningMinColor: "red"
        warningMaxColor: "red"
        wireframe: true
        timeScale: graph.timeScale
        visible: originalLineCb.checked
    }

    Timer {
        id: timer
        interval: 500
        repeat: true
        running: timerRun.checked
        onTriggered: model.appendSample(root.newSample(++root.offset))
    }
}
