#ifndef GENOMEEXPLORERMODEL_H
#define GENOMEEXPLORERMODEL_H

#include <QObject>
#include <QRect>
#include <QPoint>
#include <QVector>
#include <vtkImageData.h>
#include "gitldef.h"
#include "model/parcellationlabels.h"
#include "calculation_backend/Common/Timer.h"
#include "calculation_backend/Common/Sockets.h"
#include "calculation_backend/Common/CArray1D.h"
#include "calculation_backend/Common/CArray1DTCP.h"
#include "calculation_backend/Common/CArray2D.h"
#include "calculation_backend/Common/CArray2DTCP.h"
#include "calculation_backend/AnovaTestClient/clientdriver.h"


class GenomeExplorerModel : public QObject
{
    Q_OBJECT
    SINGLETON_PATTERN_DECLARE(GenomeExplorerModel)
private:
    explicit GenomeExplorerModel(QObject *parent = 0);

public:
    bool connectToServer(const QString& strIPAddr,  const QString& strPort = DEFAULT_PORT);
    bool doANOVA();
    bool disconnectFromServer();
    bool loadVoxelSubject(const QString& strFilename);
    bool loadSNPSubject(const QString& strFilename);
    bool uploadVoxelSubjAndSNPSubjToServer();
    bool updateVoxelSNPList(const QVector<int>& rcVoxelList, const QVector<int>& rcSNPList);

signals:
    void onConnectedToServer();
    void onDoANOVAComplete();
    void onDisconnectFromServer();
    void onGMBGDataChanged();
    void onVoxelSubjectDataChanged();
    void onSNPSubjectDataChanged();
    void onVoxelSNPDataChanged();
    void onVisibleRectChanged();
    void onSelectedPointChanged();
    void onHoveringPointChanged();
    void onDataUploaded();
    void onSetOpaqueMode(bool);
    void onOpacityChanged(double);

public slots:
    ADD_CLASS_FIELD(ParcellationLabels*, pcLabels, getLabels, setLabels)    /// not used for now
    ADD_CLASS_FIELD(vtkImageData*, pcGMBGData, getGMBGData, setGMBGData)    /// MRI gray matter background image for volume rendering in brain explorer
    ADD_CLASS_FIELD(CSocket, cClientSocket, getClientSocket, setClientSocket)   /// Client socket for connecting CPU/GPU server

    ADD_CLASS_FIELD(int, iNumberOfSubjects, getNumberOfSubjects,    setNumberOfSubjects)    /// number of subjects
    ADD_CLASS_FIELD(int, iNumberOfVoxels,   getNumberOfVoxels,      setNumberOfVoxels)      /// number of voxels
    ADD_CLASS_FIELD(int, iNumberOfSNPs,     getNumberOfSNPs,        setNumberOfSNPs)        /// number of SNPs

    ADD_CLASS_FIELD(CArray2D_TCP<float>*,   pcVoxelSubject, getVoxelSubject,    setVoxelSubject)/// Voxel-Subject table
    ADD_CLASS_FIELD(CArray2D_TCP<int>*,     pcSNPSubject,   getSNPSubject,      setSNPSubject)  /// SNP-Subject table
    ADD_CLASS_FIELD(CArray2D_TCP<float>*,   pcVoxelSNP,     getVoxelSNP,        setVoxelSNP)    /// Voxel-SNP result
    ADD_CLASS_FIELD(CArray1D_TCP<int>*,     pcVoxelList,    getVoxelList,       setVoxelList)   /// Voxel filter
    ADD_CLASS_FIELD(CArray1D_TCP<int>*,     pcSNPList,      getSNPList,         setSNPList)     /// SNP filter
    ADD_CLASS_FIELD(bool, bDataUploaded, getDataUploaded, setDataUploaded)                      /// Indicate if the Voxel-Subject & SNP-Subject data have been uploaded to server

    ADD_CLASS_FIELD(QVector<int>, cVisibleRect, getVisibleCols, setVisibleCols)    /// the part of the Voxel-SNP matrix that is currently visible
    ADD_CLASS_FIELD(QVector<int>, cVisibleRows, getVisibleRows, setVisibleRows)
    ADD_CLASS_FIELD(QPoint, cSelectedPoint, getSelectedPoint, setSelectedPoint) /// index of the element in the Voxel-SNP matrix that is currently selected by user

    ADD_CLASS_FIELD_NOSETTER(QPoint, cHoveringPoint, getHoveringPoint) /// index of the element in the Voxel-SNP matrix that is currently hovering by user
public:
    void setHoveringPoint(const QPoint& cPoint)
    {
        m_cHoveringPoint = cPoint;
        emit onHoveringPointChanged();
    }
};

#endif // GENOMEEXPLORERMODEL_H
