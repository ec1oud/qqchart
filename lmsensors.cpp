#include "lmsensors.h"

LmSensors::LmSensors(QObject *parent) :
    QObject(parent)
{
    m_initialized = init();
}

void appendItems(QQmlListProperty<SensorItem> *property, SensorItem *item)
{
    Q_UNUSED(property);
    Q_UNUSED(item);
    //Do nothing. can't add to a directory using this method
}

int itemSize(QQmlListProperty<SensorItem> *property)
{
    return static_cast< QList<SensorItem *> *>(property->data)->size();
}

SensorItem *itemAt(QQmlListProperty<SensorItem> *property, int index)
{
    return static_cast< QList<SensorItem *> *>(property->data)->at(index);
}

void clearitemPtr(QQmlListProperty<SensorItem> *property)
{
    return static_cast< QList<SensorItem *> *>(property->data)->clear();
}

QQmlListProperty<SensorItem> LmSensors::getItems()
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
    const char *adap=NULL;


    // add CPU load

    SensorItem *new_item = new SensorItem();
    new_item->type = new_item->CPU;
    new_item->index = m_sensorItems.count();
    new_item->label = "CPU Load";
    new_item->adapter = "proc-stat";
    new_item->max_samples = 10000;
    new_item->tmin = 5 * 60 * 1000;    // now is 0
    new_item->ymin=0;
    new_item->ymax=105;
    new_item->unit = "%";
    m_sensorItems.append(new_item);

    // add lm-sensors

    if(int err = sensors_init(NULL))
    {
        m_errorMessage = sensors_strerror(err);
        return false;
    }
    else
    {
        chip_nr=0;

        while ((chip = sensors_get_detected_chips(NULL, &chip_nr)))
        {
            if (sensors_snprintf_chip_name(buf, BUF_SIZE, chip) < 0)
                sprintf(buf,"%i",chip_nr);

            adap = sensors_get_adapter_name(&chip->bus);
            //            if(adap) qDebug() << " " << adap;
            a = 0;
            while ((feature = sensors_get_features(chip, &a)))
            {
                //                qDebug() << "  " << sensors_get_label(chip, feature);
                SensorItem *new_item = new SensorItem();

                sub = sensors_get_subfeature(chip, feature, (sensors_subfeature_type)(((int)feature->type) << 8));

                new_item->type = new_item->LM;
                new_item->index = m_sensorItems.count();
                new_item->label = sensors_get_label(chip, feature);
                if(adap) new_item->adapter = adap;
                new_item->chip = chip;
                new_item->chipid = chip_nr;
                new_item->chipname = buf;
                new_item->feature = feature;
                new_item->sub = sub;
                new_item->max_samples = 10000;
                new_item->tmin = 5 * 60 * 1000;    // now is 0

                switch(new_item->feature->type)
                {
                case SENSORS_FEATURE_IN:
                case SENSORS_FEATURE_VID:
                    new_item->ymin=0;
                    new_item->ymax=6;
                    new_item->unit = "V";
                    break;
                case SENSORS_FEATURE_FAN:
                    new_item->ymin=300;
                    new_item->ymax=1200;
                    new_item->unit = "1/min";
                    break;
                case SENSORS_FEATURE_TEMP:
                    new_item->unit = "°C";
                    break;
                case SENSORS_FEATURE_POWER:
                    new_item->unit = "W";
                    break;
                case SENSORS_FEATURE_ENERGY:
                    new_item->unit = "J";
                    break;
                case SENSORS_FEATURE_CURR:
                    new_item->unit = "A";
                    break;
                case SENSORS_FEATURE_HUMIDITY:
                    new_item->unit = "%";
                    break;
                case SENSORS_FEATURE_INTRUSION:
                    new_item->ymin=0;
                    new_item->ymax=12;
                    break;
                default:;
                }

                //            if(new_item->index == 0) new_item->checked=true;

                m_sensorItems.append(new_item);
            }
        }
    }

    return true;
}

bool LmSensors::do_sampleValues()
{
    qint64 timestamp = QDateTime().currentDateTime().toMSecsSinceEpoch();
    foreach (SensorItem* item, m_sensorItems)
    {
        item->do_sample(timestamp);
    }
    return true;
}


SensorItem::SensorItem(QObject *parent) :
    QObject(parent)
{
}

bool SensorItem::do_sample(const qint64 &timestamp)
{
    double val;

    switch(type)
    {
    case LM: sensors_get_value(chip, sub->number,&val); break;
    case CPU: getCPULoad(val); break;
    default: val=0;
    }

    //    val = (val>32000)?0:val;

    if(val<minval) minval=val;
    if(val>maxval) maxval=val;

//qDebug() << "new sample @" << timestamp << timestamp / 1000.0 << "have" << m_vertices.size() << "max" << max_samples;
    // make time values smaller; TODO why does this even matter?
    static qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    LineNode::appendVertices(m_vertices, (timestamp - startTime) / 1000.0, float(val));
    emit currentsampleChanged(&m_vertices);

    if (m_vertices.size() > max_samples * LineNode::verticesPerSample)
        m_vertices.remove(0, m_vertices.size() - max_samples * LineNode::verticesPerSample);

    return true;
}

void SensorItem::getCPULoad(double &val)
{
    // http://stackoverflow.com/questions/3017162/how-to-get-total-cpu-usage-in-linux-c
    // http://www.linuxhowtos.org/System/procstat.htm

    // cpu  63536 963 11961 946741 2090 0 107 0 0 0

    QFile data("/proc/stat");
    if (data.open(QFile::ReadOnly))
    {
        QTextStream in(&data);
        QStringList list;
        qint64 total_jiffies=0, work_jiffies=0;

        do{
            list = in.readLine(1024).split(" ", QString::SkipEmptyParts);
        }while(list.at(0) != "cpu");

        for(int x=1;x<list.length();x++) total_jiffies+=list.at(x).toInt();
        for(int x=1;x<4;x++) work_jiffies+=list.at(x).toInt();

        if(m_total_jiffies) val = float(work_jiffies - m_work_jiffies) / float(total_jiffies - m_total_jiffies)*100.;
        else val=0;

        m_work_jiffies = work_jiffies;
        m_total_jiffies = total_jiffies;
    }
    else val=0;
}

float SensorItem::valueAt(const qint64 &timestamp)
{
    for(int x=0; x<m_vertices.size(); x += LineNode::verticesPerSample)
        if (m_vertices.at(x).x >= timestamp) return m_vertices.at(x).y;

    return (m_vertices.length())?m_vertices.at(m_vertices.length()-1).y:0;
}

QPointF SensorItem::map2canvas(const QRectF &bounds, const qint64 &timestamp, const float &val)
{
    float scale_y = bounds.height() / (ymax-ymin);
    float scale_x = -bounds.width() / tmin;
    qint64 now = QDateTime().currentDateTime().toMSecsSinceEpoch();

    return QPointF(bounds.left()+(now - timestamp - tmin) * scale_x,
                   bounds.top()+bounds.height()-(val - ymin) * scale_y
                   );

}

float SensorItem::getvalue()
{
    double val;

    switch(type)
    {
    case LM: sensors_get_value(chip, sub->number,&val); break;
    case CPU: getCPULoad(val); break;
    default: val=0;
    }
    return (float) val;
}
