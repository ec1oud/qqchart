import QtQuick
import QtQuick.Controls
import org.ecloud.charts 1.0

// The four ways LineGraph can draw the same data - plain stroke, fill below, fill above
// and wireframe - with checkboxes to switch the join style and to lay a stroke on top of
// the filled panels.
Rectangle {
    id: root
    width: 900
    height: 640
    color: "#222"

    property int joinStyle: roundJoinCb.checked ? Qt.RoundJoin : Qt.BevelJoin

    // one model shared by every panel
    LineGraphModel {
        id: sharedModel
        clipValues: false
        minValue: -1
        maxValue: 1
        downsampleMethod: LineGraphModel.NoDownsample
        Component.onCompleted: {
            for (var i = 0; i < 60; ++i) {
                var v = Math.sin(i / 60 * Math.PI * 4) * 0.6
                if (i % 11 === 0)                       // occasional sharp spike
                    v = (i % 22 === 0) ? 0.95 : -0.95
                appendSample(v, i)
            }
        }
    }

    Grid {
        id: grid
        columns: 2
        spacing: 12
        anchors {
            left: parent.left; right: parent.right; top: parent.top
            bottom: controls.top; margins: 12
        }

        Repeater {
            model: ["stroke", "fill below", "fill above", "wireframe"]

            Rectangle {
                width: (grid.width - grid.spacing) / 2
                height: (grid.height - grid.spacing) / 2
                color: "#1a1a1a"
                border.color: "#3a3a3a"

                readonly property bool isFill: modelData === "fill below" || modelData === "fill above"

                // base layer: stroke, fill or wireframe
                LineGraph {
                    anchors.fill: parent
                    anchors.margins: 10
                    model: sharedModel
                    minValue: -1; maxValue: 1; timeSpan: 60
                    antialiasing: true
                    joinStyle: root.joinStyle
                    lineWidth: 3
                    color: "cyan"
                    wireframe: modelData === "wireframe"
                    fillColorBelow: modelData === "fill below" ? "#8000c0ff" : "transparent"
                    fillColorAbove: modelData === "fill above" ? "#80ffaa00" : "transparent"
                }

                // optional stroke laid on top of the filled panels
                LineGraph {
                    anchors.fill: parent
                    anchors.margins: 10
                    visible: isFill && strokeOverFillCb.checked
                    model: sharedModel
                    minValue: -1; maxValue: 1; timeSpan: 60
                    antialiasing: true
                    joinStyle: root.joinStyle
                    lineWidth: 2
                    color: "white"
                }

                Text { color: "white"; x: 12; y: 8; text: modelData }
            }
        }
    }

    Row {
        id: controls
        anchors { left: parent.left; bottom: parent.bottom; margins: 16 }
        spacing: 28
        palette.windowText: "white"

        CheckBox {
            id: roundJoinCb
            text: checked ? "join: round" : "join: bevel"
            checked: true
        }
        CheckBox {
            id: strokeOverFillCb
            text: "stroke over fill"
            checked: true
        }
    }
}
