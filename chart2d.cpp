#include "chart2d.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgflatcolormaterial.h>

Chart2D::Chart2D() :
    m_program(0),
    m_hzoom(0.05)
{
    setFlag(ItemHasContents, true);
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
        geometry->setLineWidth(2);
        geometry->setDrawingMode(GL_LINE_STRIP);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(QColor(255, 0, 0));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
        geometry->allocate(m_model->rowCount());
    }

//    QRectF bounds = boundingRect();
    QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
    GLfloat *values = m_model->columnValues(0);
    for (int i = 0; i < m_model->rowCount(); ++i) {
        vertices[i].set(values[i * 2] * 0.1, values[i * 2 + 1]);
    }

    return node;
}

void Chart2D::paint()
{
    if (!m_program) {
        m_program = new QOpenGLShaderProgram();
        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
            "attribute highp vec4 vertex;\n"
            "uniform highp mat4 matrix;\n"
            "void main(void)\n"
            "{\n"
            "   gl_Position = matrix * vertex;\n"
            "}");
        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
            "uniform mediump vec4 color;\n"
            "void main(void)\n"
            "{\n"
            "   gl_FragColor = color;\n"
            "}");

        m_program->link();
        connect(window()->openglContext(), SIGNAL(aboutToBeDestroyed()),
                this, SLOT(cleanup()), Qt::DirectConnection);
    }
    m_program->bind();

    int vertexLocation = m_program->attributeLocation("vertex");
    int matrixLocation = m_program->uniformLocation("matrix");
    int colorLocation = m_program->uniformLocation("color");
    GLfloat *values = m_model->columnValues(0);
    qDebug("%f %f %f %f %f %f ...", values[0], values[1], values[2], values[3], values[4], values[5]);

    QColor color(255, 255, 255, 255);

    QMatrix4x4 pmvMatrix;
    /*
      0.003125         0         0        -1
             0  -0.00625         0         1
             0         0        -1         0
             0         0         0         1
    */
//    pmvMatrix.ortho(QRect(QPoint(0,0), window()->size()));
    /*
      0.003125         0         0        -1
             0   0.00625         0        -1
             0         0        -1         0
             0         0         0         1
    */
    pmvMatrix.ortho(0, width(), 0, height(), -1, 1);
    float vrange = m_model->columnMaxValue(0) - m_model->columnMinValue(0);
    float vscale = height() / vrange / 2.0;
    pmvMatrix.scale(m_hzoom, vscale, 1.0);
    pmvMatrix.translate(x() / m_hzoom, y() / vscale - m_model->columnMinValue(0), 0);
    qDebug() << "x" << x() << "height" << height() << "window height" << window()->height()
             << "matrix" << pmvMatrix << "min" << m_model->columnMinValue(0)
             << "max" << m_model->columnMaxValue(0) << "vrange" << vrange << "vscale" << vscale;

    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeArray(vertexLocation, values, 2);
    m_program->setUniformValue(matrixLocation, pmvMatrix);
    m_program->setUniformValue(colorLocation, color);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_LINE_STRIP, 0, m_model->rowCount());

    m_program->disableAttributeArray(vertexLocation);
//    glViewport(0, 0, window()->width(), window()->height());
}

void Chart2D::cleanup()
{
    if (m_program) {
        delete m_program;
        m_program = 0;
    }
}
