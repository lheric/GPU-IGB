#include "heatmapelementrectitem.h"
#include <QDebug>
HeatmapElementRectItem::HeatmapElementRectItem(QGraphicsItem *parent):
    QGraphicsPolygonItem(parent)
{
    this->setValue(-1.0);
    this->setDataIdxX(-1);
    this->setDataIdxY(-1);
    this->setLogicalIdxX(-1);
    this->setLogicalIdxY(-1);

    this->setAcceptHoverEvents(true);
}

void HeatmapElementRectItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    emit mouseEnteredElement(this->getDataIdxX(),   this->getDataIdxY(),
                             this->getLogicalIdxX(),this->getLogicalIdxY());
    QGraphicsPolygonItem::hoverEnterEvent(event);
}

void HeatmapElementRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit mouseClickElement(this->getDataIdxX(),   this->getDataIdxY(),
                           this->getLogicalIdxX(),this->getLogicalIdxY());
    QGraphicsPolygonItem::mousePressEvent(event);
}
