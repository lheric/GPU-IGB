#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include "gitlview.h"

namespace Ui {
class ControlPanel;
}

class ControlPanel : public QWidget, public GitlView
{
    Q_OBJECT

public:
    explicit ControlPanel(QWidget *parent = 0);
    double getSampleRatio();
    ~ControlPanel();
public:
    void onUpdateROIList(GitlUpdateUIEvt &rcEvt);
    void onUpdateTrkFile(GitlUpdateUIEvt &rcEvt);
    void onUpdateTrkConnFile(GitlUpdateUIEvt &rcEvt);
    void onUpdateTrkICCFile(GitlUpdateUIEvt &rcEvt);
    void onUpdateTrkVarFile(GitlUpdateUIEvt &rcEvt);
    void onUpdateTrkCountFile(GitlUpdateUIEvt &rcEvt);
    void onUpdateISOFile(GitlUpdateUIEvt &rcEvt);
    void onUpdateParcellationFile(GitlUpdateUIEvt &rcEvt);
    void onUpdateTimeSeriesFile(GitlUpdateUIEvt &rcEvt);
    void onUpdatePickedLabel(GitlUpdateUIEvt &rcEvt);
    void onUpdateTimePointNum(GitlUpdateUIEvt &rcEvt);
    void onUpdateICCSliderRange(GitlUpdateUIEvt &rcEvt);
    void onUpdateVarSliderRange(GitlUpdateUIEvt &rcEvt);
    void onUpdateCountSliderRange(GitlUpdateUIEvt &rcEvt);
    void onUpdateNetworkFile(GitlUpdateUIEvt &rcEvt);
    void onUpdateMDNetworkFile(GitlUpdateUIEvt &rcEvt);

    void onUpdateMultiDiseaseFile(GitlUpdateUIEvt &rcEvt);


private slots:
    void on_startEndTh_valueChanged(double arg1);

    void on_sampleRate_valueChanged(double arg1);

    void on_shortTrkTh_valueChanged(double arg1);


    void on_gayMatOpacity_valueChanged(double arg1);


    void on_isoSurfaceSpinBox_valueChanged(double arg1);

    void on_openFiber_clicked();

    void on_openGMISO_clicked();

    void on_openGMParcellation_clicked();

    void on_ROICobombox_activated(const QString &arg1);

    void on_timeDataPointSlider_valueChanged(int value);

    void on_openTimeSeriesData_clicked();

    void on_openFiberConnBtn_clicked();

    void on_openFiberICCBtn_clicked();

    void on_openFiberVarBtn_clicked();

    void on_enableMaskGroupbox_toggled(bool arg1);

    void on_ICCSlider_valueChanged(int value);

    void on_VarianceSlider_valueChanged(int value);

    void on_reproducibleCheckbox_toggled(bool checked);

    void on_backboneCheckbox_toggled(bool checked);

    void on_noiseCheckbox_toggled(bool checked);

    void on_UnknownCheckbox_toggled(bool checked);

    void on_countSlider_valueChanged(int value);

    void on_openFiberCountBtn_clicked();

    void on_timeSeriseGroupbox_toggled(bool arg1);

    void on_textureNone_toggled(bool checked);

    void on_textureColor_toggled(bool checked);

    void on_textureStrip_toggled(bool checked);

    void on_textureCircle_toggled(bool checked);

    void on_textureContour_toggled(bool checked);

    void on_openFCSC_clicked();

    void on_connectivitySlider_valueChanged(int value);

    void on_openMutiDisease_clicked();

    void on_openMDConnectivityBtn_clicked();

    void on_MDConnectivitySlider_valueChanged(int value);

    void on_change_bgcolor_btn_clicked();

    void on_fiberModuleEnableBtn_toggled(bool checked);

    void on_grayMatterModuleEnableBtn_toggled(bool checked);

    void on_ROIProjectionBox_toggled(bool checked);

    void on_flipZCheckBox_toggled(bool checked);

    void on_fMRIEnableBtn_toggled(bool checked);

    void on_multiDiseaseEnableBtn_toggled(bool checked);


    void on_raycastBtn_clicked();

    void on_raycastFMRITextureMappingBtn_clicked();

    void on_testBtn_clicked();

    void on_raycastMDTextureMappingBtn_clicked();

    void on_raycastFMRINetworkBtn_clicked();

    void on_raycastMDNetworkBtn_clicked();

    void on_openCLBtn_clicked();

    void on_raycastTranspSlider_valueChanged(int value);

    void on_raycastAmbientSlider_valueChanged(int value);

    void on_raycastDiffuseSlider_valueChanged(int value);

    void on_raycastSpecularSlider_valueChanged(int value);

    void on_raycastFSlider_valueChanged(int value);

    void on_adjustCamPosBox_toggled(bool checked);

    void on_checkBox_2_toggled(bool checked);

signals:
    void open_trk_file_clicked();
    void open_trk_connection_file_clicked();
    void open_trk_icc_file_clicked();
    void open_trk_var_file_clicked();
    void open_trk_count_file_clicked();
    void open_fcsc_file_clicked();

    void open_nifti_for_surface_clicked();
    void open_nifti_for_label_clicked();

    void open_time_series_file_clicked();
    void open_multi_disease_measurement_clicked();
    void open_multi_disease_connecivity_clicked();

private:
    Ui::ControlPanel *ui;
};

#endif // CONTROLPANEL_H
