#ifndef DATASEQUENCEMODEL_H
#define DATASEQUENCEMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QIODevice>
#include <QUrl>
#include <QVector>

class DataSequenceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString csvSourceFile WRITE setCSVFile)

public:
    explicit DataSequenceModel(QObject *parent = 0);
    static DataSequenceModel *fromCSV(const char* filepath);
    
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const { return m_times.count(); }
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    float *times() { return m_times.data(); }
    float minTime() { return m_minTime; }
    float maxTime() { return m_maxTime; }
    float *columnValues(int col);
//    float *columnTimeValues(int col);
    float columnMinValue(int col) { return m_minValues[col]; }
    float columnMaxValue(int col) { return m_maxValues[col]; }

signals:

    
public slots:
    void setCSVSource(QUrl sourcePath); // TODO column spec - which columns are data and where is the timestamp, if any; or else periodicity
    void setCSVFile(QString filePath);

protected:
    void readCSV(QIODevice &io);

protected:
    QVector<QVector<float> > m_data; // columns
    QVector<float> m_times;
    QVector<QDateTime> m_dataTimestamps;
    QVector<float> m_maxValues;
    QVector<float> m_minValues;
    float m_minTime;
    float m_maxTime;
};

#endif // DATASEQUENCEMODEL_H
