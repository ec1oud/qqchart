import QtQuick
import org.ecloud.charts 1.0

// Shows the same data stroked at a range of line widths, including a sub-pixel
// width and some sharp spikes, to check that antialiasing stays consistent and
// the stroke reads as a solid flat-topped band at any width or angle.
Column {
    width: 900; height: 600

    Repeater {
        model: [0.5, 1, 2, 4, 8]

        Rectangle {
            width: parent.width
            height: parent.height / 5
            color: (index % 2) ? "#1a1a1a" : "#202020"

            LineGraph {
                anchors.fill: parent
                anchors.margins: 8
                color: "cyan"
                lineWidth: modelData
                minValue: -1
                maxValue: 1
                timeSpan: 40
                antialiasing: true
                model: LineGraphModel {
                    clipValues: false
                    minValue: -1
                    maxValue: 1
                    downsampleMethod: LineGraphModel.NoDownsample
                    Component.onCompleted: {
                        // gentle curve plus sharp spikes, to stress steep angles and joins
                        for (var i = 0; i < 40; ++i) {
                            var v = Math.sin(i / 40 * Math.PI * 3) * 0.6
                            if (i % 7 === 0)
                                v = (i % 14 === 0) ? 0.95 : -0.95
                            appendSample(v, i)
                        }
                    }
                }
            }
            Text { color: "white"; x: 12; y: 6; text: "lineWidth " + modelData }
        }
    }
}
