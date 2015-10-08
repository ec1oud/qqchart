#include "chart2d.h"

#include <QtQuick/QSGSimpleMaterialShader>
#include <QtQuick/qsgnode.h>
#include <private/qquickitem_p.h>
#include <private/qquicktextnode_p.h>
#include <QTextLayout>

extern bool multisample;

Q_LOGGING_CATEGORY(lcChart2D, "qqchart.chart2d")

class TimeValueShader : public QSGSimpleMaterialShader<Chart2D::TimeValueShaderParams>
{
    QSG_DECLARE_SIMPLE_COMPARABLE_SHADER(TimeValueShader, Chart2D::TimeValueShaderParams)
public:
    const char *vertexShader() const {
        return
            "attribute highp vec4 vertex;\n"
            "uniform highp vec4 xclip;\n"
            "uniform highp mat4 data_transform;\n"
            "uniform highp mat4 qt_Matrix;\n"
            "void main(void)\n"
            "{\n"
            "   vec4 vtx = vertex;\n"
            "   if (vertex.x > xclip.z) {\n"
            "     vtx.x = xclip.z;\n"
            "     vtx.y = xclip.w;\n"
            "   }\n"
            "   if (vertex.x < xclip.x) {\n"
            "     vtx.x = xclip.x;\n"
            "     vtx.y = xclip.y;\n"
            "   }\n"
            "   gl_Position = qt_Matrix * data_transform * vtx;\n"
            "}";
    }
    const char *fragmentShader() const {
        return
            "uniform lowp float qt_Opacity;\n"
            "uniform mediump vec4 color;\n"
            "void main(void)\n"
            "{\n"
            "   gl_FragColor = color * qt_Opacity;\n"
            "}";
    }

    QList<QByteArray> attributes() const { return QList<QByteArray>() << "vertex"; }

    void updateState(const Chart2D::TimeValueShaderParams *p, const Chart2D::TimeValueShaderParams *) {
//qDebug() << "xclip" << p->xclip;
        program()->setUniformValue(m_uXClip, p->xclip);
        program()->setUniformValue(m_uColor, p->color);
        program()->setUniformValue(m_uMatrix, p->pmvMatrix);
    }

    void resolveUniforms() {
        m_uXClip = program()->uniformLocation("xclip");
        m_uColor = program()->uniformLocation("color");
        m_uMatrix = program()->uniformLocation("data_transform");
    }

private:
    int m_uXClip;
    int m_uColor;
    int m_uMatrix;
};



Chart2D::Chart2D(QQuickItem *parent) :
    QQuickItem(parent),
    m_program(0),
    m_hzoom(1.),
    m_labelSpaceUnder(20.)
{
    setFlag(ItemHasContents, true);
    m_material = TimeValueShader::createMaterial();
    m_gridMaterial = TimeValueShader::createMaterial();
}

void Chart2D::setHorizontalZoom(qreal t)
{
    if (t == m_hzoom)
        return;
    m_hzoom = t;
    emit horizontalZoomChanged();
    update();
}

void Chart2D::fitAll()
{
    m_defaultHzoom = width() / (m_model->maxTime() - m_model->minTime());
    emit defaultHorizontalZoomChanged();
}

void Chart2D::setModel(DataSequenceModel *m)
{
    m_model = m;
    fitAll();
    emit modelChanged();
}

QSGNode *Chart2D::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    QSGNode *node = 0;
    QSGGeometry *geometry = 0;
    static int yearCount = 0;
    float minTime = m_model->minTime();
    int firstYear = QDateTime::fromMSecsSinceEpoch(qint64(minTime * 1000.0)).date().year();

    if (!oldNode) {
        node = new QSGNode;
        qsgnode_set_description(node, QLatin1String("Chart2D"));
        yearCount = 0;

        QSGGeometryNode *graphNode = new QSGGeometryNode;
//        qsgnode_set_description(graphNode, QLatin1String("Chart2D"));
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), m_model->rowCount());
//        if (subsample) geometry->setLineWidth(1.5);
        geometry->setDrawingMode(GL_LINE_STRIP);
        graphNode->setGeometry(geometry);
        graphNode->setFlag(QSGNode::OwnsGeometry);
        graphNode->setFlag(QSGNode::OwnsMaterial);
        graphNode->setMaterial(m_material);
        QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
        GLfloat *times = m_model->times();
        GLfloat *values = m_model->columnValues(0);
        float nextYear = QDateTime(QDate(firstYear, 1, 1)).toMSecsSinceEpoch() / 1000.0;
        for (int i = 0; i < m_model->rowCount(); ++i) {
            vertices[i].set(times[i] - minTime, values[i]);
            if (times[i] >= nextYear) {
                ++yearCount;
                nextYear = QDateTime(QDate(firstYear + yearCount, 1, 1)).toMSecsSinceEpoch() / 1000.0;
            }
//qDebug() << vertices[i].x << vertices[i].y;
        }
        node->appendChildNode(graphNode);
        graphNode->markDirty(QSGNode::DirtyMaterial | QSGNode::DirtyGeometry | QSGNode::DirtyForceUpdate | QSGNode::DirtySubtreeBlocked);


qDebug() << "data spans years" << firstYear << firstYear + yearCount;
        // TODO show quarters too; clip unnecessary ticks
        QSGGeometryNode *gridNode = new QSGGeometryNode;
        QSGGeometry *gridGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), yearCount * 2);
        QSGGeometry::Point2D *gridv = gridGeometry->vertexDataAsPoint2D();
        for (int yearI = 0; yearI < yearCount; ++yearI) {
            float t = QDateTime(QDate(firstYear + yearI, 1, 1)).toMSecsSinceEpoch() / 1000.0 - minTime;
            gridv[yearI * 2].set(t, 0.);
            gridv[yearI * 2 + 1].set(t, 5.);

            QQuickTextNode *text = new QQuickTextNode(this);
            node->appendChildNode(text);
            m_horizontalAxisLabels.insert(yearI, text);
        }
        if (multisample) gridGeometry->setLineWidth(1.5);
        gridGeometry->setDrawingMode(GL_LINES);
        gridNode->setGeometry(gridGeometry);
        gridNode->setFlag(QSGNode::OwnsGeometry);
        gridNode->setFlag(QSGNode::OwnsMaterial);
        gridNode->setMaterial(m_gridMaterial);
        node->appendChildNode(gridNode);
        gridNode->markDirty(QSGNode::DirtyMaterial | QSGNode::DirtyGeometry | QSGNode::DirtyForceUpdate | QSGNode::DirtySubtreeBlocked);

        fitAll();
        node->markDirty(QSGNode::DirtyMatrix | QSGNode::DirtyNodeAdded | QSGNode::DirtyMaterial | QSGNode::DirtyForceUpdate | QSGNode::DirtySubtreeBlocked);
    } else {
        node = oldNode;
        node->markDirty(QSGNode::DirtyForceUpdate);
    }

    float vrange = m_model->columnMaxValue(0) - m_model->columnMinValue(0);
    float vscale = (height() - m_labelSpaceUnder) / m_model->columnMaxValue(0);
    /*
      0.003125         0         0        -1
             0   0.00625         0        -1
             0         0        -1         0
             0         0         0         1
    */
    QMatrix4x4 matrix;
    // Flip vertically because we are graphing data in the first quadrant
    matrix.scale(1.0, -1.0, 1.0);
    matrix.translate(0., m_labelSpaceUnder - height());
    // scale to fit
    matrix.scale(m_hzoom, vscale, 1.0);
    float rightClipTime = width() / m_hzoom;
    int nearestIndex = std::lower_bound(m_model->m_times.begin(), m_model->m_times.end(), rightClipTime) - m_model->m_times.begin();
    m_material->state()->xclip = QVector4D(0., m_model->columnValues(0)[0], rightClipTime, m_model->columnValues(0)[nearestIndex]);
    m_material->state()->pmvMatrix = matrix;
    m_material->state()->color = m_color;

    // matrix for tick marks
    matrix = QMatrix4x4();
    // Flip vertically because we are graphing data in the first quadrant
    matrix.scale(1.0, -1.0, 1.0);
    matrix.translate(0., m_labelSpaceUnder - height());
    // scale horizontally only
    matrix.scale(m_hzoom, 1.0, 1.0);

    m_gridMaterial->state()->xclip = QVector4D(0., m_model->columnValues(0)[0], rightClipTime, m_model->columnValues(0)[nearestIndex]);
    m_gridMaterial->state()->pmvMatrix = matrix;
    m_gridMaterial->state()->color = m_gridColor;
    qCDebug(lcChart2D) << "bounds" << boundingRect()
                       << "matrix" << m_material->state()->pmvMatrix << "min" << m_model->columnMinValue(0)
                       << "max" << m_model->columnMaxValue(0) << "vrange" << vrange << "vscale" << vscale
                       << "time range" << m_model->m_times[0] << m_model->minTime() << "->" << m_model->maxTime() << "hzoom" << m_hzoom
                       << "right clip at time" << rightClipTime << "index" << nearestIndex << "value" << m_material->state()->xclip.w()
                       << "colors" << m_material->state()->color << m_gridMaterial->state()->color;

    for (int yearI = 0; yearI < yearCount; ++yearI) {
        float t = QDateTime(QDate(firstYear + yearI, 1, 1)).toMSecsSinceEpoch() / 1000.0 - minTime;
        QQuickTextNode *text = m_horizontalAxisLabels[yearI];
        text->deleteContent();
        QTextLayout *tl = new QTextLayout(QString::number(firstYear + yearI));
qDebug() << tl->text() << "@" << t << t * m_hzoom;
        tl->beginLayout();
        /* QTextLine line = */ tl->createLine();
//qDebug() << "line" << line.naturalTextRect() << "glyph runs" << line.glyphRuns().count();
        tl->endLayout();
        text->addTextLayout(QPointF(t * m_hzoom, height() - 14), tl, Qt::white);
    }

    return node;
}

void Chart2D::cleanup()
{
    if (m_program) {
        delete m_program;
        m_program = 0;
    }
}

void Chart2D::setLabelSpaceUnder(qreal labelSpaceUnder)
{
    if (m_labelSpaceUnder == labelSpaceUnder)
        return;

    m_labelSpaceUnder = labelSpaceUnder;
    emit labelSpaceUnderChanged();
}
