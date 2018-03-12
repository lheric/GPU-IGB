#include "genomeconfigpanel.h"
#include "ui_genomeconfigpanel.h"
#include "calculation_backend/AnovaTestClient/clientdriver.h"
#include "genomeexplorer.h"
#include "genomeexplorermodel.h"
#include "algorithm/commonutils.h"
#include <QtMath>
#include <QDebug>
#include <iostream>
using namespace std;

GenomeConfigPanel::GenomeConfigPanel(GenomeExplorer* pcGenomeExplorer, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenomeConfigPanel)
{
    ui->setupUi(this);
    ui->VEGASInputGroupBox->hide(); /// hide VEGAS input on start

    m_pcGenomeExplorer = pcGenomeExplorer;
    initServerList();

}

GenomeConfigPanel::~GenomeConfigPanel()
{
    delete ui;
}

void GenomeConfigPanel::initServerList()
{
    this->ui->serverListComboBox->addItem("localhost (127.0.0.1)", "localhost");
    this->ui->serverListComboBox->addItem("IU Big Red II", "IU Big Red II not implemented");
    this->ui->serverListComboBox->addItem("Other");

    this->ui->IPAddrInput->hide();
}


void GenomeConfigPanel::on_serverListComboBox_activated(const QString &arg1)
{
    if(arg1 == "Other")
    {
        this->ui->IPAddrInput->setText("");
        this->ui->IPAddrInput->show();
    }
    else
    {
        this->ui->IPAddrInput->setText("127.0.0.1");
        this->ui->IPAddrInput->hide();
    }
}

void GenomeConfigPanel::on_connServerBtn_clicked()
{
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();


    qDebug() << "basic Anova client is running ..." ;

    srand(1);
    int iResult = CSocket::Initialize();
    if (iResult)
    {
        this->ui->connStatusLabel->setText("connection init failed");
        qCritical() << "error during CSocket::Initialize";
        return;
    }

    QString strServerAddr;
    if( ui->serverListComboBox->currentData().isValid() )
        strServerAddr = ui->serverListComboBox->currentData().toString();
    else
        strServerAddr = ui->IPAddrInput->text();
    qDebug() << "connecting to" << strServerAddr << ":" << DEFAULT_PORT;
    bool bSuccess = pcModel->connectToServer(strServerAddr.toLocal8Bit().data(), DEFAULT_PORT);
    this->ui->inputDataGroupBox->setEnabled(bSuccess);
    if ( bSuccess == false)
    {
        this->ui->connStatusLabel->setText("Failed to establish connection");
        qCritical() << "fail to connect to server" << strServerAddr << ":" << DEFAULT_PORT;
        return;
    }
    this->ui->connStatusLabel->setText("Connected");
    this->ui->connServerBtn->setEnabled(false);
}

void GenomeConfigPanel::on_openSNPSubjectBtn_clicked()
{
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    QString strFilepath;
    bool bSuccess = CommonUtils::openFileDialog("Open Subject - SNP File",
                                                "CSV (*.csv)",
                                                "open_subject_SNP_path",
                                                strFilepath);

    if(bSuccess == false)
        return;

    bSuccess = pcModel->loadSNPSubject(strFilepath);

    if(bSuccess == true)
    {
        this->ui->SNPsubjectLabel->setText(strFilepath);
        this->ui->SNPSubjDimLable->setText(QString("%1 X %2")
                                           .arg(pcModel->getSNPSubject()->GetHeight())
                                           .arg(pcModel->getSNPSubject()->GetWidth()));
        this->ui->readyBtn->setEnabled(true);
    }

}

void GenomeConfigPanel::on_openVoxelSubjectBtn_clicked()
{
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    QString strFilepath;
    bool bSuccess = CommonUtils::openFileDialog("Open Subject - SNP File",
                                                "CSV (*.csv)",
                                                "open_subject_voxel_path",
                                                strFilepath);

    if(bSuccess == false)
        return;

    bSuccess = pcModel->loadVoxelSubject(strFilepath);

    if(bSuccess == true)
    {
        this->ui->voxelSubjectLabel->setText(strFilepath);
        this->ui->VoxelSubjDimLable->setText(QString("%1 X %2")
                                             .arg(pcModel->getVoxelSubject()->GetHeight())
                                             .arg(pcModel->getVoxelSubject()->GetWidth()));
        this->ui->readyBtn->setEnabled(true);
    }
}

void GenomeConfigPanel::on_readyBtn_clicked()
{
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();

    /// sanity check
    if(pcModel->getVoxelSubject()->GetHeight() != pcModel->getSNPSubject()->GetHeight())
    {
        qCritical() << "subject-voxel matrix and subject-SNP matrix do not have the same number of rows.";
        return;
    }
    if( pcModel->uploadVoxelSubjAndSNPSubjToServer() == false )
    {
        qDebug() << "upload failed. please make sure you have loaded both matrices";
        return;
    }

    this->ui->numSNPsLabel->setText(QString("%1").arg(pcModel->getNumberOfSNPs()));
    this->ui->numSubjectsLabel->setText(QString("%1").arg(pcModel->getNumberOfSubjects()));
    this->ui->numVoxelsLabel->setText(QString("%1").arg(pcModel->getNumberOfVoxels()));

    emit pcModel->onDataUploaded();
    this->ui->readyBtn->setEnabled(false);

    this->ui->brainExpGroupBox->setEnabled(true);
    this->ui->genomeExpGroupBox->setEnabled(true);
    this->ui->heatmapGroupBox->setEnabled(true);
}


void GenomeConfigPanel::on_VEGASRadioButton_toggled(bool checked)
{
    this->ui->VEGASInputGroupBox->setVisible(checked);
    this->ui->simulationIterationGroup->setEnabled(checked);
}

void GenomeConfigPanel::on_opaqueModeGroupBox_toggled(bool bChecked)
{
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    emit pcModel->onSetOpaqueMode(bChecked);
}

void GenomeConfigPanel::on_openGeneSNPBtn_clicked()
{
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    QString strFilepath;
    bool bSuccess = CommonUtils::openFileDialog("Open Gene - SNP File",
                                                "CSV (*.csv)",
                                                "open_gene_SNP_path",
                                                strFilepath);

    if(bSuccess == false)
        return;
}

void GenomeConfigPanel::on_openLDMatrixBtn_clicked()
{
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    QString strFilepath;
    bool bSuccess = CommonUtils::openFileDialog("Open LD Matrix File",
                                                "CSV (*.csv)",
                                                "open_LD_matrix_path",
                                                strFilepath);

    if(bSuccess == false)
        return;
}

void GenomeConfigPanel::on_brainExpOpacitySlider_actionTriggered(int action)
{
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    if(action == QAbstractSlider::SliderMove)
    {
        double dOpacity = this->ui->brainExpOpacitySlider->value()/100.0;
        dOpacity = qPow(dOpacity, 4);
        this->ui->opaqueModeGroupBox->setTitle(QString("Voxel Opacity (%1)").arg(dOpacity, 0, 'f', 3));
        emit pcModel->onOpacityChanged(dOpacity);
    }
}
