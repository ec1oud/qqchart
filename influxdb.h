#ifndef INFLUXDB_H
#define INFLUXDB_H

#include <QObject>

#include <QTime>
#include <QDebug>
#include <QQmlListProperty>
#include <QPointF>
#include <QRectF>
#include <QFile>
#include <QStringList>
#include <QtQml>
#include "linegraphmodel.h"

class InfluxValueSeries : public LineGraphModel
{
    Q_OBJECT
    Q_PROPERTY(qreal additiveCorrection READ additiveCorrection WRITE setAdditiveCorrection NOTIFY additiveCorrectionChanged)
    Q_PROPERTY(qreal multiplicativeCorrection READ multiplicativeCorrection WRITE setMultiplicativeCorrection NOTIFY multiplicativeCorrectionChanged)

public:
    explicit InfluxValueSeries(QString fieldName, QObject *parent = 0);

    qreal additiveCorrection() const { return m_additiveCorrection; }
    void setAdditiveCorrection(qreal additiveCorrection);

    qreal multiplicativeCorrection() const { return m_multiplicativeCorrection; }
    void setMultiplicativeCorrection(qreal multiplicativeCorrection);

signals:
    void additiveCorrectionChanged();
    void multiplicativeCorrectionChanged();

protected:
    virtual void finagle(qreal &time, qreal &value);

private:
    qreal m_additiveCorrection = 0;
    qreal m_multiplicativeCorrection = 1;
};


// curl -GET 'http://localhost:8086/query' --data-urlencode "db=weather" --data-urlencode "q=SELECT temperature,pressure FROM \"uradmonitor\" WHERE "stationId" = '41000008' AND time > now() - 10h"

class InfluxQuery : public QObject
{
    Q_OBJECT
    // input
    Q_PROPERTY(QUrl server READ server WRITE setServer NOTIFY serverChanged)
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool ignoreSslErrors READ ignoreSslErrors WRITE setIgnoreSslErrors NOTIFY ignoreSslErrorsChanged)
    Q_PROPERTY(QString database READ database WRITE setDatabase NOTIFY databaseChanged)
    Q_PROPERTY(QString measurement READ measurement WRITE setMeasurement NOTIFY measurementChanged)
    Q_PROPERTY(QStringList fields READ fields WRITE setFields NOTIFY fieldsChanged)
    Q_PROPERTY(QJsonArray wherePairs READ wherePairs WRITE setWherePairs NOTIFY wherePairsChanged)
    Q_PROPERTY(QString timeConstraint READ timeConstraint WRITE setTimeConstraint NOTIFY timeConstraintChanged) // e.g. "> now() - 12h"
    Q_PROPERTY(int updateIntervalMs READ updateIntervalMs WRITE setUpdateIntervalMs NOTIFY updateIntervalMsChanged)
    Q_PROPERTY(int sampleInterval READ sampleInterval WRITE setSampleInterval NOTIFY sampleIntervalChanged) // seconds between graphed samples

    // output
    Q_PROPERTY(bool initialized READ initialized NOTIFY initializedChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(QQmlListProperty<InfluxValueSeries> values READ values NOTIFY valuesChanged)
    Q_PROPERTY(QDateTime lastUpdate READ lastUpdate NOTIFY valuesUpdated) // time when the query ran
    Q_PROPERTY(QDateTime lastSampleTime READ lastSampleTime NOTIFY valuesUpdated) // timestamp of the last datapoint in values

public:
    explicit InfluxQuery(QObject *parent = nullptr);

    Q_INVOKABLE bool sampleAllValues();

    bool initialized() { return m_initialized; }
    QString errorMessage() { return m_errorMessage; }

    int updateIntervalMs() const { return m_updateIntervalMs; }
    void setUpdateIntervalMs(int updateIntervalMs);

    QQmlListProperty<InfluxValueSeries> values();

    QDateTime lastUpdate() { return m_lastUpdate; }
    QDateTime lastSampleTime() const { return m_lastSampleTime; }

    QUrl server() const { return m_server; }
    void setServer(QUrl server);

    QString user() const { return m_user;}
    void setUser(QString user);

    QString password() const { return m_password; }
    void setPassword(QString password);

    bool ignoreSslErrors() const { return m_ignoreSslErrors; }
    void setIgnoreSslErrors(bool ignoreSslErrors);

    QString database() const { return m_database; }
    void setDatabase(QString database);

    QString measurement() const { return m_measurement; }
    void setMeasurement(QString measurement);

    QStringList fields() const { return m_fields; }
    void setFields(QStringList fields);

    QJsonArray wherePairs() const { return m_wherePairs; }
    void setWherePairs(QJsonArray wherePairs);

    QString timeConstraint() const { return m_timeConstraint; }
    void setTimeConstraint(QString timeConstraint);

    int sampleInterval() const { return m_sampleInterval; }
    void setSampleInterval(int sampleInterval);

signals:
    void valuesChanged(); // that means values property definition, not actual data!
    void valuesUpdated(); // actual data points
    void initializedChanged();
    void errorMessageChanged();
    void updateIntervalMsChanged();
    void serverChanged();
    void userChanged();
    void passwordChanged();
    void ignoreSslErrorsChanged();
    void databaseChanged();
    void measurementChanged();
    void fieldsChanged();
    void wherePairsChanged();    
    void timeConstraintChanged();
    void sampleIntervalChanged();

protected:
    void timerEvent(QTimerEvent *) Q_DECL_OVERRIDE { sampleAllValues(); }

protected slots:
    void onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void networkError(QNetworkReply::NetworkError e);
    void networkFinished();

private:
    void init();

private:
    QList<InfluxValueSeries *> m_values;
    QString m_errorMessage;
    QUrl m_server;
    QString m_user;
    QString m_password;
    QString m_database;
    QString m_measurement;
    QStringList m_fields;
    QJsonArray m_wherePairs;
    QString m_timeConstraint;
    QString m_queryString;
    bool m_initialized = false;
    bool m_ignoreSslErrors = false;
    int m_updateIntervalMs = -1;
    int m_timerId = 0;
    QUrl m_queryUrl;
    QDateTime m_lastUpdate;
    QDateTime m_lastSampleTime;

    QNetworkAccessManager m_nam;
    QNetworkRequest m_influxReq;
    QNetworkReply *m_netReply = nullptr;
    int m_sampleInterval = 0;
};

#endif // INFLUXDB_H
