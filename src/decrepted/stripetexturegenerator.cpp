#include "stripetexturegenerator.h"
#include <QDebug>
#include <QImage>
#include <QPainter>
StripeTextureGenerator::StripeTextureGenerator()
{

}

StripeTextureGenerator::~StripeTextureGenerator()
{

}

QString StripeTextureGenerator::generateTexture(ROI *pcROI, QVector<double>& adTimeSeriesData)
{
    /// determine texture size
    int iNumTimePoints = adTimeSeriesData.size();
    if(iNumTimePoints == 0)
    {
        qWarning() << "No Data for Texture Generator";
        return QString();
    }

    double dCurValue = 0;
    double adMappedColor[3];

    pcROI->getTextureMapper()->SetInputData(pcROI->getSurface());
    pcROI->getTextureMapper()->AutomaticPlaneGenerationOn();
    pcROI->getTextureMapper()->Update();


    pcROI->getTransTCoord()->SetOrigin(0, 0, 0);
    pcROI->getTransTCoord()->SetScale(1, 1, 1);
    pcROI->getTransTCoord()->SetPosition(0, 0, 0);
    pcROI->getTransTCoord()->Modified();
    pcROI->getTransTCoord()->Update();

    pcROI->setTextureSize(QSize(iNumTimePoints, iNumTimePoints));

    QImage cTextureImage(pcROI->getTextureSize(), QImage::Format_ARGB32);
    QPainter cPainter;


    cPainter.begin(&cTextureImage);
    cPainter.setPen(Qt::NoPen);
    int iStripWidth = pcROI->getTextureSize().width() / iNumTimePoints;
    for(int iStripCount = 0; iStripCount < iNumTimePoints; iStripCount++)
    {
        dCurValue = adTimeSeriesData[iStripCount];
        pcROI->getTextureBlendingLUT()->GetColor(dCurValue, adMappedColor);
        cPainter.setBrush(QColor(adMappedColor[0]*255,adMappedColor[1]*255,adMappedColor[2]*255));
        cPainter.drawRect(iStripWidth*iStripCount, 0, iStripWidth, pcROI->getTextureSize().height());
    }
    cPainter.end();

    QString strTextureFilename = QString("stripe_#%1.png").arg(pcROI->getLabel());
    cTextureImage.save(strTextureFilename);
    return strTextureFilename;
}

