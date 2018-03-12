#include "interactorstylewidget.h"
#include "ui_interactorstylewidget.h"
#include "gitlivkcmdevt.h"
InteractorStyleWidget::InteractorStyleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InteractorStyleWidget)
{
    ui->setupUi(this);
}

InteractorStyleWidget::~InteractorStyleWidget()
{
    delete ui;
}

void InteractorStyleWidget::on_moveCamBtn_clicked()
{
    GitlIvkCmdEvt cEvt("change_interactor");
    cEvt.setParameter("interactor_style", "camera_style");
    cEvt.dispatch();
}

void InteractorStyleWidget::on_moveObjBtn_clicked()
{
    GitlIvkCmdEvt cEvt("change_interactor");
    cEvt.setParameter("interactor_style", "actor_style");
    cEvt.dispatch();
}
