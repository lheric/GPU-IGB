#ifndef THUMBNAILGRAPHICSVIEW_H
#define THUMBNAILGRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include "gitldef.h"
#include "heatmapgraphicsview.h"

class ThumbnailGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ThumbnailGraphicsView(QWidget *parent = 0);
    void drawThumbnail();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

signals:

public slots:

    ADD_CLASS_FIELD_NOSETTER(HeatmapGraphicsView*, pcHeatmapView, getHeatmapView)
public:
    void setHeatmapView(HeatmapGraphicsView* pcHeatmapView)
    {
        m_pcHeatmapView = pcHeatmapView;
        if(pcHeatmapView != nullptr)
        {
            connect(pcHeatmapView, &HeatmapGraphicsView::onRedraw,
                    this, &ThumbnailGraphicsView::drawThumbnail);
        }
    }

};



class ThumbnailRectItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    ThumbnailRectItem(QGraphicsItem *parent = Q_NULLPTR): QGraphicsRectItem(parent) {}

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

};

#endif // THUMBNAILGRAPHICSVIEW_H
