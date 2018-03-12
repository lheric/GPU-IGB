#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "model/rom.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    /// version number
    ui->versionLabel->setText(VERSION);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}


