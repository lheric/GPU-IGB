#include "busydialog.h"
#include "ui_busydialog.h"
#include <QDebug>

BusyDialog::BusyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BusyDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    this->setFixedSize(this->size());
    setModuleName("busy_dialog");


    listenToParams("busy_dialog_visible", [&](GitlUpdateUIEvt &rcEvt) {        
                    setVisible(rcEvt.getParameter("busy_dialog_visible").toBool());
                  });

    listenToParams("busy_dialog_text",   [&](GitlUpdateUIEvt &rcEvt) {
                    QVariant cVariant = rcEvt.getParameter("busy_dialog_text");
                    this->ui->busyDynamicText->setText(cVariant.toString());
                  });
}

BusyDialog::~BusyDialog()
{
    delete ui;
}

void BusyDialog::setHintText(QString& str)
{
    this->ui->busyDynamicText->setText(str);
}

