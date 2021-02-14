#!/bin/env qml

import QtQuick
import QtQuick.Effects
import QtCore
import org.ecloud.charts

Window {
    id: window
    width: 580
    height: 360
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnBottomHint
    visible: true

    Settings {
        category: "SensorSummaryDesktopWidget"
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }

    Rectangle {
        id: content
        anchors.fill: parent
        visible: false
        color: "transparent"
        border.color: "#6e4b4d"
        antialiasing: true
        radius: 20
        Item {
            id: bottom
            anchors.fill: parent
            anchors.topMargin: cpuFrequencyRepeater.implicitHeight

            Repeater {
                id: batteries
                model: LmSensors.filtered(Sensor.Energy)
                LabeledSensorGraph {
                    model: modelData
                    color: "lightgreen"
                    labelLine: index + 1
                    lineWidth: 1
                    //visible: modelData.maxSampleValue > 0
                    hoverX: hoverCursor.hoverX - 4
                }
            }
            Repeater {
                id: fans
                model: LmSensors.filtered(Sensor.Fan)
                LabeledSensorGraph {
                    model: modelData
                    color: Qt.rgba(0.3 + index * 0.03, 0.2 + index * 0.17, 1, 1)
                    labelLine: batteries.count + index
                    lineWidth: 3
                    //visible: modelData.maxSampleValue > 0
                    hoverX: hoverCursor.hoverX - 4
                }
            }
            Repeater {
                id: temperatures
                model: LmSensors.filtered(Sensor.Temperature)
                LabeledSensorGraph {
                    model: modelData
                    color: "#F85"
                    labelLine: batteries.count + fans.count + index
                    minValue: 25
                    maxValue: 85
                    hoverX: hoverCursor.hoverX - 4
                }
            }
            Repeater {
                model: LmSensors.filtered(Sensor.Cpu)
                LabeledSensorGraph {
                    model: modelData
                    color: "wheat"
                    labelLine: batteries.count + fans.count + temperatures.count
                    hoverX: hoverCursor.hoverX - 4
                }
            }
            Repeater {
                model: LmSensors.filtered(Sensor.Memory, "Memory Used")
                LabeledSensorGraph {
                    model: modelData
                    color: "cyan"
                    labelLine: batteries.count + fans.count + temperatures.count + 1
                    lineWidth: 2
                    hoverX: hoverCursor.hoverX - 4
                }
            }
            Repeater {
                model: LmSensors.filtered(Sensor.Memory, "Memory Cached")
                LabeledSensorGraph {
                    model: modelData
                    color: "darkgreen"
                    labelLine: batteries.count + fans.count + temperatures.count + 2
                    lineWidth: 1
                    hoverX: hoverCursor.hoverX - 4
                }
            }
            Repeater {
                model: LmSensors.filtered(Sensor.Memory, "Swap Free")
                LabeledSensorGraph {
                    model: modelData
                    color: "midnightblue"
                    labelLine: batteries.count + fans.count + 4
                    lineWidth: 3
                    visible: modelData.maxValue > 0
                    hoverX: hoverCursor.hoverX - 4
                }
            }
        }
        Item {
            anchors.top: parent.top
            anchors.topMargin: 4
            width: parent.width
            height: 100
            Repeater {
                id: cpuFrequencyRepeater
                model: LmSensors.filtered(Sensor.Frequency)
                implicitHeight: (count + 1) * 13
                LabeledSensorGraph {
                    y: 13 * index
                    height: 13
                    model: modelData
                    warningMinColor: "saddlebrown"
                    color: Qt.rgba(0, 0.5 + index * 0.5 / cpuFrequencyRepeater.count, 0.1, 1)
                    warningMaxColor: "cyan"
                    lineWidth: 1
                }
                onItemAdded: model[index].downsampleInterval = 15
            }
        }
        Rectangle {
            anchors.bottom: bottom.top
            anchors.bottomMargin: 2
            anchors.margins: 8
            anchors.left: parent.left
            anchors.right: parent.right
            height: 1
            color: content.border.color
        }
    }
    MultiEffect {
        source: content
        anchors.fill: content
        shadowEnabled: true
        shadowColor: "#80000000"
        shadowHorizontalOffset: 1
        shadowVerticalOffset: 1
        shadowBlur: 0.3
    }

    HoverCursor { id: hoverCursor }
}
