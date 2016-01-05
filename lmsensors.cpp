#include "lmsensors.h"

LmSensors::LmSensors(QObject *parent) : QObject(parent)
{
    m_initialized = init();
    // throw away first sample - tends to be wrong
    foreach (Sensor *item, m_sensors)
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

    Sensor *new_item = new Sensor();
    new_item->m_type = Sensor::SensorType::Cpu;
    new_item->m_index = m_sensors.count();
    new_item->setLabel(tr("CPU Load"));
    new_item->m_adapter = "proc-stat";
    new_item->setMinValue(0);
    new_item->setMaxValue(100);
    new_item->m_unit = "%";
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

    emit sensorsChanged();
    return true;
}

bool LmSensors::sampleAllValues()
{
    qint64 timestamp = QDateTime().currentDateTime().toMSecsSinceEpoch();
    foreach (Sensor *item, m_sensors)
        item->recordSample(timestamp);
    return true;
}

QList<QObject *> LmSensors::filtered(int t, const QString substring)
{
    Sensor::SensorType type = static_cast<Sensor::SensorType>(t);
    QList<QObject *> ret;
    foreach (Sensor * item, m_sensors)
        if ((t == Sensor::SensorType::Unknown || item->type() == type) &&
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

Sensor::Sensor(QObject *parent) : LineGraphModel(parent)
{
    m_maxValue = 100; // good for temperatures and percentages at least
    m_normalMaxValue = 100;
}

bool Sensor::recordSample(qint64 timestamp)
{
    qreal val;

    switch (m_type) {
    case Sensor::SensorType::Cpu:
        getCPULoad(val);
        break;
    default: // LM sensors
        sensors_get_value(m_chip, m_subfeature->number, &val);
        break;
    }
    if (val == 65535)
        return false;

    appendSample(val, timestamp);

    return true;
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
    qreal val;

    switch (m_type) {
    case Sensor::SensorType::Cpu:
        getCPULoad(val);
        break;
    default:
        sensors_get_value(m_chip, m_subfeature->number, &val);
        break;
    }
    return (qreal)val;
}
