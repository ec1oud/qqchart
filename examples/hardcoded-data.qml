import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import org.ecloud.charts 1.0

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
        timeScale: hZoomSlider.value
        color: "lightsteelblue"
        model: LineGraphModel {
            label: "example data"
        }
        Component.onCompleted: {
            var time = new Date()
            time = new Date(time.getTime() - 1000 * 30)
            model.appendSample(0.3, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.5, time)

            // upward spikes
            time = new Date(time.getTime() + 50)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 50)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 20)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 50)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(0, time)

            time = new Date(time.getTime() + 50)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 50)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 50)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 50)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 200)
            model.appendSample(0, time)

            // downward spikes
            time = new Date(time.getTime() + 50)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(1, time)

            time = new Date(time.getTime() + 50)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 20)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 50)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(1, time)

            time = new Date(time.getTime() + 50)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 50)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 50)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(1, time)

			// "square" waves
            time = new Date(time.getTime() + 40)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 40)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0, time)

			// backtrack
            time = new Date(time.getTime() - 500)
            model.appendSample(0.5, time)
            time = new Date(time.getTime() + 700)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(0.2, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 100)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 1850)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(1, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.9, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(1.01, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.98, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.9, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.2, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.02, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.01, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.03, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.02, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.04, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.07, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.05, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.06, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.18, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.09, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.1, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.08, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(-0.06, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.04, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.02, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0, time)
            time = new Date(time.getTime() + 1000)
            model.appendSample(0.1, time)
        }

    }

    Rectangle {
        anchors.fill: graphs
        color: "transparent"
        border.color: "#40444444"
        border.width: 2
        Flow {
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
                    minimumValue: 1
                    maximumValue: 25
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
                    minimumValue: -1
                    maximumValue: +1
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
                    minimumValue: 1
                    maximumValue: 50
                }
                Text {
                    text: "time scale " + hZoomSlider.value.toFixed(2)
                    color: "white"
                }
            }
            Component {
                id: whiteCheckboxStyle
                CheckBoxStyle {
                    label: Text {
                        text: control.text
                        color: "white"
                    }
                }
            }

            CheckBox {
                id: fillCb
                text: "fill"
                style: whiteCheckboxStyle
                checked: true
            }
            CheckBox {
                id: aaCb
                text: "antialiasing"
                style: whiteCheckboxStyle
                checked: true
            }
            CheckBox {
                id: originalLineCb
                text: "actual samples"
                style: whiteCheckboxStyle
//                checked: true
            }
            CheckBox {
                id: wireframeCb
                text: "wireframe"
                style: whiteCheckboxStyle
//                checked: true
            }
            CheckBox {
                id: timerRun
                text: "periodic update"
                style: whiteCheckboxStyle
            }
            CheckBox {
                id: fillBelowCb
                text: "fill below"
                style: whiteCheckboxStyle
            }
            CheckBox {
                id: fillAboveCb
                text: "fill above"
                style: whiteCheckboxStyle
            }
        }
    }
}
