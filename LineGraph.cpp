#include "LineGraph.h"
#include "LineNode.h"

LineGraph::LineGraph()
    : m_samplesChanged(false)
    , m_geometryChanged(false)
    , m_wireframe(false)
    , m_lineWidth(2)
    , m_minValue(0)
    , m_maxValue(1)
    , m_alertMinValue(0)
    , m_alertMaxValue(1)
    , m_timeScale(1)
{
    m_samples.reserve(30);
    setFlag(ItemHasContents, true);
    setAcceptHoverEvents(true);
}

void LineGraph::registerMetaType()
{
    qmlRegisterType<LineGraph>("LineGraph", 1, 0, "LineGraph");
}

void LineGraph::appendSample(qreal value)
{
    m_samples << value;
    m_samplesChanged = true;
    update();
}

void LineGraph::removeFirstSample()
{
    m_samples.removeFirst();
    m_samplesChanged = true;
    update();
}

void LineGraph::setLineWidth(qreal lineWidth)
{
    if (m_lineWidth == lineWidth)
        return;

    m_lineWidth = lineWidth;
    emit lineWidthChanged();
    update();
}

void LineGraph::setColor(QColor color)
{
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged();
    update();
}

void LineGraph::setAlertMinColor(QColor alertMinColor)
{
    if (m_alertMinColor == alertMinColor)
        return;

    m_alertMinColor = alertMinColor;
    emit alertMinColorChanged();
    update();
}

void LineGraph::setAlertMaxColor(QColor alertMaxColor)
{
    if (m_alertMaxColor == alertMaxColor)
        return;

    m_alertMaxColor = alertMaxColor;
    emit alertMaxColorChanged();
    update();
}

void LineGraph::setMinValue(qreal minValue)
{
    if (m_minValue == minValue)
        return;

    m_minValue = minValue;
    emit minValueChanged();
}

void LineGraph::setMaxValue(qreal maxValue)
{
    if (m_maxValue == maxValue)
        return;

    m_maxValue = maxValue;
    emit maxValueChanged();
}

void LineGraph::setAlertMinValue(qreal alertMinValue)
{
    if (m_alertMinValue == alertMinValue)
        return;

    m_alertMinValue = alertMinValue;
    emit alertMinValueChanged();
}

void LineGraph::setAlertMaxValue(qreal alertMaxValue)
{
    if (m_alertMaxValue == alertMaxValue)
        return;

    m_alertMaxValue = alertMaxValue;
    emit alertMaxValueChanged();
}

void LineGraph::setTimeScale(qreal timeScale)
{
    if (m_timeScale == timeScale)
        return;

    m_timeScale = timeScale;
    m_geometryChanged = true;
    emit timeScaleChanged();
}

qreal LineGraph::valueAtX(qreal x)
{
    return m_samples.at(qMax(0, qMin(qRound(x / width() * m_samples.length()), m_samples.length())));
}

qreal LineGraph::yPixelAtX(qreal x)
{
    qreal vscale = height() / (m_maxValue - m_minValue);
    return height() - vscale * (valueAtX(x) - m_minValue);
}

void LineGraph::setWireframe(bool wireframe)
{
    if (m_wireframe == wireframe)
        return;

    m_wireframe = wireframe;
    emit wireframeChanged();
}

void LineGraph::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_geometryChanged = true;
    update();
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}


class GraphNode : public QSGNode
{
public:
    LineNode *line;
};


QSGNode *LineGraph::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    GraphNode *n= static_cast<GraphNode *>(oldNode);

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
    n->line->setLineWidth(m_lineWidth);
    n->line->setColor(m_color);
    n->line->setAlertMinColor(m_alertMinColor);
    n->line->setAlertMaxColor(m_alertMaxColor);
    n->line->setAlertMinValue(m_alertMinValue);
    n->line->setAlertMaxValue(m_alertMaxValue);
    n->line->setMinValue(m_minValue);
    n->line->setMaxValue(m_maxValue);
    n->line->setSpread(antialiasing() && !m_wireframe ? 1.0 : 0.0);
    n->line->setWireframe(m_wireframe);

    if (m_geometryChanged) {
        float vscale = height() / (m_maxValue - m_minValue);
        QMatrix4x4 matrix;
        // Scale to fit vertically, flipped for first quadrant
        // Horizontally: m_timeScale is actually pixels-per-sample
        matrix.scale(m_timeScale, -vscale);
        matrix.translate(0, -m_maxValue);
//qDebug() << "dataTransform" << matrix;
        n->line->m_material->state()->dataTransform = matrix;
    }

    if (m_geometryChanged || m_samplesChanged) {
        n->line->updateGeometry(rect, m_samples);
    }

    m_geometryChanged = false;
    m_samplesChanged = false;

    emit dataChanged();

    return n;
}
