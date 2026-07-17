#ifndef LINENODE_H
#define LINENODE_H

#include <QSGGeometryNode>
#include <QtQuick/QSGMaterial>
#include <QColor>
#include <Qt>

class LineMaterial : public QSGMaterial
{
public:
    LineMaterial();

    QSGMaterialType *type() const override;

    QSGMaterialShader *createShader(QSGRendererInterface::RenderMode) const override;

    int compare(const QSGMaterial *m) const override;

    // Qt::RoundJoin selects the round-cap/round-join shader; anything else selects the
    // bevel (non-overlapping ribbon) shader. See LineGraph::joinStyle for the trade-offs.
    Qt::PenJoinStyle joinStyle = Qt::RoundJoin;

    struct {
        QColor color;
        QColor warningMinColor;
        QColor warningMaxColor;
        float height = 100;
        float lineWidth = 1;
        float warningMinValue = -1000;
        float warningMaxValue = 1000;
        float fillDirection = 0;
        float aa = 1;
        QMatrix4x4 dataTransform;
    } state;
};

class LineNode : public QSGGeometryNode
{
public:
    LineNode();

    struct LineVertex {
    // should this correspond?
//    layout(location = 0) in vec4 pos;       // x, y, i, t; x and y are time and value, in real-world units
//    layout(location = 1) in vec4 prevNext;  // time and value of the previous datapoint, and the next one
    // and/or ?
//    layout(location = 0) in float vT;
//    layout(location = 1) in vec4 vColor;
        float x;        // time (assuming your x axis is that); first sample has x=0
        float y;        // original sample value
        float i;        // incrementing index
        float t;        // -1 or 1 to inset or outset by half-line-width and achieve antialiasing
        float prevX;    // previous/next values are for the shader to calculate slopes, miter angles etc.
        float prevY;
        float nextX;
        float nextY;
        inline void set(int ii, float tt, float xx, float yy, float px, float py, float nx, float ny) {
            x = xx; y = yy; i = ii; t = tt;
//    qDebug() << "x" << xx << "y" << yy << "i" << ii << "t" << tt;
            prevX = px; prevY = py; nextX = nx; nextY = ny;
        }
    };

    static const int verticesPerSample = 4;

    void updateGeometry(const QRectF &bounds, const QVector<LineVertex> *v, qreal timeScale);
    void setHeight(float height);
    void setLineWidth(float width);
    void setColor(QColor color);
    void setWarningMinColor(QColor color);
    void setWarningMaxColor(QColor color);
    void setMinValue(qreal v);
    void setMaxValue(qreal v);
    void setWarningMinValue(qreal v);
    void setWarningMaxValue(qreal v);
    void setFillDirection(qreal v);
    void setSpread(qreal v);
    void setWireframe(bool v);
    void setJoinStyle(Qt::PenJoinStyle v);

private:
    QSGGeometry m_geometry;
    LineMaterial *m_material;
    qreal m_minValue = 0;
    qreal m_maxValue = 1;
    bool m_wireframe;

    friend class LineShader;
    friend class LineGraph;
};

#endif // LINENODE_H
