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
    Q_PROPERTY(DataSequenceModel* model READ model WRITE setModel)
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    struct TimeValueShaderParams
    {
        float vscale;
        QVector4D xclip;
        QMatrix4x4 pmvMatrix;
        QColor color;

        int compare(const TimeValueShaderParams *p) const {
            return p->vscale - vscale;
        }
    };

public:
    Chart2D(QQuickItem * parent = 0);

    qreal horizontalZoom() const { return m_hzoom; }
    void setHorizontalZoom(qreal hz);

    Q_INVOKABLE void fitAll();

    void setModel(DataSequenceModel *m) { m_model = m; }
    DataSequenceModel *model() { return m_model; }

    void setColor(QColor c) { m_color = c; }
    QColor color() { return m_color; }

    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

signals:
    void horizontalZoomChanged();

public slots:
    void cleanup();

//protected:
//    void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry) {
//        qDebug() << "geometryChanged" << newGeometry;
//        update(); QQuickItem::geometryChanged(newGeometry, oldGeometry);
//    }

protected:
    DataSequenceModel *m_model;
    QOpenGLShaderProgram *m_program;
    QSGSimpleMaterial<TimeValueShaderParams> *m_material;
    qreal m_hzoom;
    QColor m_color;
};

#endif // CHART2D_H
