#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QDialog>
#include <gitlivkcmdevt.h>
#include "model/modellocator.h"
#include "model/rom.h"
#include "vtkRenderWindow.h"
#include <QThread>
#include <vtkWindowToImageFilter.h>
#include <vtkRendererCollection.h>
#include <vtkPNGWriter.h>
#include <QMouseEvent>
#include <QEvent>
#include <QMdiSubWindow>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /// custom pop up menu
    connect(this->ui->qvtkWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onShowContextMenu(QPoint)));
    connect(this->ui->controlPanel, SIGNAL(open_trk_file_clicked()), this, SLOT(on_actionOpen_trk_triggered()));
    connect(this->ui->controlPanel, SIGNAL(open_trk_connection_file_clicked()), this, SLOT(on_actionOpen_Fiber_Connection_Info_csv_triggered()));
    connect(this->ui->controlPanel, SIGNAL(open_trk_var_file_clicked()), this, SLOT(on_actionOpen_Fiber_Variance_ROI_based_csv_triggered()));
    connect(this->ui->controlPanel, SIGNAL(open_trk_icc_file_clicked()), this, SLOT(on_actionOpen_Fiber_ICC_ROI_based_csv_triggered()));
    connect(this->ui->controlPanel, SIGNAL(open_trk_count_file_clicked()), this, SLOT(on_actionOpen_Fiber_Count_ROI_based_csv_triggered()));
    connect(this->ui->controlPanel, SIGNAL(open_nifti_for_surface_clicked()), this, SLOT(on_actionOpen_Gray_Matter_File_NIFTI_nii_triggered()));
    connect(this->ui->controlPanel, SIGNAL(open_nifti_for_label_clicked()), this, SLOT(on_actionOpen_Gray_Matter_Parcellation_File_nii_triggered()));
    connect(this->ui->controlPanel, SIGNAL(open_time_series_file_clicked()), this, SLOT(on_actionOpen_Time_Serise_Data_csv_triggered()));
    connect(this->ui->controlPanel, SIGNAL(open_fcsc_file_clicked()), this, SLOT(on_actionOpen_SC_FC_Connectivity_Matrix_csv_triggered()));
    connect(this->ui->controlPanel, SIGNAL(open_multi_disease_measurement_clicked()), this, SLOT(on_actionOpen_Muti_disease_File_csv_triggered()));
    connect(this->ui->controlPanel, SIGNAL(open_multi_disease_connecivity_clicked()), this, SLOT(on_actionOpen_Multi_disease_Connectivity_csv_triggered()));
    connect(this->ui->qvtkWidget, SIGNAL(mouseEvent(QMouseEvent*)), this, SLOT(onQVTKWidgetMouseEvt(QMouseEvent*)));
    connect(this->ui->qvtkWidget, SIGNAL(exitFullscreen()), this, SLOT(toggleFullScreen()));

    /// connect vtk pipeline
    GitlIvkCmdEvt cEvt("connect_pipeline");
    cEvt.setParameter("render_win", QVariant::fromValue<void*>(this->ui->qvtkWidget->GetRenderWindow()));
    cEvt.setParameter("plugin_prop_tabs", QVariant::fromValue<void*>(this->ui->rendererPropTabs));
    cEvt.setParameter("center_widget_splitter", QVariant::fromValue<void*>(this->ui->centerWidgetSplitter));
    cEvt.dispatch();

    // load filters command
    GitlIvkCmdEvt cRequest("reload_filter");
    cRequest.dispatch();



    this->ui->raycastWin->hide();
    this->ui->msgDockWidget->hide();

    this->ui->circleGraphView->hide();
    this->ui->ctrlPaneldockWidget->hide();

    this->ui->loadedSubjects->hide();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onShowContextMenu(const QPoint &pos)
{
    QPoint globalPos = this->ui->qvtkWidget->mapToGlobal(pos);
    QMenu myMenu;
    QAction* pcFullScrAct = myMenu.addAction("Fullscreen");
    pcFullScrAct->setCheckable(true);
    pcFullScrAct->setChecked(this->isFullScreen());

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem == pcFullScrAct)
    {
        this->toggleFullScreen();
    }

}


void MainWindow::on_actionOpen_trk_triggered()
{

    QString strFilename;
    QString strLastPath = g_cAppSetting.value("open_track_path",".").toString();
    strFilename=QFileDialog::getOpenFileName(this,
                                             tr("Open Fiber File"),
                                             strLastPath,
                                             tr("All Files (*.*)"));

    if(!strFilename.isEmpty())
        g_cAppSetting.setValue("open_track_path",strFilename);

    if(strFilename.isEmpty() || !QFileInfo(strFilename).exists() )
    {
        qWarning() << "File not found.";
        return;
    }


//    GitlIvkCmdEvt cBusyShow("busy_dialog");
//    cBusyShow.setParameter("busy_dialog_visible", true);
//    cBusyShow.dispatch();

    GitlIvkCmdEvt cOpenEvt("open_fiber_file");
    cOpenEvt.setParameter("filename", strFilename);
    cOpenEvt.dispatch();

    // reset camera
    GitlIvkCmdEvt cResetCam("reset_camera");
    cResetCam.dispatch();

    // schedule rerender
    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch();

//    GitlIvkCmdEvt cBusyHide("busy_dialog");
//    cBusyHide.setParameter("busy_dialog_visible", false);
//    cBusyHide.dispatch();



}

void MainWindow::on_screenshotBtn_clicked()
{
    /// file location selection dialog
    QString strFilename;
    QString strLastPath = g_cAppSetting.value("snapshot_saving_path",".").toString();
    strFilename=QFileDialog::getSaveFileName(this,
                                          tr("Save Snapshot"),
                                          strLastPath,
                                          tr("Images (*.png)"));
    if(strFilename.isEmpty())
        return;
    g_cAppSetting.setValue("snapshot_saving_path",strFilename);


    /// save screenshot
    vtkRenderWindow* renderWindow = this->ui->qvtkWidget->GetRenderWindow();

    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
        vtkSmartPointer<vtkWindowToImageFilter>::New();
      windowToImageFilter->SetInput(renderWindow);
      //windowToImageFilter->SetMagnification(4); //set the resolution of the output image (4 times the current resolution of vtk render window)
      //windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
      windowToImageFilter->SetInputBufferTypeToRGB(); //also record the alpha (transparency) channel
      windowToImageFilter->ReadFrontBufferOff(); // read from the back buffer
      windowToImageFilter->Update();

      vtkSmartPointer<vtkPNGWriter> writer =
        vtkSmartPointer<vtkPNGWriter>::New();
      writer->SetFileName(strFilename.toStdString().c_str());
      writer->SetInputConnection(windowToImageFilter->GetOutputPort());
      writer->Write();

      /// update screen
      GitlIvkCmdEvt cEvt("rerender");
      cEvt.dispatch();

}

void MainWindow::on_actionOpen_Gray_Matter_File_NIFTI_nii_triggered()
{
    QString strFilename;
    QString strLastPath = g_cAppSetting.value("open_graymat_intensity_path",".").toString();
    strFilename=QFileDialog::getOpenFileName(this,
                                             tr("Open Gray Matter Image File"),
                                             strLastPath,
                                             tr("All Files (*.*)"));

    if(!strFilename.isEmpty())
        g_cAppSetting.setValue("open_graymat_intensity_path",strFilename);

    if(strFilename.isEmpty() || !QFileInfo(strFilename).exists() )
    {
        qWarning() << "File not found.";
        return;
    }


//    GitlIvkCmdEvt cBusyShow("busy_dialog");
//    cBusyShow.setParameter("busy_dialog_visible", true);
//    cBusyShow.dispatch();

    GitlIvkCmdEvt cOpenEvt("open_gm_image_file");
    cOpenEvt.setParameter("filename", strFilename);
    cOpenEvt.dispatch();

    // schedule update
    GitlIvkCmdEvt cResetCam("reset_camera");
    cResetCam.dispatch();

    GitlIvkCmdEvt cReRender("rerender");
    cReRender.dispatch();

//    GitlIvkCmdEvt cBusyHide("busy_dialog");
//    cBusyHide.setParameter("busy_dialog_visible", false);
//    cBusyHide.dispatch();
}

void MainWindow::on_actionTwo_side_Lighting_triggered(bool checked)
{
    GitlIvkCmdEvt cEvt("change_rendering_pref");
    cEvt.setParameter("two_side_lighting", checked);
    cEvt.dispatch();
}

void MainWindow::on_actionOpen_Gray_Matter_Parcellation_File_nii_triggered()
{
    QString strFilename;
    QString strLastPath = g_cAppSetting.value("open_graymat_parcellation_path",".").toString();
    strFilename=QFileDialog::getOpenFileName(this,
                                             tr("Open Gray Matter Parcellation File"),
                                             strLastPath,
                                             tr("All Files (*.*)"));

    if(!strFilename.isEmpty())
        g_cAppSetting.setValue("open_graymat_parcellation_path",strFilename);

    if(strFilename.isEmpty() || !QFileInfo(strFilename).exists() )
    {
        qWarning() << "File not found.";
        return;
    }
//    GitlIvkCmdEvt cBusyShow("busy_dialog");
//    cBusyShow.setParameter("busy_dialog_visible", true);
//    cBusyShow.dispatch();

    GitlIvkCmdEvt cEvt("open_gm_parcel_file");
    cEvt.setParameter("filename", strFilename);
    cEvt.dispatch();

//    GitlIvkCmdEvt cBusyHide("busy_dialog");
//    cBusyHide.setParameter("busy_dialog_visible", false);
//    cBusyHide.dispatch();



}

void MainWindow::onQVTKWidgetMouseEvt(QMouseEvent *pcEvt)
{
    static QPointF cPt;
    static bool bMouseDown = false;
    QEvent::Type eType = pcEvt->type();
    if(eType == QEvent::MouseButtonPress )
    {
        bMouseDown = true;

    }
    else if(eType == QEvent::MouseButtonDblClick)
    {
        cPt = pcEvt->windowPos();
        xFlipYCoord(cPt);
        qDebug() << "clicked" << cPt;
        GitlIvkCmdEvt cEvt("pick_coord");
        cEvt.setParameter("coord", cPt);
        cEvt.setParameter("renderer", QVariant::fromValue<void*>(ui->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()));
        cEvt.dispatch();
    }
    else if(eType == QEvent::MouseButtonRelease)
    {
        bMouseDown = false;
    }
    else if(eType == QEvent::MouseMove)
    {


        if( (pcEvt->modifiers()&Qt::AltModifier) && bMouseDown)
        {
            static GitlIvkCmdEvt cEvt("rotate_projection_plane");
            cEvt.dispatch();
        }
    }

}

void MainWindow::xFlipYCoord(QPointF &rcPts)
{
    int iWinHeight = ui->qvtkWidget->height();
    rcPts.setY(iWinHeight - rcPts.y());
}

void MainWindow::toggleFullScreen()
{
    if(!this->isFullScreen())
    {
        this->ui->ctrlPaneldockWidget->hide();
        this->ui->msgDockWidget->hide();
        this->ui->statusBar->hide();
        this->ui->menuBar->hide();
        this->showFullScreen();
    }
    else
    {
        this->ui->ctrlPaneldockWidget->show();
        this->ui->msgDockWidget->show();
        this->ui->statusBar->show();
        this->ui->menuBar->show();
        this->showNormal();
    }
}

void MainWindow::on_actionDeep_Peeling_triggered(bool checked)
{
    GitlIvkCmdEvt cEvt("change_rendering_pref");
    cEvt.setParameter("deep_peeling", checked);
    cEvt.dispatch();
}

void MainWindow::on_actionOpen_Time_Serise_Data_csv_triggered()
{
    QString strFilename;
    QString strLastPath = g_cAppSetting.value("open_time_series_path",".").toString();
    strFilename=QFileDialog::getOpenFileName(this,
                                             tr("Open Time Series File"),
                                             strLastPath,
                                             tr("All Files (*.*)"));

    if(!strFilename.isEmpty())
        g_cAppSetting.setValue("open_time_series_path",strFilename);

    if(strFilename.isEmpty() || !QFileInfo(strFilename).exists() )
    {
        qWarning() << "File not found.";
        return;
    }
//    GitlIvkCmdEvt cBusyShow("busy_dialog");
//    cBusyShow.setParameter("busy_dialog_visible", true);
//    cBusyShow.dispatch();

    GitlIvkCmdEvt cEvt("open_time_series_file");
    cEvt.setParameter("filename", strFilename);
    cEvt.dispatch();

//    GitlIvkCmdEvt cBusyHide("busy_dialog");
//    cBusyHide.setParameter("busy_dialog_visible", false);
//    cBusyHide.dispatch();
}

void MainWindow::on_actionOpen_Fiber_Connection_Info_csv_triggered()
{
    QString strFilename;
    QString strLastPath = g_cAppSetting.value("open_fiber_conn_path",".").toString();
    strFilename=QFileDialog::getOpenFileName(this,
                                             tr("Fiber Connection Table File"),
                                             strLastPath,
                                             tr("All Files (*.*)"));

    if(!strFilename.isEmpty())
        g_cAppSetting.setValue("open_fiber_conn_path",strFilename);

    if(strFilename.isEmpty() || !QFileInfo(strFilename).exists() )
    {
        qWarning() << "File not found.";
        return;
    }

    GitlIvkCmdEvt cEvt("open_fiber_conn_file");
    cEvt.setParameter("filename", strFilename);
    cEvt.dispatch();


}

void MainWindow::on_actionOpen_Fiber_ICC_ROI_based_csv_triggered()
{
    QString strFilename;
    QString strLastPath = g_cAppSetting.value("open_fiber_icc_path",".").toString();
    strFilename=QFileDialog::getOpenFileName(this,
                                             tr("Fiber ICC File"),
                                             strLastPath,
                                             tr("All Files (*.*)"));

    if(!strFilename.isEmpty())
        g_cAppSetting.setValue("open_fiber_icc_path",strFilename);

    if(strFilename.isEmpty() || !QFileInfo(strFilename).exists() )
    {
        qWarning() << "File not found.";
        return;
    }

    GitlIvkCmdEvt cEvt("open_fiber_icc_file");
    cEvt.setParameter("filename", strFilename);
    cEvt.dispatch();
}

void MainWindow::on_actionOpen_Fiber_Variance_ROI_based_csv_triggered()
{
    QString strFilename;
    QString strLastPath = g_cAppSetting.value("open_fiber_var_path",".").toString();
    strFilename=QFileDialog::getOpenFileName(this,
                                             tr("Fiber Variance File"),
                                             strLastPath,
                                             tr("All Files (*.*)"));

    if(!strFilename.isEmpty())
        g_cAppSetting.setValue("open_fiber_var_path",strFilename);

    if(strFilename.isEmpty() || !QFileInfo(strFilename).exists() )
    {
        qWarning() << "File not found.";
        return;
    }

    GitlIvkCmdEvt cEvt("open_fiber_var_file");
    cEvt.setParameter("filename", strFilename);
    cEvt.dispatch();
}

void MainWindow::on_actionOpen_Fiber_Count_ROI_based_csv_triggered()
{
    QString strFilename;
    QString strLastPath = g_cAppSetting.value("open_fiber_count_path",".").toString();
    strFilename=QFileDialog::getOpenFileName(this,
                                             tr("Fiber Count File"),
                                             strLastPath,
                                             tr("All Files (*.*)"));

    if(!strFilename.isEmpty())
        g_cAppSetting.setValue("open_fiber_count_path",strFilename);

    if(strFilename.isEmpty() || !QFileInfo(strFilename).exists() )
    {
        qWarning() << "File not found.";
        return;
    }

    GitlIvkCmdEvt cEvt("open_fiber_count_file");
    cEvt.setParameter("filename", strFilename);
    cEvt.dispatch();
}

void MainWindow::on_actionOpen_SC_FC_Connectivity_Matrix_csv_triggered()
{
    QString strFilename;
    QString strLastPath = g_cAppSetting.value("open_SCFC_connectivity_path",".").toString();
    strFilename=QFileDialog::getOpenFileName(this,
                                             tr("SC/FC Connectivity File"),
                                             strLastPath,
                                             tr("All Files (*.*)"));

    if(!strFilename.isEmpty())
        g_cAppSetting.setValue("open_SCFC_connectivity_path",strFilename);

    if(strFilename.isEmpty() || !QFileInfo(strFilename).exists() )
    {
        qWarning() << "File not found.";
        return;
    }

    GitlIvkCmdEvt cEvt("open_gm_network");
    cEvt.setParameter("filename", strFilename);
    cEvt.dispatch();
}

void MainWindow::on_actionOpen_Muti_disease_File_csv_triggered()
{
    QString strFilename;
    QString strLastPath = g_cAppSetting.value("open_muti_disease_measurement_path",".").toString();
    strFilename=QFileDialog::getOpenFileName(this,
                                             tr("Multi-disease File"),
                                             strLastPath,
                                             tr("All Files (*.*)"));

    if(!strFilename.isEmpty())
        g_cAppSetting.setValue("open_muti_disease_measurement_path",strFilename);

    if(strFilename.isEmpty() || !QFileInfo(strFilename).exists() )
    {
        qWarning() << "File not found.";
        return;
    }

    GitlIvkCmdEvt cEvt("open_multi_disease_measurement");
    cEvt.setParameter("filename", strFilename);
    cEvt.dispatch();
}

void MainWindow::on_actionOpen_Multi_disease_Connectivity_csv_triggered()
{
    QString strFilename;
    QString strLastPath = g_cAppSetting.value("open_muti_disease_connectivity_path",".").toString();
    strFilename=QFileDialog::getOpenFileName(this,
                                             tr("Multi-disease Connectivity File"),
                                             strLastPath,
                                             tr("All Files (*.*)"));

    if(!strFilename.isEmpty())
        g_cAppSetting.setValue("open_muti_disease_connectivity_path",strFilename);

    if(strFilename.isEmpty() || !QFileInfo(strFilename).exists() )
    {
        qWarning() << "File not found.";
        return;
    }

    GitlIvkCmdEvt cEvt("open_multi_disease_connectivity");
    cEvt.setParameter("filename", strFilename);
    cEvt.dispatch();
}

void MainWindow::on_actionAbout_BECA_triggered()
{
    m_cAboutDialog.exec();
}

void MainWindow::on_actionParallel_toggled(bool arg1)
{
    GitlIvkCmdEvt cEvt("switch_parallel_projection");
    cEvt.setParameter("enabled", arg1);
    cEvt.dispatch();
}

void MainWindow::on_actionRaycasting_Window_toggled(bool arg1)
{
    this->ui->raycastWin->setVisible(arg1);
}

void MainWindow::on_actionToggleMsgWin_toggled(bool arg1)
{
    this->ui->msgDockWidget->setVisible(arg1);
}
