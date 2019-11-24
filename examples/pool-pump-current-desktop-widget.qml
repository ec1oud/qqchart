#!/bin/env qml

import QtQuick 2.12
import QtQuick.Window 2.12
import Qt.labs.settings 1.0
import org.ecloud.charts 1.0

Window {
    id: window
    width: 740
    height: 360
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnBottomHint
    visible: true

    Settings {
        id: settings
        category: "PoolPumpDesktopWidget"
        property alias x: window.x
        property alias y: window.y
        property string server
        property string user
        property string password
    }

    property int timespanHours : 720
    InfluxQuery {
        id: query1
        server: settings.server
        user: settings.user
        password: settings.password
        ignoreSslErrors: true // such as self-signed certificate or hostname not matching
        // graph for data from https://github.com/ec1oud/devcron/blob/master/statistics/cs5490-current.lua
        database: "homeauto"
        measurement: "current_max_hourly"
        fields: ["max_current"]
        wherePairs: [{"device": "poolpump"}]
        timeConstraint: "> now() - " + window.timespanHours + "h"
        updateIntervalMs: sampleInterval * 1000
        sampleInterval: 3600
        Component.onCompleted: {
            sampleAllValues()
            values[0].maxValue = 3.5
        }
    }
    LineGraphWithHoverFeedback {
        id: graph
		model: query1.values[0]
		anchors.fill: parent
        anchors.margins: 10
        timeSpan: query1.values[0].timeSpan
        color: "lightsteelblue"
//        fillColorBelow: "lightsteelblue"
        warningMaxColor: "lightsteelblue"
        minValue: 0
        maxValue: 3.5
        lineWidth: 1
        hoverX: hoverCursor.hoverX
        HoverCursor {
            id: hoverCursor
            timeSpan: query1.values[0].timeSpan
            lastSampleTime: query1.lastSampleTime
        }
    }
	Text {
        text: "Pool pump current: last " + (window.timespanHours > 24 ?
                                       (window.timespanHours / 24) + " days" :
                                       window.timespanHours + " hours") +
              " as of " + query1.lastSampleTime
		font.bold: true
		color: "black"
	}
	Text {
		anchors {
			right: parent.right
			top: parent.top
            rightMargin: 10
        }
		horizontalAlignment: Text.Right
        text: query1.values[0].maxValue
		color: "black"
	}
	Text {
		anchors {
			right: parent.right
            bottom: parent.bottom
            rightMargin: 10
        }
		horizontalAlignment: Text.Right
        text: query1.values[0].minValue
		color: "black"
    }
}
