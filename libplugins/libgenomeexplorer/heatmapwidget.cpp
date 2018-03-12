#include "heatmapwidget.h"
#include "ui_heatmapwidget.h"



HeatmapWidget::HeatmapWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HeatMapWidget)
{
    ui->setupUi(this);
    this->setHeatmapMode(SNP_MODE);
    this->ui->xLabelView->setOrientation(Qt::Horizontal);
    this->ui->xLabelView->setHeatmapView(this->ui->heatmapView);
    this->ui->yLabelView->setOrientation(Qt::Vertical);
    this->ui->yLabelView->setHeatmapView(this->ui->heatmapView);
    this->ui->thumbnailView->setHeatmapView(this->ui->heatmapView);
}

HeatmapWidget::~HeatmapWidget()
{
    delete ui;
}

void HeatmapWidget::setHeatmapMode(HEATMAP_MODE eHeatmapMode)
{
    this->m_eHeatmapMode = eHeatmapMode;
    if( eHeatmapMode == GENE_MODE)
        this->ui->xLabelView->setTitle("Gene ID");
    else
        this->ui->xLabelView->setTitle("SNP ID");
    this->ui->yLabelView->setTitle("Voxel ID");
}
