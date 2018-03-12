#include "roi.h"
#include "rom.h"
#include <vtkProperty.h>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QPolygonF>
#include <QColor>
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkPointData.h>
#include <vtkNew.h>

ROI::ROI(QObject *parent) : QObject(parent)
{
    init();
}

ROI::~ROI()
{
    this->getSurface()->Delete();
    this->getMapper()->Delete();
    this->getActor()->Delete();
}

void ROI::init()
{
    /// create
    this->setSurface(vtkPolyData::New());
#if TEXTURE_MAP_TO_PLANE
    this->setTextureMapper(vtkTextureMapToPlane::New());
#elif TEXTURE_MAP_TO_SPHERE
    this->setTextureMapper(vtkTextureMapToSphere::New());
#endif
    this->setTextureReader(vtkPNGReader::New());
    this->setTransTCoord(vtkTransformTextureCoords::New());
    this->setTexture(vtkTexture::New());
    this->setMapper(vtkPolyDataMapper::New());
    this->setActor(vtkActor::New());

    this->getMapper()->ScalarVisibilityOff();
    this->getMapper()->SetScalarModeToUseCellData();
    this->getMapper()->SetScalarRange(1,83);

    this->getTexture()->RepeatOff();


    this->setRenderer(NULL);

    /// connect
    this->getTextureMapper()->SetInputData(this->getSurface());
    this->getTransTCoord()->SetInputConnection(this->getTextureMapper()->GetOutputPort());
    this->getMapper()->SetInputConnection(this->getTransTCoord()->GetOutputPort());
    this->getActor()->SetMapper(this->getMapper());

    /// init parameters
    this->setLabel(-1);
    this->setOpacity(0.15);
    this->m_dHighlightedOpacity = 1.0;
    this->setIsHighlighted(false);

}


bool ROI::removeFromRenderer(vtkRenderer *pcRenderer)
{
    if(pcRenderer == NULL)
        pcRenderer = this->getRenderer();
    if(pcRenderer == NULL)
        return false;
    pcRenderer->RemoveActor(this->getActor());
    return true;
}

bool ROI::addToRenderer(vtkRenderer *pcRenderer)
{
    pcRenderer->AddActor(this->getActor());
    this->setRenderer(pcRenderer);
    return true;
}

void ROI::setLabel(int iLabel)
{
    if(iLabel < 0)
        return;
    this->m_iLabel = iLabel;
//    const double* adColorRGB = g_getColorFromTable(m_iLabel);
//    this->getActor()->GetProperty()->SetColor(  adColorRGB[0]/255.0,
//                                                adColorRGB[1]/255.0,
//                                                adColorRGB[2]/255.0 );
}

void ROI::setColor(const QColor &rcColor)
{
    this->getActor()->GetProperty()->SetColor(  rcColor.redF(),
                                                rcColor.greenF(),
                                                rcColor.blueF() );
}

void ROI::setIsHighlighted(bool bIsHightlighted)
{
    this->m_bIsHightlighted = bIsHightlighted;
    this->applyOpacity();
}

void ROI::setOpacity(double dOpacity)
{
    this->m_dOpacity = dOpacity;
    applyOpacity();
}

void ROI::applyOpacity()
{
    if(this->getIsHighlighted())
    {
        this->getActor()->GetProperty()->SetOpacity(m_dHighlightedOpacity);
    }
    else
    {
        this->getActor()->GetProperty()->SetOpacity(m_dOpacity);
    }
}

void ROI::setTextureNormal(QVector3D &cNormal)
{
    m_cTextureNormal = cNormal;
    return;
}



