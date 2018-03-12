#include "multidiseaseconnectivity.h"
#include <QDebug>
MultiDiseaseConnectivity::MultiDiseaseConnectivity(QObject *parent) : QObject(parent)
{

}

MultiDiseaseConnectivity::~MultiDiseaseConnectivity()
{

}

bool MultiDiseaseConnectivity::readFromCSV(const QString &strFilename)
{
    Matrix< double > cConcatenatedMatrix;
    if( !cConcatenatedMatrix.readFromCSV(strFilename,
                                         [](const QString& strRead){return strRead.toDouble();}) )
        return false;

    int iROWNum = cConcatenatedMatrix.getNumRow();
    int iColNum = cConcatenatedMatrix.getNumCol();

    if(iROWNum % iColNum != 0)
    {
        qDebug() << "Incomplete Matrix:" << iROWNum << "X" << iColNum;
        return false;
    }

    int iROINum = cConcatenatedMatrix.getNumCol();
    int iDiseaseNum = iROWNum / iColNum;


    auto& cSrcMatrix = cConcatenatedMatrix.getMatrix();
    auto& cDstMatrix = m_cMatrix.getMatrix();

    cDstMatrix.clear();

    /// allocation
    m_cMatrix.allocate(iROINum, iROINum);

    /// reformat the matrix
    for(int i = 0; i < iDiseaseNum; i++)
    {
        for(int j = 0; j < iROINum; j++)
        {
            for(int k = 0; k < iROINum; k++)
            {
                cDstMatrix[j][k].push_back(cSrcMatrix[i*iROINum+j][k]);
            }
        }
    }

    emit updated();
    return true;
}
