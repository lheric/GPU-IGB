#include "contourtexturegenerator.h"
#include <QImage>
#include <QPolygonF>
#include <QPainter>
#include <QDebug>
#include <QColor>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkIdList.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include "model/rom.h"
#include "imageutils.h"


ContourTextureGenerator::ContourTextureGenerator()
{

}

ContourTextureGenerator::~ContourTextureGenerator()
{

}

QString ContourTextureGenerator::generateTexture(ROI *pcROI, QVector<double>& adTimeSeriesData)
{
    /// determine texture size
    int iNumTimePoints = adTimeSeriesData.size();
    if(iNumTimePoints == 0)
    {
        qWarning() << "No Data for Texture Generator";
        return QString();
    }
    pcROI->setTextureSize(QSize(iNumTimePoints*2, iNumTimePoints*2));
    int iTextureWidth = pcROI->getTextureSize().width();
    int iTextureHeight = pcROI->getTextureSize().height();

    /// find mapping plane
    double* adCenter = pcROI->getSurface()->GetCenter();
    double dTextureScale = 8;

    double x1, y1, z1;
    x1 = y1 = 10.0;
    z1 = -x1*(pcROI->getTextureNormal().x()+pcROI->getTextureNormal().y())/pcROI->getTextureNormal().z();
    QVector3D cPt1 (x1, y1, z1);
    cPt1.normalize();
    QVector3D cPt2 = QVector3D::normal(cPt1, pcROI->getTextureNormal());
    cPt1 *= iTextureWidth;
    cPt2 *= iTextureHeight;

    pcROI->getTextureMapper()->SetInputData(pcROI->getSurface());

    pcROI->getTextureMapper()->SetOrigin(adCenter);
    pcROI->getTextureMapper()->SetPoint1(adCenter[0]+cPt1.x(), adCenter[1]+cPt1.y(), adCenter[2]+cPt1.z());
    pcROI->getTextureMapper()->SetPoint2(adCenter[0]+cPt2.x(), adCenter[1]+cPt2.y(), adCenter[2]+cPt2.z());
    pcROI->getTextureMapper()->AutomaticPlaneGenerationOff();

    pcROI->getTextureMapper()->Modified();
    pcROI->getTextureMapper()->Update();

    pcROI->getTransTCoord()->SetOrigin(0, 0, 0);
    pcROI->getTransTCoord()->SetScale(dTextureScale, dTextureScale, 1);
    pcROI->getTransTCoord()->SetPosition(0.5, 0.5, 0);
    pcROI->getTransTCoord()->Modified();
    pcROI->getTransTCoord()->Update();

    /// map it to plane
    vtkPolyData* pcMappedSurface = pcROI->getTransTCoord()->GetPolyDataOutput();
    QImage cTextureImage(iTextureWidth, iTextureHeight, QImage::Format_ARGB32);
    QPainter cPainter;

    vtkCellArray* pcPolyArray = pcMappedSurface->GetPolys();
    vtkDataArray* pcTCoords = pcMappedSurface->GetPointData()->GetTCoords();
    int iNumCell =pcPolyArray->GetNumberOfCells();
    vtkIdList* pcPtList = vtkIdList::New();
    QPolygonF cPolygon;
    qDebug() << "#" << pcROI->getLabel() << "has" << iNumCell << "polygons in total";

    cTextureImage.fill(qRgba(255, 255, 255, 0));

    pcPolyArray->InitTraversal();
    cPainter.begin(&cTextureImage);
    cPainter.setPen(Qt::red);
    cPainter.setBrush(Qt::blue);
    while(pcPolyArray->GetNextCell(pcPtList))
    {
        int iTotalPoints = pcPtList->GetNumberOfIds();
        for(int iPtIdx = 0; iPtIdx < iTotalPoints; iPtIdx++ )
        {
            double* adPtCoord = pcTCoords->GetTuple2(pcPtList->GetId(iPtIdx));
            cPolygon << QPointF(adPtCoord[0]*iTextureWidth, (iTextureHeight-(adPtCoord[1]*iTextureHeight)));
        }
        cPainter.drawPolygon(cPolygon);
        pcPtList->Reset();
        cPolygon.clear();
    }
    pcPtList->Delete();
    cPainter.end();

    cTextureImage.save(QString("contour_#%1_map_to_plane.png").arg(pcROI->getLabel()));

    /// iterative erode
    QImage cErodeSrc = cTextureImage;
    QImage cErodeDst(iTextureWidth, iTextureHeight, QImage::Format_ARGB32);

    /// decide erode radius
    QRgb iErodeColor = qRgb(128,128,128);
    int iErodedArea = ImageUtils::erode(cErodeSrc, cErodeDst, 1, iErodeColor);
    int iMaxErodeRadius = 0;
    qDebug() << "Start Searching";

    while(iErodedArea > 0)
    {
        cErodeDst.fill(Qt::transparent);
        iErodedArea = ImageUtils::erode(cErodeSrc, cErodeDst, 5, iErodeColor);
        ImageUtils::overlayImage(cTextureImage, cErodeSrc);
        QImage cTmp = cErodeSrc;
        cErodeSrc = cErodeDst;
        cErodeDst = cTmp;
        iMaxErodeRadius+=2;
    }
    qDebug() << "Max Erode Raduis For" << pcROI->getLabel() << "is" << iMaxErodeRadius;


    /// iterative erode
    cErodeSrc = cTextureImage;
    cErodeDst = QImage(iTextureWidth, iTextureHeight, QImage::Format_ARGB32);

    int iErodeRadius = iMaxErodeRadius/iNumTimePoints;
    iErodeRadius = qMax(1, iErodeRadius);
    int iNumErode = iMaxErodeRadius/iErodeRadius;
    double dStep = iNumTimePoints*1.0/iNumErode;

    double dCurValue;
    double adMappedColor[3];
    for(int iErodeCount = 0; iErodeCount < iNumErode; iErodeCount++)
    {

        dCurValue = adTimeSeriesData[qRound(iErodeCount*dStep)];
        pcROI->getTextureBlendingLUT()->GetColor(dCurValue, adMappedColor);

        cErodeDst.fill(Qt::transparent);
        if(iErodeCount == 0)
            ImageUtils::erode(cErodeSrc, cErodeDst, 1, qRgb(adMappedColor[0]*255,adMappedColor[1]*255,adMappedColor[2]*255));
        else
            ImageUtils::erode(cErodeSrc, cErodeDst, 1+iErodeRadius*2, qRgb(adMappedColor[0]*255,adMappedColor[1]*255,adMappedColor[2]*255));
        ImageUtils::overlayImage(cTextureImage, cErodeSrc);

        // TODO remove
//        if(pcROI->getLabel() == 48)
//            cErodeDst.save(QString("contour_#%1_eroded_%2.png").arg(pcROI->getLabel()).arg(iErodeCount));

        QImage cTmp = cErodeSrc;
        cErodeSrc = cErodeDst;
        cErodeDst = cTmp;
    }


    QString strSavedTexturePath = QString("contour_#%1.png").arg(pcROI->getLabel());
    cTextureImage.save(strSavedTexturePath);

    /// Blurred
    ImageUtils::blur(cTextureImage);
    strSavedTexturePath = QString("contour_blurred_#%1.png").arg(pcROI->getLabel());
    cTextureImage.save(strSavedTexturePath);

    return strSavedTexturePath;

}

bool ContourTextureGenerator::generateTexture(QString strFilePath, QVector<double> &adTimeSeriesData, ROI *pcROI, double dMin, double dMax)
{

    int iNumTimePoints = adTimeSeriesData.size();
    if(iNumTimePoints == 0)
    {
        qWarning() << "No Data for Texture Generator";
        return false;
    }

    QImage cTextureImage;
    cTextureImage.load(strFilePath);
    int iTextureWidth = cTextureImage.width();
    int iTextureHeight = cTextureImage.height();

    /// iterative erode
    QImage cErodeSrc = cTextureImage;
    QImage cErodeDst(iTextureWidth, iTextureHeight, QImage::Format_ARGB32);

    /// decide erode radius
    QRgb iErodeColor = qRgb(128,128,128);
    int iErodedArea = ImageUtils::erode(cErodeSrc, cErodeDst, 1, iErodeColor);
    int iMaxErodeRadius = 0;
    qDebug() << "Start Searching";

    while(iErodedArea > 0)
    {
        cErodeDst.fill(Qt::transparent);
        iErodedArea = ImageUtils::erode(cErodeSrc, cErodeDst, 5, iErodeColor);
        ImageUtils::overlayImage(cTextureImage, cErodeSrc);
        QImage cTmp = cErodeSrc;
        cErodeSrc = cErodeDst;
        cErodeDst = cTmp;
        iMaxErodeRadius+=2;
    }
    qDebug() << "Max Erode Raduis For" << strFilePath << "is" << iMaxErodeRadius;

    /// prepare color LUT for this ROI
    const double* adColorRGB = g_getColorFromTable(pcROI->getLabel());
    static double adColorHSV[3];
    vtkMath::RGBToHSV(adColorRGB, adColorHSV);
    pcROI->getTextureBlendingLUT()->SetHueRange(adColorHSV[0], adColorHSV[0]);
    pcROI->getTextureBlendingLUT()->SetSaturationRange(0.0,1.0);
    pcROI->getTextureBlendingLUT()->SetValueRange(1.0,1.0);
    pcROI->getTextureBlendingLUT()->SetTableRange(dMin, dMax);
    pcROI->getTextureBlendingLUT()->Build();

    /// iterative erode
    cErodeSrc = cTextureImage;
    cErodeDst = QImage(iTextureWidth, iTextureHeight, QImage::Format_ARGB32);

    int iErodeRadius = iMaxErodeRadius/iNumTimePoints;
    iErodeRadius = qMax(1, iErodeRadius);
    int iNumErode = iMaxErodeRadius/iErodeRadius;
    double dStep = iNumTimePoints*1.0/iNumErode;

    double dCurValue;
    double adMappedColor[3];
    for(int iErodeCount = 0; iErodeCount < iNumErode; iErodeCount++)
    {

        dCurValue = adTimeSeriesData[qRound(iErodeCount*dStep)];
        pcROI->getTextureBlendingLUT()->GetColor(dCurValue, adMappedColor);

        cErodeDst.fill(Qt::transparent);
        if(iErodeCount == 0)
            ImageUtils::erode(cErodeSrc, cErodeDst, 1, qRgb(adMappedColor[0]*255,adMappedColor[1]*255,adMappedColor[2]*255));
        else
            ImageUtils::erode(cErodeSrc, cErodeDst, 1+iErodeRadius*2, qRgb(adMappedColor[0]*255,adMappedColor[1]*255,adMappedColor[2]*255));
        ImageUtils::overlayImage(cTextureImage, cErodeSrc);

        QImage cTmp = cErodeSrc;
        cErodeSrc = cErodeDst;
        cErodeDst = cTmp;
    }


    QString strSavedTexturePath = QString("raycast_contour_#%1.png").arg(pcROI->getLabel());
    cTextureImage.save(strSavedTexturePath);

    /// Blurred
    ImageUtils::blur(cTextureImage);
    strSavedTexturePath = QString("raycast_contour_blurred_#%1.png").arg(pcROI->getLabel());
    cTextureImage.save(strSavedTexturePath);


    return true;
}

bool ContourTextureGenerator::generateTexture(QImage cInputImg, QVector<double> &adTimeSeriesData, ROI *pcROI, double dMin, double dMax, QPointF* pcCenter)
{

    int iNumTimePoints = adTimeSeriesData.size();
    if(iNumTimePoints == 0)
    {
        qWarning() << "No Data for Texture Generator";
        return false;
    }

    int iTextureWidth = cInputImg.width();
    int iTextureHeight = cInputImg.height();

    /// iterative erode
    QImage cErodeSrc = cInputImg;
    QImage cErodeDst(iTextureWidth, iTextureHeight, QImage::Format_ARGB32);

    /// decide erode radius & the center point of the shape
    QRgb iErodeColor = qRgb(128,128,128);
    int iErodedArea = ImageUtils::erode(cErodeSrc, cErodeDst, 1, iErodeColor);
    int iMaxErodeRadius = 0;
    qDebug() << "Start Searching";
    while(iErodedArea > 0)
    {
        cErodeDst.fill(Qt::transparent);
        iErodedArea = ImageUtils::erode(cErodeSrc, cErodeDst, 5, iErodeColor, pcCenter);
        ImageUtils::overlayImage(cInputImg, cErodeSrc);
        QImage cTmp = cErodeSrc;
        cErodeSrc = cErodeDst;
        cErodeDst = cTmp;
        iMaxErodeRadius+=2;
    }
    qDebug() << "Max Erode Raduis For ROI #" << pcROI->getLabel() << "is" << iMaxErodeRadius;

    /// prepare color LUT for this ROI
    const double* adColorRGB = g_getColorFromTable(pcROI->getLabel());
    static double adColorHSV[3];
    vtkMath::RGBToHSV(adColorRGB, adColorHSV);
    pcROI->getTextureBlendingLUT()->SetHueRange(adColorHSV[0], adColorHSV[0]);
    pcROI->getTextureBlendingLUT()->SetSaturationRange(0.0,1.0);
    pcROI->getTextureBlendingLUT()->SetValueRange(1.0,1.0);
    pcROI->getTextureBlendingLUT()->SetTableRange(dMin, dMax);
    pcROI->getTextureBlendingLUT()->Build();

    /// iterative erode
    cErodeSrc = cInputImg;
    cErodeDst = QImage(iTextureWidth, iTextureHeight, QImage::Format_ARGB32);

    int iErodeRadius = iMaxErodeRadius/iNumTimePoints;
    iErodeRadius = qMax(1, iErodeRadius);
    int iNumErode = iMaxErodeRadius/iErodeRadius;
    double dStep = iNumTimePoints*1.0/iNumErode;

    double dCurValue;
    double adMappedColor[3];
    for(int iErodeCount = 0; iErodeCount < iNumErode; iErodeCount++)
    {

        dCurValue = adTimeSeriesData[qRound(iErodeCount*dStep)];
        pcROI->getTextureBlendingLUT()->GetColor(dCurValue, adMappedColor);

        cErodeDst.fill(Qt::transparent);
        if(iErodeCount == 0)
            ImageUtils::erode(cErodeSrc, cErodeDst, 1, qRgb(adMappedColor[0]*255,adMappedColor[1]*255,adMappedColor[2]*255));
        else
            ImageUtils::erode(cErodeSrc, cErodeDst, 1+iErodeRadius*2, qRgb(adMappedColor[0]*255,adMappedColor[1]*255,adMappedColor[2]*255));
        ImageUtils::overlayImage(cInputImg, cErodeSrc);

        QImage cTmp = cErodeSrc;
        cErodeSrc = cErodeDst;
        cErodeDst = cTmp;
    }


    QString strSavedTexturePath = QString("raycast_contour_#%1.png").arg(pcROI->getLabel());
    cInputImg.save(strSavedTexturePath);

    /// Blurred
    ImageUtils::blur(cInputImg);
    strSavedTexturePath = QString("raycast_contour_blurred_#%1.png").arg(pcROI->getLabel());
    cInputImg.save(strSavedTexturePath);


    return true;
}

