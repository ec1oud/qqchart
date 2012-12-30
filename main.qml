import QtQuick 2.0
import QQChart 1.0

Item {

    width: 640
    height: 320

    DataSequenceModel {
        id: tableModel
        csvSourceFile: "table.csv"
    }

    Chart2D {
        model: tableModel
    }
}
