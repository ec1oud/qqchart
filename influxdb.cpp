#include "influxdb.h"

void appendItems(QQmlListProperty<InfluxValueSeries> *property, InfluxValueSeries *item)
{
    Q_UNUSED(property);
    Q_UNUSED(item);
    // Do nothing; can't actually append
}

int itemSize(QQmlListProperty<InfluxValueSeries> *property) { return static_cast<QList<InfluxValueSeries *> *>(property->data)->size(); }

InfluxValueSeries *itemAt(QQmlListProperty<InfluxValueSeries> *property, int index)
{
    return static_cast<QList<InfluxValueSeries *> *>(property->data)->at(index);
}

void clearitemPtr(QQmlListProperty<InfluxValueSeries> *property) { return static_cast<QList<InfluxValueSeries *> *>(property->data)->clear(); }

QQmlListProperty<InfluxValueSeries> InfluxQuery::values()
{
    return QQmlListProperty<InfluxValueSeries>(this, &m_values, &appendItems, &itemSize, &itemAt, &clearitemPtr);
}

void InfluxQuery::setServer(QUrl server)
{
    server.setPath(QLatin1String("/query"));
    if (m_server == server)
        return;
    m_server = server;
    emit serverChanged();
}

void InfluxQuery::setDatabase(QString db)
{
    if (m_database == db)
        return;

    m_initialized = false;
    m_database = db;
    emit databaseChanged();
}

void InfluxQuery::setMeasurement(QString measurement)
{
    if (m_measurement == measurement)
        return;

    m_initialized = false;
    m_measurement = measurement;
    emit measurementChanged();
}

void InfluxQuery::setFields(QStringList fields)
{
    if (m_fields == fields)
        return;

    m_initialized = false;
    m_fields = fields;
    emit fieldsChanged();
}

void InfluxQuery::setWherePairs(QJsonArray wherePairs)
{
//qDebug() << wherePairs;
    m_initialized = false;
    m_wherePairs = wherePairs;
    emit wherePairsChanged();
}

void InfluxQuery::setTimeConstraint(QString timeConstraint)
{
    if (m_timeConstraint == timeConstraint)
        return;

    m_initialized = false;
    m_timeConstraint = timeConstraint;
    emit timeConstraintChanged();
}

void InfluxQuery::setUpdateIntervalMs(int updateIntervalMs)
{
    if (m_updateIntervalMs == updateIntervalMs)
        return;

    m_updateIntervalMs = updateIntervalMs;
    if (m_timerId > 0)
        killTimer(m_timerId);
    m_timerId = startTimer(m_updateIntervalMs);
    emit updateIntervalMsChanged();
}

void InfluxQuery::init()
{
    // SELECT temperature,pressure FROM \"uradmonitor\" WHERE "stationId" = '41000008' AND time > now() - 10h"
    m_queryString = QLatin1String("SELECT ") + m_fields.join(QLatin1Char(',')) +
        QLatin1String(" FROM ") + m_measurement;
    QStringList whereAnds;
    for (const QJsonValue &wpair : m_wherePairs) {
        const QJsonObject o = wpair.toObject();
        for (const QString &k : o.keys())
            whereAnds << QString(QLatin1String("\"%1\" = '%2'")).arg(k).arg(o.value(k).toString());
    }
    if (!whereAnds.isEmpty())
        m_queryString += QLatin1String(" WHERE ") + whereAnds.join(QLatin1String(" AND "));
    if (!m_timeConstraint.isEmpty())
        m_queryString += QLatin1String(" AND time ") + m_timeConstraint;
qDebug() << m_queryString;

    QUrlQuery quq;
    quq.addQueryItem(QLatin1String("db"), m_database);
    quq.addQueryItem(QLatin1String("q"), m_queryString);
    m_queryUrl = m_server;
    m_queryUrl.setQuery(quq);
qDebug() << m_queryUrl;
    m_values.clear();
    for (const QString & field : m_fields) {
        InfluxValueSeries *v = new InfluxValueSeries(field);
        v->setMinValue(0);
        v->setMaxValue(100);
//        v->m_unit = "%";
        m_values.append(v);
    }
    emit valuesChanged();
    m_initialized = true;
    emit initializedChanged();
}

bool InfluxQuery::sampleAllValues()
{
    if (!m_initialized)
        init();
    // TODO
    return true;
}

InfluxValueSeries::InfluxValueSeries(QString fieldName, QObject *parent)
  : LineGraphModel(parent)
{
    setLabel(fieldName);
}
