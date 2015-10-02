#ifndef LINEGRAPH_H
#define LINEGRAPH_H

#include <QQuickItem>

class LineGraph : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor alertMinColor READ alertMinColor WRITE setAlertMinColor NOTIFY alertMinColorChanged)
    Q_PROPERTY(QColor alertMaxColor READ alertMaxColor WRITE setAlertMaxColor NOTIFY alertMaxColorChanged)
    Q_PROPERTY(qreal minValue READ minValue WRITE setMinValue NOTIFY minValueChanged)
    Q_PROPERTY(qreal maxValue READ maxValue WRITE setMaxValue NOTIFY maxValueChanged)
    Q_PROPERTY(qreal alertMinValue READ alertMinValue WRITE setAlertMinValue NOTIFY alertMinValueChanged)
    Q_PROPERTY(qreal alertMaxValue READ alertMaxValue WRITE setAlertMaxValue NOTIFY alertMaxValueChanged)
    Q_PROPERTY(qreal timeScale READ timeScale WRITE setTimeScale NOTIFY timeScaleChanged)
    Q_PROPERTY(bool wireframe READ wireframe WRITE setWireframe NOTIFY wireframeChanged)

public:
    LineGraph();

    qreal lineWidth() const { return m_lineWidth; }
    QColor color() const { return m_color; }
    QColor alertMinColor() const { return m_alertMinColor; }
    QColor alertMaxColor() const { return m_alertMaxColor; }
    qreal minValue() const { return m_minValue; }
    qreal maxValue() const { return m_maxValue; }
    qreal alertMinValue() const { return m_alertMinValue; }
    qreal alertMaxValue() const { return m_alertMaxValue; }
    qreal timeScale() const { return m_timeScale; }
    bool wireframe() const { return m_wireframe; }

    static void registerMetaType();

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

public slots:
    void appendSample(qreal value);
    virtual void removeFirstSample();

    void setLineWidth(qreal lineWidth);
    void setColor(QColor color);
    void setAlertMinColor(QColor alertMinColor);
    void setAlertMaxColor(QColor alertMaxColor);
    void setMinValue(qreal minValue);
    void setMaxValue(qreal maxValue);
    void setAlertMinValue(qreal alertMinValue);
    void setAlertMaxValue(qreal alertMaxValue);
    void setTimeScale(qreal timeScale);
    void setWireframe(bool wireframe);

    qreal valueAtX(qreal x);
    qreal yPixelAtX(qreal x);

signals:
    void lineWidthChanged();
    void colorChanged();
    void alertMinColorChanged();
    void alertMaxColorChanged();
    void minValueChanged();
    void maxValueChanged();
    void alertMinValueChanged();
    void alertMaxValueChanged();
    void timeScaleChanged();
    void dataChanged();
    void wireframeChanged();

protected:
    QList<qreal> m_samples;

    bool m_samplesChanged;
    bool m_geometryChanged;
    bool m_wireframe;
    qreal m_lineWidth;
    QColor m_color;
    QColor m_alertMinColor;
    QColor m_alertMaxColor;
    qreal m_minValue;
    qreal m_maxValue;
    qreal m_alertMinValue;
    qreal m_alertMaxValue;
    qreal m_timeScale;
};

#endif // LINEGRAPH_H
