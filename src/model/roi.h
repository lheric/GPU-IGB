#ifndef ROI_H
#define ROI_H

#include "gitldef.h"
#include <QObject>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkPNGWriter.h>
#include <vtkTexture.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkTextureMapToSphere.h>
#include <vtkTextureMapToPlane.h>
#include <vtkLookupTable.h>
#include <vtkTransformTextureCoords.h>
#include <QSize>
#include <vtkMath.h>
#include <QVector3D>
#include <vtkPNGReader.h>


#define TEXTURE_SINGLE_COLOR    0

#define TEXTURE_MAP_TO_SPHERE   0
#define TEXTURE_MAP_TO_PLANE    1

#define TEXTURE_TYPE_STRIP      0
#define TEXTURE_TYPE_CIRCLE     0
#define TEXTURE_TYPE_CONTOUR    1

#define TEXTURE_CONTOUR_DEBUG   0


class FMRITimeSeriesData;

class ROI : public QObject
{
    Q_OBJECT
public:
    explicit ROI(QObject *parent = 0);
    ~ROI();
    /**
     * @brief init
     */
    void init();


    /**
     * @brief removeFromRenderer remove this ROI from renderer
     * @param pcRenderer
     * @return
     */
    bool removeFromRenderer(vtkRenderer* pcRenderer);

    /**
     * @brief addToRenderer add this ROI to renderer
     * @param pcRenderer
     * @return
     */
    bool addToRenderer(vtkRenderer* pcRenderer);

    /**
     * @brief setLabel set the label of this ROI. In single color mode, it will also set the ROI color
     * @param iLabel
     */
    void setLabel(int iLabel);

    /**
     * @brief setColor set the color of ROI
     * @param rcColor
     */
    void setColor(const QColor& rcColor);

    /**
     * @brief setIsHighlighted highlight this ROI or not
     */
    void setIsHighlighted(bool bIsHightlighted);

    /**
     * @brief setOpacity set the opacity of this ROI
     * @param dOpacity
     */
    void setOpacity(double dOpacity);

    /**
     * @brief applyOpacity apply opacity or highlighted opacity if this ROI is selected or not
     */
    void applyOpacity();

    /**
     * @brief setTextureNormal set the normal of the plane for texture mapping
     * @param cNormal
     */
    void setTextureNormal(QVector3D& cNormal);

    /// ROI Label
    ADD_CLASS_FIELD_NOSETTER(int, iLabel, getLabel)

    /// ROI surface
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPolyData>, pcSurface, getSurface, setSurface)

    /// Textrue map to sphere
#if TEXTURE_MAP_TO_PLANE
    ADD_CLASS_FIELD(vtkSmartPointer<vtkTextureMapToPlane>, pcTextureMapper, getTextureMapper, setTextureMapper)
#elif TEXTURE_MAP_TO_SPHERE
    ADD_CLASS_FIELD(vtkSmartPointer<vtkTextureMapToSphere>, pcTextureMapper, getTextureMapper, setTextureMapper)
#endif

    /// Texture Transformer
    ADD_CLASS_FIELD(vtkSmartPointer<vtkTransformTextureCoords>, pcTransTCoord, getTransTCoord, setTransTCoord)

    /// Texture Reader
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPNGReader>, pcTextureReader, getTextureReader, setTextureReader)

    /// Texture
    ADD_CLASS_FIELD(vtkSmartPointer<vtkTexture>, pcTexture, getTexture, setTexture)

    /// Mapper
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPolyDataMapper>, pcMapper, getMapper, setMapper)

    /// Actor
    ADD_CLASS_FIELD(vtkSmartPointer<vtkActor>, pcActor, getActor, setActor)

    /// Normal of the texture mapped plane
    ADD_CLASS_FIELD_NOSETTER(QVector3D, cTextureNormal, getTextureNormal)

    /// Opacity
    ADD_CLASS_FIELD_NOSETTER(double, dOpacity, getOpacity)                          /// not selected/highlighted
    ADD_CLASS_FIELD_NOSETTER(double, dHighlightedOpacity, getHighlightedOpacity)    /// selected/highlighted

    /// This ROI is selected/highlighted or not
    ADD_CLASS_FIELD_NOSETTER(bool, bIsHightlighted, getIsHighlighted)

    /// Renderer it **has been** added to
    ADD_CLASS_FIELD(vtkSmartPointer<vtkRenderer>, pcRenderer, getRenderer, setRenderer)

signals:

public slots:
};

#endif // ROI_H
