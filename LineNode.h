#ifndef LINENODE_H
#define LINENODE_H

#include <QSGGeometryNode>
#include <QtQuick/QSGSimpleMaterial>
#include <QColor>

class LineNode : public QSGGeometryNode
{
public:
    LineNode();

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

    static const int verticesPerSample = 4;

    static void appendVertices(QVector<LineNode::LineVertex> &v, float time, float value);

    void updateGeometry(const QRectF &bounds, const QVector<qreal> &samples);
    void updateGeometry(const QRectF &bounds, const QVector<LineNode::LineVertex> &v);
    void setLineWidth(float width);
    void setColor(QColor color);
    void setAlertMinColor(QColor color);
    void setAlertMaxColor(QColor color);
    void setMinValue(qreal v);
    void setMaxValue(qreal v);
    void setAlertMinValue(qreal v);
    void setAlertMaxValue(qreal v);
    void setSpread(qreal v);
    void setWireframe(bool v);

    struct LineMaterial
    {
        QColor color;
        QColor alertMinColor;
        QColor alertMaxColor;
        float lineWidth;
        float alertMinValue;
        float alertMaxValue;
        float aa;
        QMatrix4x4 dataTransform;
    };

private:
    QSGGeometry m_geometry;
    QSGSimpleMaterial<LineMaterial> *m_material;
    qreal m_minValue;
    qreal m_maxValue;
    bool m_wireframe;

    friend class LineShader;
    friend class LineGraph;
};

#endif // LINENODE_H
