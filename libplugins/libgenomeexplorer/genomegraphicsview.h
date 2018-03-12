#ifndef GENOMEGRAPHICSVIEW_H
#define GENOMEGRAPHICSVIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QPoint>
#include <QVector>
#include "gitldef.h"
#include "heatmapelementrectitem.h"
#include "calculation_backend/Common/CArray2DTCP.h"
#include "model/parcellationlabels.h"

class GenomeGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GenomeGraphicsView(QWidget * parent=0);
    ~GenomeGraphicsView(){}

protected:
    void wheelEvent(QWheelEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void xRerenderCurrentViewport();

    ADD_CLASS_FIELD(int, iElementGap, getElementGap, setElementGap)
    ADD_CLASS_FIELD(int, iElementWidth, getElementWidth, setElementWidth)
    ADD_CLASS_FIELD(int, iMaxBarHeight, getMaxBarHeight, setMaxBarHeight)
    ADD_CLASS_FIELD(QVector<QString>*, pcSNPNames, getSNPNames, setSNPNames)


};

#endif // GENOMEGRAPHICSVIEW_H
