import QtQuick 2.5

MouseArea {
    id: hoverCursor
    anchors.fill: parent
    hoverEnabled: true
    property real hoverX: containsMouse ? rect.x : -1
    Rectangle {
        id: rect
        width: 1
        height: parent.height
        color: "red"
        x: visible ? Math.min(parent.mouseX, parent.width - 10) : -1
        visible: parent.containsMouse
    }
}
