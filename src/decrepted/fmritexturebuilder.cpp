#include "fmritexturebuilder.h"
#include "labelsurface.h"

#include <QDebug>
#include "algorithm/circletexturegenerator.h"
#include "algorithm/stripetexturegenerator.h"
#include "algorithm/contourtexturegenerator.h"
#include "subject.h"
#include "rom.h"

FMRITextureBuilder::FMRITextureBuilder(QObject *parent): QObject(parent)
{
    init();
}

FMRITextureBuilder::~FMRITextureBuilder()
{

}

void FMRITextureBuilder::init()
{
    m_bEnable = false;
    m_pcTextureGenerator = NULL;
    m_pcSubject = NULL;
    m_pcLabelSurface = NULL;
    m_eTextureType = TEXTURE_NONE;
}

void FMRITextureBuilder::setLabelSurface(LabelSurface *pcLabelSurface)
{
    m_pcLabelSurface = pcLabelSurface;
    this->connect(pcLabelSurface, SIGNAL(updated()), this, SLOT(buildTexture()));
}

void FMRITextureBuilder::setTextureType(TextureType eType)
{
    m_eTextureType = eType;
    this->buildTexture();
}

void FMRITextureBuilder::setEnable(bool bEnabled)
{
    m_bEnable = bEnabled;
    this->buildTexture();
}

void FMRITextureBuilder::buildTexture()
{
    if( m_pcLabelSurface == NULL)
        return;


    auto acROIs = this->getLabelSurface()->getROIs();
    Matrix<double> cMatrix = this->getSubject()->getFMRITSData().getMatrix();

    /// check
    if(acROIs.size() != cMatrix.getNumRow())
    {
        qDebug() << "There are" << acROIs.size() << "ROIs, but there are" << cMatrix.getNumRow() << "rows in the matrix";
        return;
    }

    delete m_pcTextureGenerator;
    m_pcTextureGenerator = NULL;
    if( m_eTextureType == TEXTURE_CIRCLE )
        m_pcTextureGenerator = new CircleTextureGenerator();
    else if( m_eTextureType == TEXTURE_CONTOUR )
        m_pcTextureGenerator = new ContourTextureGenerator();
    else if( m_eTextureType == TEXTURE_STRIPE )
        m_pcTextureGenerator = new StripeTextureGenerator();
    else if( m_eTextureType == TEXTURE_NONE)
        m_pcTextureGenerator = NULL;
    else
    {
        qWarning() << "Unsupported Texture Type.";
        return;
    }

    foreach(ROI* pcROI, acROIs)
    {
        if(pcROI->getLabel() <= 0)
            continue;

        /// generate the texture
        QVector<double> adTimeSeriesData = cMatrix[pcROI->getLabel()-1];

        if( getEnable() && m_pcTextureGenerator!=NULL )
        {

            /// prepare color LUT for this ROI
            const double* adColorRGB = g_getColorFromTable(pcROI->getLabel());
            static double adColorHSV[3];
            vtkMath::RGBToHSV(adColorRGB, adColorHSV);
            pcROI->getTextureBlendingLUT()->SetHueRange(adColorHSV[0], adColorHSV[0]);
            pcROI->getTextureBlendingLUT()->SetSaturationRange(0.0,1.0);
            pcROI->getTextureBlendingLUT()->SetValueRange(1.0,1.0);
            pcROI->getTextureBlendingLUT()->SetTableRange(cMatrix.getMin(), cMatrix.getMax());
            pcROI->getTextureBlendingLUT()->Build();

            /// generate texture
            QString strTexturePath = m_pcTextureGenerator->generateTexture(pcROI, adTimeSeriesData);

            /// apply the texture
            pcROI->getTextureReader()->SetFileName(strTexturePath.toStdString().c_str());
            pcROI->getTexture()->SetInputConnection(pcROI->getTextureReader()->GetOutputPort());
            pcROI->getActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
            pcROI->getTexture()->RepeatOff();
            pcROI->getActor()->SetTexture(pcROI->getTexture());

        }
        else
        {
            pcROI->getActor()->SetTexture(NULL);
        }
    }

}

