#include "thumbnailgraphicsview.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QGraphicsSceneMouseEvent>
#include "genomeexplorermodel.h"

ThumbnailGraphicsView::ThumbnailGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    this->setScene(new QGraphicsScene());
    this->setHeatmapView(nullptr);
}

void ThumbnailGraphicsView::drawThumbnail()
{
    HeatmapGraphicsView* pcHeatmapView = this->getHeatmapView();
    if(pcHeatmapView == nullptr)
        return;



    this->scene()->clear();
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    if(pcModel->getDataUploaded() == false)
        return;

    QRectF cHeatmapSceneRect = QRectF(0.0, 0.0,
                                      pcHeatmapView->getElementOffset()*pcModel->getNumberOfSNPs(),
                                      pcHeatmapView->getElementOffset()*pcModel->getNumberOfVoxels());
    QRectF cViewRect = pcHeatmapView->mapToScene(pcHeatmapView->rect()).boundingRect();

    double dScale = qMin( this->width()*1.0/cHeatmapSceneRect.width(),
                          this->height()*1.0/cHeatmapSceneRect.height());

    cHeatmapSceneRect.setX(cHeatmapSceneRect.x()*dScale);
    cHeatmapSceneRect.setY(cHeatmapSceneRect.y()*dScale);
    cHeatmapSceneRect.setWidth(cHeatmapSceneRect.width()*dScale);
    cHeatmapSceneRect.setHeight(cHeatmapSceneRect.height()*dScale);

    cViewRect.setX(cViewRect.x()*dScale);
    cViewRect.setY(cViewRect.y()*dScale);
    cViewRect.setWidth(cViewRect.width()*dScale);
    cViewRect.setHeight(cViewRect.height()*dScale);

    /// center the rects
    double dXOffset = (this->width()  - cHeatmapSceneRect.width())/2.0;
    double dYOffset = (this->height() - cHeatmapSceneRect.height())/2.0;
    cHeatmapSceneRect.translate(dXOffset, dYOffset);
    cViewRect.translate(dXOffset, dYOffset);

    /// clip by heatmap rect
    cViewRect = cViewRect.intersected(cHeatmapSceneRect);
    cViewRect.setWidth(qMax(2.0,cViewRect.width()));
    cViewRect.setHeight(qMax(2.0,cViewRect.height()));

    QPen cHeatmapPen;
    cHeatmapPen.setColor(Qt::white);
    cHeatmapPen.setWidth(2);

    QPen cViewPen;
    cViewPen.setColor(Qt::red);
    cViewPen.setWidth(3);

    ThumbnailRectItem* pcHeatmapRectItem = new ThumbnailRectItem();
    pcHeatmapRectItem->setRect(cHeatmapSceneRect);
    pcHeatmapRectItem->setPen(cHeatmapPen);
    this->scene()->addItem(pcHeatmapRectItem);
    this->scene()->addRect(cViewRect, cViewPen);
    this->centerOn(cHeatmapSceneRect.center());
}

void ThumbnailGraphicsView::resizeEvent(QResizeEvent *event)
{
    this->scene()->setSceneRect(0, 0,
                                event->size().width(),
                                event->size().height());
}

void ThumbnailGraphicsView::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    this->scene()->setSceneRect(0, 0,
                                this->size().width(),
                                this->size().height());
}

void ThumbnailRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();

    qDebug() << event->pos();
}
