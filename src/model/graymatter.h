#ifndef GRAYMATTER_H
#define GRAYMATTER_H

//#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL)
//#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)
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
#include <vtkLookupTable.h>
#include <vtkUnstructuredGrid.h>
#include <vtkImageFlip.h>
#include <QObject>
#include <QDataStream>
#include <QPixmap>
#include <QVector>
#include "labelvolume.h"
#include "intensityvolume.h"
#include "parcellationlabels.h"
#include "parcellationcolorscheme.h"
#include "roi.h"


class GrayMatter : public QObject
{
    Q_OBJECT
public:
    explicit GrayMatter(QObject *parent = 0);
    ~GrayMatter();

    /**
     * @brief openIntensityVolume open .nii(.gz) file for building ISO surface
     * @param strFilepath
     * @return
     */
    bool openIntensityVolume(const QString& strFilepath);

    /**
     * @brief openLabelVolume open .nii(.gz) file for labeling ISO surface
     * @param strFilepath
     * @param rdLabelMin
     * @param rdLabelMax
     * @return
     */
    bool openLabelVolume(const QString& strFilepath, double& rdLabelMin, double& rdLabelMax);

    /**
     * @brief setFilp flip Z axe or not
     * @param bStatus
     */
    void setFilp(bool bStatus);


    /// intensity volume
    ADD_CLASS_FIELD_NOSETTER(IntensityVolume, cIntensityVol, getIntensityVol)

    /// label volume
    ADD_CLASS_FIELD_NOSETTER(LabelVolume, cLabelVol, getLabelVol)

    /// label names <label> to <string> map
    ADD_CLASS_FIELD_NOSETTER(ParcellationLabels, cLabelNames, getLabelNames)

    /// Color Scheme <label> to <color> map
    ADD_CLASS_FIELD_NOSETTER(ParcellationColorScheme, cColorScheme, getColorScheme)


};

#endif // GRAYMATTER_H
