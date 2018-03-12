#ifndef LABELSURFACE_H
#define LABELSURFACE_H

#include <gitldef.h>
#include <QObject>
#include <QVector>
#include <vtkSmartPointer.h>
#include <vtkNIFTIImageReader.h>
#include <vtkImageFlip.h>
#include <vtkImageData.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCellLocator.h>
#include <vtkClipVolume.h>
#include <vtkContourFilter.h>
#include <vtkClipPolyData.h>
#include <vtkThreshold.h>
#include <vtkThresholdPoints.h>
#include <vtkGeometryFilter.h>
#include <vtkPointDataToCellData.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkRenderer.h>
#include <vtkCleanPolyData.h>
#include "multidiseasemeasurement.h"
#include "intensityvolume.h"
#include "labelvolume.h"
#include "roi.h"
/**
 * @brief The LabelSurface class
 * gray matter surface with label indicating different ROIs
 */
class LabelSurface : public QObject
{
    Q_OBJECT
public:
    explicit LabelSurface(QObject *parent = 0);
    ~LabelSurface();

    /**
     * @brief init
     */
    void init();



//    /**
//     * @brief addActorsToRenderer add all ROI actors to the renderer
//     * @param pcRenderer
//     * @return
//     */
//    bool addActorsToRenderer(vtkRenderer* pcRenderer);

//    /**
//     * @brief removeActorsFromRenderer remove all ROI actors from the renderer
//     * @param pcRenderer
//     * @return
//     */
//    bool removeActorsFromRenderer(vtkRenderer* pcRenderer);

    /**
     * @brief getROIByActor return the corresponding ROI of a actor
     * @return NULL is it is not a ROI actor
     */
    ROI* getROIByActor(vtkObject* pcObj);

    /**
     * @brief getROIByLabel
     * @param iLabel
     * @return
     */
    ROI* getROIByLabel(int iLabel);

    /**
     * @brief setHighlightedROI
     * @param pcHighlightedROI
     */
    void setHighlightedROI(ROI *pcHighlightROI);


protected:
    /**
     * @brief xSplitIntoROIByLabel
     */
    void xSplitIntoROIByLabel();

    /**
     * @brief xDeleteAllROIs delete ROIs and free memory
     */
    void xDeleteAllROIs();


    /// intensity volume
    ADD_CLASS_FIELD(IntensityVolume*, pcIntensityVol, getIntensityVol, setIntensityVol)

    /// label volume
    ADD_CLASS_FIELD(LabelVolume*, pcLabelVol, getLabelVol, setLabelVol)

    /// iso value for contour filter
    ADD_CLASS_FIELD_NOSETTER(double, dISO, getISO)

    /// contour filter for iso surface extraction
    ADD_CLASS_FIELD(vtkSmartPointer<vtkContourFilter>, pcContourFilter, getContourFilter, setContourFilter)

    /// map labels on points to cells (labels are point data for nifti file)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPointDataToCellData>, pcLabelMapper, getLabelMapper, setLabelMapper)

    /// ISO surface
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPolyData>, pcSurface, getSurface, setSurface)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkIntArray>, pcSurfaceLabels, getSurfaceLabels, setSurfaceLabels)

    /// ISO surface cleaner
    ADD_CLASS_FIELD(vtkSmartPointer<vtkCleanPolyData>, pcPolyCleaner, getPolyCleaner, setPolyCleaner )

    /// Clipper which is used to extract each ROI from labelled suface
    ADD_CLASS_FIELD(vtkSmartPointer<vtkClipPolyData>, pcROIClipUpBound, getROIClipUpBound, setROIClipUpBound)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkClipPolyData>, pcROIClipLowBound, getROIClipLowBound, setROIClipLowBound)

    ADD_CLASS_FIELD(vtkSmartPointer<vtkThreshold>, pcROIExtractor, getROIExtractor, setROIExtractor)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkGeometryFilter>, pcGeoFilter, getGeoFilter, setGeoFilter)

    /// ROIs
    ADD_CLASS_FIELD_NOSETTER(QVector<ROI*>, cROIs, getROIs)

    /// Selected/Highlighted ROI
    ADD_CLASS_FIELD_NOSETTER(ROI*, pcHighlightedROI, getHighlightedROI)

    /// Opacity
    ADD_CLASS_FIELD_NOSETTER(double, dOpacity, getOpacity)



signals:
    void updated();


public slots:

    /**
     * @brief buildLabelSurace build the iso surface with probability volume and then label it with label volume
     * @return
     */
//    bool buildLabelSurace();
};

#endif // LABELSURFACE_H
