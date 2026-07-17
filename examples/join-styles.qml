import QtQuick
import org.ecloud.charts 1.0

// Compares LineGraph.joinStyle. The same spiky data is drawn opaque and translucent,
// each way, so you can see the trade-off:
//   Qt.RoundJoin - truly round joins/caps, full width at any angle, but a thick
//                  translucent stroke shows a slight blob where segments overlap at a join.
//   Qt.BevelJoin - a single non-overlapping ribbon, so a translucent stroke blends once
//                  (no blobs), at the cost of beveled rather than round joins.
Column {
    width: 800; height: 600

    // sharp zig-zag to stress the joins
    property var pts: [0, 0.9, -0.9, 0.85, -0.85, 0.2, 0.2, -0.5, 0.7, -0.7, 0.1, 0.6, -0.95, 0.95, 0]

    Repeater {
        model: [
            { js: Qt.RoundJoin, c: "cyan",      lw: 10, label: "Qt.RoundJoin  opaque" },
            { js: Qt.BevelJoin, c: "cyan",      lw: 10, label: "Qt.BevelJoin  opaque" },
            { js: Qt.RoundJoin, c: "#66ff2222", lw: 14, label: "Qt.RoundJoin  translucent (blobs at joins)" },
            { js: Qt.BevelJoin, c: "#66ff2222", lw: 14, label: "Qt.BevelJoin  translucent (no blobs)" }
        ]

        Rectangle {
            width: parent.width
            height: parent.height / 4
            color: (index % 2) ? "#1c1c1c" : "#242424"

            Text { color: "white"; x: 12; y: 6; text: modelData.label }
            LineGraph {
                // opacity: 0.1
                anchors.fill: parent
                anchors.margins: 10
                color: modelData.c
                lineWidth: modelData.lw
                joinStyle: modelData.js
                antialiasing: true
                minValue: -1
                maxValue: 1
                timeSpan: 15
                model: LineGraphModel {
                    clipValues: false
                    minValue: -1
                    maxValue: 1
                    downsampleMethod: LineGraphModel.NoDownsample
                    Component.onCompleted: {
                        for (var i = 0; i < pts.length; ++i)
                            appendSample(pts[i], i)
                    }
                }
            }
        }
    }
}
