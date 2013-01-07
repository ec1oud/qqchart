#include "chart2d.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtQuick/QSGSimpleMaterialShader>
#include <QtGui/QOpenGLContext>
#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgflatcolormaterial.h>

class TimeValueShader : public QSGSimpleMaterialShader<Chart2D::TimeValueShaderParams>
{
    QSG_DECLARE_SIMPLE_COMPARABLE_SHADER(TimeValueShader, Chart2D::TimeValueShaderParams)
public:
    const char *vertexShader() const {
        return
            "attribute highp vec4 vertex;\n"
            "uniform highp mat4 qt_Matrix;\n"
            "void main(void)\n"
            "{\n"
            "   gl_Position = qt_Matrix * vertex;\n"
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
        program()->setUniformValue(m_uColor, p->color);
//        program()->setUniformValue(m_uMatrix, p->pmvMatrix);
    }

    void resolveUniforms() {
        m_uColor = program()->uniformLocation("color");
        m_uMatrix = program()->uniformLocation(uniformMatrixName());
    }

private:
    int m_uColor;
    int m_uMatrix;
};



Chart2D::Chart2D() :
    m_program(0),
    m_hzoom(0.05)
{
    setFlag(ItemHasContents, true);
    m_material = TimeValueShader::createMaterial();
}

void Chart2D::setHorizontalZoom(qreal t)
{
    if (t == m_hzoom)
        return;
    m_hzoom = t;
    emit horizontalZoomChanged();
    if (window())
        window()->update();
}

QSGNode *Chart2D::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    QSGGeometryNode *node = 0;
    QSGGeometry *geometry = 0;

    if (!oldNode) {
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), m_model->rowCount());
//        geometry->setLineWidth(2);
        geometry->setDrawingMode(GL_LINE_STRIP);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        node->setFlag(QSGNode::OwnsMaterial);
        node->setMaterial(m_material);
        QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
        GLfloat *values = m_model->columnValues(0);
        for (int i = 0; i < m_model->rowCount(); ++i) {
            vertices[i].set(values[i * 2], values[i * 2 + 1]);
        }
    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
//        geometry->allocate(m_model->rowCount());
    }
    node->markDirty(QSGNode::DirtyMaterial | QSGNode::DirtyForceUpdate);

    /*
      0.003125         0         0        -1
             0   0.00625         0        -1
             0         0        -1         0
             0         0         0         1
    */
    QMatrix4x4 matrix;
    matrix.ortho(x(), width(), y() + height(), y(), -1, 1);
    float vrange = m_model->columnMaxValue(0) - m_model->columnMinValue(0);
    float vscale = height() / vrange / 2.0;
    matrix.translate(0, - m_model->columnMinValue(0));
    matrix.scale(m_hzoom, vscale, 1.0);
    m_material->state()->pmvMatrix = matrix;
    m_material->state()->color = m_color;
    qDebug() << "bounds" << boundingRect()
             << "matrix" << m_material->state()->pmvMatrix << "min" << m_model->columnMinValue(0)
             << "max" << m_model->columnMaxValue(0) << "vrange" << vrange << "vscale" << vscale << "node" << node;

    return node;
}

void Chart2D::cleanup()
{
    if (m_program) {
        delete m_program;
        m_program = 0;
    }
}
