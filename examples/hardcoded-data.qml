import QtQuick
import QtQuick.Controls
import org.ecloud.charts

Rectangle {
    width: 990
    height: 480
    color: "black"

    GraphStack {
        id: graphs
        anchors.fill: parent
        anchors.margins: 20
        anchors.bottomMargin: 100
        antialiasing: aaCb.checked
        joinStyle: joinCb.checked ? Qt.RoundJoin : Qt.BevelJoin
        timeScale: hZoomSlider.value
        color: "lightsteelblue"
        stroke: fillCb.checked
        fillBelow: fillBelowCb.checked
        fillAbove: fillAboveCb.checked
        model: LineGraphModel {
            label: "example data"
        }
        Component.onCompleted: {
            // hand-crafted time series; timestamps are in ms since the epoch,
            // starting 30s ago, fed via appendSampleMs (which converts to the
            // internal seconds axis - passing them to appendSample() would treat
            // ms as seconds and blow past float precision on the x coordinate).
            var time = Date.now() - 1000 * 30
            model.appendSampleMs(0.3, time)
            time += 1000
            model.appendSampleMs(0.5, time)

            // upward spikes
            time += 50
            model.appendSampleMs(0, time)
            time += 100
            model.appendSampleMs(0, time)
            time += 50
            model.appendSampleMs(1, time)
            time += 20
            model.appendSampleMs(1, time)
            time += 50
            model.appendSampleMs(0, time)
            time += 100
            model.appendSampleMs(0, time)

            time += 50
            model.appendSampleMs(0, time)
            time += 100
            model.appendSampleMs(0, time)
            time += 50
            model.appendSampleMs(1, time)
            time += 50
            model.appendSampleMs(1, time)
            time += 50
            model.appendSampleMs(0, time)
            time += 200
            model.appendSampleMs(0, time)

            // downward spikes
            time += 50
            model.appendSampleMs(1, time)
            time += 100
            model.appendSampleMs(1, time)

            time += 50
            model.appendSampleMs(0, time)
            time += 20
            model.appendSampleMs(0, time)
            time += 50
            model.appendSampleMs(1, time)
            time += 100
            model.appendSampleMs(1, time)

            time += 50
            model.appendSampleMs(0, time)
            time += 50
            model.appendSampleMs(0, time)
            time += 50
            model.appendSampleMs(1, time)
            time += 100
            model.appendSampleMs(1, time)

			// "square" waves
            time += 40
            model.appendSampleMs(0, time)
            time += 1000
            model.appendSampleMs(0, time)
            time += 40
            model.appendSampleMs(1, time)
            time += 1000
            model.appendSampleMs(1, time)
            time += 100
            model.appendSampleMs(0, time)
            time += 1000
            model.appendSampleMs(0, time)
            time += 100
            model.appendSampleMs(1, time)
            time += 1000
            model.appendSampleMs(1, time)
            time += 100
            model.appendSampleMs(0, time)
            time += 1000
            model.appendSampleMs(0, time)
            time += 100
            model.appendSampleMs(1, time)
            time += 1000
            model.appendSampleMs(1, time)
            time += 100
            model.appendSampleMs(0, time)
            time += 1000
            model.appendSampleMs(0, time)
            time += 100
            model.appendSampleMs(1, time)
            time += 1000
            model.appendSampleMs(0, time)

			// backtrack
            time -= 500
            model.appendSampleMs(0.5, time)
            time += 700
            model.appendSampleMs(0, time)
            time += 100
            model.appendSampleMs(0.2, time)
            time += 100
            model.appendSampleMs(0, time)
            time += 100
            model.appendSampleMs(1, time)
            time += 1850
            model.appendSampleMs(0, time)
            time += 1000
            model.appendSampleMs(1, time)
            time += 1000
            model.appendSampleMs(0.9, time)
            time += 1000
            model.appendSampleMs(1.01, time)
            time += 1000
            model.appendSampleMs(0.98, time)
            time += 1000
            model.appendSampleMs(0.9, time)
            time += 1000
            model.appendSampleMs(0.2, time)
            time += 1000
            model.appendSampleMs(0.02, time)
            time += 1000
            model.appendSampleMs(0, time)
            time += 1000
            model.appendSampleMs(0.01, time)
            time += 1000
            model.appendSampleMs(0.03, time)
            time += 1000
            model.appendSampleMs(0.02, time)
            time += 1000
            model.appendSampleMs(0.04, time)
            time += 1000
            model.appendSampleMs(0.07, time)
            time += 1000
            model.appendSampleMs(0.05, time)
            time += 1000
            model.appendSampleMs(0.06, time)
            time += 1000
            model.appendSampleMs(0.18, time)
            time += 1000
            model.appendSampleMs(0.09, time)
            time += 1000
            model.appendSampleMs(0.1, time)
            time += 1000
            model.appendSampleMs(0.08, time)
            time += 1000
            model.appendSampleMs(-0.06, time)
            time += 1000
            model.appendSampleMs(0.04, time)
            time += 1000
            model.appendSampleMs(0.02, time)
            time += 1000
            model.appendSampleMs(0, time)
            time += 1000
            model.appendSampleMs(0.1, time)
        }

    }

    Rectangle {
        anchors.fill: graphs
        color: "transparent"
        border.color: "#40444444"
        border.width: 2
        Flow {
            palette.buttonText: "white"
            palette.text: "white"
            palette.windowText: "white"

            anchors.top: parent.bottom
            anchors.margins: 6
            width: parent.width
            spacing: 20
            opacity: 0.5
            Column {
                width: 300
                Slider {
                    id: widthSlider
                    width: 300
                    from: 1
                    to: 25
                    value: 1.3
                }
                Text {
                    text: "line width " + widthSlider.value.toFixed(2)
                    color: "white"
                }
            }
            Column {
                width: 300
                Slider {
                    id: offsetSlider
                    width: 300
                    from: -1
                    to: +1
                }
                Text {
                    text: "y offset " + offsetSlider.value.toFixed(2)
                    color: "white"
                }
            }
            Column {
                width: 300
                Slider {
                    id: hZoomSlider
                    width: 300
                    from: 1
                    to: 50
                }
                Text {
                    text: "time scale " + hZoomSlider.value.toFixed(2)
                    color: "white"
                }
            }

            CheckBox {
                id: fillCb
                text: "fill"
                checked: true
            }
            CheckBox {
                id: aaCb
                text: "antialiasing"
                checked: true
            }
            CheckBox {
                id: joinCb
                text: checked ? "round" : "bevel"
                checked: true
            }
            CheckBox {
                id: originalLineCb
                text: "actual samples"
//                checked: true
            }
            CheckBox {
                id: wireframeCb
                text: "wireframe"
//                checked: true
            }
            CheckBox {
                id: timerRun
                text: "periodic update"
            }
            CheckBox {
                id: fillBelowCb
                text: "fill below"
            }
            CheckBox {
                id: fillAboveCb
                text: "fill above"
            }
        }
    }
}
