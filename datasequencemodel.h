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
    
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const { return m_data.count(); }
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    float *columnValues(int col);
    float *columnTimeValues(int col);

signals:

    
public slots:
    void setCSVSource(QUrl sourcePath); // TODO column spec - which columns are data and where is the timestamp, if any; or else periodicity
    void setCSVFile(QString filePath);

protected:
    void readCSV(QIODevice &io);

protected:
    QVector<QVector<float> > m_data;
    QVector<QDateTime> m_dataTimestamps;
    float *m_columnValuesCache;
};

#endif // DATASEQUENCEMODEL_H
