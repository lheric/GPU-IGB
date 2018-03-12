#ifndef HEATMAP_H
#define HEATMAP_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QGraphicsView>
#include "gitldef.h"
#include "heatmapgraphicsview.h"
#include "heatmapwidget.h"

class Heatmap
{

public:
    Heatmap();
    ~Heatmap();

    ADD_CLASS_FIELD_NOSETTER(HeatmapWidget, cView, getView)
};

#endif // HEATMAP_H
