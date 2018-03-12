#include "functionalconnectivitynetwork.h"
#include "labelsurface.h"
#include "functionalconnectivitydata.h"
#include "rom.h"

FunctionalConnectivityNetwork::FunctionalConnectivityNetwork(QObject *parent) : QObject(parent)
{
    init();
}

FunctionalConnectivityNetwork::~FunctionalConnectivityNetwork()
{

}

void FunctionalConnectivityNetwork::init()
{
    m_pcLabelSurface = NULL;
    m_pcFCData = NULL;
    m_iCurveHeight = 50;
    m_dThreshold = 0.85;
    m_bEnable = false;

    m_pcPosValLUT = vtkLookupTable::New();
    m_pcNegValLUT = vtkLookupTable::New();
    m_pcROICentroidLUT = vtkLookupTable::New();
    m_pcPosBarActor = vtkScalarBarActor::New();
    m_pcNegBarActor = vtkScalarBarActor::New();

    this->getROICentroidLUT()->SetHueRange(0.125, 1);    ///all color range
    this->getROICentroidLUT()->SetSaturationRange(1.0,1.0);
    this->getROICentroidLUT()->SetValueRange(1.0,1.0);
    this->getROICentroidLUT()->SetTableRange(g_indgrees_100631_min, g_indgrees_100631_max);
    this->getROICentroidLUT()->Build();

    this->getPosValLUT()->SetHueRange(0, 0);    ///red
    this->getPosValLUT()->SetSaturationRange(0.35,1.0);
    this->getPosValLUT()->SetValueRange(1.0,1.0);
    this->getPosValLUT()->SetTableRange(getThreshold(), 1);
    this->getPosValLUT()->Build();

    this->getPosBarActor()->SetLookupTable(this->getPosValLUT());
    this->getPosBarActor()->SetTitle("  ");
    this->getPosBarActor()->SetLabelFormat("%.2f");
    this->getPosBarActor()->SetNumberOfLabels(4);
    this->getPosBarActor()->SetHeight(0.50);
    this->getPosBarActor()->SetWidth(0.07);
    this->getPosBarActor()->SetPosition(0.90, 0.25);
    this->getPosBarActor()->SetOrientationToVertical();


    this->getNegValLUT()->SetHueRange(0.67, 0.67);    ///blue
    this->getNegValLUT()->SetSaturationRange(1.0,0.35);
    this->getNegValLUT()->SetValueRange(1.0,1.0);
    this->getNegValLUT()->SetTableRange(-1, -getThreshold());
    this->getNegValLUT()->Build();

    this->getNegBarActor()->SetLookupTable(this->getNegValLUT());
    //this->getNegBarActor()->SetLookupTable(this->getROICentroidLUT());  ////// for Shen Li's proposal only TODO
    this->getNegBarActor()->SetTitle("  ");
    this->getNegBarActor()->SetLabelFormat("%.2f");
    this->getNegBarActor()->SetNumberOfLabels(4);
    this->getNegBarActor()->SetHeight(0.50);
    this->getNegBarActor()->SetWidth(0.08);
    this->getNegBarActor()->SetPosition(0.80, 0.25);
    this->getNegBarActor()->SetOrientationToVertical();




    this->setVisibility(m_bEnable);
}

void FunctionalConnectivityNetwork::setLabelSurface(LabelSurface *pcLabelSurface)
{
    m_pcLabelSurface = pcLabelSurface;
    this->connect(pcLabelSurface, SIGNAL(updated()), this, SLOT(buildNetwork()));
}

void FunctionalConnectivityNetwork::setFCData(FunctionalConnectivityData *pcFCData)
{
    m_pcFCData = pcFCData;
    this->connect(pcFCData, SIGNAL(updated()), this, SLOT(buildNetwork()));
}

void FunctionalConnectivityNetwork::setThreshold(double dThreshold)
{
    this->m_dThreshold = dThreshold;
    /// update LUT
    this->getPosValLUT()->SetTableRange(getThreshold(), 1);
    this->getNegValLUT()->SetTableRange(-1, -getThreshold());
    /// rebuild network
    this->buildNetwork();
}

void FunctionalConnectivityNetwork::setVisibility(bool bVisible)
{
    this->getNetwork().setVisibility(bVisible);
    this->getPosBarActor()->SetVisibility(bVisible);
    this->getNegBarActor()->SetVisibility(bVisible);
}

void FunctionalConnectivityNetwork::addActorsToRenderer(vtkRenderer *pcRenderer)
{
    this->getNetwork().addActorsToRenderer(pcRenderer);
    pcRenderer->AddActor(this->getPosBarActor());
    pcRenderer->AddActor(this->getNegBarActor());
}

void FunctionalConnectivityNetwork::removeActorsFromRenderer(vtkRenderer *pcRenderer)
{
    this->getNetwork().removeActorsFromRenderer(pcRenderer);
    pcRenderer->RemoveActor(this->getPosBarActor());
    pcRenderer->RemoveActor(this->getNegBarActor());
}

void FunctionalConnectivityNetwork::setEnable(bool bEnabled)
{
    m_bEnable = bEnabled;
    this->buildNetwork();
    this->setVisibility(bEnabled);
}


void FunctionalConnectivityNetwork::xGetConnectionColor(double dValue, unsigned char *auhColor)
{
    unsigned char* auhMappedColor = NULL;
    if(dValue < 0)
        auhMappedColor = this->getNegValLUT()->MapValue(dValue);
    else
        auhMappedColor = this->getPosValLUT()->MapValue(dValue);

    auhColor[0] = auhMappedColor[0];
    auhColor[1] = auhMappedColor[1];
    auhColor[2] = auhMappedColor[2];
}

void FunctionalConnectivityNetwork::buildNetwork()
{
    if( m_pcLabelSurface == NULL || m_pcFCData == NULL )
    {
        return;
    }

    if( m_bEnable == false )
        return;

    Matrix<QVector<QVector3D>> cCtrlPntMatrix;
    Matrix<QColor> cColorMatrix;

    QVector<ROI*> apcROIs = m_pcLabelSurface->getROIs();
    int iNumROI = apcROIs.size();

    if(this->getFCData()->getMatrix().getSize() != QSize(iNumROI, iNumROI))
    {
        qDebug() << "FC Matrix is" << this->getFCData()->getMatrix().getSize() << ", but there are" << iNumROI << "ROIs.";
        return;
    }


    cCtrlPntMatrix.allocate(iNumROI,iNumROI);
    cColorMatrix.allocate(iNumROI,iNumROI);


    /// only fill the upper right triangle
    for(int i = 0; i < iNumROI; i++)
    {
        ROI* pcStartROI = apcROIs[i];
        double* adStartCenter = pcStartROI->getActor()->GetCenter();
        QVector3D cStartPos(adStartCenter[0],
                            adStartCenter[1],
                            adStartCenter[2]);
        QVector3D cStartNorm = pcStartROI->getTextureNormal().normalized();

        for(int j = i; j < iNumROI; j++)
        {
            /// diagonal
            if( j == i)
            {
                cCtrlPntMatrix[i][j].push_back(cStartPos);
                const double *adColor = g_getColorFromTable(i);
                cColorMatrix[i][j] = QColor(adColor[0], adColor[1], adColor[2]);

                /// for Shen Li's proposal only TODO
                //const unsigned char* auhColor = this->getROICentroidLUT()->MapValue(g_indegrees_100631_scale33[i-1]);
                //cColorMatrix[i][j] = QColor(auhColor[0], auhColor[1], auhColor[2]);
                continue;
            }


            /// skip
            double dCorrelation = this->getFCData()->getMatrix()[i][j];
            if(qAbs(dCorrelation) < this->getThreshold())
            {
                //qDebug() << "#" << i << j << "Skipped" << dCorrelation;
                continue;
            }
            /// set color for this curve
            static unsigned char auhCurveColor[3];
            xGetConnectionColor(dCorrelation, auhCurveColor);
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

