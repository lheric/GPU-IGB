#include "labelgraphicsview.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QResizeEvent>

LabelGraphicsView::LabelGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    this->setScene(new QGraphicsScene());
    this->setHeatmapView(nullptr);
    this->setOrientation(Qt::Horizontal);
}

void LabelGraphicsView::drawLabel()
{
    HeatmapGraphicsView* pcHeatmapView = this->getHeatmapView();
    if(pcHeatmapView == nullptr)
        return;

    this->scene()->clear();

    QFont cTitleFont;
    cTitleFont.setBold(true);
    cTitleFont.setPointSize(11);
    if(this->getOrientation() == Qt::Horizontal)
    {
        QGraphicsTextItem* pcHorLabelTitle = this->scene()->addText( this->getTitle() );
        pcHorLabelTitle->setPos((this->scene()->width()-pcHorLabelTitle->boundingRect().width())/2, 0);
        pcHorLabelTitle->setDefaultTextColor(Qt::white);
        pcHorLabelTitle->setFont(cTitleFont);

        foreach(HeatmapElementRectItem* pcLeadEle, pcHeatmapView->getLeadingEleX())
        {
            QPointF cPos = pcHeatmapView->mapFromScene(pcLeadEle->boundingRect().center());
            QGraphicsTextItem* pcLabelItem = this->scene()->addText(QString("#%1").arg(pcLeadEle->getLogicalIdxX()));
            pcLabelItem->setDefaultTextColor(Qt::white);
            pcLabelItem->setPos(cPos.x()-pcLabelItem->boundingRect().height()/2, this->scene()->height());
            pcLabelItem->setRotation(-90);

        }
    }
    else if(this->getOrientation() == Qt::Vertical)
    {
        QGraphicsTextItem* pcVerLabelTitle = this->scene()->addText( this->getTitle() );
        pcVerLabelTitle->setPos(0, (this->scene()->height()+pcVerLabelTitle->boundingRect().width())/2);
        pcVerLabelTitle->setRotation(-90);
        pcVerLabelTitle->setDefaultTextColor(Qt::white);
        pcVerLabelTitle->setFont(cTitleFont);
        foreach(HeatmapElementRectItem* pcLeadEle, pcHeatmapView->getLeadingEleY())
        {

            QPointF cPos = pcHeatmapView->mapFromScene(pcLeadEle->boundingRect().center());
            QGraphicsTextItem* pcLabelItem = this->scene()->addText(QString("#%1").arg(pcLeadEle->getLogicalIdxY()));
            pcLabelItem->setDefaultTextColor(Qt::white);
            pcLabelItem->setPos(this->scene()->width()-pcLabelItem->boundingRect().width(),
                                cPos.y()-pcLabelItem->boundingRect().height()/2);
        }
    }

}

void LabelGraphicsView::resizeEvent(QResizeEvent *event)
{
    this->scene()->setSceneRect(0, 0,
                                event->size().width(),
                                event->size().height());
}

void LabelGraphicsView::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    this->scene()->setSceneRect(0, 0,
                                this->size().width(),
                                this->size().height());
}
