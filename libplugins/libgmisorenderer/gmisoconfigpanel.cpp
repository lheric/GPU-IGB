#include "gmisoconfigpanel.h"
#include "ui_gmisoconfigpanel.h"
#include "algorithm/commonutils.h"
#include "gitlivkcmdevt.h"
#include "gmisorenderer.h"
#include "colorschemedelegate.h"
#include "model/parcellationlabels.h"
#include "model/rom.h"
#include <QDebug>

GMISOConfigPanel::GMISOConfigPanel(GMISORenderer* pcGMISORenderer, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GMISOConfigPanel)
{
    ui->setupUi(this);
    m_pcGMISORenderer = pcGMISORenderer;

    /// setup the color table model
    this->ui->colorSchemeTableView->setModel(&getColorSchemeModel());
    this->ui->colorSchemeTableView->setItemDelegate(&getColorSchemeDelegate());

    ///
    listenToParams("GM_intensity_file", MAKE_CALLBACK(GMISOConfigPanel::onUpdateGMIntesntiyFile));
    listenToParams("GM_parcellation_file", MAKE_CALLBACK(GMISOConfigPanel::onUpdateGMParcellationFile));
    listenToParams("picked_actor", MAKE_CALLBACK(GMISOConfigPanel::onUpdatePickedLabel));
    listenToParams("ROI_names_file", MAKE_CALLBACK(GMISOConfigPanel::onUpdateLabelNameFile));


}

GMISOConfigPanel::~GMISOConfigPanel()
{
    delete ui;
}


void GMISOConfigPanel::onUpdatePickedLabel(GitlUpdateUIEvt &rcEvt)
{
    vtkActor* pcActor = (vtkActor*)(rcEvt.getParameter("picked_actor").value<void*>());
    bool bHit = rcEvt.getParameter("picker_hit").toBool();
    int iPickLabel = 0;
    ROI* pcSelectedROI = nullptr;
    if(bHit)
    {
        QVector<ROI*> acParcROIs = m_pcGMISORenderer->getParcROIs();
        foreach(ROI* pcParcROI, acParcROIs)
        {
            if(pcParcROI->getActor().GetPointer() == pcActor)
            {
                iPickLabel = pcParcROI->getLabel();
                pcSelectedROI = pcParcROI;
                break;
            }
        }
    }
    m_pcGMISORenderer->setHightlightedROI(pcSelectedROI);
    ui->ROICobombox->setCurrentIndex(iPickLabel);
}

void GMISOConfigPanel::onUpdateGMIntesntiyFile(GitlUpdateUIEvt &rcEvt)
{
    ui->GMISOFileLabel->setText(rcEvt.getParameter("GM_intensity_file").toString());

}

void GMISOConfigPanel::onUpdateGMParcellationFile(GitlUpdateUIEvt &rcEvt)
{
    ui->GMParFileLabel->setText(rcEvt.getParameter("GM_parcellation_file").toString());

}

void GMISOConfigPanel::onUpdateLabelNameFile(GitlUpdateUIEvt &rcEvt)
{
    ui->parcellationLabelName->setText(rcEvt.getParameter("ROI_names_file").toString());
    QMap<int, QString>* pcNameTable = (QMap<int, QString>*)(rcEvt.getParameter("ROI_names").value<void*>());
    QMap<int, QColor>* pcColorTable = (QMap<int, QColor>*)(rcEvt.getParameter("ROI_color_scheme").value<void*>());
    /// populate the ROI list
    ui->ROICobombox->clear();
    QMapIterator<int, QString> i(*pcNameTable);
    while (i.hasNext()) {
        i.next();
        int iLabel = i.key();
        QString iLabelName = i.value();
        ui->ROICobombox->addItem(QString("#%1 ").arg(iLabel) + iLabelName);
    }

    /// update table view
    ColorSchemeTableModel* pcColorModel = &this->getColorSchemeModel();
    pcColorModel->layoutAboutToBeChanged();
    pcColorModel->setLabelNames(pcNameTable);   /// set new labels
    pcColorModel->setColorMap(pcColorTable);    /// set new colors
    pcColorModel->layoutChanged();
}



void GMISOConfigPanel::on_openGMIntensity_clicked()
{
    QString strFilepath;
    bool bSuccess = CommonUtils::openFileDialog("Open Gray Matter Intensity File",
                                                "All Files (*.*)",
                                                "open_graymat_intensity_path",
                                                strFilepath);
    if(bSuccess == false)
        return;

    GitlIvkCmdEvt cOpenEvt("open_gm_image_file");
    cOpenEvt.setParameter("filename", strFilepath);
    cOpenEvt.dispatch(this->getEventBus());

    // reset camera
    GitlIvkCmdEvt cResetCam("reset_camera");
    cResetCam.dispatch(this->getEventBus());

    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch(this->getEventBus());


}

void GMISOConfigPanel::on_openGMParcellation_clicked()
{
    QString strFilepath;
    bool bSuccess = CommonUtils::openFileDialog("Open Gray Matter Parcellation File",
                                                "All Files (*.*)",
                                                "open_graymat_parcellation_path",
                                                strFilepath);
    if(bSuccess == false)
        return;

    GitlIvkCmdEvt cOpenEvt("open_gm_parcel_file");
    cOpenEvt.setParameter("filename", strFilepath);
    cOpenEvt.dispatch(this->getEventBus());

    // reset camera
    GitlIvkCmdEvt cResetCam("reset_camera");
    cResetCam.dispatch(this->getEventBus());

    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch(this->getEventBus());

}

void GMISOConfigPanel::on_isoSurfaceSpinBox_valueChanged(double arg1)
{
    // change iso value
    this->m_pcGMISORenderer->setISO(arg1);


    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch(this->getEventBus());

}

void GMISOConfigPanel::on_gayMatOpacity_valueChanged(double arg1)
{
    this->m_pcGMISORenderer->setIntensityOpacity(arg1);

    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch(this->getEventBus());
}

void GMISOConfigPanel::on_mapPacellation2SurfaceCheckBox_toggled(bool checked)
{
    this->m_pcGMISORenderer->setMapParcellation2Surface(checked);

    // reset camera
    GitlIvkCmdEvt cResetCam("reset_camera");
    cResetCam.dispatch(this->getEventBus());

    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch(this->getEventBus());
}


void GMISOConfigPanel::on_openLabelNamesFile_clicked()
{
    QString strFilepath;
    bool bSuccess = CommonUtils::openFileDialog("Open Label Names File",
                                                "All Files (*.*)",
                                                "open_label_names_path",
                                                strFilepath);
    if(bSuccess == false)
        return;

    GitlIvkCmdEvt cOpenEvt("open_label_names_file");
    cOpenEvt.setParameter("filename", strFilepath);
    cOpenEvt.dispatch(this->getEventBus());

    // reset camera
    GitlIvkCmdEvt cResetCam("reset_camera");
    cResetCam.dispatch(this->getEventBus());

    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch(this->getEventBus());

}

void GMISOConfigPanel::on_GMIntensityGroupBox_toggled(bool checked)
{
    this->m_pcGMISORenderer->setShowIntensity(checked);

    // reset camera
    GitlIvkCmdEvt cResetCam("reset_camera");
    cResetCam.dispatch(this->getEventBus());

    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch(this->getEventBus());
}

void GMISOConfigPanel::on_GMParcellationGroupBox_toggled(bool checked)
{

    this->m_pcGMISORenderer->setShowParcellation(checked);

    // reset camera
    GitlIvkCmdEvt cResetCam("reset_camera");
    cResetCam.dispatch(this->getEventBus());

    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch(this->getEventBus());
}

void GMISOConfigPanel::on_LabelSurfaceOpacity_valueChanged(int value)
{
    // change iso value
    this->m_pcGMISORenderer->setParcellationOpacity(value/100.0);


    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch(this->getEventBus());
}

void GMISOConfigPanel::on_ROICobombox_activated(int index)
{
    QVector<ROI*> acParcROIs = m_pcGMISORenderer->getParcROIs();

    if( index == 0 )
    {
        m_pcGMISORenderer->setHightlightedROI(nullptr);
    }
    else if( (index-1) < acParcROIs.size() )
    {
        m_pcGMISORenderer->setHightlightedROI(acParcROIs[index-1]);
    }

    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch(this->getEventBus());
}
