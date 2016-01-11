#include "linegraph.h"
#include "linenode_p.h"
#include <QDateTime>

LineGraph::LineGraph()
{
    setFlag(ItemHasContents, true);
    setAntialiasing(true);
}

void LineGraph::registerMetaType()
{
    qmlRegisterType<LineGraph>("LineGraph", 1, 0, "LineGraph");
}

void LineGraph::appendSample(qreal value)
{
    m_model->appendSample(value);
}

void LineGraph::removeFirstSample()
{
    m_model->removeFirstSample();
}

void LineGraph::setLineWidth(qreal lineWidth)
{
    if (m_lineWidth == lineWidth)
        return;

    m_lineWidth = lineWidth;
    m_propertiesChanged = true;
    emit lineWidthChanged();
    update();
}

void LineGraph::setColor(QColor color)
{
    if (m_color == color)
        return;

    m_color = color;
    m_propertiesChanged = true;
    emit colorChanged();
    update();
}

void LineGraph::setFillColorBelow(QColor fillColorBelow)
{
    if (m_fillColorBelow == fillColorBelow)
        return;

    m_fillColorBelow = fillColorBelow;
    m_propertiesChanged = true;
    emit fillColorBelowChanged();
    update();
}

void LineGraph::setFillColorAbove(QColor fillColorAbove)
{
    if (m_fillColorAbove == fillColorAbove)
        return;

    m_fillColorAbove = fillColorAbove;
    m_propertiesChanged = true;
    emit fillColorAboveChanged();
    update();
}

void LineGraph::setWarningMinColor(QColor warningMinColor)
{
    if (m_warningMinColor == warningMinColor)
        return;

    m_warningMinColor = warningMinColor;
    m_propertiesChanged = true;
    emit warningMinColorChanged();
    update();
}

void LineGraph::setWarningMaxColor(QColor warningMaxColor)
{
    if (m_warningMaxColor == warningMaxColor)
        return;

    m_warningMaxColor = warningMaxColor;
    m_propertiesChanged = true;
    emit warningMaxColorChanged();
    update();
}

void LineGraph::setMinValue(qreal minValue)
{
    if (m_model)
        m_model->setMinValue(minValue);

    m_geometryChanged = true;
    m_propertiesChanged = true;
    emit minValueChanged();
    update();
}

void LineGraph::setMaxValue(qreal maxValue)
{
    if (m_model)
        m_model->setMaxValue(maxValue);

    m_geometryChanged = true;
    m_propertiesChanged = true;
    emit maxValueChanged();
    update();
}

void LineGraph::setTimeSpan(qreal timeSpan)
{
    if (m_timeSpan == timeSpan)
        return;

    m_timeSpan = timeSpan;
    m_geometryChanged = true;
    emit timeSpanChanged();
    update();
}

qreal LineGraph::xAtTime(qint64 time)
{
    return width() * time / m_timeSpan;
}

qint64 LineGraph::timeAtX(qreal x)
{
    if (!m_model)
        return -1;
    return m_model->sampleTimeNearest(x / width() * m_timeSpan);
}

qreal LineGraph::valueAtX(qreal x)
{
    if (!m_model)
        return qQNaN();
    return m_model->sampleNearest(x / width() * m_timeSpan);
}

qreal LineGraph::yPixelAtX(qreal x)
{
    if (!m_model)
        return qQNaN();
    qreal vscale = height() / (m_model->maxValue() - m_model->minValue());
    return height() - vscale * (valueAtX(x) - m_model->minValue());
}

void LineGraph::setModel(LineGraphModel *model)
{
    if (m_model == model)
        return;

    if (m_model) {
        disconnect(m_model, &LineGraphModel::samplesChanged, this, &LineGraph::updateVertices);
        disconnect(m_model, &LineGraphModel::minValueChanged, this, &LineGraph::minValueChanged);
        disconnect(m_model, &LineGraphModel::maxValueChanged, this, &LineGraph::maxValueChanged);
    }
    m_model = model;
    connect(model, &LineGraphModel::samplesChanged, this, &LineGraph::updateVertices);
    connect(model, &LineGraphModel::minValueChanged, this, &LineGraph::minValueChanged);
    connect(model, &LineGraphModel::maxValueChanged, this, &LineGraph::maxValueChanged);
    emit modelChanged();
    updateVertices();
}

void LineGraph::setWireframe(bool wireframe)
{
    if (m_wireframe == wireframe)
        return;

    m_wireframe = wireframe;
    m_propertiesChanged = true;
    emit wireframeChanged();
}

void LineGraph::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_geometryChanged = true;
    update();
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

void LineGraph::updateVertices()
{
    m_samplesChanged = true;
    update();
}


class GraphNode : public QSGNode
{
public:
    LineNode *line;
};


QSGNode *LineGraph::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    if (!m_model)
        return oldNode;

    GraphNode *n = static_cast<GraphNode *>(oldNode);

    QRectF rect = boundingRect();

    if (rect.isEmpty()) {
        delete n;
        return 0;
    }
    if (!n) {
        n = new GraphNode();
        n->line = new LineNode();
        n->appendChildNode(n->line);
    }
    if (m_propertiesChanged) {
        float fillDirection = 0;
        QColor fillColor = m_color;
        if (m_fillColorAbove != Qt::transparent) {
            fillDirection = -1;
            fillColor = m_fillColorAbove;
        }
        if (m_fillColorBelow != Qt::transparent) {
            fillDirection = 1;
            fillColor = m_fillColorBelow;
        }
        n->line->setHeight(height());
        n->line->setLineWidth(m_lineWidth);
        n->line->setColor(fillColor);
        n->line->setWarningMinColor(m_warningMinColor);
        n->line->setWarningMaxColor(m_warningMaxColor);
        n->line->setWarningMinValue(m_model->normalMinValue());
        n->line->setWarningMaxValue(m_model->normalMaxValue());
        n->line->setFillDirection(fillDirection);
        n->line->setMinValue(m_model->minValue());
        n->line->setMaxValue(m_model->maxValue());
        n->line->setWireframe(m_wireframe);
    }
    n->line->setSpread(antialiasing() && !m_wireframe ? 1.0 : 0.0);

    if ((m_geometryChanged || m_samplesChanged) && !m_model->vertices()->isEmpty())
        n->line->updateGeometry(rect, m_model->vertices(), width() / m_timeSpan);

    m_geometryChanged = false;
    m_samplesChanged = false;
    m_propertiesChanged = false;

    emit samplesChanged();

    return n;
}
