#include "influxdb.h"

InfluxValueSeries::InfluxValueSeries(QString fieldName, QObject *parent)
  : LineGraphModel(parent)
{
    setLabel(fieldName);
    setTimeSpan(INT_MAX);
    setDownsampleMethod(NoDownsample); // influx will do that for us
}

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

void InfluxQuery::setSampleInterval(int sampleInterval)
{
    if (m_sampleInterval == sampleInterval)
        return;

    m_sampleInterval = sampleInterval;
    emit sampleIntervalChanged();
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
    // or
    // SELECT mean(temperature),mean(pressure) FROM uradmonitor WHERE "stationId" = '41000008' AND time > now() - 240h GROUP BY time(10m)
    m_queryString = QLatin1String("SELECT ");
    int i = 0;
    for (auto f : m_fields) {
        if (i++)
            m_queryString += QLatin1Char(',');
        if (m_sampleInterval)
            m_queryString += QString(QLatin1String("mean(%1)")).arg(f);
        else
            m_queryString += f;
    }
    m_queryString += QLatin1String(" FROM ") + m_measurement;
    QStringList whereAnds;
    for (const QJsonValue &wpair : m_wherePairs) {
        const QJsonObject o = wpair.toObject();
        for (const QString &k : o.keys())
            whereAnds << QString(QLatin1String("\"%1\" = '%2'")).arg(k).arg(o.value(k).toString());
    }
    if (!whereAnds.isEmpty())
        m_queryString += QLatin1String(" WHERE ") + whereAnds.join(QLatin1String(" AND "));

    // TODO only get the full range initially or when timeConstraint changes;
    // otherwise just get incremental updates
    if (!m_timeConstraint.isEmpty())
        m_queryString += QLatin1String(" AND time ") + m_timeConstraint;

    if (m_sampleInterval)
        m_queryString += QString(QLatin1String(" GROUP BY time(%1s)")).arg(m_sampleInterval);

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
        // TODO get the min/max from influx
        if (field == QLatin1String("temperature")) {
            v->setMinValue(-20);
            v->setNormalMinValue(0);
            v->setMaxValue(40);
            v->setNormalMaxValue(40);
        } else if (field == QLatin1String("pressure")) {
            v->setMinValue(95000);
            v->setNormalMinValue(98000);
            v->setMaxValue(105000);
            v->setNormalMaxValue(103000);
        }
//        v->m_unit = "%";
        m_values.append(v);
    }
    emit valuesChanged();

    m_influxReq = QNetworkRequest(m_queryUrl);

    m_initialized = true;
    emit initializedChanged();
}

bool InfluxQuery::sampleAllValues()
{
    if (!m_initialized)
        init();

    m_netReply = m_nam.get(m_influxReq);
    connect(m_netReply, &QNetworkReply::finished, this, &InfluxQuery::networkFinished);
    connect(m_netReply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(networkError(QNetworkReply::NetworkError)));

    return true;
}

void InfluxQuery::networkError(QNetworkReply::NetworkError e)
{
    qDebug() << e;
    m_netReply->disconnect();
    m_netReply->deleteLater();
    m_netReply = nullptr;
}

void InfluxQuery::networkFinished()
{
    QJsonParseError err;
    QJsonDocument jdoc = QJsonDocument::fromJson(m_netReply->readAll(), &err);
    if (err.error == QJsonParseError::NoError) {
//        qDebug() << jdoc.object().value(QLatin1String("results")).toArray();
        QJsonObject o = jdoc.object().value(QLatin1String("results")).toArray().first().toObject();
        QJsonArray arr = o.value("series").toArray().first().toObject().value("values").toArray();
        QDateTime first;
        QDateTime last;
        int count = 0;
        for (auto o : arr) {
            QJsonArray samples = o.toArray();
            last = QDateTime::fromString(samples.takeAt(0).toString(), Qt::ISODateWithMs);
            if (first.isNull())
                first = last;
            ++count;
//            qDebug() << last.toString() << samples;
            for (int i = 0; i < m_values.count(); ++i) {
                qreal val = samples.takeAt(0).toDouble(qQNaN());
                m_values[i]->appendSample(val, last);
            }
        }
        int timeSpan = int(qAbs(first.secsTo(last)));
        qDebug() << "for" << m_fields << "got" << count << "samples from" << first << "to" << last << "timespan" << timeSpan;
        for (int i = 0; i < m_values.count(); ++i) {
            m_values[i]->setTimeSpan(timeSpan);
            qDebug() << "value range of" << m_fields.at(i) << m_values.at(i)->minSampleValue() << m_values.at(i)->maxSampleValue();
        }
    } else {
        qWarning() << err.errorString();
    }
    m_netReply->disconnect();
    m_netReply->deleteLater();
    m_netReply = nullptr;
}
