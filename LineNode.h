#ifndef LINENODE_H
#define LINENODE_H

#include <QSGGeometryNode>
#include <QtQuick/QSGSimpleMaterial>
#include <QColor>

class LineNode : public QSGGeometryNode
{
public:
    LineNode();

    void updateGeometry(const QRectF &bounds, const QList<qreal> &samples);
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
        float spread;
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
