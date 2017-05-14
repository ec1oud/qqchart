#!/bin/env qml

import QtQuick 2.5
import QtQuick.Window 2.1
import Qt.labs.settings 1.0

Window {
    id: window
    width: 580
    height: 728
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnBottomHint
    visible: true

    Settings {
        category: "WeatherSummaryDesktopWidget"
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }

    Column {
        id: column
        spacing: 8
        anchors.fill: parent
        InfluxWeatherGraph {
            timespanHours: 72
            height: (window.height - column.spacing) / 2
            currentVisible: true
        }
        InfluxWeatherGraph {
           timespanHours: 720 // a month
           height: (window.height - column.spacing) / 2
        }
    }
}
