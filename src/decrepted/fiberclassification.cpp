#include "fiberclassification.h"

FiberClassification::FiberClassification(QObject *parent) : QObject(parent)
{
    init();
}

FiberClassification::~FiberClassification()
{

}

void FiberClassification::init()
{
    this->m_dICCThreshold = 0.5;
    this->m_dVarThreshold = 0.5;
    this->m_iCountThreshold = 0;

    this->m_bShowReproducible = false;
    this->m_bShowBackbone = false;
    this->m_bShowNoise = false;
    this->m_bShowUnknown = false;
}

bool FiberClassification::openICCFile(QString strFilename)
{
    QFile cFile(strFilename);
    bool bSuccess = cFile.open(QIODevice::ReadOnly);
    if( !bSuccess )
        return false;

    /// clear
    m_cICC.clear();
    this->m_dMaxICC = -1;
    this->m_dMinICC = INT_MAX;

    QTextStream cStream;
    cStream.setDevice(&cFile);
    int line = 0;
    int iLineEleNum = -1;
    while(!cStream.atEnd())
    {
        /// every line in the data file is the data for one ROI
        line++;
        QStringList cList = cStream.readLine().trimmed().split(',');
        if(iLineEleNum == -1)
            iLineEleNum = cList.size();
        if(iLineEleNum != cList.size())
            qCritical() <<  QString("Reading %1 Failed. Element Number is Different For Line %2:\n Others:%3\n Line %2:%4")
                            .arg(strFilename).arg(line).arg(iLineEleNum).arg(cList.size());

        QVector<double> aiMatrixLine;
        for(int i = 0; i < iLineEleNum; i++)
        {

            bool bOK;
            double dValue = cList[i].toDouble(&bOK);
            if(bOK == false)
                qCritical() << QString("Reading %1 Failed. Non-number Found In Line %2.").arg(strFilename).arg(line);
            aiMatrixLine.push_back(dValue);

            /// update max and min
            if(dValue < m_dMinICC)
                m_dMinICC = dValue;
            if(dValue > m_dMaxICC)
                m_dMaxICC = dValue;

        }
        this->m_cICC.push_back(aiMatrixLine);
    }
    /// check if it is square
    if( iLineEleNum != iLineEleNum )
    {
        qCritical() << QString("Nonsquare (%1x%2) Matrix Is NOT a Valid ICC Matrix.").arg(line).arg(iLineEleNum);
        return false;
    }

    qDebug() << line << "x" << iLineEleNum << "Matrix Read";

    this->update();

    return true;
}

bool FiberClassification::openVarFile(QString strFilename)
{
    QFile cFile(strFilename);
    bool bSuccess = cFile.open(QIODevice::ReadOnly);
    if( !bSuccess )
        return false;

    /// clear
    m_cVar.clear();
    this->m_dMaxVar = -1;
    this->m_dMinVar = INT_MAX;

    QTextStream cStream;
    cStream.setDevice(&cFile);
    int line = 0;
    int iLineEleNum = -1;
    while(!cStream.atEnd())
    {
        /// every line in the data file is the data for one ROI
        line++;
        QStringList cList = cStream.readLine().trimmed().split(',');
        if(iLineEleNum == -1)
            iLineEleNum = cList.size();
        if(iLineEleNum != cList.size())
            qCritical() <<  QString("Reading %1 Failed. Element Number is Different For Line %2:\n Others:%3\n Line %2:%4")
                            .arg(strFilename).arg(line).arg(iLineEleNum).arg(cList.size());

        QVector<double> aiMatrixLine;
        for(int i = 0; i < iLineEleNum; i++)
        {

            bool bOK;
            double dValue = cList[i].toDouble(&bOK);
            if(bOK == false)
                qCritical() << QString("Reading %1 Failed. Non-number Found In Line %2.").arg(strFilename).arg(line);
            aiMatrixLine.push_back(dValue);

            /// update max and min
            if(dValue < m_dMinVar)
                m_dMinVar = dValue;
            if(dValue > m_dMaxVar)
                m_dMaxVar = dValue;

        }
        this->m_cVar.push_back(aiMatrixLine);
    }
    /// check if it is square
    if( iLineEleNum != iLineEleNum )
    {
        qCritical() << QString("Nonsquare (%1x%2) Matrix Is NOT a Valid Variance Matrix.").arg(line).arg(iLineEleNum);
        return false;
    }

    qDebug() << line << "x" << iLineEleNum << "Matrix Read";

    this->update();

    return false;
}

bool FiberClassification::openCountFile(QString strFilename)
{
    QFile cFile(strFilename);
    bool bSuccess = cFile.open(QIODevice::ReadOnly);
    if( !bSuccess )
        return false;

    /// clear
    m_cCount.clear();
    this->m_iMaxCount = -1;
    this->m_iMinCount = INT_MAX;

    QTextStream cStream;
    cStream.setDevice(&cFile);
    int line = 0;
    int iLineEleNum = -1;
    while(!cStream.atEnd())
    {
        /// every line in the data file is the data for one ROI
        line++;
        QStringList cList = cStream.readLine().trimmed().split(',');
        if(iLineEleNum == -1)
            iLineEleNum = cList.size();
        if(iLineEleNum != cList.size())
            qCritical() <<  QString("Reading %1 Failed. Element Number is Different For Line %2:\n Others:%3\n Line %2:%4")
                            .arg(strFilename).arg(line).arg(iLineEleNum).arg(cList.size());

        QVector<int> aiMatrixLine;
        for(int i = 0; i < iLineEleNum; i++)
        {

            bool bOK;
            int iValue = cList[i].toInt(&bOK);
            if(bOK == false)
                qCritical() << QString("Reading %1 Failed. Non-number Found In Line %2.").arg(strFilename).arg(line);
            aiMatrixLine.push_back(iValue);

            /// update max and min
            if(iValue < m_iMinCount)
                m_iMinCount = iValue;
            if(iValue > m_iMaxCount)
                m_iMaxCount = iValue;

        }
        this->m_cCount.push_back(aiMatrixLine);
    }
    /// check if it is square
    if( iLineEleNum != iLineEleNum )
    {
        qCritical() << QString("Nonsquare (%1x%2) Matrix Is NOT a Valid Variance Matrix.").arg(line).arg(iLineEleNum);
        return false;
    }

    qDebug() << line << "x" << iLineEleNum << "Matrix Read";

    this->update();

    return false;
}

void FiberClassification::setICCThreshold(double dICCThreshold)
{
    this->m_dICCThreshold = dICCThreshold;
    this->update();
}

void FiberClassification::setVarThreshold(double dVarThreshold)
{
    this->m_dVarThreshold = dVarThreshold;
    this->update();
}

void FiberClassification::setCountThreshold(int iCountThreshold)
{
    this->m_iCountThreshold = iCountThreshold;
    this->update();
}

bool FiberClassification::queryConnection(int iROILabelStart, int iROILabelEnd)
{
    auto eFiberType = CONNECT_UNKNOWN;

    int iMatrixSize = m_cConnectType.size();

    if(iROILabelStart <= 0 || iROILabelEnd <= 0)    /// for fibers do not connect two ROIs
    {
        eFiberType = CONNECT_UNKNOWN;
    }
    else if(iROILabelStart > iMatrixSize || iROILabelEnd > iMatrixSize)    /// for fibers ROI labe out of range
    {
        qCritical() << "ROI Label Overflow. Start ROI:" << iROILabelStart << "End ROI:" << iROILabelEnd;
        eFiberType = CONNECT_UNKNOWN;
    }
    else
    {
        eFiberType = m_cConnectType[iROILabelStart-1][iROILabelEnd-1];
    }

    switch( eFiberType )
    {
    case CONNECT_UNKNOWN:
        return getShowUnknown();
    case CONNECT_BACKBONE:
        return getShowBackbone();
    case CONNECT_REPRODUCIBLE:
        return getShowReproducible();
    case CONNECT_NOISE:
        return getShowNoise();
    default:
        return true;
    }
}


//bool FiberClassification::queryByFiberIdx(int iIdx)
//{
//    /// check if index out of range
//    int iMatrixSize = m_cConnectType.size();
//    int iConnectionTableSize = m_cConnectedROIs.size();
//    ConnectionType eFiberType = CONNECT_UNKNOWN;

//    if(iIdx >= iConnectionTableSize)
//    {
//        qCritical() << "Fiber Query Index Out of Range:" << iIdx << ", Max:" << iConnectionTableSize;
//        eFiberType = CONNECT_UNKNOWN;
//    }
//    else
//    {
//        int iStartROI = m_cConnectedROIs[iIdx].getStartROI();
//        int iEndROI = m_cConnectedROIs[iIdx].getEndROI();

//        if(iStartROI <= 0 || iEndROI <= 0)    /// for fibers do not connect two ROIs
//        {
//            eFiberType = CONNECT_UNKNOWN;
//        }
//        else if(iStartROI > iMatrixSize || iEndROI > iMatrixSize)    /// for fibers ROI labe out of range
//        {
//            qCritical() << "ROI Label Overflow. Start ROI:" << iStartROI << "End ROI:" << iEndROI;
//            eFiberType = CONNECT_UNKNOWN;
//        }
//        else
//        {
//            eFiberType = m_cConnectType[iStartROI-1][iEndROI-1];
//        }
//    }

//    switch( eFiberType )
//    {
//    case CONNECT_UNKNOWN:
//        return getShowUnknown();
//    case CONNECT_BACKBONE:
//        return getShowBackbone();
//    case CONNECT_REPRODUCIBLE:
//        return getShowReproducible();
//    case CONNECT_NOISE:
//        return getShowNoise();
//    default:
//        return true;
//    }

//}

void FiberClassification::update()
{
    /// clear
    m_cConnectType.clear();

    /// check if we have loaded both ICC and variance matrix.
    int iICCMatrixSize = m_cICC.size();
    int iVarMatrixSize = m_cVar.size();
    int iCountMatrixSize = m_cCount.size();
    if( iICCMatrixSize == 0 ||
        iVarMatrixSize == 0 ||
        iCountMatrixSize == 0 ||
        iICCMatrixSize != iVarMatrixSize ||
        iICCMatrixSize != iCountMatrixSize )
        return;

    for(int i = 0; i < iICCMatrixSize; i++)
    {
        QVector<ConnectionType> cMatrixRow;
        ConnectionType eType = CONNECT_UNKNOWN;
        for(int j = 0; j < iICCMatrixSize; j++)
        {
            if(m_cCount[i][j] >= getCountThreshold() )
            {
                if (m_cICC[i][j] > getICCThreshold())
                    eType = CONNECT_REPRODUCIBLE;
                else if(m_cVar[i][j] < getVarThreshold())
                    eType = CONNECT_BACKBONE;
                else
                    eType = CONNECT_NOISE;
            }
            else
            {
                eType = CONNECT_UNKNOWN;
            }
            cMatrixRow.push_back(eType);
        }
        m_cConnectType.push_back(cMatrixRow);
    }

    qDebug() << "Connection Type Matrix Update Complete";

}

bool FiberClassification::isReady()
{
    return  !this->getICC().empty() &&
            !this->getVar().empty() &&
            !this->getCount().empty();
}
