import QtQuick 2.5
import org.ecloud.charts 1.0

Rectangle {
    id: root
    width: 600
    height: 640
    color: "black"
    property int timespanHours : 2400
    InfluxQuery {
        id: query
        server: "http://localhost:8086"
        database: "weather"
        measurement: "uradmonitor"
        fields: ["temperature", "pressure"]
        wherePairs: [{"stationId": "41000008"}]
        timeConstraint: "> now() - " + root.timespanHours + "h"
        updateIntervalMs: 600000
        sampleInterval: timespanHours * 3600 / root.width * 2
        Component.onCompleted: sampleAllValues()
    }
    ListView {
        id: list
        anchors.fill: parent
        anchors.margins: 6
        model: query.values

        delegate: Rectangle {
            width: parent.width
            height: 75
            border.color: "#111"
            color: "transparent"
            LineGraph {
                id: graph
                model: modelData
                anchors.fill: parent
                timeSpan: root.timespanHours * 3600
                property bool isTemperature: label === "temperature"
                color: isTemperature ? "green" : "yellow"
                minValue: model.minValue // continuously, along with autoScale
                maxValue: model.maxValue
                warningMinColor: isTemperature ? "cyan" : "orange"
                warningMaxColor: "orange"
                Component.onCompleted: {
                    model.clipValues = false // because we will auto-scale
                    if (isTemperature)
                        model.additiveCorrection = -11 // known-inaccurate sensor
                }
                onSamplesChanged: model.autoScale()
            }
            Text {
                text: label + "\nscale " + Math.round(graph.timeSpan / parent.width / 60) + " min / px;" +
                      " min " + minSampleValue.toFixed(2) + " max " + maxSampleValue.toFixed(2) +
                      " norm " + normalMinValue.toFixed(2) + ".." + normalMaxValue.toFixed(2)
                color: "grey"
            }
            Text {
                anchors {
                    right: parent.right
                    top: parent.top
                }
                horizontalAlignment: Text.Right
                text: maxValue
                color: "grey"
            }
            Text {
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                }
                horizontalAlignment: Text.Right
                text: minValue
                color: "grey"
            }
        }
    }
}
