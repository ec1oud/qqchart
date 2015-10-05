#include "LineNode.h"

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

    QList<QByteArray> attributes() const {  return QList<QByteArray>() << "pos" << "t" << "prevNext"; }

    void updateState(const LineNode::LineMaterial *m, const LineNode::LineMaterial *) {
        program()->setUniformValue(id_lineWidth, m->lineWidth);
        program()->setUniformValue(id_alertBelowMinimum, m->alertMinValue);
        program()->setUniformValue(id_alertAboveMaximum, m->alertMaxValue);
        program()->setUniformValue(id_normalColor, m->color);
        program()->setUniformValue(id_alertMinColor, m->alertMinColor);
        program()->setUniformValue(id_alertMaxColor, m->alertMaxColor);
        program()->setUniformValue(id_dataTransform, m->dataTransform);
        program()->setUniformValue(id_spread, m->spread);
    }

    void resolveUniforms() {
        id_lineWidth = program()->uniformLocation("lineWidth");
        id_alertBelowMinimum = program()->uniformLocation("alertBelowMinimum");
        id_alertAboveMaximum = program()->uniformLocation("alertAboveMaximum");
        id_normalColor = program()->uniformLocation("normalColor");
        id_alertMinColor = program()->uniformLocation("alertMinColor");
        id_alertMaxColor = program()->uniformLocation("alertMaxColor");
        id_dataTransform = program()->uniformLocation("dataTransform");
        id_spread = program()->uniformLocation("spread");
    }

private:
    int id_lineWidth;
    int id_alertBelowMinimum;
    int id_alertAboveMaximum;
    int id_normalColor;
    int id_alertMinColor;
    int id_alertMaxColor;
    int id_dataTransform;
    int id_spread;
};

struct LineVertex {
    float x;
    float y;
    float t;
    float prevX;
    float prevY;
    float nextX;
    float nextY;
    inline void set(float xx, float yy, float tt, float px ,float py, float nx, float ny) {
        x = xx; y = yy; t = tt;
        prevX = px; prevY = py; nextX = nx; nextY = ny;
    }
};

static const QSGGeometry::AttributeSet &attributes()
{
    static QSGGeometry::Attribute attr[] = {
        QSGGeometry::Attribute::create(0, 2, GL_FLOAT),
        QSGGeometry::Attribute::create(1, 1, GL_FLOAT),
        QSGGeometry::Attribute::create(2, 4, GL_FLOAT)
    };
    static QSGGeometry::AttributeSet set = { 2, 7 * sizeof(float), attr };
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

void LineNode::updateGeometry(const QRectF &bounds, const QList<qreal> &samples)
{
    m_geometry.setDrawingMode(m_wireframe ? GL_LINE_STRIP : GL_TRIANGLE_STRIP);
    m_geometry.allocate(samples.size() * 2);

    float dx = 1.0;
    float x = 0;
    float xp = 0;
    float xn = x + dx;
    float sample = samples.at(0);
    float sampleNext = samples.at(1);
    float samplePrev = sample;
    LineVertex *v = (LineVertex *) m_geometry.vertexData();
    int lastI = samples.size() - 2;
    for (int i = 0; i < lastI; ++i) {
        v[i*2  ].set(x, sample, -0.5, xp, samplePrev, xn, sampleNext);
        v[i*2+1].set(x, sample, 0.5, xp, samplePrev, xn, sampleNext);
        xp = x;
        x = xn;
        xn += dx;
        samplePrev = sample;
        sample = sampleNext;
        sampleNext = samples.at(i + 2);
    }
    v[lastI*2  ].set(x, sample, -0.5, xp, samplePrev, xn, sampleNext);
    v[lastI*2+1].set(x, sample, 0.5, xp, samplePrev, xn, sampleNext);
    xp = x;
    x = xn;
    xn += dx;
    samplePrev = sample;
    sample = sampleNext;
    ++lastI;
    v[lastI*2  ].set(x, sample, -0.5, xp, samplePrev, xn, sampleNext);
    v[lastI*2+1].set(x, sample, 0.5, xp, samplePrev, xn, sampleNext);

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

void LineNode::setAlertMinColor(QColor color)
{
    m_material->state()->alertMinColor = color;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setAlertMaxColor(QColor color)
{
    m_material->state()->alertMaxColor = color;
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

void LineNode::setAlertMinValue(qreal v)
{
    m_material->state()->alertMinValue = v;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setAlertMaxValue(qreal v)
{
    m_material->state()->alertMaxValue = v;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setSpread(qreal v)
{
    m_material->state()->spread = v;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setWireframe(bool v)
{
    m_wireframe = v;
}
