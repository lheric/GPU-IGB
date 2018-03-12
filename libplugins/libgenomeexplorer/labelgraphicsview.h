#ifndef LABELGRAPHICSVIEW_H
#define LABELGRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include "gitldef.h"
#include "heatmapgraphicsview.h"

class LabelGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit LabelGraphicsView(QWidget *parent = 0);
    void drawLabel();

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
                    this, &LabelGraphicsView::drawLabel);
        }
    }

    ADD_CLASS_FIELD(int, iOrientation, getOrientation, setOrientation)
    ADD_CLASS_FIELD(QString, strTitle, getTitle, setTitle)

};

#endif // LABELGRAPHICSVIEW_H
