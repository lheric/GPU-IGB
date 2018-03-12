#ifndef FIBERROICONNECTION_H
#define FIBERROICONNECTION_H
#include "gitldef.h"
#include <QVector>

/**
 * @brief The ConnectedROIs class connected ROIs info. for each fiber
 */
class ConnectedROIs
{
public:
    ConnectedROIs() {m_iStartROI = -1; m_iEndROI = -1;}

    bool operator == (const ConnectedROIs& other) const
    {
        return  (m_iStartROI == other.m_iStartROI && m_iEndROI == other.m_iEndROI) ||
                (m_iStartROI == other.m_iEndROI && m_iEndROI == other.m_iStartROI);
    }

    bool isConnecting(int iROIIdx)
    {
        return (m_iStartROI == iROIIdx || m_iEndROI == iROIIdx);
    }

    ADD_CLASS_FIELD(int, iStartROI, getStartROI, setStartROI)
    ADD_CLASS_FIELD(int, iEndROI, getEndROI, setEndROI)

};



/**
 * @brief The FiberROIConnection class
 */
class FiberROIConnection
{
public:
    FiberROIConnection();
    ~FiberROIConnection();
    void init();

    bool openConnectionInfo(QString strFilename);
//    bool openICCFile(QString strFilename);
//    bool openVarFile(QString strFilename);
//    bool openCountFile(QString strFilename);
//    void setICCThreshold(double dICCThreshold);
//    void setVarThreshold(double dVarThreshold);
//    void setCountThreshold(int iCountThreshold);
//    bool queryByFiberIdx(int iIdx);
//    void update();
//    bool isReady();


    ADD_CLASS_FIELD_NOSETTER(QVector<ConnectedROIs>, cConnectedROIs, getFiberConnectionInfo)

//    ADD_CLASS_FIELD_NOSETTER(QVector<QVector<double> >, cICC, getICC)
//    ADD_CLASS_FIELD(double, dMinICC, getMinICC, setMinICC)
//    ADD_CLASS_FIELD(double, dMaxICC, getMaxICC, setMaxICC)

//    ADD_CLASS_FIELD_NOSETTER(QVector<QVector<double> >, cVar, getVar)
//    ADD_CLASS_FIELD(double, dMinVar, getMinVar, setMinVar)
//    ADD_CLASS_FIELD(double, dMaxVar, getMaxVar, setMaxVar)

//    ADD_CLASS_FIELD_NOSETTER(QVector<QVector<int> >,    cCount, getCount)
//    ADD_CLASS_FIELD(int, iMinCount, getMinCount, setMinCount)
//    ADD_CLASS_FIELD(int, iMaxCount, getMaxCount, setMaxCount)


//    ADD_CLASS_FIELD_NOSETTER(QVector<QVector<ConnectionType> >, cConnectType, getConnectType)
//    ADD_CLASS_FIELD_NOSETTER(double, dICCThreshold, getICCThreshold)
//    ADD_CLASS_FIELD_NOSETTER(double, dVarThreshold, getVarThreshold)
//    ADD_CLASS_FIELD_NOSETTER(int,    iCountThreshold, getCountThreshold)

//    ADD_CLASS_FIELD(bool, bShowReproducible, getShowReproducible, setShowReproducible)
//    ADD_CLASS_FIELD(bool, bShowBackbone, getShowBackbone, setShowBackbone)
//    ADD_CLASS_FIELD(bool, bShowNoise, getShowNoise, setShowNoise)
//    ADD_CLASS_FIELD(bool, bShowUnknown, getShowUnknown, setShowUnknown)

};

#endif // FIBERROICONNECTION_H
