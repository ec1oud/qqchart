#include "linenode_p.h"

#include <QtGui/QColor>

#include <QtQuick/QSGSimpleMaterial>

class LineShader : public QSGSimpleMaterialShader<LineNode::LineMaterial>
{
    QSG_DECLARE_SIMPLE_SHADER(LineShader, LineNode::LineMaterial)

public:
    LineShader() {
        setShaderSourceFile(QOpenGLShader::Vertex, ":/shaders/LineNode.vsh");
        setShaderSourceFile(QOpenGLShader::Fragment, ":/shaders/LineNode.fsh");
    }

    QList<QByteArray> attributes() const {  return QList<QByteArray>() << "pos" << "prevNext"; }

    void updateState(const LineNode::LineMaterial *m, const LineNode::LineMaterial *) {
        program()->setUniformValue(id_lineWidth, GLfloat(m->aa ? m->lineWidth * 1.7 : m->lineWidth));
        program()->setUniformValue(id_warningBelowMinimum, m->warningMinValue);
        program()->setUniformValue(id_warningAboveMaximum, m->warningMaxValue);
        program()->setUniformValue(id_normalColor, m->color);
        program()->setUniformValue(id_warningMinColor, m->warningMinColor);
        program()->setUniformValue(id_warningMaxColor, m->warningMaxColor);
        program()->setUniformValue(id_dataTransform, m->dataTransform);
        program()->setUniformValue(id_aa, m->aa);
//qDebug() << "colors" << m->color << m->warningMinColor << m->warningMaxColor;
    }

    void resolveUniforms() {
        id_lineWidth = program()->uniformLocation("lineWidth");
        id_warningBelowMinimum = program()->uniformLocation("warningBelowMinimum");
        id_warningAboveMaximum = program()->uniformLocation("warningAboveMaximum");
        id_normalColor = program()->uniformLocation("normalColor");
        id_warningMinColor = program()->uniformLocation("warningMinColor");
        id_warningMaxColor = program()->uniformLocation("warningMaxColor");
        id_dataTransform = program()->uniformLocation("dataTransform");
        id_aa = program()->uniformLocation("aa");
    }

private:
    int id_lineWidth;
    int id_warningBelowMinimum;
    int id_warningAboveMaximum;
    int id_normalColor;
    int id_warningMinColor;
    int id_warningMaxColor;
    int id_dataTransform;
    int id_aa;
};

static const QSGGeometry::AttributeSet &attributes()
{
    static QSGGeometry::Attribute attr[] = {
        QSGGeometry::Attribute::create(0, 4, GL_FLOAT),
        QSGGeometry::Attribute::create(1, 4, GL_FLOAT)
    };
    static QSGGeometry::AttributeSet set = { 2, 8 * sizeof(float), attr };
    return set;
}

LineNode::LineNode()
    : m_geometry(attributes(), 0)
{
    setGeometry(&m_geometry);

    m_material = LineShader::createMaterial();
    m_material->setFlag(QSGMaterial::Blending);
    setMaterial(m_material);
    setFlag(OwnsMaterial);
}

void LineNode::updateGeometry(const QRectF &bounds, const QVector<LineVertex> *v, qreal timeScale)
{
    float vscale = bounds.height() / (m_maxValue - m_minValue);
    // TODO only adjust the x offset each frame
    QMatrix4x4 &matrix = m_material->state()->dataTransform;
    matrix.setToIdentity();
    matrix.scale(timeScale, -vscale);
    matrix.translate(bounds.width() / timeScale - v->last().x, -m_maxValue);
//qDebug() << v->size() << timeScale << vscale << matrix << v->first().x << v->first().y << v->last().x << v->last().y;

    m_geometry.setDrawingMode(m_wireframe ? GL_LINE_STRIP : GL_TRIANGLE_STRIP);
    if (m_geometry.vertexCount() != v->size())
        m_geometry.allocate(v->size());
    // TODO limit on left side to stay in bounds
    memcpy(m_geometry.vertexData(), v->constData(), sizeof(LineVertex) * v->size());
    markDirty(QSGNode::DirtyGeometry);
}

void LineNode::setLineWidth(float width)
{
    m_material->state()->lineWidth = width;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setColor(QColor color)
{
    m_material->state()->color = color;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setWarningMinColor(QColor color)
{
    m_material->state()->warningMinColor = color;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setWarningMaxColor(QColor color)
{
    m_material->state()->warningMaxColor = color;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setMinValue(qreal v)
{
    m_minValue = v;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setMaxValue(qreal v)
{
    m_maxValue = v;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setWarningMinValue(qreal v)
{
    m_material->state()->warningMinValue = v;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setWarningMaxValue(qreal v)
{
    m_material->state()->warningMaxValue = v;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setSpread(qreal v)
{
    m_material->state()->aa = v;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setWireframe(bool v)
{
    m_wireframe = v;
}
