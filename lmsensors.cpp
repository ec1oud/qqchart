#include "lmsensors.h"

LmSensors::LmSensors(QObject *parent) : QObject(parent)
{
    m_initialized = init();
    // throw away first sample - tends to be wrong
    for (Sensor *item : m_sensors)
        item->sample();
    m_timerId = startTimer(m_updateIntervalMs);
}

void appendItems(QQmlListProperty<Sensor> *property, Sensor *item)
{
    Q_UNUSED(property);
    Q_UNUSED(item);
    // Do nothing. can't add to a directory using this method
}

int itemSize(QQmlListProperty<Sensor> *property) { return static_cast<QList<Sensor *> *>(property->data)->size(); }

Sensor *itemAt(QQmlListProperty<Sensor> *property, int index)
{
    return static_cast<QList<Sensor *> *>(property->data)->at(index);
}

void clearitemPtr(QQmlListProperty<Sensor> *property) { return static_cast<QList<Sensor *> *>(property->data)->clear(); }

QQmlListProperty<Sensor> LmSensors::sensors()
{
    return QQmlListProperty<Sensor>(this, &m_sensors, &appendItems, &itemSize, &itemAt, &clearitemPtr);
}

bool LmSensors::init()
{
#define BUF_SIZE 200
    static char buf[BUF_SIZE];

    int chip_nr, a;
    const sensors_chip_name *chip;
    const sensors_subfeature *sub;
    const sensors_feature *feature;
    const char *adap = NULL;

    // add CPU load

    Sensor *new_item = new Sensor(Sensor::Cpu);
    new_item->m_index = m_sensors.count();
    new_item->setLabel(tr("CPU Load"));
    new_item->m_adapter = "proc-stat";
    new_item->setMinValue(0);
    new_item->setMaxValue(100);
    new_item->m_unit = "%";
    m_sensors.append(new_item);

    // add memory metrics

    qreal memoryTotal = Sensor(Sensor::MemoryTotal).sample();

    new_item = new Sensor(Sensor::MemoryFree);
    new_item->m_index = m_sensors.count();
    new_item->setLabel(tr("Memory Free"));
    new_item->m_adapter = "proc-stat";
    new_item->setMinValue(0);
    new_item->setMaxValue(memoryTotal);
    new_item->setNormalMinValue(memoryTotal * 0.02);
    new_item->setNormalMaxValue(memoryTotal);
    new_item->m_unit = "KB";
    m_sensors.append(new_item);

    new_item = new Sensor(Sensor::MemoryUsed);
    new_item->m_index = m_sensors.count();
    new_item->setLabel(tr("Memory Used"));
    new_item->m_adapter = "proc-stat";
    new_item->setMinValue(0);
    new_item->setMaxValue(memoryTotal);
    new_item->setNormalMinValue(0);
    new_item->setNormalMaxValue(memoryTotal * 0.98);
    new_item->m_unit = "KB";
    m_sensors.append(new_item);

    new_item = new Sensor(Sensor::MemoryCache);
    new_item->m_index = m_sensors.count();
    new_item->setLabel(tr("Memory Cached"));
    new_item->m_adapter = "proc-stat";
    new_item->setMinValue(0);
    new_item->setMaxValue(memoryTotal);
    new_item->setNormalMinValue(0);
    new_item->setNormalMaxValue(memoryTotal);
    new_item->m_unit = "KB";
    m_sensors.append(new_item);

    qreal swapTotal = Sensor(Sensor::SwapTotal).sample();

    new_item = new Sensor(Sensor::SwapFree);
    new_item->m_index = m_sensors.count();
    new_item->setLabel(tr("Swap Free"));
    new_item->m_adapter = "proc-stat";
    new_item->setMinValue(0);
    new_item->setMaxValue(swapTotal);
    new_item->setNormalMinValue(swapTotal * 0.02);
    new_item->setNormalMaxValue(swapTotal);
    new_item->m_unit = "KB";
    m_sensors.append(new_item);

    new_item = new Sensor(Sensor::SwapUsed);
    new_item->m_index = m_sensors.count();
    new_item->setLabel(tr("Swap Used"));
    new_item->m_adapter = "proc-stat";
    new_item->setMinValue(0);
    new_item->setMaxValue(swapTotal);
    new_item->setNormalMinValue(0);
    new_item->setNormalMaxValue(swapTotal * 0.98);
    new_item->m_unit = "KB";
    m_sensors.append(new_item);

    // add lm-sensors

    if (int err = sensors_init(NULL)) {
        m_errorMessage = sensors_strerror(err);
        emit errorMessageChanged();
        emit sensorsChanged();
        return false;
    } else {
        chip_nr = 0;

        while ((chip = sensors_get_detected_chips(NULL, &chip_nr))) {
            if (sensors_snprintf_chip_name(buf, BUF_SIZE, chip) < 0)
                sprintf(buf, "%i", chip_nr);

            adap = sensors_get_adapter_name(&chip->bus);
            //            if(adap) qDebug() << " " << adap;
            a = 0;
            while ((feature = sensors_get_features(chip, &a))) {
                //                qDebug() << "  " << sensors_get_label(chip, feature) << "type" << feature->type;
                Sensor *new_item = new Sensor();

                sub = sensors_get_subfeature(chip, feature, (sensors_subfeature_type)(((int)feature->type) << 8));

                new_item->m_index = m_sensors.count();
                new_item->setLabel(QLatin1String(sensors_get_label(chip, feature)));
                if (adap)
                    new_item->m_adapter = adap;
                new_item->m_chip = chip;
                new_item->m_chipId = chip_nr;
                new_item->m_chipName = buf;
                new_item->m_feature = feature;
                new_item->m_subfeature = sub;
                new_item->setMinValue(0);
                new_item->setMaxValue(100);

                const sensors_subfeature *limitSub = 0;

                switch (new_item->m_feature->type) {
                case SENSORS_FEATURE_IN:
                    new_item->m_type = Sensor::SensorType::Input;
                    // fallthrough
                case SENSORS_FEATURE_VID:
                    if (new_item->m_type == Sensor::SensorType::Unknown)
                        new_item->m_type = Sensor::SensorType::Vid;
                    new_item->setMinValue(0);
                    new_item->setMaxValue(15);
                    new_item->m_unit = "V";
                    limitSub = sensors_get_subfeature(chip, feature, SENSORS_SUBFEATURE_IN_MIN);
                    sensors_get_value(chip, limitSub->number, &new_item->m_normalMinValue);
                    limitSub = sensors_get_subfeature(chip, feature, SENSORS_SUBFEATURE_IN_MAX);
                    sensors_get_value(chip, limitSub->number, &new_item->m_normalMaxValue);
                    break;
                case SENSORS_FEATURE_FAN:
                    new_item->m_type = Sensor::SensorType::Fan;
                    new_item->setMinValue(0);
                    new_item->setMaxValue(7200);
                    new_item->setNormalMaxValue(5500);
                    new_item->m_unit = "RPM";
                    limitSub = sensors_get_subfeature(chip, feature, SENSORS_SUBFEATURE_FAN_MIN);
                    if (limitSub) sensors_get_value(chip, limitSub->number, &new_item->m_normalMinValue);
                    limitSub = sensors_get_subfeature(chip, feature, SENSORS_SUBFEATURE_FAN_MAX);
                    if (limitSub) sensors_get_value(chip, limitSub->number, &new_item->m_normalMaxValue);
                    break;
                case SENSORS_FEATURE_TEMP:
                    new_item->m_type = Sensor::SensorType::Temperature;
                    new_item->m_unit = "°C";
                    limitSub = sensors_get_subfeature(chip, feature, SENSORS_SUBFEATURE_TEMP_MIN);
                    if (limitSub) sensors_get_value(chip, limitSub->number, &new_item->m_normalMinValue);
                    limitSub = sensors_get_subfeature(chip, feature, SENSORS_SUBFEATURE_TEMP_MAX);
                    if (limitSub) sensors_get_value(chip, limitSub->number, &new_item->m_normalMaxValue);
                    break;
                case SENSORS_FEATURE_POWER:
                    new_item->m_type = Sensor::SensorType::Power;
                    new_item->m_unit = "W";
                    limitSub = sensors_get_subfeature(chip, feature, SENSORS_SUBFEATURE_POWER_MAX);
                    if (limitSub) sensors_get_value(chip, limitSub->number, &new_item->m_normalMaxValue);
                    break;
                case SENSORS_FEATURE_ENERGY:
                    new_item->m_type = Sensor::SensorType::Energy;
                    new_item->m_unit = "J";
                    break;
                case SENSORS_FEATURE_CURR:
                    new_item->m_type = Sensor::SensorType::Current;
                    new_item->m_unit = "A";
                    limitSub = sensors_get_subfeature(chip, feature, SENSORS_SUBFEATURE_CURR_MIN);
                    if (limitSub) sensors_get_value(chip, limitSub->number, &new_item->m_normalMinValue);
                    limitSub = sensors_get_subfeature(chip, feature, SENSORS_SUBFEATURE_CURR_MAX);
                    if (limitSub) sensors_get_value(chip, limitSub->number, &new_item->m_normalMaxValue);
                    break;
                case SENSORS_FEATURE_HUMIDITY:
                    new_item->m_type = Sensor::SensorType::Humidity;
                    new_item->m_unit = "%";
                    break;
                case SENSORS_FEATURE_INTRUSION:
                    new_item->m_type = Sensor::SensorType::Intrusion;
                    new_item->setMinValue(0);
                    new_item->setMaxValue(12);
                    break;
                default:;
                }
//                qDebug() << "range of" << new_item->m_label << new_item->m_normalMinValue << new_item->m_normalMaxValue;

                m_sensors.append(new_item);
            }
        }
    }

    // take inventory of batteries and power adapters
    QDir powerSupplyDir(QLatin1String("/sys/class/power_supply"));
    for (const QString &powerSupply : powerSupplyDir.entryList()) {
        if (powerSupply.startsWith(QLatin1String(".")))
            continue;

        bool isBattery = powerSupply.toLower().startsWith(QLatin1String("bat"));
        QDir dir(powerSupplyDir);
        dir.cd(powerSupply);
        if (isBattery) {
            for (QString metric : dir.entryList(QStringList() << "*_now")) {
                QString fullMetric(metric);
                fullMetric.replace(QLatin1String("_now"), QLatin1String("_full"));
                QFile fullFile(dir.absoluteFilePath(fullMetric));
                if (fullFile.open(QFile::ReadOnly)) {
                    bool ok = false;
                    qreal fullAmount = fullFile.readAll().trimmed().toDouble(&ok) / 1000000;
                    fullFile.close();
                    qDebug() << powerSupply << metric << fullFile.fileName() << fullAmount;
                    if (!ok)
                        continue;

                    // OK we're good: we have a means of reading the "now" value, and have already read the "full" value
                    new_item = new Sensor(Sensor::Energy);
                    new_item->m_file = new QFile(dir.absoluteFilePath(metric));
                    new_item->m_index = m_sensors.count();
                    new_item->setLabel(powerSupply);
                    new_item->m_adapter = "power_supply";
                    new_item->setMinValue(0);
                    new_item->setMaxValue(fullAmount);
                    new_item->setNormalMinValue(fullAmount * 0.02);
                    new_item->setNormalMaxValue(fullAmount);
                    new_item->m_unit = "Ah"; // guessing uAh from the size of the number... so we divided down above
                    m_sensors.append(new_item);
                } else {
//                    qDebug() << fullFile.fileName() << "isn't readable";
                }
            }
        } else if (powerSupply.toLower().startsWith(QLatin1String("ac"))) {
            // AC adapter
            new_item = new Sensor(Sensor::Connected);
            new_item->m_file = new QFile(dir.absoluteFilePath(QLatin1String("online")));
            new_item->m_index = m_sensors.count();
            new_item->setLabel(powerSupply);
            new_item->m_adapter = "power_supply";
            new_item->setMinValue(0);
            new_item->setMaxValue(1);
            new_item->setNormalMinValue(0);
            new_item->setNormalMaxValue(1);
            new_item->m_unit = "connected";
            m_sensors.append(new_item);
        }
    }

    emit sensorsChanged();
    return true;
}

bool LmSensors::sampleAllValues()
{
    qint64 timestamp = QDateTime().currentDateTime().toMSecsSinceEpoch();
    for (Sensor *item : m_sensors)
        item->recordSample(timestamp);
    return true;
}

void LmSensors::setDownsampleInterval(qreal downsampleInterval)
{
    for (Sensor *item : m_sensors)
        item->setDownsampleInterval(downsampleInterval);
}

QList<QObject *> LmSensors::filtered(int t, const QString substring)
{
    QList<QObject *> ret;
    QVector<Sensor::SensorType> types;
    if (t == Sensor::SensorType::Memory)
        types << Sensor::MemoryFree << Sensor::MemoryUsed << Sensor::MemoryCache << Sensor::SwapFree << Sensor::SwapUsed;
    else
        types << static_cast<Sensor::SensorType>(t);
    for (Sensor * item : m_sensors)
        if ((t == Sensor::SensorType::Unknown || types.contains(item->type())) &&
                (substring.isEmpty() || item->label().contains(substring) ||
                 item->chipName().contains(substring) || item->adapter().contains(substring)))
            ret << item;
//qDebug() << "found" << ret.count() << "of type" << type;
    return ret;
}

void LmSensors::setUpdateIntervalMs(int updateIntervalMs)
{
    if (m_updateIntervalMs == updateIntervalMs)
        return;

    m_updateIntervalMs = updateIntervalMs;
    killTimer(m_timerId);
    m_timerId = startTimer(m_updateIntervalMs);
    emit updateIntervalMsChanged();
}

Sensor::Sensor(SensorType type, QObject *parent) : LineGraphModel(parent)
{
    m_type = type;
    m_maxValue = 100; // good for temperatures and percentages at least
    m_normalMaxValue = 100;
}

bool Sensor::recordSample(qint64 timestamp)
{
    qreal val = sample();
    if (val == 65535)
        return false;
    appendSample(val, timestamp);
    return true;
}

void Sensor::getMemoryMetric(const char *metric, qreal &val)
{
    QFile data("/proc/meminfo");
    QLatin1String metricStr(metric);
    if (data.open(QFile::ReadOnly)) {
        QTextStream in(&data);
        QString line;

        do {
            line = in.readLine(1024);
        } while (!line.startsWith(metricStr));

        QString afterColon = line.mid(line.indexOf(':') + 1).trimmed();
        int spaceIdx = afterColon.indexOf(' ');
        QString beforeUnits = afterColon.left(spaceIdx);
        m_unit = afterColon.mid(spaceIdx + 1);
        bool ok = false;
        val = beforeUnits.toDouble(&ok);
        if (!ok)
            val = 0;
        else if (val > 10000 && m_unit.toLower() == QLatin1String("kb")) {
            val /= 1000;
            m_unit = QLatin1String("MB");
        }
    } else {
        val = 0;
    }
}

void Sensor::getCPULoad(qreal &val)
{
    // http://stackoverflow.com/questions/3017162/how-to-get-total-cpu-usage-in-linux-c
    // http://www.linuxhowtos.org/System/procstat.htm

    // cpu  63536 963 11961 946741 2090 0 107 0 0 0

    QFile data("/proc/stat");
    if (data.open(QFile::ReadOnly)) {
        QTextStream in(&data);
        QStringList list;
        qint64 total_jiffies = 0, work_jiffies = 0;

        do {
            list = in.readLine(1024).split(" ", QString::SkipEmptyParts);
        } while (list.at(0) != "cpu");

        for (int x = 1; x < list.length(); x++)
            total_jiffies += list.at(x).toInt();
        for (int x = 1; x < 4; x++)
            work_jiffies += list.at(x).toInt();

        if (m_totalJiffies)
            val = qreal(work_jiffies - m_workJiffies) / qreal(total_jiffies - m_totalJiffies) * 100.;
        else
            val = 0;

        m_workJiffies = work_jiffies;
        m_totalJiffies = total_jiffies;
    } else {
        val = 0;
    }
}

qreal Sensor::sample()
{
    qreal valTotal = 0;
    qreal val = 0;

    switch (m_type) {
    case Sensor::MemoryTotal:
        getMemoryMetric("MemTotal", val);
        break;
    case Sensor::MemoryFree:
        getMemoryMetric("MemFree", val);
        break;
    case Sensor::MemoryUsed:
        getMemoryMetric("MemTotal", valTotal);
        getMemoryMetric("MemFree", val);
        val = valTotal - val;
        break;
    case Sensor::MemoryCache:
        getMemoryMetric("Cached", val);
        break;
    case Sensor::SwapTotal:
        getMemoryMetric("SwapTotal", val);
        break;
    case Sensor::SwapFree:
        getMemoryMetric("SwapFree", val);
        break;
    case Sensor::SwapUsed:
        getMemoryMetric("SwapTotal", valTotal);
        getMemoryMetric("SwapFree", val);
        val = valTotal - val;
        break;
    case Sensor::SensorType::Cpu:
        getCPULoad(val);
        break;
    case Sensor::Connected:
    case Sensor::Energy:
        if (m_file && m_file->open(QFile::ReadOnly)) {
            val = m_file->readAll().trimmed().toDouble();
            if (m_type == Sensor::Energy)
                val /= 1000000;
            m_file->close();
        }
        break;
    default: // LM sensors
        sensors_get_value(m_chip, m_subfeature->number, &val);
        break;
    }
    return val;
}
