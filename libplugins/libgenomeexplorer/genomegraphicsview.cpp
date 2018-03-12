#include "genomegraphicsview.h"
#include <limits>
#include <QDebug>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include "genomeexplorer.h"
#include "genomeexplorermodel.h"
#include "model/subject.h"


GenomeGraphicsView::GenomeGraphicsView(QWidget *parent):
    QGraphicsView(parent)
{
    // init member
    this->m_pcSNPNames = nullptr;
    this->setElementWidth(6);
    this->setElementGap(1);
    this->setMaxBarHeight(60);

    //
    this->setBackgroundBrush(Qt::gray);
    this->setRenderHints(QPainter::Antialiasing |
                         QPainter::SmoothPixmapTransform);

    // Update all the view port when needed, otherwise, the drawInViewPort may experience trouble
//    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // When zooming, the view stay centered over the mouse
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    //
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    connect(pcModel, &GenomeExplorerModel::onHoveringPointChanged,
            this, &GenomeGraphicsView::xRerenderCurrentViewport);


}


// Called when a mouse button is pressed
void GenomeGraphicsView::mousePressEvent(QMouseEvent *event)
{
    // Drag mode : change the cursor's shape
    if (event->button() == Qt::LeftButton)  this->setDragMode(QGraphicsView::ScrollHandDrag);

    xRerenderCurrentViewport();

    QGraphicsView::mousePressEvent(event);
}

void GenomeGraphicsView::xRerenderCurrentViewport()
{
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    CArray2D_TCP<float>* pcData = pcModel->getVoxelSNP();
    CArray1D_TCP<int>* pcSNPList = pcModel->getSNPList();
    QPoint cIdx = pcModel->getHoveringPoint();

    if(pcData == nullptr || pcSNPList == nullptr)
        return;



    this->setUpdatesEnabled(false);
    int iWidth = pcData->GetWidth();
    int iRow = cIdx.y();

    /// max & min
    float fMax = -9999;
    float fMin = +9999;
//    for(int i = 0; i < iWidth; ++i)
//    {
//        fMax = qMax(fMax,-log10((*pcData)(iRow, i)));
//        fMin = qMin(fMin,-log10((*pcData)(iRow, i)));
//    }
    fMax = 21.0;
    fMin = 0.0;

    /// draw
    this->scene()->clear();
    QFont cFont;
    cFont.setPointSize(5);
    int iSpace = getElementWidth()+getElementGap();
    for(int i = 0; i < iWidth; ++i)
    {
        float fValue = (*pcData)(iRow, i);
        QGraphicsEllipseItem* pRectItem = new QGraphicsEllipseItem();
        pRectItem->setPen(Qt::NoPen);
        pRectItem->setBrush(QColor::fromHsvF(0.667-qBound(0.0f, fValue, 1.0f)*0.666, 1.0, 1.0));
        double dHeight = (-(log10(fValue))/fMax)*getMaxBarHeight();
        pRectItem->setRect(i*iSpace, -dHeight, 2, 2);
        this->scene()->addItem(pRectItem);

        int iLabel = (*pcSNPList)(i);
        QString strLabel = QString("#%1 ").arg(iLabel);

        QGraphicsTextItem* pTextItem = new QGraphicsTextItem(strLabel);
        pTextItem->setFont(cFont);
        pTextItem->setDefaultTextColor(Qt::white);

        pTextItem->setRotation(90);
        pTextItem->setPos(i*iSpace+8, 0);
        this->scene()->addItem(pTextItem);
    }


    int iAxisOffsetX = 3;
    int iAxisOffsetY = 3;
    /// draw x axis
    this->scene()->addLine(-iAxisOffsetX,iAxisOffsetY,
                           iSpace*iWidth,iAxisOffsetY,
                           QPen(Qt::white));


    /// draw y axis
    this->scene()->addLine(-iAxisOffsetX,iAxisOffsetY,
                           -iAxisOffsetX,-this->getMaxBarHeight(),
                           QPen(Qt::white));
    for(int i = 0; i < fMax; i+=5)
    {
        this->scene()->addLine(-iAxisOffsetX-3, -i*getMaxBarHeight()/fMax,
                -iAxisOffsetX, -i*getMaxBarHeight()/fMax, QPen(Qt::white));
        QGraphicsTextItem* pTickerItem = this->scene()->addText(QString("%1").arg(i), cFont);
        pTickerItem->setDefaultTextColor(Qt::white);
        pTickerItem->setPos(-iAxisOffsetX-pTickerItem->boundingRect().width(),
                            -i*getMaxBarHeight()/fMax-8);
    }
    QGraphicsTextItem* pTickerItem = this->scene()->addText(QString("-log10(P)"), cFont);
    pTickerItem->setDefaultTextColor(Qt::white);
    pTickerItem->setPos(-iAxisOffsetX-pTickerItem->boundingRect().width(),
                        -getMaxBarHeight()-18);



    /// extent the scene rect to make it more draggable
    int iMarginLeft = this->scene()->width()/2;
    this->setSceneRect(this->scene()->itemsBoundingRect()
                       .marginsAdded(QMargins(iMarginLeft,
                                              20,
                                              iMarginLeft,
                                              20)));



    this->setUpdatesEnabled(true);
}

// Called when a mouse button is pressed
void GenomeGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    // Exit drag mode : change the cursor's shape
    if (event->button() == Qt::LeftButton) this->setDragMode(QGraphicsView::NoDrag);

    xRerenderCurrentViewport();

    QGraphicsView::mouseReleaseEvent(event);
}


// Call when there is a scroll event (zoom in or zoom out)
void GenomeGraphicsView::wheelEvent(QWheelEvent *event)
{
    // When zooming, the view stay centered over the mouse
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    double factor = 1.1;
    if(event->delta() > 0)
        // Zoom in
        scale(factor, factor);
    else
        // Zooming out
        scale(1.0 / factor, 1.0 / factor);



    qDebug() << "Current Scale:" << this->transform().m11();

    xRerenderCurrentViewport();


    // The event is processed
    event->accept();
}

