#ifndef FIBERRENDERER_H
#define FIBERRENDERER_H
#include "model/renderengine/abstractpluginrenderer.h"
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVectorText.h>
#include <vtkProperty.h>
#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkNIFTIImageReader.h>
#include <vtkNIFTIImageHeader.h>
#include <vtkNIFTIImageWriter.h>
#include <vtkImageData.h>
#include <vtkContourFilter.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkTransformFilter.h>
#include <vtkTransform.h>
#include <vtkCellLocator.h>
#include <vtkClipVolume.h>
#include <vtkUnstructuredGrid.h>
#include <vtkNew.h>
#include "vtklinecolorfilter.h"
#include "vtkfiberconnectionfilter.h"
#include <QDataStream>
#include <QPixmap>
#include <QVector>
#include "trkfileio.h"
#include "fiberconfigpanel.h"
#include "gitlmodule.h"


class FiberRenderer : public QObject, public AbstractPluginRenderer
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PLUGIN_RENDERER_IID)
    Q_INTERFACES(AbstractPluginRenderer)
public:
    explicit FiberRenderer(QObject *parent = 0);
    ~FiberRenderer();

    virtual bool init();

    virtual bool uninit();

    virtual bool redraw();

    virtual bool onEnabled();

    virtual bool onDisabled();

protected slots:
    void xRebuildFiberConnTable();


    ADD_CLASS_FIELD_PRIVATE(FiberConfigPanel*, pcConfigPanel)

    /// vtk pipeline display fibers
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPolyData>, pcFiberPolyData, getFiberPolyData, setFiberPolyData)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkFiberConnectionFilter>, pcFiberConnFilter, getFiberConnFilter, setFiberConnFilter)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkLineColorFilter>, pcColorFilter, getColorFilter, setColorFilter)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPolyDataMapper>, pcFiberMapper, getFiberMapper, setFiberMapper)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkActor>, pcFiberActors, getFiberActors, setFiberActors)


signals:

public slots:

};

#endif // FIBERRENDERER_H
