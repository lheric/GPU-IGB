#include "genomeexplorermainwin.h"
#include "ui_genomeexplorermainwin.h"

#include "brainexpvolrenderwidget.h"


GenomeExplorerMainwin::GenomeExplorerMainwin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenomeExplorerMainwin)
{
    ui->setupUi(this);
    init();

}

GenomeExplorerMainwin::~GenomeExplorerMainwin()
{
    delete ui;
}

void GenomeExplorerMainwin::init()
{

    ui->genome->layout()->addWidget(&m_cGenomeChart.getView());
    ui->heatmap->layout()->addWidget(&m_cHeatmap.getView());
    this->ui->verticalSplitter->setStretchFactor(0, 3);
    this->ui->verticalSplitter->setStretchFactor(1, 7);

    this->ui->horizontalSplitter->setStretchFactor(0, 5);
    this->ui->horizontalSplitter->setStretchFactor(1, 7);

}

void GenomeExplorerMainwin::setBGData(vtkImageData *pcImageData)
{
    ui->qvtkWidget->setBGImage(pcImageData);
}

