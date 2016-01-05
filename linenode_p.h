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

    void updateGeometry(const QRectF &bounds, const QVector<LineVertex> *v, qreal timeScale);
    void setLineWidth(float width);
    void setColor(QColor color);
    void setWarningMinColor(QColor color);
    void setWarningMaxColor(QColor color);
    void setMinValue(qreal v);
    void setMaxValue(qreal v);
    void setWarningMinValue(qreal v);
    void setWarningMaxValue(qreal v);
    void setSpread(qreal v);
    void setWireframe(bool v);

    struct LineMaterial
    {
        QColor color;
        QColor warningMinColor;
        QColor warningMaxColor;
        float lineWidth;
        float warningMinValue;
        float warningMaxValue;
        float aa;
        QMatrix4x4 dataTransform;
    };

private:
    QSGGeometry m_geometry;
    QSGSimpleMaterial<LineMaterial> *m_material;
    qreal m_minValue = 0;
    qreal m_maxValue = 1;
    bool m_wireframe;

    friend class LineShader;
    friend class LineGraph;
};

#endif // LINENODE_H
