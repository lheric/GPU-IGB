#include "circletexturegenerator.h"
#include <QDebug>
#include <QImage>
#include <QPainter>

CircleTextureGenerator::CircleTextureGenerator()
{

}

CircleTextureGenerator::~CircleTextureGenerator()
{

}

QString CircleTextureGenerator::generateTexture(ROI *pcROI, QVector<double>& adTimeSeriesData)
{
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

    pcROI->getTransTCoord()->SetOrigin(0, 0, 0);
    pcROI->getTransTCoord()->SetScale(1, 1, 1);
    pcROI->getTransTCoord()->SetPosition(0.0, 0.0, 0);
    pcROI->getTransTCoord()->Modified();
    pcROI->getTransTCoord()->Update();

    pcROI->setTextureSize(QSize(iNumTimePoints*2, iNumTimePoints*2));

    QImage cTextureImage(pcROI->getTextureSize(), QImage::Format_ARGB32);
    QPainter cPainter;


    cPainter.begin(&cTextureImage);
    cPainter.setPen(Qt::NoPen);
    int iCenterX = pcROI->getTextureSize().width()/2;
    int iCenterY = pcROI->getTextureSize().height()/2;
    for(int iStripCount = iNumTimePoints-1; iStripCount >= 0; iStripCount--)
    {
        dCurValue = adTimeSeriesData[iStripCount];
        pcROI->getTextureBlendingLUT()->GetColor(dCurValue, adMappedColor);
        cPainter.setBrush(QColor(adMappedColor[0]*255,adMappedColor[1]*255,adMappedColor[2]*255));
        cPainter.drawEllipse(QPoint(iCenterX, iCenterY), iStripCount, iStripCount);
                                                                                                                                                                                                                                         
    }
    cPainter.end();

    QString strTextureFilename = QString("circle_#%1.png").arg(pcROI->getLabel());
    cTextureImage.save(strTextureFilename);
    return strTextureFilename;
}
