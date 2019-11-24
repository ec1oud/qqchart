import QtQuick 2.5

MouseArea {
    id: hoverCursor
    anchors.fill: parent
    hoverEnabled: true
    property real hoverX: containsMouse ? rect.x : -1
    property real timeSpan: -1
    property date lastSampleTime: new Date()
    Rectangle {
        id: rect
        width: 1
        height: parent.height
        color: "red"
        x: visible ? Math.min(parent.mouseX, parent.width - 10) : -1
        visible: parent.containsMouse
        Rectangle {
            color: "#AAFFDDDD"
            border.color: "red"
            anchors.fill: text
            anchors.margins: -2
            anchors.leftMargin: 0
            anchors.topMargin: 2
            rotation: -90
            transformOrigin: Item.TopLeft
        }
        Text {
            id: text
            color: "#882222"
            visible: hoverCursor.timeSpan > 0
            property date cursorTime: new Date(hoverCursor.lastSampleTime.getTime() + (hoverCursor.timeSpan * (hoverCursor.mouseX - hoverCursor.width) / hoverCursor.width * 1000))
            text: cursorTime.toISOString().slice(0, 10) + "  " + cursorTime.toISOString().slice(11, 16)
            rotation: -90
            transformOrigin: Item.TopLeft
            y: parent.height / 2 + width / 2
        }
    }
}
