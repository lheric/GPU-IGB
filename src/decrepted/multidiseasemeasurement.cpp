#include "multidiseasemeasurement.h"
#include "algorithm/csvmatrixreader.h"
#include <QDebug>
MultiDiseaseMeasurement::MultiDiseaseMeasurement(QObject *parent) : QObject(parent)
{
    init();
}

void MultiDiseaseMeasurement::init()
{
    this->m_cMatrix.init();
}

bool MultiDiseaseMeasurement::openMutiDiseasMeasurement(const QString &strFilename)
{
    bool bSuccess = m_cMatrix.readFromCSV(strFilename, [](const QString& strRead){return strRead.toDouble();});
    if(bSuccess == true)
        emit updated();
    return bSuccess;
}

QVector<double> MultiDiseaseMeasurement::getMutiDiseaseDataForROI(int iROILabel)
{
    if(iROILabel-1 < 0)
    {
        qCritical() << "Time ROI Label Underflow.";
    }
    if(iROILabel-1 >= m_cMatrix.getNumRow())
    {
        qCritical() << "Time ROI Label Overflow.";
    }    
    return m_cMatrix[iROILabel-1];
}
