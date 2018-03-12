#include "multidiseasetexturebuilder.h"
#include "model/subject.h"
#include "algorithm/perlintexturegenerator.h"
#include <QString>

MultiDiseaseTextureBuilder::MultiDiseaseTextureBuilder(QObject *parent) : QObject(parent)
{
    init();
}

MultiDiseaseTextureBuilder::~MultiDiseaseTextureBuilder()
{

}

void MultiDiseaseTextureBuilder::init()
{
    m_bEnable = false;
    m_pcLabelSurface = NULL;
    m_pcSubject = NULL;
}

void MultiDiseaseTextureBuilder::setLabelSurface(LabelSurface *pcLabelSurface)
{
    m_pcLabelSurface = pcLabelSurface;
    this->connect(pcLabelSurface, SIGNAL(updated()), this, SLOT(buildTexture()));
}

void MultiDiseaseTextureBuilder::setEnable(bool bEnabled)
{
    m_bEnable = bEnabled;
    this->buildTexture();
}

void MultiDiseaseTextureBuilder::buildTexture()
{
    if( m_pcLabelSurface == NULL)
        return;

    auto acROIs = this->getLabelSurface()->getROIs();
    Matrix<double> cMatrix = this->getSubject()->getMDMeasurement().getMatrix();

    /// check
    if(acROIs.size() != cMatrix.getNumRow())
    {
        qDebug() << "There are" << acROIs.size() << "ROIs, but there are" << cMatrix.getNumRow() << "rows in the matrix";
        return;
    }


    PerlinTextureGenerator cTextureGen;

    foreach(ROI* pcROI, acROIs)
    {
        if(pcROI->getLabel() <= 0)
            continue;

        if( getEnable() )
        {
            pcROI->setTextureSize(QSize(100, 100));
            int iTextureWidth = pcROI->getTextureSize().width();
            int iTextureHeight = pcROI->getTextureSize().height();

            /// find mapping plane
            double* adCenter = pcROI->getSurface()->GetCenter();

            double x1, y1, z1;
            x1 = y1 = 10.0;
            z1 = -x1*(pcROI->getTextureNormal().x()+pcROI->getTextureNormal().y())/pcROI->getTextureNormal().z();
            QVector3D cPt1 (x1, y1, z1);
            cPt1.normalize();
            QVector3D cPt2 = QVector3D::normal(cPt1, pcROI->getTextureNormal());
            cPt1 *= iTextureWidth;
            cPt2 *= iTextureHeight;

            pcROI->getTextureMapper()->SetOrigin(adCenter);
            pcROI->getTextureMapper()->SetPoint1(adCenter[0]+cPt1.x(), adCenter[1]+cPt1.y(), adCenter[2]+cPt1.z());
            pcROI->getTextureMapper()->SetPoint2(adCenter[0]+cPt2.x(), adCenter[1]+cPt2.y(), adCenter[2]+cPt2.z());
            pcROI->getTextureMapper()->AutomaticPlaneGenerationOff();
            pcROI->getTextureMapper()->Modified();
            pcROI->getTextureMapper()->Update();


            pcROI->getTransTCoord()->SetOrigin(0, 0, 0);
            pcROI->getTransTCoord()->SetScale(1, 1, 1);
            pcROI->getTransTCoord()->SetPosition(0, 0, 0);

            pcROI->getTransTCoord()->Modified();
            pcROI->getTransTCoord()->Update();

            QString strTexturePath;
            cTextureGen.generatePerlineTexture(cMatrix[pcROI->getLabel()-1], 3, pcROI->getLabel(), strTexturePath);

            pcROI->getTextureReader()->SetFileName(strTexturePath.toStdString().c_str());
            pcROI->getTextureReader()->Modified();
            pcROI->getTextureReader()->Update();
            pcROI->getTexture()->SetInputConnection(pcROI->getTextureReader()->GetOutputPort());
            pcROI->getTexture()->RepeatOn();
            pcROI->getActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
            pcROI->getActor()->SetTexture(pcROI->getTexture());
        }
        else
        {
            pcROI->getActor()->SetTexture(NULL);
        }
    }

    return;
}
