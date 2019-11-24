import QtQuick 2.5
import org.ecloud.charts 1.0

Rectangle {
    id: root
    width: 580
    height: 360
    color: "transparent"
    border.color: "#6e4b4d"
    antialiasing: true
    radius: 20
    property int timespanHours : 240
    property bool currentVisible: false
    InfluxQuery {
        id: query
        server: "http://localhost:8086"
        database: "weather"
        measurement: "uradmonitor"
        fields: ["temperature", "pressure", "humidity"]
        //wherePairs: [{"stationId": "41000008"}]
        wherePairs: [{"stationId": "820000ED"}]
        timeConstraint: "> now() - " + root.timespanHours + "h"
        updateIntervalMs: sampleInterval * 1000
        sampleInterval: timespanHours * 3600 / root.width * 2
        Component.onCompleted: sampleAllValues()
    }
    ListView {
        id: list
        anchors.fill: parent
        anchors.margins: 8
        model: query.values
        spacing: 4
        clip: true

        delegate: Item {
            width: list.width
            property int visibleRows: Math.min(4, list.model.length)
            property bool isTemperature: label === "temperature"
            property bool isPressure: label === "pressure"
            height: (list.height - 4 * (visibleRows - 1)) / visibleRows
            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                color: "darkgrey"
                styleColor: "black"
                style: Text.Outline
                font.pointSize: 24
                font.bold: true
                text: model.currentValue.toFixed(isPressure ? 0 : 2) + model.unit
                visible: currentVisible
            }
            LineGraphWithHoverFeedback {
                id: graph
                model: modelData
                anchors.fill: parent
                timeSpan: root.timespanHours * 3600
                color: isTemperature ? "green" : "yellow"
                minValue: model.minValue // continuously, along with autoScale
                maxValue: model.maxValue
                warningMinColor: isTemperature ? "cyan" : "orange"
                warningMaxColor: "orange"
                lineWidth: isTemperature ? 2 : 1
                hoverX: hoverCursor.mouseX
                Component.onCompleted: {
                    model.clipValues = false // because we will auto-scale
                    if (isTemperature)
                        model.additiveCorrection = -5 // -11 // known-inaccurate sensor
                }
                onSamplesChanged: model.autoScale()
            }
            Text {
                text: label + ": last " + (root.timespanHours > 24 ?
                                               (root.timespanHours / 24) + " days" :
                                               root.timespanHours + " hours")
                font.bold: true
                color: "black"
            }
            Text {
                anchors {
                    right: parent.right
                    top: parent.top
                }
                horizontalAlignment: Text.Right
                text: maxValue
                color: "black"
            }
            Text {
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                }
                horizontalAlignment: Text.Right
                text: minValue
                color: "black"
            }
        }
    }
    HoverCursor {
        id: hoverCursor
        anchors.margins: 8
        timeSpan: query1.values[0].timeSpan
        lastSampleTime: query1.lastSampleTime
    }
}
