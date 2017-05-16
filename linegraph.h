#ifndef LINEGRAPH_H
#define LINEGRAPH_H

#include <QQuickItem>
#include "linegraphmodel.h"

class LineGraph : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(LineGraphModel * model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(qreal lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor fillColorBelow READ fillColorBelow WRITE setFillColorBelow NOTIFY fillColorBelowChanged)
    Q_PROPERTY(QColor fillColorAbove READ fillColorAbove WRITE setFillColorAbove NOTIFY fillColorAboveChanged)
    Q_PROPERTY(QColor warningMinColor READ warningMinColor WRITE setWarningMinColor NOTIFY warningMinColorChanged)
    Q_PROPERTY(QColor warningMaxColor READ warningMaxColor WRITE setWarningMaxColor NOTIFY warningMaxColorChanged)
    Q_PROPERTY(qreal minValue READ minValue WRITE setMinValue NOTIFY minValueChanged) // convenience
    Q_PROPERTY(qreal maxValue READ maxValue WRITE setMaxValue NOTIFY maxValueChanged) // convenience
    Q_PROPERTY(qreal timeSpan READ timeSpan WRITE setTimeSpan NOTIFY timeSpanChanged) // in seconds
    Q_PROPERTY(bool wireframe READ wireframe WRITE setWireframe NOTIFY wireframeChanged)

public:
    LineGraph();

    LineGraphModel * model() const { return m_model; }
    void setModel(LineGraphModel * model);

    qreal lineWidth() const { return m_lineWidth; }
    void setLineWidth(qreal lineWidth);

    QColor color() const { return m_color; }
    void setColor(QColor color);

    QColor fillColorBelow() const { return m_fillColorBelow; }
    void setFillColorBelow(QColor fillColorBelow);

    QColor fillColorAbove() const { return m_fillColorAbove; }
    void setFillColorAbove(QColor fillColorAbove);

    QColor warningMinColor() const { return m_warningMinColor; }
    void setWarningMinColor(QColor warningMinColor);

    QColor warningMaxColor() const { return m_warningMaxColor; }
    void setWarningMaxColor(QColor warningMaxColor);

    qreal minValue() const { return m_model ? m_model->minValue() : 0; }
    void setMinValue(qreal minValue);

    qreal maxValue() const { return m_model ? m_model->maxValue() : 0; }
    void setMaxValue(qreal maxValue);

    qreal timeSpan() const { return m_timeSpan; }
    void setTimeSpan(qreal timeSpan);

    bool wireframe() const { return m_wireframe; }
    void setWireframe(bool wireframe);

    Q_INVOKABLE QJSValue sampleNearestX(qreal x);
    Q_INVOKABLE qreal xAtTime(qreal time);

    static void registerMetaType();

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void updateVertices();

public slots:
    void appendSample(qreal value); // convenience
    void removeFirstSample(); // convenience

signals:
    void modelChanged();
    void lineWidthChanged();
    void colorChanged();
    void fillColorBelowChanged();
    void fillColorAboveChanged();
    void warningMinColorChanged();
    void warningMaxColorChanged();
    void minValueChanged();
    void maxValueChanged();
    void timeSpanChanged();
    void samplesChanged(); // the model has samplesChanged too, but this occurs at a max of once per frame
    void wireframeChanged();

protected:
    LineGraphModel *m_model = 0;
    bool m_samplesChanged = false;
    bool m_geometryChanged = false;
    bool m_propertiesChanged = true;
    bool m_wireframe = false;
    QColor m_color = Qt::cyan;
    QColor m_fillColorBelow = Qt::transparent;
    QColor m_fillColorAbove = Qt::transparent;
    QColor m_warningMinColor = Qt::yellow;
    QColor m_warningMaxColor = Qt::red;
    qreal m_lineWidth = 1.3;
    qreal m_timeSpan = 60;
};

#endif // LINEGRAPH_H
