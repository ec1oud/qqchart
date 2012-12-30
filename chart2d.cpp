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
#if 1
    // Just a triangle
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

//        m_program->bindAttributeLocation("vertices", 0);
        m_program->link();

        m_program->link();
        connect(window()->openglContext(), SIGNAL(aboutToBeDestroyed()),
                this, SLOT(cleanup()), Qt::DirectConnection);
    }
    if (!m_program->bind())
        qDebug() << "bind failed";
    int vertexLocation = m_program->attributeLocation("vertex");
    int matrixLocation = m_program->uniformLocation("matrix");
    int colorLocation = m_program->uniformLocation("color");
    static GLfloat const triangleVertices[] = {
        60.0f,  10.0f,  0.0f,
        110.0f, 110.0f, 0.0f,
        10.0f,  110.0f, 0.0f
    };

    QColor color(0, 255, 0, 255);

    QMatrix4x4 pmvMatrix;
    pmvMatrix.ortho(window()->geometry());

    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeArray(vertexLocation, triangleVertices, 3);
    m_program->setUniformValue(matrixLocation, pmvMatrix);
    m_program->setUniformValue(colorLocation, color);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    m_program->disableAttributeArray(vertexLocation);
#else
    if (!m_program) {
        m_program = new QOpenGLShaderProgram();
        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                           "attribute highp vec2 vertex;\n"
                                           "uniform highp float hscale;\n"
                                           "uniform highp float vscale;\n"
                                           "void main(void)\n"
                                           "{\n"
                                           "   gl_Position = vec4(vertex.x * hscale, vertex.y * vscale, 0, 0);\n"
                                           "}");
        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                           "uniform mediump vec4 color;\n"
                                           "void main(void)\n"
                                           "{\n"
                                           "   gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
                                           "}");
        /*
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
        */

//        m_program->bindAttributeLocation("vertices", 0);
        m_program->link();

        connect(window()->openglContext(), SIGNAL(aboutToBeDestroyed()),
                this, SLOT(cleanup()), Qt::DirectConnection);
    }
    if (!m_program->bind())
        qDebug() << "bind failed";

    int vertexLocation = m_program->attributeLocation("vertex");
    qDebug("vertex location %d", vertexLocation);
//    int hscaleLocation = m_program->uniformLocation("hscale");
//    int vscaleLocation = m_program->uniformLocation("vscale");
//    int matrixLocation = m_program->uniformLocation("matrix");
//    int colorLocation = m_program->uniformLocation("color");

    m_program->enableAttributeArray(vertexLocation);
//    m_program->setUniformValue("t", (float) m_hzoom);

    GLfloat *values = m_model->columnValues(0);
    qDebug("%f %f %f %f ...", values[0], values[1], values[2], values[3]);

//    m_program->setUniformValue("hscale", GL_FLOAT, 1.0 / 5000.0);
//    m_program->setUniformValue("vscale", GL_FLOAT, 1.0 / 40.0);
//    m_program->setAttributeArray(vertexLocation, values, 2);
//    glViewport(0, 0, window()->width(), window()->height());

//    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
//    static GLfloat const triangleVertices[] = {
//        60.0f,  10.0f,  0.0f,
//        110.0f, 110.0f, 0.0f,
//        10.0f,  110.0f, 0.0f
//    };

//    QColor color(0, 255, 0, 255);
//    QMatrix4x4 pmvMatrix;
//    pmvMatrix.ortho(rect());

//    m_program->enableAttributeArray(vertexLocation);
//    m_program->setAttributeArray(vertexLocation, triangleVertices, 3);
//    m_program->setUniformValue(matrixLocation, pmvMatrix);
//    m_program->setUniformValue(colorLocation, color);

//    glDrawArrays(GL_TRIANGLES, 0, 3);

//    m_program->disableAttributeArray(vertexLocation);

//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

//    glDrawArrays(GL_LINE_STRIP, 0, m_model->rowCount() / 2);

    GLfloat triangle[] = {
        -500.0, -500.0,
        0, 500.0,
        500.0, -500.0
    };

    m_program->setUniformValue("hscale", GL_FLOAT, 1.0);
    m_program->setUniformValue("vscale", GL_FLOAT, 1.0);
    m_program->setAttributeArray("vertex", triangle, 2);

    glDrawArrays(GL_LINE_LOOP, 0, 3);

    m_program->disableAttributeArray(vertexLocation);
//    m_program->release();
#endif
}

void Chart2D::cleanup()
{
    if (m_program) {
        delete m_program;
        m_program = 0;
    }
}
