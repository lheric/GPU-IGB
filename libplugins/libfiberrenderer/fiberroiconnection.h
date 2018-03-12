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


    ADD_CLASS_FIELD_NOSETTER(QVector<ConnectedROIs>, cConnectedROIs, getFiberConnectionInfo)
};

#endif // FIBERROICONNECTION_H
