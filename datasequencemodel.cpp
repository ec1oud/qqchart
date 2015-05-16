#include "datasequencemodel.h"
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <math.h>

/*!
 * \brief A data model providing one or more sequences of values (often time-based)
 * \param parent
 */
DataSequenceModel::DataSequenceModel(QObject *parent) :
    QAbstractListModel(parent), m_columnValuesCache(0)
{
}

QVariant DataSequenceModel::data ( const QModelIndex & index, int role ) const
{
//    if (index.column() > 0) {
        QVector<float> row = m_data[index.row()];
        qDebug() << "DataSequenceModel::data" << index.row() << ":" << row;
        return row[index.column()];
//    }
}

float *DataSequenceModel::columnValues(int col)
{
    int rowCount = m_data.count();
    if (!m_columnValuesCache)
        m_columnValuesCache = (float *)malloc(rowCount * 2 * sizeof(float));
    // TODO avoid this if the same col is requested twice
    // or maybe just store data in cols in the first place
    for (int i = 0; i < rowCount; ++i) {
        int j = i * 2;
        m_columnValuesCache[j] = i;
        m_columnValuesCache[j + 1] = m_data[i][col];
    }
    return m_columnValuesCache;
}

void DataSequenceModel::setCSVSource(QUrl sourcePath)
{
}

void DataSequenceModel::setCSVFile(QString filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
        readCSV(file);
}

void DataSequenceModel::readCSV(QIODevice &io)
{
    m_data.clear();
    m_dataTimestamps.clear();

    QByteArray firstLine = io.readLine();
//    QList<QByteArray> labels = firstLine.split(',');
    while (!io.atEnd()) {
        // TODO less stupid, avoid overflow, always read a whole line
        QByteArray line = io.readLine(firstLine.length() * 4);
        QList<QByteArray> cols = line.trimmed().split(',');
//qDebug() << cols;
        QVector<float> colNumbers;
        bool allOK = true;
        foreach(QByteArray val, cols) {
            val = val.trimmed();
            bool ok = false;
            double dv = val.toDouble(&ok);
            if (ok)
                colNumbers.append(dv);
            else {
                QDateTime dt = QDateTime::fromString(val, "yyyy-MM-dd");
                ok = dt.isValid();
                if (ok)
                    m_dataTimestamps.append(dt);
            }
            if (!ok)
                allOK = false;
        }
        if (!allOK) {
            qDebug() << "ignoring line" << line;
            continue;
        }
//        if (!m_dataTimestamps.isEmpty())
//            qDebug() << m_dataTimestamps.last() << colNumbers;
//        else
//            qDebug() << colNumbers;
        m_data.append(colNumbers);
        if (m_maxValues.count() == 0) {
            m_maxValues = QVector<float>(colNumbers.count(), -INFINITY);
            m_minValues = QVector<float>(colNumbers.count(), +INFINITY);
        }
        for (int i = 0; i < colNumbers.count(); ++i) {
            if (colNumbers[i] > m_maxValues[i])
                m_maxValues[i] = colNumbers[i];
            if (colNumbers[i] < m_minValues[i])
                m_minValues[i] = colNumbers[i];
        }
    }
    qDebug() << "rows" << m_data.count() << "mins" << m_minValues << "maxes" << m_maxValues;
}
