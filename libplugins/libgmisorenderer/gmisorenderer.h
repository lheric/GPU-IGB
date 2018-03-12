#ifndef GMISORENDERER_H
#define GMISORENDERER_H
#include "model/renderengine/abstractpluginrenderer.h"
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkClipPolyData.h>
#include <vtkThreshold.h>
#include <vtkCleanPolyData.h>
#include <vtkContourFilter.h>
#include <vtkPointDataToCellData.h>
#include <vtkGeometryFilter.h>

#include <QVector>

#include "model/roi.h"
#include "gitlmodule.h"
#include "gmisoconfigpanel.h"



class IntensityVolume;
class LabelVolume;
class ParcellationLabels;
class ParcellationColorScheme;

class GMISORenderer : public QObject, public AbstractPluginRenderer
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PLUGIN_RENDERER_IID)
    Q_INTERFACES(AbstractPluginRenderer)
public:
    explicit GMISORenderer(QObject *parent = 0);
    ~GMISORenderer();

    virtual bool init();

    virtual bool uninit();

    virtual bool redraw();

    virtual bool onEnabled();

    virtual bool onDisabled();


public:

    /// change ISO surface value
    void setISO(double dISO);

    /// highlight
    void setHightlightedROI(ROI* pcROI);
    void clearHightlight();

    /// change gray matter opacity
    bool setIntensityOpacity(double dOpacity, int iROILabel = 0);
    bool setParcellationOpacity(double dOpacity, int iROILabel = 0);

    /// remove ISO surface from renderer
    bool addActorsToRenderer();

    /// add ISO surface to renderer
    bool removeActorsFromRenderer();
protected slots:

    /// rebuild iso surface, according to new iso value
    bool buildIntensitySurface();
    bool buildLabelSurface();
    void onNewColorScheme(ParcellationColorScheme* pcNewColorScheme);

protected:


    /**
     * @brief xSplitIntoROIByLabel
     */
    void xSplitIntoROIByLabel();

    /**
     * @brief xDeleteIntensityROIs delete Intensity ROIs and free memory
     */
    void xDeleteIntensityROIs();

    /**
     * @brief xDeleteLabelROIs delete Intensity ROIs and free memory
     */
    void xDeleteLabelROIs();


//    bool colorSchemeCallback(GitlEvent& rcEvt);



    ADD_CLASS_FIELD_PRIVATE(GMISOConfigPanel*, pcConfigPanel)

    /// vtk pipeline display gray matter(GM) ISO Surface
    /// intensity volume
    ADD_CLASS_FIELD(IntensityVolume*, pcIntensityVol, getIntensityVol, setIntensityVol)

    /// label volume
    ADD_CLASS_FIELD(LabelVolume*, pcLabelVol, getLabelVol, setLabelVol)

    /// parcellation labels
    ADD_CLASS_FIELD(ParcellationLabels*, pcLabelNames, getLabelNames, setLabelNames)

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

    /// ISO surface splitted into ROIs
    ADD_CLASS_FIELD_NOSETTER(QVector<ROI*>, cIntensityROIs, getIntensityROIs)

    /// Label Volumn ROI Surfaces
    ADD_CLASS_FIELD_NOSETTER(QVector<ROI*>, cParcROIs, getParcROIs)

    /// Selected/Highlighted ROI
    ADD_CLASS_FIELD_NOSETTER(ROI*, pcHighlightedROI, getHighlightedROI)

    /// Opacity
    ADD_CLASS_FIELD_NOSETTER(double, dIntensityOpacity, getIntensityOpacity)
    ADD_CLASS_FIELD_NOSETTER(double, dParcellationOpacity, getParcellationOpacity)

    /// Show/Hide intensity surface
    ADD_CLASS_FIELD_NOSETTER(bool, bShowIntensity, getShowIntensity)
public:
    void setShowIntensity(bool bShow)
    {
        this->m_bShowIntensity = bShow;
        this->update();
    }

    /// Show/Hide parcellation surface
    ADD_CLASS_FIELD_NOSETTER(bool, bShowParcellation, getShowParcellatio)
public:
    void setShowParcellation(bool bShow)
    {
        this->m_bShowParcellation = bShow;
        this->update();
    }

    /// Map Parcellation to Intensity ISO Surface
    ADD_CLASS_FIELD_NOSETTER(bool, bMapParcellation2Surface, getMapParcellation2Surface)
public:
    void setMapParcellation2Surface(bool bMap)
    {
        this->m_bMapParcellation2Surface = bMap;
        buildIntensitySurface();
    }


signals:

public slots:

};

#endif // GMISORENDERER_H
