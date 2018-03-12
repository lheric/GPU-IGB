#include "heatmapgraphicsview.h"
#include "genomeexplorermodel.h"
#include <QDebug>
#include <QMouseEvent>
#include <QTextDocument>
#include <QFlags>

HeatmapGraphicsView::HeatmapGraphicsView(QWidget *parent):
    QGraphicsView(parent)
{
    // init member
    this->setElementSize(8);
    this->setElementGap(2);
//    this->setLabelXView(nullptr);
//    this->setLabelYView(nullptr);
    this->setFloatTooltip(new QGraphicsTextItem());
    this->getFloatTooltip()->hide();
    this->getFloatTooltip()->setAcceptHoverEvents(false);
    this->getFloatTooltip()->setZValue(5);
    this->getHeatmapScene().addItem(getFloatTooltip());
    this->setHoverHighlight(new QGraphicsRectItem());
    this->getHoverHighlight()->hide();
    this->getHoverHighlight()->setRect(0, 0, this->getElementSize(), this->getElementSize());
    this->getHoverHighlight()->setZValue(1);
    this->getHeatmapScene().addItem(this->getHoverHighlight());
    //
    this->setScene(&this->getHeatmapScene());    
    this->setBackgroundBrush(Qt::black);
    this->setRenderHints(QPainter::Antialiasing |
                         QPainter::SmoothPixmapTransform);
    this->setAttribute (Qt::WA_AlwaysShowToolTips);

    // Update all the view port when needed, otherwise, the drawInViewPort may experience trouble
//    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // When zooming, the view stay centered over the mouse
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    // hide scrollbars
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    connect(pcModel, &GenomeExplorerModel::onDataUploaded,
            this, &HeatmapGraphicsView::xRerenderCurrentViewport);
}

void HeatmapGraphicsView::onMouseEnterElement(int iDataIdxX, int iDataIdxY,
                                              int iLogicalIdxX, int iLogicalIdxY)
{
    Q_UNUSED(iLogicalIdxX)
    Q_UNUSED(iLogicalIdxY)

    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();

    /// sanity check
    if(pcModel->getDataUploaded() == false)
        return;

    pcModel->setHoveringPoint(QPoint(iDataIdxX, iDataIdxY));
    CArray1D_TCP<int>* pcSNPList = pcModel->getSNPList();
    CArray1D_TCP<int>* pcVoxelList = pcModel->getVoxelList();
    /// display floating tooltip
    HeatmapElementRectItem* pcItem = dynamic_cast<HeatmapElementRectItem*>(sender());
    if(pcItem != nullptr)   /// if mouse enters a heatmap element
    {
        double dScale = this->transform().m11();
        double dValue = pcItem->getValue();
        this->getFloatTooltip()->setHtml(QString("<div style='background-color:rgba(255, 255, 255, 60%);'><font color='black'>&nbsp; Value: %1 &nbsp; <br> &nbsp; SNP #%2&nbsp;<br>&nbsp; Voxel #%3 &nbsp;</font></div>")
                                         .arg(dValue).arg((*pcSNPList)(iDataIdxX)).arg((*pcVoxelList)(iDataIdxY)));
        this->getFloatTooltip()->setPos(pcItem->boundingRect().center());
        this->getFloatTooltip()->setScale(1.5/dScale);
        this->getFloatTooltip()->show();

        this->getHoverHighlight()->setPen(QPen(QColor(255, 255, 255, 200), dScale < 1.0 ? 2.0/dScale : 2.0));
        this->getHoverHighlight()->setBrush(Qt::NoBrush);
        this->getHoverHighlight()->setRect(pcItem->boundingRect());
        this->getHoverHighlight()->show();

    }
}


// Called when a mouse button is pressed
void HeatmapGraphicsView::mousePressEvent(QMouseEvent *event)
{
    // Drag mode : change the cursor's shape
    if (event->button() == Qt::LeftButton)  this->setDragMode(QGraphicsView::ScrollHandDrag);

    QGraphicsView::mousePressEvent(event);
}

void HeatmapGraphicsView::resizeEvent(QResizeEvent *event)
{
    xRerenderCurrentViewport();
    QGraphicsView::resizeEvent(event);
}

void HeatmapGraphicsView::showEvent(QShowEvent *event)
{
    xRerenderCurrentViewport();
    QGraphicsView::showEvent(event);
}

void HeatmapGraphicsView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Home)
    {
        this->scale(1.0/this->transform().m11(), 1.0/this->transform().m11());
        this->repaint();
        xRerenderCurrentViewport();
        xRerenderCurrentViewport();
    }
    else if(event->key() == Qt::Key_End)
    {
        GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
        if(pcModel->getDataUploaded())
        {
            int iHeatmapWidth = pcModel->getNumberOfSNPs()*this->getElementOffset();
            int iHeatmapHeight = pcModel->getNumberOfVoxels()*this->getElementOffset();
            int iWidgetWidth = this->width();
            int iWidgetHeight = this->height();
            double dTargetScale = qMin(iWidgetHeight*1.0/iHeatmapHeight,
                                       iWidgetWidth*1.0/iHeatmapWidth);
            this->resetTransform();
            this->scale(dTargetScale, dTargetScale);
        }
        xRerenderCurrentViewport();
    }
    else if (event->key() == Qt::SHIFT)
    {

    }



    QGraphicsView::keyPressEvent(event);
}

void HeatmapGraphicsView::paintEvent(QPaintEvent *event)
{
//    qDebug() << "Redarw";
    emit onRedraw();
    QGraphicsView::paintEvent(event);
}

void HeatmapGraphicsView::xPrepareFilterList()
{
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    int iHeatmapWidth = 0;
    int iHeatmapHeight = 0;
    if(pcModel->getSNPSubject() != nullptr)
        iHeatmapWidth = pcModel->getNumberOfSNPs()*this->getElementOffset();
    if(pcModel->getVoxelSubject() != nullptr)
        iHeatmapHeight = pcModel->getNumberOfVoxels()*this->getElementOffset();

    QPoint cTopLeftWidget = mapFromScene(0,0);
    cTopLeftWidget.setX(qMax(cTopLeftWidget.x(),0));
    cTopLeftWidget.setY(qMax(cTopLeftWidget.y(),0));
    QPointF cTopLeftScene = mapToScene(cTopLeftWidget);

    QPoint cBottomRightWidget = mapFromScene(iHeatmapWidth, iHeatmapHeight);
    cBottomRightWidget.setX(qMin(cBottomRightWidget.x(), this->width()));
    cBottomRightWidget.setY(qMin(cBottomRightWidget.y(), this->height()));
    QPointF cBottomRightScene = mapToScene(cBottomRightWidget);

    double dCurScale = this->transform().m11();
    int iElementOffset = getElementOffset();

    /// prepare the "window" (voxel & SNP filter list) into the big matrix
    QVector<int> rcVisRows = pcModel->getVisibleRows();     // visible Ys
    QVector<int> rcVisCols = pcModel->getVisibleCols();     // visible Xs
    rcVisRows.clear();
    rcVisCols.clear();

    static int iIdxY = -1, iIdxX = -1;
    if(dCurScale < 1.0)
    {
        QPointF cPoint;
        for(int x = cTopLeftWidget.x(); x < cBottomRightWidget.x(); x+=iElementOffset)
        {
            cPoint = mapToScene(x,0);
            iIdxX = qRound(cPoint.x()/iElementOffset);
            if( xCheckSubscriptX(iIdxX) == false)
                continue;
            rcVisCols.append(iIdxX);

            for(int y = cTopLeftWidget.y(); y < cBottomRightWidget.y(); y+=iElementOffset)
            {
                /// map coordinate to array subscript & check
                cPoint = mapToScene(x,y);
                iIdxY = qRound(cPoint.y()/iElementOffset);
                if( xCheckSubscriptY(iIdxY) == false)
                    continue;
                if(rcVisCols.size() == 1)
                    rcVisRows.append(iIdxY);
                else
                    break;
            }
        }
    }
    else
    {
        for(int x = cTopLeftScene.x(); x < cBottomRightScene.x(); x+=iElementOffset)
        {
            iIdxX = qRound(x*1.0/iElementOffset);
            if( xCheckSubscriptX(iIdxX) == false)
                continue;
            rcVisCols.append(iIdxX);

            for(int y = cTopLeftScene.y(); y < cBottomRightScene.y(); y+=iElementOffset)
            {
                /// check array subscript
                iIdxY = qRound(y*1.0/iElementOffset);

                if( xCheckSubscriptY(iIdxY) == false)
                    continue;
                if(rcVisCols.size() == 1)
                    rcVisRows.append(iIdxY);
                else
                    break;
            }
        }
    }

    pcModel->updateVoxelSNPList(rcVisRows, rcVisCols);

}

void HeatmapGraphicsView::xRerenderCurrentViewport()
{
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    if(pcModel->getDataUploaded() == false)
        return;

    xPrepareFilterList();


    CArray2D_TCP<float>* pcData = pcModel->getVoxelSNP();
    CArray1D_TCP<int>* pcXFilter = pcModel->getSNPList();
    CArray1D_TCP<int>* pcYFilter = pcModel->getVoxelList();

    if(pcModel->getVoxelSNP() == nullptr)
        return;

    int iHeatmapWidth = 0;
    int iHeatmapHeight = 0;
    if(pcModel->getSNPSubject() != nullptr)
        iHeatmapWidth = pcModel->getNumberOfSNPs()*this->getElementOffset();
    if(pcModel->getVoxelSubject() != nullptr)
        iHeatmapHeight = pcModel->getNumberOfVoxels()*this->getElementOffset();

    this->setUpdatesEnabled(false);
    QPoint cTopLeftWidget = mapFromScene(0,0);
    cTopLeftWidget.setX(qMax(cTopLeftWidget.x(),0));
    cTopLeftWidget.setY(qMax(cTopLeftWidget.y(),0));
    QPointF cTopLeftScene = mapToScene(cTopLeftWidget);

    QPoint cBottomRightWidget = mapFromScene(iHeatmapWidth, iHeatmapHeight);
    cBottomRightWidget.setX(qMin(cBottomRightWidget.x(), this->width()));
    cBottomRightWidget.setY(qMin(cBottomRightWidget.y(), this->height()));
    QPointF cBottomRightScene = mapToScene(cBottomRightWidget);

    /// delete all element except floating tooltip and hovering hightlight rect
    /// just remove the tooltip and hovering hightlight rect from scene but not delete it
    if(this->getFloatTooltip()->scene() != nullptr)
        this->scene()->removeItem(this->getFloatTooltip());
    if(this->getHoverHighlight()->scene() != nullptr)
        this->scene()->removeItem(this->getHoverHighlight());
    this->scene()->clear();
//    xClearAllLabel(this->getLabelXView());  /// remove x labels
//    xClearAllLabel(this->getLabelYView());  /// remove y labels
    this->getLeadingEleX().clear();
    this->getLeadingEleY().clear();

    HeatmapElementRectItem *newElement = NULL;

    double dCurScale = this->transform().m11();
    float fValue = 0.0f; float fClippedValue = 0.0f;
    int iElementSize = getElementSize();
    int iElementOffset = getElementOffset();

    /// do ANOVA
    pcModel->doANOVA();

    /// display
    static int iIdxY = -1, iIdxX = -1;

    int iNextIdxX = 0;
    if(dCurScale < 1.0)
    {
        /// zooming out
        QPointF cPoint;
        for(int x = cTopLeftWidget.x(); x < cBottomRightWidget.x(); x+=iElementOffset)
        {
            cPoint = mapToScene(x,0);
            iIdxX = qRound(cPoint.x()/iElementOffset);
            if( xCheckSubscriptX(iIdxX) == false)
                continue;





            int iNextIdxY = 0;
            for(int y = cTopLeftWidget.y(); y < cBottomRightWidget.y(); y+=iElementOffset)
            {
                /// map coordinate to array subscript & check
                cPoint = mapToScene(x,y);
                iIdxY = qRound(cPoint.y()/iElementOffset);
                if( xCheckSubscriptY(iIdxY) == false)
                    continue;



                /// draw heatmap elements
                fValue = (*pcData)(iNextIdxY, iNextIdxX);
                fClippedValue = qBound(0.0f, fValue, 1.0f);

                newElement = new HeatmapElementRectItem();
                connect(newElement, &HeatmapElementRectItem::mouseEnteredElement,
                        this, &HeatmapGraphicsView::onMouseEnterElement);
                newElement->setPolygon(mapToScene(QRect(x, y, iElementSize, iElementSize)));

                newElement->setPen(Qt::NoPen);
                newElement->setBrush(QColor::fromHsvF(0.667-fClippedValue*0.666, 1.0, 1.0));
                newElement->setValue(fValue);
                newElement->setDataIdxX(iNextIdxX);
                newElement->setDataIdxY(iNextIdxY);
                newElement->setLogicalIdxX((*pcXFilter)(iNextIdxX));
                newElement->setLogicalIdxY((*pcYFilter)(iNextIdxY));
                this->scene()->addItem(newElement);


                /// draw X label
                if( iNextIdxY == 0)
                {
//                    QGraphicsTextItem* pcLabelItem = this->scene()->addText(QString("SNP #%1").arg((*pcXFilter)(iNextIdxX)));
//                    pcLabelItem->setDefaultTextColor(Qt::white);
//                    pcLabelItem->setScale(1.0/dCurScale);
//                    pcLabelItem->setPos(mapToScene(x-iElementSize, y-iElementSize/3));
//                    pcLabelItem->setRotation(-90);

//                    xAddLabel(this->getLabelXView(),
//                              QString("SNP #%1").arg((*pcXFilter)(iNextIdxX)),
//                              QPointF(x-iElementSize, this->getLabelXView()->scene()->height()),
//                              -90);
                    this->getLeadingEleX().push_back(newElement);
                }

                /// draw Y label
                if( iNextIdxX == 0)
                {
//                    QGraphicsTextItem* pcLabelItem = this->scene()->addText(QString("Voxel #%1").arg((*pcYFilter)(iNextIdxY)));
//                    pcLabelItem->document()->setDefaultTextOption((QTextOption(Qt::AlignRight)));
//                    pcLabelItem->setDefaultTextColor(Qt::white);
//                    pcLabelItem->setScale(1.0/dCurScale);
//                    pcLabelItem->setPos(mapToScene(x-pcLabelItem->boundingRect().width(), y-iElementSize));

//                    xAddLabel(this->getLabelYView(),
//                              QString("Voxel #%1").arg((*pcYFilter)(iNextIdxY)),
//                              QPointF(this->getLabelYView()->scene()->width()- pcLabelItem->boundingRect().width(), y-iElementSize));
                    this->getLeadingEleY().push_back(newElement);
                }

                ++iNextIdxY;
            }

            ++iNextIdxX;
        }
    }
    else
    {
        /// zooming in
        for(int x = cTopLeftScene.x(); x < cBottomRightScene.x(); x+=iElementOffset)
        {
            iIdxX = qRound(x*1.0/iElementOffset);
            if( xCheckSubscriptX(iIdxX) == false)
                continue;

            int iNextIdxY = 0;
            for(int y = cTopLeftScene.y(); y < cBottomRightScene.y(); y+=iElementOffset)
            {
                /// check array subscript
                iIdxY = qRound(y*1.0/iElementOffset);

                if( xCheckSubscriptY(iIdxY) == false)
                    continue;



                /// draw heatmap elements
                fValue = (*pcData)(iNextIdxY, iNextIdxX);
                fClippedValue = qBound(0.0f, fValue, 1.0f);
                newElement = new HeatmapElementRectItem();
                connect(newElement, &HeatmapElementRectItem::mouseEnteredElement,
                        this, &HeatmapGraphicsView::onMouseEnterElement);
                newElement->setPolygon(QRectF(x, y, iElementSize, iElementSize));
                newElement->setPen(Qt::NoPen);
                newElement->setBrush(QColor::fromHsvF(0.667-fValue*0.666, 1.0, 1.0));
                newElement->setValue(fValue);
                newElement->setDataIdxX(iNextIdxX);
                newElement->setDataIdxY(iNextIdxY);
                newElement->setLogicalIdxX((*pcXFilter)(iNextIdxX));
                newElement->setLogicalIdxY((*pcYFilter)(iNextIdxY));
                this->scene()->addItem(newElement);

                /// draw X label
                if( iNextIdxY == 0)
                {
//                    QGraphicsTextItem* pcLabelItem = this->scene()->addText(QString("SNP #%1").arg((*pcXFilter)(iNextIdxX)));
//                    pcLabelItem->setDefaultTextColor(Qt::white);
//                    pcLabelItem->setPos(x-iElementSize, y-iElementSize/3);
//                    pcLabelItem->setRotation(-90);

//                    xAddLabel(this->getLabelXView(),
//                              QString("SNP #%1").arg((*pcXFilter)(iNextIdxX)),
//                              QPointF(mapFromScene(x, 0).x(), this->getLabelXView()->scene()->height()),
//                              -90);
                    this->getLeadingEleX().push_back(newElement);

                }


                /// draw Y label
                if( iNextIdxX == 0)
                {
//                    QGraphicsTextItem* pcLabelItem = this->scene()->addText(QString("Voxel #%1").arg((*pcYFilter)(iNextIdxY)));
//                    pcLabelItem->document()->setDefaultTextOption((QTextOption(Qt::AlignRight)));
//                    pcLabelItem->setDefaultTextColor(Qt::white);
//                    pcLabelItem->setPos(x-pcLabelItem->boundingRect().width(), y-iElementSize);

//                    xAddLabel(this->getLabelYView(),
//                              QString("Voxel #%1").arg((*pcYFilter)(iNextIdxY)),
//                              QPointF(this->getLabelYView()->scene()->width()- pcLabelItem->boundingRect().width(), mapFromScene(0, y-iElementSize).y()));
                    this->getLeadingEleY().push_back(newElement);
                }

                ++iNextIdxY;
            }

            ++iNextIdxX;
        }
    }
    this->getFloatTooltip()->hide();
    this->scene()->addItem(this->getFloatTooltip());
    this->getHoverHighlight()->hide();
    this->scene()->addItem(this->getHoverHighlight());

    /// extent the scene rect to make it more draggable
    int iMarginLeft = this->scene()->itemsBoundingRect().width()/2;
    int iMarginTop = this->scene()->itemsBoundingRect().height()/2;

    QRectF cNewSceneRect = this->scene()->itemsBoundingRect();
    this->setSceneRect(cNewSceneRect.marginsAdded(QMargins(iMarginLeft,
                                                           iMarginTop,
                                                           iMarginLeft,
                                                           iMarginTop)));


    this->setUpdatesEnabled(true);


}

bool HeatmapGraphicsView::xCheckSubscriptY(int y)
{
    static GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    return y >= 0 && y < pcModel->getNumberOfVoxels();
}

bool HeatmapGraphicsView::xCheckSubscriptX(int x)
{
    static GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    return x >= 0 && x < pcModel->getNumberOfSNPs();
}

//void HeatmapGraphicsView::xAddLabel(QGraphicsView *pcLabelView, const QString &rcLabel, const QPointF &rcPos, const int iRotation, const QColor &rcColor)
//{
//    if(pcLabelView == nullptr)
//        return;
//    QGraphicsTextItem* pcLabelItem = pcLabelView->scene()->addText(rcLabel);
//    pcLabelItem->setDefaultTextColor(rcColor);
//    pcLabelItem->setPos(rcPos);
//    pcLabelItem->setRotation(iRotation);

//}

//void HeatmapGraphicsView::xClearAllLabel(QGraphicsView *pcLabelView)
//{
//    if(pcLabelView == nullptr)
//        return;
//    pcLabelView->scene()->clear();
//}

// Called when a mouse button is pressed
void HeatmapGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    // Exit drag mode : change the cursor's shape
    if (event->button() == Qt::LeftButton) this->setDragMode(QGraphicsView::NoDrag);

    xRerenderCurrentViewport();

    QGraphicsView::mouseReleaseEvent(event);
}


// Call when there is a scroll event (zoom in or zoom out)
void HeatmapGraphicsView::wheelEvent(QWheelEvent *event)
{
    // When zooming, the view stay centered over the mouse
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    double factor = 1.1;

    /// when shift key is pressed. speed up zooming in/out
    QFlags<Qt::KeyboardModifier> cKeyboardModifier(event->modifiers());
    if( cKeyboardModifier.testFlag(Qt::ShiftModifier) == true )
    {
        factor *= 1.8;
    }

    if(event->delta() > 0)
        // Zoom in
        scale(factor, factor);
    else
        // Zooming out
        scale(1.0 / factor, 1.0 / factor);

    qDebug() << "current scale:" << this->transform().m11();

    /// rerender the viewport
    xRerenderCurrentViewport();

    // The event is processed
    event->accept();
}

