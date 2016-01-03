#include "lmsensors.h"

LmSensors::LmSensors(QObject *parent) : QObject(parent)
{
    m_initialized = init();
}

void appendItems(QQmlListProperty<SensorItem> *property, SensorItem *item)
{
    Q_UNUSED(property);
    Q_UNUSED(item);
    // Do nothing. can't add to a directory using this method
}

int itemSize(QQmlListProperty<SensorItem> *property) { return static_cast<QList<SensorItem *> *>(property->data)->size(); }

SensorItem *itemAt(QQmlListProperty<SensorItem> *property, int index)
{
    return static_cast<QList<SensorItem *> *>(property->data)->at(index);
}

void clearitemPtr(QQmlListProperty<SensorItem> *property) { return static_cast<QList<SensorItem *> *>(property->data)->clear(); }

QQmlListProperty<SensorItem> LmSensors::items()
{
    return QQmlListProperty<SensorItem>(this, &m_sensorItems, &appendItems, &itemSize, &itemAt, &clearitemPtr);
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

    SensorItem *new_item = new SensorItem();
    new_item->m_type = new_item->CPU;
    new_item->m_index = m_sensorItems.count();
    new_item->m_label = "CPU Load";
    new_item->m_adapter = "proc-stat";
    new_item->m_valueMin = 0;
    new_item->m_valueMax = 100;
    new_item->m_unit = "%";
    m_sensorItems.append(new_item);

    // add lm-sensors

    if (int err = sensors_init(NULL)) {
        m_errorMessage = sensors_strerror(err);
        emit errorMessageChanged();
        emit itemsChanged();
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
                SensorItem *new_item = new SensorItem();

                sub = sensors_get_subfeature(chip, feature, (sensors_subfeature_type)(((int)feature->type) << 8));

                new_item->m_type = new_item->LM;
                new_item->m_index = m_sensorItems.count();
                new_item->m_label = sensors_get_label(chip, feature);
                if (adap)
                    new_item->m_adapter = adap;
                new_item->m_chip = chip;
                new_item->m_chipId = chip_nr;
                new_item->m_chipName = buf;
                new_item->m_feature = feature;
                new_item->m_subfeature = sub;
                new_item->m_valueMin = 0;
                new_item->m_valueMax = 100;

                switch (new_item->m_feature->type) {
                case SENSORS_FEATURE_IN:
                case SENSORS_FEATURE_VID:
                    new_item->m_valueMin = 0;
                    new_item->m_valueMax = 15;
                    new_item->m_unit = "V";
                    break;
                case SENSORS_FEATURE_FAN:
                    new_item->m_valueMin = 0;
                    new_item->m_valueMax = 3000;
                    new_item->m_unit = "RPM";
                    break;
                case SENSORS_FEATURE_TEMP:
                    new_item->m_unit = "°C";
                    break;
                case SENSORS_FEATURE_POWER:
                    new_item->m_unit = "W";
                    break;
                case SENSORS_FEATURE_ENERGY:
                    new_item->m_unit = "J";
                    break;
                case SENSORS_FEATURE_CURR:
                    new_item->m_unit = "A";
                    break;
                case SENSORS_FEATURE_HUMIDITY:
                    new_item->m_unit = "%";
                    break;
                case SENSORS_FEATURE_INTRUSION:
                    new_item->m_valueMin = 0;
                    new_item->m_valueMax = 12;
                    break;
                default:;
                }

                m_sensorItems.append(new_item);
            }
        }
    }

    emit itemsChanged();
    return true;
}

bool LmSensors::sampleAllValues()
{
    qint64 timestamp = QDateTime().currentDateTime().toMSecsSinceEpoch();
    foreach (SensorItem *item, m_sensorItems) {
        item->recordSample(timestamp);
    }
    return true;
}

SensorItem::SensorItem(QObject *parent) : QObject(parent) {}

bool SensorItem::recordSample(const qint64 &timestamp)
{
    qreal val;

    switch (m_type) {
    case LM:
        sensors_get_value(m_chip, m_subfeature->number, &val);
        break;
    case CPU:
        getCPULoad(val);
        break;
    default:
        val = 0;
    }

    if (val < m_minValue) {
        m_minValue = val;
        emit minValueChanged();
    }
    if (val > m_maxValue) {
        m_maxValue = val;
        emit maxValueChanged();
    }

    // qDebug() << "new sample @" << timestamp << timestamp / 1000.0 << "have" << m_vertices.size() << "max" << max_samples;
    // make time values smaller; TODO why does this even matter?
    static qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    LineNode::appendVertices(m_vertices, (timestamp - startTime) / 1000.0, qreal(val));
    emit currentSampleChanged(&m_vertices);

    if (m_vertices.size() > m_maxSamples * LineNode::verticesPerSample)
        m_vertices.remove(0, m_vertices.size() - m_maxSamples * LineNode::verticesPerSample);

    return true;
}

void SensorItem::getCPULoad(qreal &val)
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

        if (m_total_jiffies)
            val = qreal(work_jiffies - m_work_jiffies) / qreal(total_jiffies - m_total_jiffies) * 100.;
        else
            val = 0;

        m_work_jiffies = work_jiffies;
        m_total_jiffies = total_jiffies;
    } else
        val = 0;
}

qreal SensorItem::valueAt(const qint64 &timestamp)
{
    for (int x = 0; x < m_vertices.size(); x += LineNode::verticesPerSample)
        if (m_vertices.at(x).x >= timestamp)
            return m_vertices.at(x).y;

    return (m_vertices.length()) ? m_vertices.at(m_vertices.length() - 1).y : 0;
}

void SensorItem::setValueMin(qreal val)
{
    m_valueMin = val;
    emit valueMinChanged();
}

void SensorItem::setValueMax(qreal val)
{
    m_valueMax = val;
    emit valueMaxChanged();
}

qreal SensorItem::currentSample()
{
    if (m_vertices.length())
        return m_vertices.last().y;
    else
        return 0;
}

qreal SensorItem::sample()
{
    qreal val;

    switch (m_type) {
    case LM:
        sensors_get_value(m_chip, m_subfeature->number, &val);
        break;
    case CPU:
        getCPULoad(val);
        break;
    default:
        val = 0;
    }
    return (qreal)val;
}
