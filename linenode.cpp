#include "linenode_p.h"

#include <QtGui/QColor>

#include <QtQuick/QSGMaterial>

class LineShader : public QSGMaterialShader
{
public:
    LineShader() : QSGMaterialShader() {
        setShaderFileName(VertexStage, "shaders/LineNode.vert.qsb");
        setShaderFileName(FragmentStage, "shaders/LineNode.frag.qsb");
    }

    bool updateUniformData(RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial) override;
};

// Set of helper functions that Qt ought to provide, but apparently doesn't
static int uniformBufferAppendFloat(QByteArray *buf, int offset, float f)
{
    memcpy(buf->data() + offset, &f, sizeof(float));
    return offset + sizeof(float);
}
static int uniformBufferAppendColor(QByteArray *buf, int offset, QColor c)
{
    float cv[4] = { float(c.redF()),
                    float(c.greenF()),
                    float(c.blueF()),
                    float(c.alphaF()) };
    memcpy(buf->data() + offset, cv, 16);
    return offset + 16;
}
static int uniformBufferAppendVec2(QByteArray *buf, int offset, QVector2D v)
{
    memcpy(buf->data() + offset, &v, 8);
    return offset + 8;
}
static int uniformBufferAppendMatrix2x2(QByteArray *buf, int offset, const QMatrix2x2 &m)
{
    memcpy(buf->data() + offset, m.constData(), 16);
    return offset + 16;
}
static int uniformBufferAppendMatrix4x4(QByteArray *buf, int offset, const QMatrix4x4 &m)
{
    memcpy(buf->data() + offset, m.constData(), 64);
    return offset + 64;
}

bool LineShader::updateUniformData(RenderState &state, QSGMaterial *newMaterial, QSGMaterial *)
{
    QByteArray *buf = state.uniformData();
    Q_ASSERT(buf->size() >= (16 + 4 + (4 * 3) + 2 + 7) * 4);
    LineMaterial *mat = static_cast<LineMaterial *>(newMaterial);
    int offset = 0;
qDebug() << "incoming size" << buf->size() << "data transform" << mat->state.dataTransform; // never happens!
//    offset = uniformBufferAppendMatrix4x4(buf, offset, mat->state.dataTransform); // wrong? should be item transform
    offset = uniformBufferAppendMatrix4x4(buf, offset, state.combinedMatrix());
    offset = uniformBufferAppendMatrix2x2(buf, offset, mat->state.dataTransform.toGenericMatrix<2, 2>());
    offset = uniformBufferAppendColor(buf, offset, mat->state.color);
    offset = uniformBufferAppendColor(buf, offset, mat->state.warningMinColor);
    offset = uniformBufferAppendColor(buf, offset, mat->state.warningMaxColor);
    offset = uniformBufferAppendVec2(buf, offset, mat->state.dataTransform.column(3).toVector2D());
    offset = uniformBufferAppendFloat(buf, offset, state.opacity());
    offset = uniformBufferAppendFloat(buf, offset, mat->state.height);
    offset = uniformBufferAppendFloat(buf, offset, mat->state.lineWidth);
    offset = uniformBufferAppendFloat(buf, offset, mat->state.warningMinValue);
    offset = uniformBufferAppendFloat(buf, offset, mat->state.warningMaxValue);
    offset = uniformBufferAppendFloat(buf, offset, mat->state.fillDirection);
    offset = uniformBufferAppendFloat(buf, offset, mat->state.aa);
    return true;
}

static const QSGGeometry::AttributeSet &attributes()
{
    static QSGGeometry::Attribute attr[] = {
        QSGGeometry::Attribute::createWithAttributeType(0, 4, QSGGeometry::FloatType, QSGGeometry::PositionAttribute),
        QSGGeometry::Attribute::createWithAttributeType(1, 4, QSGGeometry::FloatType, QSGGeometry::UnknownAttribute)
    };
    static QSGGeometry::AttributeSet set = { 2, 8 * sizeof(float), attr };
    return set;
}

LineMaterial::LineMaterial() : QSGMaterial()
{
    setFlag(Blending);
}

QSGMaterialType *LineMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader *LineMaterial::createShader(QSGRendererInterface::RenderMode rm) const
{
    return new LineShader;
}

int LineMaterial::compare(const QSGMaterial *m) const
{
    const LineMaterial *other = static_cast<const LineMaterial *>(m);

    if (int diff = int(state.color.rgb()) - int(other->state.color.rgb()))
        return diff;

    if (int diff = int(state.warningMinColor.rgb()) - int(other->state.warningMinColor.rgb()))
        return diff;

    if (int diff = int(state.warningMaxColor.rgb()) - int(other->state.warningMaxColor.rgb()))
        return diff;

    if (int diff = state.height - other->state.height)
        return diff;

    if (int diff = state.lineWidth - other->state.lineWidth)
        return diff;

    if (int diff = state.warningMinValue - other->state.warningMinValue)
        return diff;

    if (int diff = state.warningMaxValue - other->state.warningMaxValue)
        return diff;

    if (int diff = state.fillDirection - other->state.fillDirection)
        return diff;

    if (int diff = state.aa - other->state.aa)
        return diff;

    return 0;
}

LineNode::LineNode()
    : QSGGeometryNode()
    , m_geometry(attributes(), 0)
    , m_material(new LineMaterial)
{
    qsgnode_set_description(this, QLatin1String("qqchart_linenode"));
    setGeometry(&m_geometry);

    m_material->setFlag(QSGMaterial::Blending);
    setMaterial(m_material);
    setFlag(OwnsMaterial);
}

void LineNode::updateGeometry(const QRectF &bounds, const QVector<LineVertex> *v, qreal timeScale)
{
    float vscale = bounds.height() / (m_maxValue - m_minValue);
    // TODO only adjust the x offset each frame
    QMatrix4x4 &matrix = m_material->state.dataTransform;
    matrix.setToIdentity();
    matrix.scale(timeScale, -vscale);
    matrix.translate(bounds.width() / timeScale - v->last().x, -m_maxValue);
    markDirty(QSGNode::DirtyMaterial);
    qDebug() << parent() << v->size() << m_geometry.vertexCount() << timeScale << vscale << matrix << v->first().x << v->first().y << v->last().x << v->last().y;

    m_geometry.setDrawingMode(m_wireframe ? QSGGeometry::DrawLineStrip : QSGGeometry::DrawTriangleStrip);
    if (v->size()) {
        if (m_geometry.vertexCount() != v->size())
            m_geometry.allocate(v->size());
        // TODO limit on left side to stay in bounds
        memcpy(m_geometry.vertexData(), v->constData(), sizeof(LineVertex) * v->size());
        markDirty(QSGNode::DirtyGeometry);
    }
}

void LineNode::setHeight(float height)
{
    m_material->state.height = height;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setLineWidth(float width)
{
    m_material->state.lineWidth = width;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setColor(QColor color)
{
    m_material->state.color = color;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setWarningMinColor(QColor color)
{
    m_material->state.warningMinColor = color;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setWarningMaxColor(QColor color)
{
    m_material->state.warningMaxColor = color;
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
    m_material->state.warningMinValue = v;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setWarningMaxValue(qreal v)
{
    m_material->state.warningMaxValue = v;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setFillDirection(qreal v)
{
    m_material->state.fillDirection = v;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setSpread(qreal v)
{
    m_material->state.aa = v;
    markDirty(QSGNode::DirtyMaterial);
}

void LineNode::setWireframe(bool v)
{
    m_wireframe = v;
}
