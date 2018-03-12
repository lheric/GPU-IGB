#ifndef HEATMAPELEMENTRECTITEM_H
#define HEATMAPELEMENTRECTITEM_H
#include <QGraphicsPolygonItem>
#include <QObject>
#include "gitldef.h"

class HeatmapElementRectItem : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
public:
    HeatmapElementRectItem(QGraphicsItem *parent = Q_NULLPTR);
    ADD_CLASS_FIELD(double, dValue, getValue, setValue)

    ADD_CLASS_FIELD(int, iDataIdxX, getDataIdxX, setDataIdxX)
    ADD_CLASS_FIELD(int, iDataIdxY, getDataIdxY, setDataIdxY)

    ADD_CLASS_FIELD(int, iLogicalIdxX, getLogicalIdxX, setLogicalIdxX)
    ADD_CLASS_FIELD(int, iLogicalIdxY, getLogicalIdxY, setLogicalIdxY)

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;



signals:
    void mouseEnteredElement(int iDataIdxX, int iDataIdxY,
                             int iLogicalIdxX, int iLogicalIdxY);
    void mouseClickElement(int iDataIdxX, int iDataIdxY,
                           int iLogicalIdxX, int iLogicalIdxY);
};

#endif // HEATMAPELEMENTRECTITEM_H
