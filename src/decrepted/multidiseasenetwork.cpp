#include "multidiseasenetwork.h"
#include "labelsurface.h"
#include "multidiseaseconnectivity.h"
#include "algorithm/colorconverter.h"
#include <QDebug>

MultiDiseaseNetwork::MultiDiseaseNetwork(QObject *parent) : QObject(parent)
{
    init();
}

MultiDiseaseNetwork::~MultiDiseaseNetwork()
{

}

void MultiDiseaseNetwork::init()
{

    m_pcLabelSurface = NULL;
    m_pcMDConnectivity = NULL;
    m_iCurveHeight = 50;
    m_dThreshold = 0.85;
    m_bEnable = false;

    this->setVisibility(m_bEnable);
}

void MultiDiseaseNetwork::setLabelSurface(LabelSurface *pcLabelSurface)
{
    m_pcLabelSurface = pcLabelSurface;
    this->connect(pcLabelSurface, SIGNAL(updated()), this, SLOT(buildNetwork()));
}

void MultiDiseaseNetwork::setMDConnectivity(MultiDiseaseConnectivity *pcMDConnectivity)
{
    m_pcMDConnectivity = pcMDConnectivity;
    this->connect(pcMDConnectivity, SIGNAL(updated()), this, SLOT(buildNetwork()));
}

void MultiDiseaseNetwork::setThreshold(double dThreshold)
{
    this->m_dThreshold = dThreshold;
    /// rebuild network
    this->buildNetwork();
}

void MultiDiseaseNetwork::setVisibility(bool bVisible)
{
    this->getNetwork().setVisibility(bVisible);
}

void MultiDiseaseNetwork::addActorsToRenderer(vtkRenderer *pcRenderer)
{
    this->getNetwork().addActorsToRenderer(pcRenderer);
}

void MultiDiseaseNetwork::removeActorsFromRenderer(vtkRenderer *pcRenderer)
{
    this->getNetwork().removeActorsFromRenderer(pcRenderer);
}

void MultiDiseaseNetwork::setEnable(bool bEnabled)
{
    m_bEnable = bEnabled;
    this->buildNetwork();
    this->setVisibility(bEnabled);
}

void MultiDiseaseNetwork::xGetConnectionColor(QVector<double>& adValues, unsigned char *auhColor)
{
    if(adValues.size() != 3)
    {
        auhColor[0] = 255;
        auhColor[1] = 255;
        auhColor[2] = 255;
        auhColor[3] = 255;
        qWarning() << "Can only handle 3 disease for now";
        return;
    }

    double dSum = adValues[0] + adValues[1] + adValues[2];

    if(dSum < 0.01)
    {
        auhColor[0] = 255;
        auhColor[1] = 255;
        auhColor[2] = 255;
        auhColor[3] = 255;
    }
    else
    {


        QColor cRgb = ColorConverter::ryb2rgb(adValues[0]/dSum, adValues[1]/dSum, adValues[2]/dSum);

        auhColor[0] = cRgb.red();
        auhColor[1] = cRgb.green();
        auhColor[2] = cRgb.blue();
        auhColor[3] = 255;
    }
}


bool MultiDiseaseNetwork::xBiasCheck(QVector<double>& adValues)
{
    static const QVector3D cZero(1.0/3, 1.0/3, 1.0/3);
    static const double dNormalizeFactor = 0.8165;  /// sqrt(6)/3

    if(adValues.size() != 3)
    {
        qWarning() << "Can only handle 3 disease for now";
        return false;
    }
    QVector3D cValue( adValues[0], adValues[1], adValues[2] );
    if(cValue.length() < 0.01)
        return false;
    double dSum = cValue[0] + cValue[1] + cValue[2];
    cValue /= dSum;

    if( cValue.distanceToPoint(cZero)/dNormalizeFactor < this->getThreshold() )
    {
        return false;
    }
    return true;
}


void MultiDiseaseNetwork::buildNetwork()
{

    if( m_pcLabelSurface == NULL || m_pcMDConnectivity == NULL )
    {
        return;
    }

    if( m_bEnable == false )
        return;

    Matrix<QVector<QVector3D>> cCtrlPntMatrix;
    Matrix<QColor> cColorMatrix;

    QVector<ROI*> apcROIs = m_pcLabelSurface->getROIs();
    int iNumROI = apcROIs.size();

    if(this->getMDConnectivity()->getMatrix().getSize() != QSize(iNumROI, iNumROI))
    {
        qDebug() << "Disease Connectivity Matrix is" << this->getMDConnectivity()->getMatrix().getSize() << ", but there are" << iNumROI << "ROIs.";
        return;
    }


    cCtrlPntMatrix.allocate(iNumROI,iNumROI);
    cColorMatrix.allocate(iNumROI,iNumROI);



    /// only fill the upper right triangle
    for(int i = 0; i < iNumROI-1; i++)
    {
        ROI* pcStartROI = apcROIs[i];
        double* adStartCenter = pcStartROI->getActor()->GetCenter();
        QVector3D cStartPos(adStartCenter[0],
                            adStartCenter[1],
                            adStartCenter[2]);
        QVector3D cStartNorm = pcStartROI->getTextureNormal().normalized();

        for(int j = i+1; j < iNumROI; j++)
        {
            /// skip
            QVector<double>& adElement = this->getMDConnectivity()->getMatrix()[i][j];
            if(!xBiasCheck(adElement))
            {
                //qDebug() << "#" << i << j << "Skipped" << dCorrelation;
                continue;
            }
            /// set color for this curve
            static unsigned char auhCurveColor[3];
            xGetConnectionColor(adElement, auhCurveColor);
            cColorMatrix[i][j] = QColor(auhCurveColor[0], auhCurveColor[1], auhCurveColor[2]);

            /// set control points for this curve
            ROI* pcEndROI = apcROIs[j];
            double* adEndCenter = pcEndROI->getActor()->GetCenter();

            QVector3D cEndPos(  adEndCenter[0],
                                adEndCenter[1],
                                adEndCenter[2]);

            m_iCurveHeight = (cEndPos - cStartPos).length();

            QVector3D cStartHandle = cStartPos + cStartNorm*m_iCurveHeight;

            QVector3D cEndNorm = pcEndROI->getTextureNormal().normalized();
            QVector3D cEndHandle = cEndPos + cEndNorm*m_iCurveHeight;

            QVector3D cMidPoint = (cStartHandle + cEndHandle)/2 + (cStartNorm+cEndNorm).normalized()*0.8*m_iCurveHeight;


            cCtrlPntMatrix[i][j].push_back(cStartPos);
            cCtrlPntMatrix[i][j].push_back(cStartHandle);
            cCtrlPntMatrix[i][j].push_back(cMidPoint);
            cCtrlPntMatrix[i][j].push_back(cEndHandle);
            cCtrlPntMatrix[i][j].push_back(cEndPos);



        }
    }


    this->getNetwork().setCtrlPntMatrix(cCtrlPntMatrix);
    this->getNetwork().setColorMatrix(cColorMatrix);
    this->getNetwork().buildNetwork();


    emit updated();
}
