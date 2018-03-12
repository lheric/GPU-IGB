#ifndef GMISOCONFIGPANEL_H
#define GMISOCONFIGPANEL_H

#include <QWidget>
#include "gitlview.h"
#include "colorschemetablemodel.h"
#include "colorschemedelegate.h"

class GMISORenderer;

namespace Ui {
class GMISOConfigPanel;
}

class GMISOConfigPanel : public QWidget, public GitlView
{
    Q_OBJECT

public:
    explicit GMISOConfigPanel(GMISORenderer* pcGMISORenderer, QWidget *parent = 0);
    ~GMISOConfigPanel();

    ADD_CLASS_FIELD_NOSETTER(ColorSchemeTableModel, colorSchemeModel, getColorSchemeModel)
    ADD_CLASS_FIELD_NOSETTER(ColorSchemeDelegate, colorSchemeDelegate, getColorSchemeDelegate)

public slots:
    void onUpdatePickedLabel(GitlUpdateUIEvt &rcEvt);
    void onUpdateGMIntesntiyFile(GitlUpdateUIEvt &rcEvt);
    void onUpdateGMParcellationFile(GitlUpdateUIEvt &rcEvt);
    void onUpdateLabelNameFile(GitlUpdateUIEvt &rcEvt);


private slots:
    void on_openGMIntensity_clicked();

    void on_openGMParcellation_clicked();

    void on_isoSurfaceSpinBox_valueChanged(double arg1);

    void on_gayMatOpacity_valueChanged(double arg1);

    void on_mapPacellation2SurfaceCheckBox_toggled(bool checked);

    void on_openLabelNamesFile_clicked();

    void on_GMIntensityGroupBox_toggled(bool checked);

    void on_GMParcellationGroupBox_toggled(bool checked);

    void on_LabelSurfaceOpacity_valueChanged(int value);

    void on_ROICobombox_activated(int index);

private:
    Ui::GMISOConfigPanel *ui;

    GMISORenderer* m_pcGMISORenderer;
};

#endif // GMISOCONFIGPANEL_H
