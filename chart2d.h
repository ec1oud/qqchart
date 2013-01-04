#ifndef CHART2D_H
#define CHART2D_H

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include "datasequencemodel.h"

class Chart2D : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(qreal horizontalZoom READ horizontalZoom WRITE setHorizontalZoom NOTIFY horizontalZoomChanged)
    Q_PROPERTY(DataSequenceModel* model READ model WRITE setModel)

public:
    Chart2D();

    qreal horizontalZoom() const { return m_hzoom; }
    void setHorizontalZoom(qreal hz);

    void setModel(DataSequenceModel *m) { m_model = m; }
    DataSequenceModel *model() { return m_model; }

    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

signals:
    void horizontalZoomChanged();

public slots:
    void paint();
    void cleanup();

protected:
    DataSequenceModel *m_model;
    QOpenGLShaderProgram *m_program;
    qreal m_hzoom;
};

#endif // CHART2D_H
