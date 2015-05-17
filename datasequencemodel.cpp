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
    QAbstractListModel(parent), m_minTime(0.), m_maxTime(0.)
{
}

QVariant DataSequenceModel::data ( const QModelIndex & index, int /*role*/ ) const
{
//    if (index.column() > 0) {
        QVector<float> row = m_data[index.row()];
        qDebug() << "DataSequenceModel::data" << index.row() << ":" << row;
        return row[index.column()];
//    }
}

float *DataSequenceModel::columnValues(int col)
{
    return m_data[col].data();
}

void DataSequenceModel::setCSVSource(QUrl sourcePath)
{
    if (sourcePath.isLocalFile())
        setCSVFile(sourcePath.toLocalFile());
    else
        qDebug() << "opening non-local files is unimplemented:" << sourcePath;
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
    QList<QByteArray> labels = firstLine.split(',');
    m_data.resize(labels.length() - 1); // assume one column is time, which is stored separately
    while (!io.atEnd()) {
        QByteArray line = io.readLine(firstLine.length() * 4);
        QList<QByteArray> cols = line.trimmed().split(',');
//qDebug() << cols;
        QVector<float> colNumbers;
        bool allOK = true;
        QDateTime dt;
        foreach(QByteArray val, cols) {
            val = val.trimmed();
            bool ok = false;
            double dv = val.toDouble(&ok);
            if (ok)
                colNumbers.append(dv);
            else {
                dt = QDateTime::fromString(val, "yyyy-MM-dd");
                ok = dt.isValid();
            }
            if (!ok)
                allOK = false;
        }
        if (!allOK) {
            qDebug() << "ignoring line" << line;
            continue;
        }
        m_dataTimestamps.append(dt);
        float time = dt.toMSecsSinceEpoch() / 1000.0;
        m_times.append(time);
        int vi = 0;
        foreach (float v, colNumbers)
            m_data[vi++].append(v);
        if (time < m_minTime || m_minTime == 0.)
            m_minTime = time;
        if (time > m_maxTime)
            m_maxTime = time;
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
    // Reverse the data, assuming the newest values came first.
    // It doesn't matter for rendering, but Chart2D uses std::lower_bound,
    // which only works if time is in ascending order.
    for (int i = 0; i < m_data.size(); ++i)
        std::reverse(m_data[i].begin(), m_data[i].end());
    std::reverse(m_times.begin(), m_times.end());

    qDebug() << "cols" << m_data.count() << "rows" << m_data[0].count()
             << "mins" << m_minValues << "maxes" << m_maxValues;
}
