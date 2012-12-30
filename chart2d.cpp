#include "chart2d.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>

Chart2D::Chart2D()
    : m_program(0)
{
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

void Chart2D::itemChange(ItemChange change, const ItemChangeData &)
{
    // The ItemSceneChange event is sent when we are first attached to a window.
    if (change == ItemSceneChange) {
        QQuickWindow *c = window();
        if (!c)
            return;

        // Connect the beforeRendering signal to our paint function.
        // Since this call is executed on the rendering thread it must be
        // a Qt::DirectConnection
        connect(c, SIGNAL(beforeRendering()), this, SLOT(paint()), Qt::DirectConnection);

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        c->setClearBeforeRendering(false);
    }
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
    qDebug("%f %f %f %f ...", values[0], values[1], values[2], values[3]);

    QColor color(255, 255, 255, 255);

    QMatrix4x4 pmvMatrix;
    pmvMatrix.ortho(QRect(QPoint(0,0), window()->size()));
    qDebug() << "matrix" << pmvMatrix;

    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeArray(vertexLocation, values, 2);
    m_program->setUniformValue(matrixLocation, pmvMatrix);
    m_program->setUniformValue(colorLocation, color);

    glClearColor(0, 0, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_LINE_STRIP, 0, m_model->rowCount());

    m_program->disableAttributeArray(vertexLocation);



//    m_program->setUniformValue("hscale", GL_FLOAT, 1.0 / 5000.0);
//    m_program->setUniformValue("vscale", GL_FLOAT, 1.0 / 40.0);
//    m_program->setAttributeArray(vertexLocation, values, 2);
//    glViewport(0, 0, window()->width(), window()->height());

}

void Chart2D::cleanup()
{
    if (m_program) {
        delete m_program;
        m_program = 0;
    }
}
