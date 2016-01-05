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
    update();
}

void LineGraph::setMaxValue(qreal maxValue)
{
    if (m_model)
        m_model->setMaxValue(maxValue);

    m_geometryChanged = true;
    update();
}

void LineGraph::setTimeScale(qreal timeScale)
{
    if (m_timeScale == timeScale)
        return;

    m_timeScale = timeScale;
    m_geometryChanged = true;
    emit timeScaleChanged();
    update();
}

qreal LineGraph::xAtTime(qint64 time)
{
    return time * m_timeScale;
}

qint64 LineGraph::timeAtX(qreal x)
{
    if (!m_model)
        return -1;
    return m_model->sampleTimeNearest(x / m_timeScale);
}

qreal LineGraph::valueAtX(qreal x)
{
    if (!m_model)
        return qQNaN();
    return m_model->sampleNearest(x / m_timeScale);
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
        n->line->setLineWidth(m_lineWidth);
        n->line->setColor(m_color);
        n->line->setWarningMinColor(m_warningMinColor);
        n->line->setWarningMaxColor(m_warningMaxColor);
        n->line->setWarningMinValue(m_model->normalMinValue());
        n->line->setWarningMaxValue(m_model->normalMaxValue());
        n->line->setMinValue(m_model->minValue());
        n->line->setMaxValue(m_model->maxValue());
        n->line->setWireframe(m_wireframe);
    }
    n->line->setSpread(antialiasing() && !m_wireframe ? 1.0 : 0.0);

    if ((m_geometryChanged || m_samplesChanged) && !m_model->vertices()->isEmpty())
        n->line->updateGeometry(rect, m_model->vertices(), m_timeScale);

    m_geometryChanged = false;
    m_samplesChanged = false;
    m_propertiesChanged = false;

    emit samplesChanged();

    return n;
}
