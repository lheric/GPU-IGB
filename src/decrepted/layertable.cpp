#include "layertable.h"
#include "ray.h"

LayerTable::LayerTable(QObject *parent) : QObject(parent)
{

    m_cLayerToLabel.push_back(QSet<int>{1,2,3,4,5,6,7,8,9,10,16,17,18,19,21,22,23,24,25,26,27,28,29,30,31,32,33,38,39,41,42,43,44,45,46,47,48,49,50,51,57,58,59,60,62,63,64,65,66,67,68,69,70,71,72,73,74,79,80,82,83});
    m_cLayerToLabel.push_back(QSet<int>{11,12,13,20,34,36,40,52,53,54,61,75,77,81});
    m_cLayerToLabel.push_back(QSet<int>{14,15,37,55,56,78});
    m_cLayerToLabel.push_back(QSet<int>{35,76});


    for(int iLayer = 0; iLayer < m_cLayerToLabel.size(); iLayer++)
    {
        for(int iLabel: m_cLayerToLabel[iLayer])
        {
            m_cLabelToLayer[iLabel] = iLayer;
        }
    }
}

LayerTable::~LayerTable()
{

}

void LayerTable::resize(int iNumLabel)
{
    this->m_cMatrix.allocate(iNumLabel, iNumLabel);
}

void LayerTable::analyzeRay(Ray *pcRay)
{
    QVector<RaySamplePoint>& acSamplePnts = pcRay->getSamplePoints();
    Matrix<int>& rcMatrix = this->getMatrix();
    int iLastLabel = 0;
    for(int i = 0; i < acSamplePnts.size(); i++)
    {
        RaySamplePoint& rcSamplePnt = acSamplePnts[i];

        int iLabel = rcSamplePnt.getLabel();

        if(iLabel == -1 || iLabel == 0 || iLabel == iLastLabel || rcSamplePnt.getNumLabel() > 1)
            continue;

        rcMatrix[iLabel][iLastLabel]++;
        iLastLabel = iLabel;
    }
}

int LayerTable::getLayerForLabel(int iLabel, double* pdPercent)
{
    Matrix<int>& rcMatrix = this->getMatrix();

    if(iLabel >= rcMatrix.getNumRow() || iLabel < 0)
    {
        qWarning() << "Label underflow/overflow in ROI Layer Table:" << iLabel;
        return -1;
    }

    int iLayer = -1;
    int iMax = 0;
    int iSum = 0;
    for(int i = 0; i < rcMatrix.getNumCol(); i++)
    {
        int iOccurance = rcMatrix[iLabel][i];
        iSum += iOccurance;
        if(iOccurance > iMax)
        {
            iLayer = i;
            iMax = iOccurance;
        }
    }
    if(pdPercent != NULL)
        *pdPercent = iMax*1.0/iSum;
    return iLayer;
}

//void LayerTable::buildTable()
//{
//    m_cLabelToLayer.clear();
//    m_cLayerToLabel.clear();

//    int iNumLabel = m_cMatrix.getNumCol();
//    for(int iLabel = 1; iLabel <iNumLabel; iLabel++)
//    {
//        double dPercent;
//        int iLayer = getLayerForLabel(iLabel, &dPercent);

//        if(iLayer != -1)
//        {
//            m_cLabelToLayer[iLabel] = iLayer;
//            m_cLayerToLabel[iLayer].insert(iLabel);
//        }

//    }


//}

void LayerTable::print()
{
    for(int iLayer = 0; iLayer < m_cLayerToLabel.size(); iLayer++)
    {
        qDebug() << "Layer" << iLayer << ":" << m_cLayerToLabel[iLayer];
    }
}

void LayerTable::hightlightLayer(int iLayer)
{
    static double adLayerTrans[] = {1.0, 0.035, 0.017, 0.015};

    QSet<int>* pcHighlightedLabels = &m_cLayerToLabel[iLayer];

    /// copy
    for(int i = 0; i < COLOR_TABLE_SIZE; i++)
    {
        if( iLayer==-1 || pcHighlightedLabels->contains(i))
        {
            g_acColorTableForRaycastHighlight[i] = g_acColorTableForRaycast[i];
            g_acColorTableForLightMap[i] = QVector4D(1,1,1,1);
        }
        else
        {
            g_acColorTableForRaycastHighlight[i] = g_acColorTableForRaycast[i];
            g_acColorTableForRaycastHighlight[i].setW(adLayerTrans[iLayer]);
            g_acColorTableForLightMap[i] = QVector4D(0,0,0,0); /// TODO
        }
    }

}

