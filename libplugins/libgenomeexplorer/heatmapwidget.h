#ifndef HEATMAPWIDGET_H
#define HEATMAPWIDGET_H

#include <QWidget>
#include "gitldef.h"

namespace Ui {
class HeatMapWidget;
}

enum HEATMAP_MODE
{
    SNP_MODE = 0,
    GENE_MODE,
};

class HeatmapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HeatmapWidget(QWidget *parent = 0);
    ~HeatmapWidget();

private:
    Ui::HeatMapWidget *ui;

    ADD_CLASS_FIELD_NOSETTER(HEATMAP_MODE, eHeatmapMode, getHeatmapMode)
public:
    void setHeatmapMode(HEATMAP_MODE eHeatmapMode);

};

#endif // HEATMAPWIDGET_H
