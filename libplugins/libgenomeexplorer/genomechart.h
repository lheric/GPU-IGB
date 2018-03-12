#ifndef GENOMECHART_H
#define GENOMECHART_H

#include "gitldef.h"
#include "genomegraphicsview.h"

class GenomeChart
{
public:
    GenomeChart();

    void init();

    ///
    ADD_CLASS_FIELD_NOSETTER(QGraphicsScene, cScene, getScene)
    ADD_CLASS_FIELD_NOSETTER(GenomeGraphicsView, cView, getView)

};

#endif // GENOMECHART_H
