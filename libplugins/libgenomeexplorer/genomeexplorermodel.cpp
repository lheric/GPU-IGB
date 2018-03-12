#include "genomeexplorermodel.h"
#include "calculation_backend/AnovaTestClient/clientdriver.h"
#include "algorithm/commonutils.h"
#include <QDebug>

SINGLETON_PATTERN_IMPLIMENT(GenomeExplorerModel)

#define FREE_IF_NOT_NULL(ptr) \
    if(ptr != nullptr) \
    { \
    delete ptr; \
    ptr = nullptr; \
    }

GenomeExplorerModel::GenomeExplorerModel(QObject *parent) : QObject(parent)
{
    this->setGMBGData(nullptr);
    this->setLabels(nullptr);
    this->setSNPList(nullptr);
    this->setSNPSubject(nullptr);
    this->setVoxelSNP(nullptr);
    this->setVoxelList(nullptr);
    this->setVoxelSubject(nullptr);
    this->setDataUploaded(false);

    CSocket::Initialize();  /// init socket
}

bool GenomeExplorerModel::connectToServer(const QString &strIPAddr, const QString &strPort)
{
    int iRet = this->getClientSocket().ConnectToTCPServer(strIPAddr.toLocal8Bit().data(), strPort.toLocal8Bit().data());
    if(iRet == 0)
        emit onConnectedToServer();
    return iRet == 0;   /// 0 means success
}

bool GenomeExplorerModel::doANOVA()
{
    /// check data loaded
    if(this->getVoxelSubject() == nullptr || this->getSNPSubject() == nullptr)
    {
        qCritical() << "data incomplete. load data first.";
        return false;
    }

    /// check if data is uploaded to server
    if(this->getDataUploaded() == false)
    {
        qCritical() << "data havent been uploaded to server yet. click the ready button to upload.";
        return false;
    }

    /// matrix size check. see if both matrix has same number of rows (i.e. subjects)
    if(this->getVoxelSubject()->GetHeight() != this->getSNPSubject()->GetHeight())
    {
        qCritical() << "voxel-subject matrix and SNP-subject matrix do not have the same number of rows.";
        return false;
    }

    int iNumberOfVoxels = this->getNumberOfVoxels();
    int iNumberOfSubjects = this->getNumberOfSubjects();
    int iNumberOfSNPs = this->getNumberOfSNPs();
//    qDebug() << "Number Of Voxels   =" << iNumberOfVoxels;
//    qDebug() << "Number Of Subjects =" << iNumberOfSubjects;
//    qDebug() << "Number Of SNPS     =" << iNumberOfSNPs;



    /// do ANOVA
    bool bResult = DoFunctionANova(
                this->getVoxelSubject()->GetServerDataPtr(),
                this->getSNPSubject()->GetServerDataPtr(),
                this->getVoxelSNP()->GetServerDataPtr(),
                iNumberOfVoxels,
                iNumberOfSubjects,
                iNumberOfSNPs,
                this->getVoxelList()->GetServerDataPtr(),
                this->getVoxelList()->GetNumElements(),
                this->getSNPList()->GetServerDataPtr(),
                this->getSNPList()->GetNumElements(),
                &this->getClientSocket());

    /// results
    if(bResult == false)
    {
        qCritical() << "ANOVA failed";
    }
    else
    {
        qDebug() << "ANOVA succeed";
        this->getVoxelSNP()->CopyFromServer();  /// copy results from server
    }
    return bResult;
}

bool GenomeExplorerModel::disconnectFromServer()
{
    /// TODO
    emit onDisconnectFromServer();
    return false;
}

bool GenomeExplorerModel::loadVoxelSubject(const QString &strFilename)
{
    /// read in csv
    QVector<QVector<float>> cData;
    bool bResult =
            CommonUtils::readCSV<float>(strFilename,
                                        &cData,
                                        [&](const QString& str, float* pValue) -> bool
    {
        bool bResult;
        *pValue = str.toFloat(&bResult);

        return bResult;
    });
    if(bResult == false)
        return false;

    /// sanity check
    int iNumberofSubject = cData.size();
    if(iNumberofSubject == 0)
        return false;
    this->setNumberOfSubjects(iNumberofSubject);

    int iNumberOfVoxel = cData[0].size();
    if(iNumberOfVoxel == 0)
        return false;
    this->setNumberOfVoxels(iNumberOfVoxel);

    qDebug() << "Number of Voxels" << iNumberOfVoxel;
    qDebug() << "Number of Subjects" <<  iNumberofSubject;


    /// create, fill and send voxel-subject data to server
    FREE_IF_NOT_NULL( this->getVoxelSubject() )
            this->setVoxelSubject(new CArray2D_TCP<float>(&this->getClientSocket(), iNumberofSubject, iNumberOfVoxel, false));
    /// fill data structure for sending
    CArray2D_TCP<float>* pcVoxelSubject = this->getVoxelSubject();
    for(int i = 0; i < iNumberofSubject; i++)
    {
        for(int j = 0; j < iNumberOfVoxel; j++)
        {
            (*pcVoxelSubject)(i,j) = cData[i][j];
        }
    }

    this->setDataUploaded(false);
    emit onVoxelSubjectDataChanged();

    return true;
}

bool GenomeExplorerModel::loadSNPSubject(const QString &strFilename)
{
    /// read in csv
    QVector<QVector<int>> cData;
    bool bResult =
            CommonUtils::readCSV<int>(strFilename,
                                      &cData,
                                      [&](const QString& str, int* pValue) -> bool
    {
        bool bResult;
        *pValue = str.toInt(&bResult);

        return bResult;
    });
    if(bResult == false)
        return false;

    /// sanity check
    int iNumberofSubject = cData.size();
    if(iNumberofSubject == 0)
        return false;
    this->setNumberOfSubjects(iNumberofSubject);

    int iNumberOfSNP = cData[0].size();
    if(iNumberOfSNP == 0)
        return false;
    this->setNumberOfSNPs(iNumberOfSNP);

    qDebug() << "Number of SNPs" << iNumberOfSNP;
    qDebug() << "Number of Subjects" <<  iNumberofSubject;

    /// create, fill and send SNP-subject data to server
    FREE_IF_NOT_NULL( this->getSNPSubject() )
    this->setSNPSubject(new CArray2D_TCP<int>(&this->getClientSocket(), iNumberofSubject, iNumberOfSNP, false));

    /// fill data structure for sending
    CArray2D_TCP<int>* pcSNPSubject = this->getSNPSubject();
    for(int i = 0; i < iNumberofSubject; i++)
    {
        for(int j = 0; j < iNumberOfSNP; j++)
        {
            (*pcSNPSubject)(i,j) = cData[i][j];
        }
    }

    this->setDataUploaded(false);
    emit onSNPSubjectDataChanged();

    return true;
}

bool GenomeExplorerModel::uploadVoxelSubjAndSNPSubjToServer()
{
    if(this->getVoxelSubject() != nullptr && this->getSNPSubject()!= nullptr)
    {
        qDebug() << "uploading...";
        this->getVoxelSubject()->CopyToServer();
        this->getSNPSubject()->CopyToServer();
        this->setDataUploaded(true);
        qDebug() << "finished";
        return true;
    }
    return false;
}

bool GenomeExplorerModel::updateVoxelSNPList(const QVector<int> &rcVoxelList, const QVector<int> &rcSNPList)
{
    /// nothing is displayed
    if(rcVoxelList.size() == 0 || rcSNPList.size() == 0)
        return false;


    /// voxel filter list
    FREE_IF_NOT_NULL( this->getVoxelList() )
    this->setVoxelList(new CArray1D_TCP<int>(&this->getClientSocket(), rcVoxelList.size(), true, 0));
    for (int i = 0; i < rcVoxelList.size(); i++)
        (*this->getVoxelList())(i) = rcVoxelList[i];
    this->getVoxelList()->CopyToServer();

    /// SNP filter list
    FREE_IF_NOT_NULL( this->getSNPList() )
    this->setSNPList(new CArray1D_TCP<int>(&this->getClientSocket(), rcSNPList.size(), true, 0));
    for (int i = 0; i < rcSNPList.size(); i++)
        (*this->getSNPList())(i) = rcSNPList[i];
    this->getSNPList()->CopyToServer();

    /// allocate voxel-SNP array for receiving the result
    FREE_IF_NOT_NULL( this->getVoxelSNP() )
    this->setVoxelSNP(new CArray2D_TCP<float>(&this->getClientSocket(),
                                              this->getVoxelList()->GetNumElements(),
                                              this->getSNPList()->GetNumElements(),
                                              false));

    return true;


}

#undef FREE_IF_NOT_NULL
