#include "affinematrixform.h"
#include "ui_affinematrixform.h"

AffineMatrixForm::AffineMatrixForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AffineMatrixForm)
{
    ui->setupUi(this);
}

AffineMatrixForm::~AffineMatrixForm()
{
    delete ui;
}

void AffineMatrixForm::on_tableWidget_cellChanged(int row, int column)
{

}
