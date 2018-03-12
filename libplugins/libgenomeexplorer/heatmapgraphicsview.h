#ifndef HEATMAPGRAPHICSVIEW_H
#define HEATMAPGRAPHICSVIEW_H

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

class HeatmapGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    HeatmapGraphicsView(QWidget * parent=0);
    ~HeatmapGraphicsView(){}

signals:
    void onRedraw();

protected slots:
    void onMouseEnterElement(int iDataIdxX, int iDataIdxY,
                             int iLogicalIdxX, int iLogicalIdxY);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event);

    void xPrepareFilterList();
    void xRerenderCurrentViewport();
    bool xCheckSubscriptY(int y);
    bool xCheckSubscriptX(int x);
//    void xAddLabel(QGraphicsView* pcLabelView,
//                   const QString& rcLabel,
//                   const QPointF& rcPos,
//                   const int iRotation = 0,
//                   const QColor& rcColor = Qt::white);
//    void xClearAllLabel(QGraphicsView* pcLabelView);


    ADD_CLASS_FIELD_NOSETTER(QGraphicsScene, cHeatmapScene, getHeatmapScene)
    ADD_CLASS_FIELD(int, iElementGap, getElementGap, setElementGap)
    ADD_CLASS_FIELD(int, iElementSize, getElementSize, setElementSize)
public:
    int getElementOffset()
    {
        return getElementGap()+getElementSize();
    }

    ADD_CLASS_FIELD(QGraphicsTextItem*, pcFloatTooltip, getFloatTooltip, setFloatTooltip)
    ADD_CLASS_FIELD(QGraphicsRectItem*, pcHoverHighlight, getHoverHighlight, setHoverHighlight)
//    ADD_CLASS_FIELD(QGraphicsView*, pcLabelXView, getLabelXView, setLabelXView)
//    ADD_CLASS_FIELD(QGraphicsView*, pcLabelYView, getLabelYView, setLabelYView)

    ADD_CLASS_FIELD_NOSETTER(QVector<HeatmapElementRectItem*>, acLeadingEleX, getLeadingEleX)
    ADD_CLASS_FIELD_NOSETTER(QVector<HeatmapElementRectItem*>, acLeadingEleY, getLeadingEleY)
};

#endif // HEATMAPGRAPHICSVIEW_H
