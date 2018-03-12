#include "controlpanel.h"
#include "ui_controlpanel.h"
#include "gitlivkcmdevt.h"
#include <QDebug>
#include <QColorDialog>
#include <QPalette>
#include <math.h>
ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel)
{
    ui->setupUi(this);

    listenToParams("roi_list", MAKE_CALLBACK(ControlPanel::onUpdateROIList));
    listenToParams("trk_file", MAKE_CALLBACK(ControlPanel::onUpdateTrkFile));
    listenToParams("trk_connection_file", MAKE_CALLBACK(ControlPanel::onUpdateTrkConnFile));
    listenToParams("trk_icc_file", MAKE_CALLBACK(ControlPanel::onUpdateTrkICCFile));
    listenToParams("trk_var_file", MAKE_CALLBACK(ControlPanel::onUpdateTrkVarFile));
    listenToParams("trk_count_file", MAKE_CALLBACK(ControlPanel::onUpdateTrkCountFile));
    listenToParams("GM_intensity_file", MAKE_CALLBACK(ControlPanel::onUpdateISOFile));
    listenToParams("GM_parcellation_file", MAKE_CALLBACK(ControlPanel::onUpdateParcellationFile));
    listenToParams("time_series_file", MAKE_CALLBACK(ControlPanel::onUpdateTimeSeriesFile));
    listenToParams("GM_network_file", MAKE_CALLBACK(ControlPanel::onUpdateNetworkFile));
    listenToParams("MD_network_file", MAKE_CALLBACK(ControlPanel::onUpdateMDNetworkFile));
    listenToParams("muti_disease_file", MAKE_CALLBACK(ControlPanel::onUpdateMultiDiseaseFile));
    listenToParams("picked_label", MAKE_CALLBACK(ControlPanel::onUpdatePickedLabel));
    listenToParams("time_point_num",MAKE_CALLBACK(ControlPanel::onUpdateTimePointNum));
    listenToParams(QStringList() << "trk_icc_min" << "trk_icc_max", MAKE_CALLBACK(ControlPanel::onUpdateICCSliderRange));
    listenToParams(QStringList() << "trk_var_min" << "trk_var_max", MAKE_CALLBACK(ControlPanel::onUpdateVarSliderRange));
    listenToParams(QStringList() << "trk_count_min" << "trk_count_max", MAKE_CALLBACK(ControlPanel::onUpdateCountSliderRange));
}

double ControlPanel::getSampleRatio()
{
    double dSampleRatio = this->ui->sampleRate->value()/100.0-0.0001;
    return dSampleRatio;
}

ControlPanel::~ControlPanel()
{
    delete ui;
}

void ControlPanel::onUpdateROIList(GitlUpdateUIEvt &rcEvt)
{
    ui->ROICobombox->clear();
    char** astrROINameList = (char**)(rcEvt.getParameter("roi_list").value<void*>());
    char* p = NULL;
    int iLabelIdx = 1;
    QString strDisplayROIName;
    ui->ROICobombox->addItem("NONE", QVariant::fromValue<int>(0));  /// NONE Selected
    while( (p=*astrROINameList) != NULL)
    {
        strDisplayROIName = p;
        strDisplayROIName += QString(" #%1").arg(iLabelIdx);
        ui->ROICobombox->addItem(strDisplayROIName, QVariant::fromValue<int>(iLabelIdx));
        astrROINameList++;
        iLabelIdx++;

    }

}

void ControlPanel::onUpdateTrkFile(GitlUpdateUIEvt &rcEvt)
{
    ui->filberFileLabel->setText(rcEvt.getParameter("trk_file").toString());
}

void ControlPanel::onUpdateTrkConnFile(GitlUpdateUIEvt &rcEvt)
{
    ui->fiberConnLabel->setText(rcEvt.getParameter("trk_connection_file").toString());
}

void ControlPanel::onUpdateTrkICCFile(GitlUpdateUIEvt &rcEvt)
{
    ui->fiberICCLabel->setText(rcEvt.getParameter("trk_icc_file").toString());
}

void ControlPanel::onUpdateTrkVarFile(GitlUpdateUIEvt &rcEvt)
{
    ui->fiberVarLabel->setText(rcEvt.getParameter("trk_var_file").toString());
}

void ControlPanel::onUpdateTrkCountFile(GitlUpdateUIEvt &rcEvt)
{
    ui->fiberCountLabel->setText(rcEvt.getParameter("trk_count_file").toString());
}

void ControlPanel::onUpdateISOFile(GitlUpdateUIEvt &rcEvt)
{
    ui->GMISOFileLabel->setText(rcEvt.getParameter("GM_intensity_file").toString());

}

void ControlPanel::onUpdateParcellationFile(GitlUpdateUIEvt &rcEvt)
{
    ui->GMParFileLabel->setText(rcEvt.getParameter("GM_parcellation_file").toString());

}

void ControlPanel::onUpdateTimeSeriesFile(GitlUpdateUIEvt &rcEvt)
{
    ui->timeSeriesFileLabel->setText(rcEvt.getParameter("time_series_file").toString());
}

void ControlPanel::onUpdateNetworkFile(GitlUpdateUIEvt &rcEvt)
{
    ui->FCNetworkLabel->setText(rcEvt.getParameter("GM_network_file").toString());
}

void ControlPanel::onUpdateMDNetworkFile(GitlUpdateUIEvt &rcEvt)
{
    ui->MDConnectivityLabel->setText(rcEvt.getParameter("MD_network_file").toString());
}

void ControlPanel::onUpdateMultiDiseaseFile(GitlUpdateUIEvt &rcEvt)
{
    ui->multiDiseaseLabel->setText(rcEvt.getParameter("muti_disease_file").toString());
}



void ControlPanel::onUpdatePickedLabel(GitlUpdateUIEvt &rcEvt)
{
    int iPickedLabel = rcEvt.getParameter("picked_label").toInt();
    ui->ROICobombox->setCurrentIndex(iPickedLabel);
}

void ControlPanel::onUpdateTimePointNum(GitlUpdateUIEvt &rcEvt)
{
    int iTimePointNum = rcEvt.getParameter("time_point_num").toInt();
    ui->timeDataPointSlider->setMinimum(0);
    ui->timeDataPointSlider->setMaximum(iTimePointNum-1);
}

void ControlPanel::onUpdateICCSliderRange(GitlUpdateUIEvt &rcEvt)
{
}

void ControlPanel::onUpdateVarSliderRange(GitlUpdateUIEvt &rcEvt)
{
    double dMin = rcEvt.getParameter("trk_var_min").toDouble();
    double dMax = rcEvt.getParameter("trk_var_max").toDouble();
    qDebug() << "Variance Min & Max:" << dMin << "-" << dMax;
    this->ui->VarianceSlider->blockSignals(true);
    this->ui->VarianceSlider->setMinimum(floor(dMin*100));
    this->ui->VarianceSlider->setMaximum(ceil(dMax*100));
    //this->ui->VarianceSlider->setValue(round((dMin+dMax)*100/2));
    this->ui->VarianceSlider->blockSignals(false);
    this->ui->varRangeLabel->setText(QString().sprintf("%.2f - %.2f", dMin, dMax));
}

void ControlPanel::onUpdateCountSliderRange(GitlUpdateUIEvt &rcEvt)
{
    int iMin = rcEvt.getParameter("trk_count_min").toInt();
    int iMax = rcEvt.getParameter("trk_count_max").toInt();
    qDebug() << "Count Min & Max:" << iMin << "-" << iMax;
    this->ui->countSlider->blockSignals(true);
    this->ui->countSlider->setMinimum(iMin);
    this->ui->countSlider->setMaximum(iMax);
    //this->ui->countSlider->setValue((iMin+iMax)/2);
    this->ui->countSlider->blockSignals(false);
    this->ui->countRangeLabel->setText(QString().sprintf("%d - %d", iMin, iMax));
}


void ControlPanel::on_startEndTh_valueChanged(double arg1)
{
    GitlIvkCmdEvt cEvt("change_start_end_th");
    cEvt.setParameter("threshold", arg1);
    cEvt.dispatch();
}

void ControlPanel::on_sampleRate_valueChanged(double arg1)
{
    double dSampleRatio = getSampleRatio();
    GitlIvkCmdEvt cEvt("simple_random_sampling");
    cEvt.setParameter("sample_ratio", dSampleRatio);
    cEvt.dispatch();
}

void ControlPanel::on_shortTrkTh_valueChanged(double arg1)
{
    GitlIvkCmdEvt cEvt("change_short_trk_th");
    cEvt.setParameter("threshold", arg1);
    cEvt.dispatch();
}


void ControlPanel::on_gayMatOpacity_valueChanged(double arg1)
{
    GitlIvkCmdEvt cEvt("change_gray_mat_opacity");
    cEvt.setParameter("opacity", arg1);
    cEvt.dispatch();
}


void ControlPanel::on_isoSurfaceSpinBox_valueChanged(double arg1)
{
    GitlIvkCmdEvt cEvt("change_gray_mat_surf_iso");
    cEvt.setParameter("iso", arg1);
    cEvt.dispatch();
}

void ControlPanel::on_openFiber_clicked()
{
    emit open_trk_file_clicked();
}

void ControlPanel::on_openGMISO_clicked()
{
    emit open_nifti_for_surface_clicked();
}

void ControlPanel::on_openGMParcellation_clicked()
{
    emit open_nifti_for_label_clicked();
}


void ControlPanel::on_ROICobombox_activated(const QString &arg1)
{
    int iROILabel = this->ui->ROICobombox->currentData().toInt();
    GitlIvkCmdEvt cEvt("highlight_roi_by_label");
    cEvt.setParameter("roi_label", iROILabel);
    cEvt.dispatch();
}

void ControlPanel::on_timeDataPointSlider_valueChanged(int value)
{
    GitlIvkCmdEvt cEvt("change_time_point");
    cEvt.setParameter("time_point", value);
    cEvt.dispatch();
}

void ControlPanel::on_openTimeSeriesData_clicked()
{
    emit open_time_series_file_clicked();
}

void ControlPanel::on_openFiberConnBtn_clicked()
{
    emit open_trk_connection_file_clicked();
}

void ControlPanel::on_openFiberICCBtn_clicked()
{
    emit open_trk_icc_file_clicked();
}

void ControlPanel::on_openFiberVarBtn_clicked()
{
    emit open_trk_var_file_clicked();
}

void ControlPanel::on_openFiberCountBtn_clicked()
{
    emit open_trk_count_file_clicked();
}


void ControlPanel::on_enableMaskGroupbox_toggled(bool arg1)
{
    GitlIvkCmdEvt cEvt("switch_icc_var_analysis");
    cEvt.setParameter("enabled", arg1);
    cEvt.dispatch();
}


void ControlPanel::on_ICCSlider_valueChanged(int value)
{
    GitlIvkCmdEvt cEvt("change_icc_var_th");
    cEvt.setParameter("icc_threshold", value/100.0);
    cEvt.dispatch();
    this->ui->curICCLabel->setText(QString().sprintf("%.2f",value/100.0));
}

void ControlPanel::on_VarianceSlider_valueChanged(int value)
{
    GitlIvkCmdEvt cEvt("change_icc_var_th");
    cEvt.setParameter("var_threshold", value/100.0);
    cEvt.dispatch();
    this->ui->curVarLabel->setText(QString().sprintf("%.2f",value/100.0));
}

void ControlPanel::on_countSlider_valueChanged(int value)
{
    GitlIvkCmdEvt cEvt("change_icc_var_th");
    cEvt.setParameter("count_threshold", value);
    cEvt.dispatch();
    this->ui->curCountLabel->setText(QString().sprintf("%d",value));
}

void ControlPanel::on_reproducibleCheckbox_toggled(bool checked)
{
    GitlIvkCmdEvt cEvt("switch_icc_visible_class");
    cEvt.setParameter("show_reproducible_enabled", checked);
    cEvt.dispatch();
}

void ControlPanel::on_backboneCheckbox_toggled(bool checked)
{
    GitlIvkCmdEvt cEvt("switch_icc_visible_class");
    cEvt.setParameter("show_backbone_enabled", checked);
    cEvt.dispatch();
}

void ControlPanel::on_noiseCheckbox_toggled(bool checked)
{
    GitlIvkCmdEvt cEvt("switch_icc_visible_class");
    cEvt.setParameter("show_noise_enabled", checked);
    cEvt.dispatch();
}

void ControlPanel::on_UnknownCheckbox_toggled(bool checked)
{
    GitlIvkCmdEvt cEvt("switch_icc_visible_class");
    cEvt.setParameter("show_unknown_enabled", checked);
    cEvt.dispatch();
}

void ControlPanel::on_timeSeriseGroupbox_toggled(bool arg1)
{
    GitlIvkCmdEvt cEvt("switch_time_series_vis");
    cEvt.setParameter("enabled", arg1);
    cEvt.dispatch();
}

void ControlPanel::on_textureNone_toggled(bool checked)
{
    if(checked)
    {
        GitlIvkCmdEvt cEvt("change_roi_texture");
        cEvt.setParameter("type", "none");
        cEvt.dispatch();
    }
}

void ControlPanel::on_textureColor_toggled(bool checked)
{
    if(checked)
    {
        GitlIvkCmdEvt cEvt("change_roi_texture");
        cEvt.setParameter("type", "color");
        cEvt.dispatch();
    }
}

void ControlPanel::on_textureStrip_toggled(bool checked)
{
    if(checked)
    {
        GitlIvkCmdEvt cEvt("change_roi_texture");
        cEvt.setParameter("type", "strip");
        cEvt.dispatch();
    }
}

void ControlPanel::on_textureCircle_toggled(bool checked)
{
    if(checked)
    {
        GitlIvkCmdEvt cEvt("change_roi_texture");
        cEvt.setParameter("type", "circle");
        cEvt.dispatch();
    }
}

void ControlPanel::on_textureContour_toggled(bool checked)
{
    if(checked)
    {
        GitlIvkCmdEvt cEvt("change_roi_texture");
        cEvt.setParameter("type", "contour");
        cEvt.dispatch();
    }
}

void ControlPanel::on_openFCSC_clicked()
{
    emit open_fcsc_file_clicked();
}


void ControlPanel::on_connectivitySlider_valueChanged(int value)
{
    GitlIvkCmdEvt cEvt("change_fc_network_th");
    cEvt.setParameter("threshold", value/100.0);
    cEvt.dispatch();
    this->ui->curConnThresLabel->setText(QString().sprintf("%.2f",value/100.0));
}

void ControlPanel::on_openMutiDisease_clicked()
{
    emit open_multi_disease_measurement_clicked();
}

void ControlPanel::on_openMDConnectivityBtn_clicked()
{
    emit open_multi_disease_connecivity_clicked();
}

void ControlPanel::on_MDConnectivitySlider_valueChanged(int value)
{
    GitlIvkCmdEvt cEvt("change_md_network_th");
    cEvt.setParameter("threshold", value/100.0);
    cEvt.dispatch();
    this->ui->curMDThresLabel->setText(QString().sprintf("%.2f",value/100.0));
}

void ControlPanel::on_change_bgcolor_btn_clicked()
{
    QPalette cPalette = ui->bgColorLabel->palette();
    QColorDialog cDialog(cPalette.color(QPalette::Window), this);
    cDialog.exec();

    GitlIvkCmdEvt cEvt("change_bg_color");
    cEvt.setParameter("color", cDialog.currentColor().name());
    cEvt.dispatch();

    cPalette.setColor(QPalette::Window, cDialog.currentColor());
    ui->bgColorLabel->setPalette(cPalette);
    ui->bgColorName->setText(cDialog.currentColor().name());
}

void ControlPanel::on_fiberModuleEnableBtn_toggled(bool checked)
{
    GitlIvkCmdEvt cEvt("switch_module");
    cEvt.setParameter("module", "fiber");
    cEvt.setParameter("status",checked);
    cEvt.dispatch();
}

void ControlPanel::on_grayMatterModuleEnableBtn_toggled(bool checked)
{
    GitlIvkCmdEvt cEvt("switch_module");
    cEvt.setParameter("module", "gray_matter");
    cEvt.setParameter("status",checked);
    cEvt.dispatch();
}

void ControlPanel::on_ROIProjectionBox_toggled(bool checked)
{
    GitlIvkCmdEvt cEvt("switch_module");
    cEvt.setParameter("module", "projection_plane");
    cEvt.setParameter("status",checked);
    cEvt.dispatch();
}

void ControlPanel::on_flipZCheckBox_toggled(bool checked)
{
    GitlIvkCmdEvt cEvt("switch_gray_mat_flip_z");
    cEvt.setParameter("status",checked);
    cEvt.dispatch();

}

void ControlPanel::on_fMRIEnableBtn_toggled(bool checked)
{
    GitlIvkCmdEvt cEvt("switch_module");
    cEvt.setParameter("module", "fMRI");
    cEvt.setParameter("status",checked);
    cEvt.dispatch();
}

void ControlPanel::on_multiDiseaseEnableBtn_toggled(bool checked)
{
    GitlIvkCmdEvt cEvt("switch_module");
    cEvt.setParameter("module", "multi_disease");
    cEvt.setParameter("status",checked);
    cEvt.dispatch();
}


void ControlPanel::on_raycastBtn_clicked()
{
    GitlIvkCmdEvt cEvt("raycast_render");
    cEvt.dispatch();
}

void ControlPanel::on_raycastFMRITextureMappingBtn_clicked()
{
    GitlIvkCmdEvt cEvt("raycast_texture_mapping");
    cEvt.setParameter("class", "fMRI");
    cEvt.dispatch();
}

void ControlPanel::on_testBtn_clicked()
{
    GitlIvkCmdEvt cEvt("test_command");
    cEvt.setParameter("lightmap_folder", "E:\\Learning\\Dropbox\\IUPUI\\Research\\2015.12.06\\lightmap");
    cEvt.setParameter("texture_folder", "E:\\Learning\\Dropbox\\IUPUI\\Research\\2015.12.06\\texture_fmri");

    cEvt.dispatch();
}

void ControlPanel::on_raycastMDTextureMappingBtn_clicked()
{
    GitlIvkCmdEvt cEvt("raycast_texture_mapping");
    cEvt.setParameter("class", "multi-disease");
    cEvt.dispatch();
}

void ControlPanel::on_raycastFMRINetworkBtn_clicked()
{
    GitlIvkCmdEvt cEvt("raycast_draw_network");
    cEvt.setParameter("class", "fMRI");
    cEvt.setParameter("lightmap_folder", "E:\\Learning\\Dropbox\\IUPUI\\Research\\2015.12.06\\lightmap");
    cEvt.setParameter("texture_folder", "E:\\Learning\\Dropbox\\IUPUI\\Research\\2015.12.06\\texture_fmri");
    cEvt.dispatch();
}

void ControlPanel::on_raycastMDNetworkBtn_clicked()
{
    GitlIvkCmdEvt cEvt("raycast_draw_network");
    cEvt.setParameter("class", "multi_disease");
    cEvt.setParameter("lightmap_folder", "E:\\Learning\\Dropbox\\IUPUI\\Research\\2015.12.06\\lightmap");
    cEvt.setParameter("texture_folder", "E:\\Learning\\Dropbox\\IUPUI\\Research\\2015.12.06\\texture_multi_disease");
    cEvt.dispatch();
}

void ControlPanel::on_openCLBtn_clicked()
{
    GitlIvkCmdEvt cEvt("raycast_render_opencl");
    cEvt.dispatch();
}


void ControlPanel::on_raycastTranspSlider_valueChanged(int value)
{
    GitlIvkCmdEvt cEvt("change_raycast_parameters");
    cEvt.setParameter("transparency", pow(value/100.0,4));
    cEvt.dispatch();
}

void ControlPanel::on_raycastAmbientSlider_valueChanged(int value)
{
    GitlIvkCmdEvt cEvt("change_raycast_parameters");
    cEvt.setParameter("ambient", value/100.0);
    cEvt.dispatch();
}

void ControlPanel::on_raycastDiffuseSlider_valueChanged(int value)
{
    GitlIvkCmdEvt cEvt("change_raycast_parameters");
    cEvt.setParameter("diffuse", value/100.0);
    cEvt.dispatch();
}

void ControlPanel::on_raycastSpecularSlider_valueChanged(int value)
{
    GitlIvkCmdEvt cEvt("change_raycast_parameters");
    cEvt.setParameter("specular", value/100.0);
    cEvt.dispatch();
}

void ControlPanel::on_raycastFSlider_valueChanged(int value)
{
    GitlIvkCmdEvt cEvt("change_raycast_parameters");
    cEvt.setParameter("f", value);
    cEvt.dispatch();
}

void ControlPanel::on_adjustCamPosBox_toggled(bool checked)
{
    GitlIvkCmdEvt cEvt("change_raycast_parameters");
    cEvt.setParameter("change_center", checked);
    cEvt.dispatch();
}

void ControlPanel::on_checkBox_2_toggled(bool checked)
{
    QString strKenelName = "far_to_near";
    if(checked)
        strKenelName = "near_to_far";

    GitlIvkCmdEvt cEvt("change_raycast_parameters");
    cEvt.setParameter("change_kernel", strKenelName);
    cEvt.dispatch();
}
