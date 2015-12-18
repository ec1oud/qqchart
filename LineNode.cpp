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

    QList<QByteArray> attributes() const {  return QList<QByteArray>() << "pos" << "prevNext"; }

    void updateState(const LineNode::LineMaterial *m, const LineNode::LineMaterial *) {
        program()->setUniformValue(id_lineWidth, m->lineWidth);
        program()->setUniformValue(id_alertBelowMinimum, m->alertMinValue);
        program()->setUniformValue(id_alertAboveMaximum, m->alertMaxValue);
        program()->setUniformValue(id_normalColor, m->color);
        program()->setUniformValue(id_alertMinColor, m->alertMinColor);
        program()->setUniformValue(id_alertMaxColor, m->alertMaxColor);
        program()->setUniformValue(id_dataTransform, m->dataTransform);
        program()->setUniformValue(id_spread, m->spread);
//qDebug() << "colors" << m->color << m->alertMinColor << m->alertMaxColor;
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
    float i;
    float t;
    float prevX;
    float prevY;
    float nextX;
    float nextY;
    inline void set(int ii, float tt, float xx, float yy, float px, float py, float nx, float ny) {
        x = xx; y = yy; i = ii; t = tt;
//qDebug() << "x" << xx << "y" << yy << "i" << ii << "t" << tt;
        prevX = px; prevY = py; nextX = nx; nextY = ny;
    }
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

void LineNode::updateGeometry(const QRectF &bounds, const QList<qreal> &samples)
{
    static const int verticesPerSample = 4;
    m_geometry.setDrawingMode(m_wireframe ? GL_LINE_STRIP : GL_TRIANGLE_STRIP);
    m_geometry.allocate((samples.size() + 1) * verticesPerSample);
qDebug() << "samples" << samples.size() << "transform" << m_material->state()->dataTransform;
qDebug() << samples;
    float dx = 1.0;

    float x = 0;
    float xp = 0;
    float xn = x + dx;
    float sample = samples.at(0);
    float sampleNext = samples.at(1);
    float samplePrev = sample;
    LineVertex *v = (LineVertex *) m_geometry.vertexData();
    int lastI = samples.size() - 1;
    for (int i = 1; i < lastI; ++i) {
qDebug() << "x" << x << "y" << sample << "i" << i;
        v[i*verticesPerSample  ].set(0, -1, x, sample, xp, samplePrev, xn, sampleNext);
        v[i*verticesPerSample+1].set(1, 1, x, sample, xp, samplePrev, xn, sampleNext);
        v[i*verticesPerSample+2].set(2, -1, x, sample, xp, samplePrev, xn, sampleNext);
        v[i*verticesPerSample+3].set(3, 1, x, sample, xp, samplePrev, xn, sampleNext);
        xp = x;
        x = xn;
        xn += dx;
        samplePrev = sample;
        sample = sampleNext;
        sampleNext = samples.at(i + 1);
    }
qDebug() << "x" << x << "y" << sample << "i" << lastI;
    v[lastI*verticesPerSample  ].set(0, -1, x, sample, xp, samplePrev, xn, sampleNext);
    v[lastI*verticesPerSample+1].set(1, 1, x, sample, xp, samplePrev, xn, sampleNext);
    v[lastI*verticesPerSample+2].set(2, -1, x, sample, xp, samplePrev, xn, sampleNext);
    v[lastI*verticesPerSample+3].set(3, 1, x, sample, xp, samplePrev, xn, sampleNext);
    xp = x;
    x = xn;
    xn += dx;
    samplePrev = sample;
    sample = sampleNext;
    ++lastI;
qDebug() << "lastI ends at" << lastI << "so we are populating samples" << lastI * verticesPerSample << "->" << lastI * verticesPerSample + 3 << "of" << samples.size() * verticesPerSample;
qDebug() << "last sample: x" << x << "y" << sample << "i" << lastI;
    v[lastI*verticesPerSample  ].set(0, -1, x, sample, xp, samplePrev, xn, sampleNext);
    v[lastI*verticesPerSample+1].set(1, 1, x, sample, xp, samplePrev, xn, sampleNext);
    v[lastI*verticesPerSample+2].set(2, -1, x, sample, xp, samplePrev, xn, sampleNext);
    v[lastI*verticesPerSample+3].set(3, 1, x, sample, xp, samplePrev, xn, sampleNext);

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
