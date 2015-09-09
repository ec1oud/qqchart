#ifndef CHART2D_H
#define CHART2D_H

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QtQuick/QSGSimpleMaterial>
#include "datasequencemodel.h"

class Chart2D : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(qreal horizontalZoom READ horizontalZoom WRITE setHorizontalZoom NOTIFY horizontalZoomChanged)
    Q_PROPERTY(qreal defaultHorizontalZoom READ defaultHorizontalZoom NOTIFY defaultHorizontalZoomChanged)
    Q_PROPERTY(DataSequenceModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(QColor gridColor READ gridColor WRITE setGridColor)
    Q_PROPERTY(qreal labelSpaceUnder READ labelSpaceUnder WRITE setLabelSpaceUnder NOTIFY labelSpaceUnderChanged)

public:
    struct TimeValueShaderParams
    {
        float vscale;
        QVector4D xclip;
        QMatrix4x4 pmvMatrix;
        QColor color;

        int compare(const TimeValueShaderParams *p) const {
            // different matrix should preclude combining in the sg renderer, but in practice only the color does that
            return p->color.rgba() - color.rgba(); // + 100.0 * (p->pmvMatrix.determinant() - pmvMatrix.determinant());
        }
    };

public:
    Chart2D(QQuickItem * parent = 0);

    qreal horizontalZoom() const { return m_hzoom; }
    void setHorizontalZoom(qreal hz);

    qreal defaultHorizontalZoom() const { return m_defaultHzoom; }

    Q_INVOKABLE void fitAll();

    void setModel(DataSequenceModel *m);
    DataSequenceModel *model() { return m_model; }

    void setColor(QColor c) { m_color = c; }
    QColor color() { return m_color; }

    void setGridColor(QColor c) { m_gridColor = c; }
    QColor gridColor() { return m_gridColor; }

    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

    qreal labelSpaceUnder() const { return m_labelSpaceUnder; }

signals:
    void modelChanged();
    void horizontalZoomChanged();
    void defaultHorizontalZoomChanged();
    void labelSpaceUnderChanged();

public slots:
    void cleanup();

//protected:
//    void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry) {
//        qDebug() << "geometryChanged" << newGeometry;
//        update(); QQuickItem::geometryChanged(newGeometry, oldGeometry);
//    }

    void setLabelSpaceUnder(qreal labelSpaceUnder);

protected:
    DataSequenceModel *m_model;
    QOpenGLShaderProgram *m_program;
    QSGSimpleMaterial<TimeValueShaderParams> *m_material;
    QSGSimpleMaterial<TimeValueShaderParams> *m_gridMaterial;
    qreal m_defaultHzoom;
    qreal m_hzoom;
    QColor m_color;
    QColor m_gridColor;
    qreal m_labelSpaceUnder;
};

#endif // CHART2D_H
