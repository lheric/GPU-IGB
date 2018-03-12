#include "projectionplane.h"
#include <QImage>
#include <vtkProperty.h>
#include <algorithm>
#include "model/rom.h"
#include "model/roi.h"
#include "model/labelsurface.h"
#include "subject.h"

#define PROJ_PANEL_WIDTH 166*2
#define PROJ_PANEL_HEIGTH 166*2

ProjectionPlane::ProjectionPlane()
{
    init();
}


ProjectionPlane::~ProjectionPlane()
{

}

void ProjectionPlane::init()
{
    m_cPlane = vtkSmartPointer<vtkPlane>::New();
    m_cPlaneSource = vtkSmartPointer<vtkPlaneSource>::New();
    m_pcSubject = NULL;
    this->setMapper(vtkPolyDataMapper::New());
    this->setActor(vtkActor::New());
    this->m_pcTextureReader = vtkPNGReader::New();
    this->m_pcTextureMapper = vtkTextureMapToPlane::New();
    this->m_pcSphereTextureMapper = vtkTextureMapToSphere::New();
    this->m_pcTexture = vtkTexture::New();

    m_cPlaneSource->SetOrigin(0, 0, 0);
    m_cPlaneSource->SetPoint1(PROJ_PANEL_WIDTH, 0, 0);
    m_cPlaneSource->SetPoint2(0, PROJ_PANEL_HEIGTH, 0);

    this->getTextureMapper()->SetInputConnection(this->getPlaneSource()->GetOutputPort());
    this->getMapper()->SetInputConnection(this->getTextureMapper()->GetOutputPort());
    this->getActor()->SetMapper(this->getMapper());

}

bool ProjectionPlane::addActorsToRenderer(vtkRenderer *pcRenderer)
{
    if(pcRenderer == NULL)
        return false;
    pcRenderer->AddActor(this->getActor());
    return true;
}

bool ProjectionPlane::removeFromRenderer(vtkRenderer *pcRenderer)
{
    if(pcRenderer == NULL)
        return false;
    pcRenderer->RemoveActor(this->getActor());
    return true;
}

bool ProjectionPlane::projectGrayMatter(LabelSurface *pcLabelSurface)
{
    /// TODO temp code
    if(this->getSubject() == NULL)
        return false;
    double *adGMCenter = this->getSubject()->getGrayMat().getIntensityVol().getVolume()->GetCenter();
    qDebug() << adGMCenter[0] << adGMCenter[1] << adGMCenter[2];
//    adGMCenter = this->getSubject()->getGrayMat().getLabelSurface().getROIs()[0]->getActor()->GetCenter();
//    qDebug() << adGMCenter[0] << adGMCenter[1] << adGMCenter[2];
    this->getSphereTextureMapper()->SetAutomaticSphereGeneration(false);
    this->getSphereTextureMapper()->SetCenter(adGMCenter);


    QImage cProjectedGM = QImage(PROJ_PANEL_WIDTH, PROJ_PANEL_HEIGTH, QImage::Format_ARGB32);
    cProjectedGM.fill(qRgba(255, 255, 255, 255));

    /// sort ROI by its distance to plane
    QVector<double> adDistance;
    QVector<ROI*>& acROI = pcLabelSurface->getROIs();
    this->getPlane()->SetOrigin(this->getPlaneSource()->GetOrigin());
    this->getPlane()->SetNormal(this->getPlaneSource()->GetNormal());
    for(int i = 0; i < acROI.size(); i++)
    {
        double dDistToPlane = this->getPlane()->DistanceToPlane(acROI[i]->getSurface()->GetCenter());
        adDistance.push_back(dDistToPlane);
    }
    QVector<int> aiSortedIdx = this->xSortIndexes(adDistance);

    QStringList cROITextureList;
    for(int i = aiSortedIdx.size()-1; i >= 0; i--)
    {
        ROI* pcROI = acROI[aiSortedIdx[i]];
        QString strFilename = QString("ROI_%1.png").arg(pcROI->getLabel());
        cROITextureList << strFilename;
        QImage cProjectedROI = projectROI(pcROI,strFilename);
        xOverlayImage(cProjectedGM, cProjectedROI);
    }

    cProjectedGM.save("Projected_GM.png");

    /// load texture
    this->getTextureMapper()->SetOrigin(this->getPlaneSource()->GetOrigin());
    this->getTextureMapper()->SetPoint1(this->getPlaneSource()->GetPoint1());
    this->getTextureMapper()->SetPoint2(this->getPlaneSource()->GetPoint2());


    this->setTextureReader(vtkPNGReader::New());
    this->getTextureReader()->SetFileName("Projected_GM.png");

    this->getTexture()->SetInputConnection(this->getTextureReader()->GetOutputPort());
    this->getActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
    this->getActor()->GetProperty()->SetOpacity(0.30);
    this->getActor()->SetTexture(this->getTexture());


    return true;
}

QImage ProjectionPlane::projectROI(ROI *pcROI, QString& strFilename)
{
    Q_UNUSED(strFilename)

//    pcROI->setTextureSize(QSize(iNumTimePoints*2, iNumTimePoints*2));
//    int iTextureWidth = pcROI->getTextureSize().width();
//    int iTextureHeight = pcROI->getTextureSize().height();

    int iTextureWidth = PROJ_PANEL_WIDTH;
    int iTextureHeight = PROJ_PANEL_HEIGTH;

//    /// find mapping plane
//    double* adCenter = pcROI->getSurface()->GetCenter();
//    double dTextureScale = 8;

//    double x1, y1, z1;
//    x1 = y1 = 10.0;
//    z1 = -x1*(pcROI->getTextureNormal().x()+pcROI->getTextureNormal().y())/pcROI->getTextureNormal().z();
//    QVector3D cPt1 (x1, y1, z1);
//    cPt1.normalize();
//    QVector3D cPt2 = QVector3D::normal(cPt1, pcROI->getTextureNormal());
//    cPt1 *= iTextureWidth;
//    cPt2 *= iTextureHeight;

    pcROI->getTextureMapper()->SetOrigin(m_cPlaneSource->GetOrigin());
    pcROI->getTextureMapper()->SetPoint1(m_cPlaneSource->GetPoint1());
    pcROI->getTextureMapper()->SetPoint2(m_cPlaneSource->GetPoint2());
    pcROI->getTextureMapper()->AutomaticPlaneGenerationOff();
    pcROI->getTextureMapper()->Modified();
    pcROI->getTextureMapper()->Update();

//    pcROI->getTransTCoord()->SetOrigin(0, 0, 0);
//    pcROI->getTransTCoord()->SetScale(dTextureScale, dTextureScale, 1);
//    pcROI->getTransTCoord()->SetPosition(0.5, 0.5, 0);
    pcROI->getTransTCoord()->Modified();
    pcROI->getTransTCoord()->Update();


    /// temp code TODO
    this->getSphereTextureMapper()->SetInputData(pcROI->getSurface());
    this->getSphereTextureMapper()->Modified();
    this->getSphereTextureMapper()->Update();
    vtkPolyData* pcMappedSurface = this->getSphereTextureMapper()->GetPolyDataOutput();

    /// map it to plane
    ///vtkPolyData* pcMappedSurface = pcROI->getTransTCoord()->GetPolyDataOutput();
    QImage cTextureImage(iTextureWidth, iTextureHeight, QImage::Format_ARGB32);
    QPainter cPainter;

    vtkCellArray* pcPolyArray = pcMappedSurface->GetPolys();
    vtkDataArray* pcTCoords = pcMappedSurface->GetPointData()->GetTCoords();
    vtkIdList* pcPtList = vtkIdList::New();
    QPolygonF cPolygon;
    //qDebug() << "#" << pcROI->getLabel() << "has" << iNumCell << "polygons in total";

    cTextureImage.fill(qRgba(255, 255, 255, 0));

    const double* adColorRGB = g_getColorFromTable(pcROI->getLabel());
//    this->getActor()->GetProperty()->SetColor(  adColorRGB[0]/255.0,
//                                                adColorRGB[1]/255.0,
//                                                adColorRGB[2]/255.0 );

    pcPolyArray->InitTraversal();
    cPainter.begin(&cTextureImage);
    cPainter.setPen(Qt::NoPen);
    cPainter.setBrush(QColor(adColorRGB[0], adColorRGB[1], adColorRGB[2]));
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

    //QString strSavedPath = QString("ROI_%1.png").arg(pcROI->getLabel());
    //cTextureImage.save(strFilename);
    return cTextureImage;
//    /// iterative erode
//    QImage cErodeSrc = cTextureImage;
//    QImage cErodeDst(iTextureWidth, iTextureWidth, QImage::Format_ARGB32);

//    /// decide erode radius
//    QRgb iErodeColor = qRgb(128,128,128);
//    int iErodedArea = xErode(cErodeSrc, cErodeDst, 1, iErodeColor);
//    int iMaxErodeRadius = 0;
//    qDebug() << "Start Searching";
//    while(iErodedArea > 0)
//    {
//        cErodeDst.fill(Qt::transparent);
//        iErodedArea = xErode(cErodeSrc, cErodeDst, 5, iErodeColor);
//        xOverlayImage(cTextureImage, cErodeSrc);
//        QImage cTmp = cErodeSrc;
//        cErodeSrc = cErodeDst;
//        cErodeDst = cTmp;
//        iMaxErodeRadius+=2;
//    }
//    qDebug() << "Max Erode Raduis For" << pcROI->getLabel() << "is" << iMaxErodeRadius;


//    /// iterative erode
//    cErodeSrc = cTextureImage;
//    cErodeDst = QImage(iTextureWidth, iTextureWidth, QImage::Format_ARGB32);

//    int iErodeRadius = iMaxErodeRadius/iNumTimePoints;
//    iErodeRadius = qMax(1, iErodeRadius);
//    int iNumErode = iMaxErodeRadius/iErodeRadius;
//    double dStep = iNumTimePoints*1.0/iNumErode;

////    qDebug() << iNumTimePoints << iErodeRadius << iNumErode << dStep;

//    double dCurValue;
//    double adMappedColor[3];
//    for(int iErodeCount = 0; iErodeCount < iNumErode; iErodeCount++)
//    {

//        dCurValue = pcROI->getTimeSeriesData()[qRound(iErodeCount*dStep)];
//        pcROI->getTextureBlendingLUT()->GetColor(dCurValue, adMappedColor);
//        //qDebug() << adMappedColor[0] << adMappedColor[1] << adMappedColor[2];
////        pcROI->getTextureCanvas()->SetDrawColor(adMappedColor[0]*255,adMappedColor[1]*255,adMappedColor[2]*255);

//        cErodeDst.fill(Qt::transparent);
//        if(iErodeCount == 0)
//            xErode(cErodeSrc, cErodeDst, 1, qRgb(adMappedColor[0]*255,adMappedColor[1]*255,adMappedColor[2]*255));
//        else
//            xErode(cErodeSrc, cErodeDst, 1+iErodeRadius*2, qRgb(adMappedColor[0]*255,adMappedColor[1]*255,adMappedColor[2]*255));
//        xOverlayImage(cTextureImage, cErodeSrc);
//        if(pcROI->getLabel() == 48)
//            cErodeDst.save(QString("contour_#%1_eroded_%2.png").arg(pcROI->getLabel()).arg(iErodeCount));
//        QImage cTmp = cErodeSrc;
//        cErodeSrc = cErodeDst;
//        cErodeDst = cTmp;


//    }
//    QString strSavedTexturePath = QString("contour_#%1.png").arg(pcROI->getLabel());
//    cTextureImage.save(strSavedTexturePath);

//    /// Blurred

//    xBlur(cTextureImage);
//    strSavedTexturePath = QString("contour_blurred_#%1.png").arg(pcROI->getLabel());
//    cTextureImage.save(strSavedTexturePath);

//    return true;
}

void ProjectionPlane::xOverlayImage(QImage &cBaseImage, const QImage &cOverlayImage)
{
    QPainter painter(&cBaseImage);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, cOverlayImage);

    painter.end();
}

QVector<int> ProjectionPlane::xSortIndexes(QVector<double> &v)
{

  // initialize original index locations
  QVector<int> idx(v.size());
  for (size_t i = 0; i != idx.size(); ++i)
      idx[int(i)] = int(i);

  // sort indexes based on comparing values in v
  std::sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[int(i1)] < v[int(i2)];});

  return idx;
}



