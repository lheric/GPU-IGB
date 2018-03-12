#ifndef FIBER_H
#define FIBER_H

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
#include "algorithm/vtktrackvistrkreader.h"
#include <QDataStream>
#include <QPixmap>
#include <QVector>
#include "trkfileio.h"
#include "gitlmodule.h"


class FiberROIConnection;
class FiberClassification;

class Fiber : public QObject
{
    Q_OBJECT
public:
    explicit Fiber(QObject *parent = 0);
    ~Fiber();

    /**
     * @brief init
     * @return
     */
    bool init();

    /**
     * @brief openTrkFile load the *.trk file
     * @param strFilepath
     * @return
     */
    bool openTrkFile(QString strFilepath);

    /// for fiber (read from vistrack fiber file [*.trk])
    ADD_CLASS_FIELD(vtkSmartPointer<vtkTrackVisTrkReader>, pcReader, getReader, setReader)    
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPolyData>, pcFiberPolyData, getFiberPolyData, setFiberPolyData)    /// fibers    


    /**
     * fiber connection information
     */
    ADD_CLASS_FIELD(FiberROIConnection*, pcConnectionInfo, getConnectionInfo, setConnectionInfo)


    /**
     * fiber classification information
     */
    ADD_CLASS_FIELD(FiberClassification*, pcFiberClassification, getFiberClassification, setFiberClassification)


signals:
    void updated();
};

#endif // FIBER_H
