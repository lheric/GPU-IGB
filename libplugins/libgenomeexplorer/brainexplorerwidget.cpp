#include "brainexplorerwidget.h"
#include "ui_brainexplorerwidget.h"

BrainExplorerWidget::BrainExplorerWidget(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::BrainExplorerWidget)
{
    ui->setupUi(this);
}

BrainExplorerWidget::~BrainExplorerWidget()
{
    delete ui;
}
