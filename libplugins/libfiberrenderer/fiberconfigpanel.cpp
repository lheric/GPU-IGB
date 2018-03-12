#include "fiberconfigpanel.h"
#include "ui_fiberconfigpanel.h"
#include "algorithm/commonutils.h"
#include "fiberrenderer.h"
#include "gitlivkcmdevt.h"
#include <QDebug>

FiberConfigPanel::FiberConfigPanel(FiberRenderer* pcFiberRenderer, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FiberConfigPanel)
{
    ui->setupUi(this);
    m_pcFiberRenderer = pcFiberRenderer;
    listenToParams("trk_file", MAKE_CALLBACK(FiberConfigPanel::onUpdateTrkFile));
    listenToParams("roi_list", MAKE_CALLBACK(FiberConfigPanel::onUpdateROIList));
}

FiberConfigPanel::~FiberConfigPanel()
{
    delete ui;
}

void FiberConfigPanel::onUpdateTrkFile(GitlUpdateUIEvt &rcEvt)
{

    ui->filberFileLabel->setText(rcEvt.getParameter("trk_file").toString());
}

void FiberConfigPanel::onUpdateROIList(GitlUpdateUIEvt &rcEvt)
{
    ui->ROIFromCobombox->clear();
    ui->ROIToCobombox->clear();

    char** astrROINameList = (char**)(rcEvt.getParameter("roi_list").value<void*>());
    char* p = NULL;
    int iLabelIdx = 1;
    QString strDisplayROIName;
    ui->ROIFromCobombox->addItem("ANY", QVariant::fromValue<int>(-1));  /// ANY Selection
    ui->ROIToCobombox->addItem("ANY", QVariant::fromValue<int>(-1));  /// ANY Selection
    ui->ROIFromCobombox->addItem("NONE", QVariant::fromValue<int>(0));  /// NONE Selection
    ui->ROIToCobombox->addItem("NONE", QVariant::fromValue<int>(0));  /// NONE Selection
    while( (p=*astrROINameList) != NULL)
    {
        strDisplayROIName = p;
        strDisplayROIName += QString(" #%1").arg(iLabelIdx);
        ui->ROIFromCobombox->addItem(strDisplayROIName, QVariant::fromValue<int>(iLabelIdx));
        ui->ROIToCobombox->addItem(strDisplayROIName, QVariant::fromValue<int>(iLabelIdx));
        astrROINameList++;
        iLabelIdx++;

    }

}


void FiberConfigPanel::on_openFiber_clicked()
{
    QString strFilepath;
    bool bSuccess = CommonUtils::openFileDialog("Open Fiber File",
                                                "All Files (*.*)",
                                                "open_track_path",
                                                strFilepath);

    if(bSuccess == false)
        return;

    GitlIvkCmdEvt cOpenEvt("open_fiber_file");
    cOpenEvt.setParameter("filename", strFilepath);
    cOpenEvt.dispatch(this->getEventBus());

    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch(this->getEventBus());

    // reset camera
    GitlIvkCmdEvt cResetCam("reset_camera");
    cResetCam.dispatch(this->getEventBus());

}

void FiberConfigPanel::on_samplingSlider_valueChanged(int value)
{
    Q_UNUSED(value)
    double dSampleRatio = ui->samplingSlider->value()*1.0/ui->samplingSlider->maximum();
    qDebug() << "sampling" << dSampleRatio*100 << "% fibers";
}

void FiberConfigPanel::on_ROIFromCobombox_activated(const QString &arg1)
{
    Q_UNUSED(arg1)
    int iROILabel = this->ui->ROIFromCobombox->currentData().toInt();
    this->m_pcFiberRenderer->getFiberConnFilter()->SetStartROIIdx(iROILabel);

    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch(this->getEventBus());
}

void FiberConfigPanel::on_ROIToCobombox_activated(const QString &arg1)
{
    Q_UNUSED(arg1)
    int iROILabel = this->ui->ROIToCobombox->currentData().toInt();
    this->m_pcFiberRenderer->getFiberConnFilter()->SetEndROIIdx(iROILabel);

    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch(this->getEventBus());
}
