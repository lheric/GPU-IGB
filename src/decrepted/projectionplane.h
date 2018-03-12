#ifndef PROJECTIONPLANE_H
#define PROJECTIONPLANE_H
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkTexture.h>
#include <vtkPNGReader.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTextureMapToSphere.h>
#include <QString>
#include <QPainter>
#include <QStringList>
#include "gitlview.h"
#include "gitldef.h"


class ROI;
class LabelSurface;
class Subject;

class ProjectionPlane:public GitlView
{
public:
    ProjectionPlane();
    ~ProjectionPlane();

    void init();
    bool addActorsToRenderer(vtkRenderer *pcRenderer);
    bool removeFromRenderer(vtkRenderer *pcRenderer);
    bool projectGrayMatter(LabelSurface* pcLabelSurface);
    void setVisibility(bool bVisible);
    void setEnable(bool bEnabled);

    QImage projectROI(ROI* pcROI, QString &strFilename);


protected:
//    bool xCombineMappedROIs(QStringList& filenames);
    void xOverlayImage(QImage& cBaseImage, const QImage& cOverlayImage);
    QVector<int> xSortIndexes(QVector<double> &v);


    ///
    ADD_CLASS_FIELD_NOSETTER(bool, bEnable, getEnable)

    ///
    ADD_CLASS_FIELD(Subject*, pcSubject, getSubject, setSubject)

    ///
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<vtkPlane>, cPlane, getPlane)

    ///
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<vtkPlaneSource>, cPlaneSource, getPlaneSource)

    /// Mapper
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPolyDataMapper>, pcMapper, getMapper, setMapper)

    /// Actor
    ADD_CLASS_FIELD(vtkSmartPointer<vtkActor>, pcActor, getActor, setActor)

    /// Texture Reader
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPNGReader>, pcTextureReader, getTextureReader, setTextureReader)

    /// Texture Mapper
    ADD_CLASS_FIELD(vtkSmartPointer<vtkTextureMapToPlane>, pcTextureMapper, getTextureMapper, setTextureMapper)

    /// Texture
    ADD_CLASS_FIELD(vtkSmartPointer<vtkTexture>, pcTexture, getTexture, setTexture)

    /// Texture Mapper for ROIs
    ADD_CLASS_FIELD(vtkSmartPointer<vtkTextureMapToSphere>, pcSphereTextureMapper, getSphereTextureMapper, setSphereTextureMapper)


};


#endif // PROJECTIONPLANE_H
