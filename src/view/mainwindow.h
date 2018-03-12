#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL)
//#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)
#include <QMainWindow>
#include <QVTKWidget.h>
#include <QTime>
#include <QDebug>
#include "gitlview.h"
#include "trkfileio.h"
#include "gitldef.h"
#include "view/busydialog.h"
#include "aboutdialog.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public GitlView
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onShowContextMenu(const QPoint& pos);

    void on_actionOpen_trk_triggered();

    void on_screenshotBtn_clicked();

    void on_actionOpen_Gray_Matter_File_NIFTI_nii_triggered();

    void on_actionTwo_side_Lighting_triggered(bool checked);

    void on_actionOpen_Gray_Matter_Parcellation_File_nii_triggered();

    void onQVTKWidgetMouseEvt(QMouseEvent *pcEvt);

    void toggleFullScreen();

    void on_actionDeep_Peeling_triggered(bool checked);

    void on_actionOpen_Time_Serise_Data_csv_triggered();

    void on_actionOpen_Fiber_Connection_Info_csv_triggered();

    void on_actionOpen_Fiber_ICC_ROI_based_csv_triggered();

    void on_actionOpen_Fiber_Variance_ROI_based_csv_triggered();

    void on_actionOpen_Fiber_Count_ROI_based_csv_triggered();

    void on_actionOpen_SC_FC_Connectivity_Matrix_csv_triggered();

    void on_actionOpen_Muti_disease_File_csv_triggered();

    void on_actionOpen_Multi_disease_Connectivity_csv_triggered();

    void on_actionAbout_BECA_triggered();

    void on_actionParallel_toggled(bool arg1);


    void on_actionRaycasting_Window_toggled(bool arg1);

    void on_actionToggleMsgWin_toggled(bool arg1);

protected:
    void xFlipYCoord(QPointF &rcPts);


private:
    Ui::MainWindow *ui;
    ADD_CLASS_FIELD_PRIVATE(BusyDialog, cBusyDialog)
    ADD_CLASS_FIELD_PRIVATE(AboutDialog, cAboutDialog)

};

#endif // MAINWINDOW_H
