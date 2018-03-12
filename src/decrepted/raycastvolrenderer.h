#ifndef RAYCASTVOLRENDERER_H
#define RAYCASTVOLRENDERER_H

#include <QObject>
#include <QVector3D>
#include <QVector4D>
#include <QVector>
#include <QImage>
#include <QColor>
#include <QSize>
#include <QSet>
#include <QPoint>
#include <vtkViewport.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include "gitldef.h"
#include "model/matrix.h"
#include "layertable.h"
#include "ray.h"

class Subject;
class FMRITimeSeriesData;
class FunctionalConnectivityData;
class MultiDiseaseMeasurement;
class MultiDiseaseConnectivity;

enum CameraType { PERSPECTIVE, SPHERICAL };

//enum RenderStyle { TRANSLUCENT,  SOLID};

class RayCastVolRenderer : public QObject
{
    Q_OBJECT
public:
    explicit RayCastVolRenderer(QObject *parent = 0);
    ~RayCastVolRenderer();
    void init();
    void render();
    void renderFMRITexture(FMRITimeSeriesData* pcFMRITimeSeriesData);
    void renderMDTexture(MultiDiseaseMeasurement* pcMDMeasurement);
    void renderNetwork(FunctionalConnectivityData* pcConnectivity, bool bDebugMode = false);
    void renderNetworkTile(FunctionalConnectivityData* pcConnectivity, bool bDebugMode = false);
    void renderNetwork(MultiDiseaseConnectivity* pcConnectivity, bool bDebugMode = false);
    void renderNetworkTile(MultiDiseaseConnectivity* pcConnectivity, bool bDebugMode = false);

    bool xBiasCheck(QVector<double>& adValues, double dThreshold);
    void xGetConnectionColor(QVector<double> &adValues, unsigned char *auhColor);


    void composeRays();
    void intersectSpheres(QVector3D& rcCenter, double dRadius, Ray &rcRay);
    void intersectBox(double* adDim, Ray &rcRay);
    void computeBound(vtkImageData* pcVolume);
    bool saveRenderedImage(QString strImgPath);
//    QImage hammerAitoffProjection(QImage& rcInputImg, bool bFillBG = true);
//    const int* colorTransferFunc(int iLabel, int *piLabel = NULL);
    void initPerspectiveRays();
    void initSphericalRays();
    void overlayTextures(QString strFilePattern);
    void drawLatiLogi();
    bool blendingTextureAndLighting(QImage& rcTexture, QImage& rcLighting, QImage &rcResult);
protected:
//    QColor interpolateLabelColor(QVector3D& rcLocation, vtkImageData *pcVol, int* piLabel = NULL);
//    double interpolateIntensity(QVector3D& rcLocation, vtkImageData *pcVol);
    void xFillSamplePointInfo(RaySamplePoint& rcSamplePnts, QVector3D& rcLocation, vtkImageData* pcLabelImg, vtkImageData* pcIntensityImg, vtkImageData* pcGradientImg);
    void xRayMarch(Ray* pcRay);



    ADD_CLASS_FIELD_NOSETTER(LayerTable, cLayerTabel, getLayerTabel)
    ADD_CLASS_FIELD(CameraType, eCameraType, getCameraType, setCameraType)
    ADD_CLASS_FIELD_NOSETTER(QVector<Matrix<int>>, cLabelImg, getLabelImg)
//    ADD_CLASS_FIELD(RenderStyle, eRenderStyle, getRenderStyle, setRenderStyle)
    ADD_CLASS_FIELD_NOSETTER(QVector<Ray>, cRays, getRays)
    ADD_CLASS_FIELD(Subject*, pcSubject, getSubject, setSubject)
    ADD_CLASS_FIELD(vtkRenderer*, pcRenderer, getRenderer, setRenderer)
    ADD_CLASS_FIELD_NOSETTER(QImage, cImage, getImage)
    ADD_CLASS_FIELD(QSize, cSize, getSize, setSize)
    ADD_CLASS_FIELD(QColor, cBackgroundColor, getBGColor, setBGColor)
    ADD_CLASS_FIELD(double, dTStep, getTStep, setTStep)

    ADD_CLASS_FIELD_NOSETTER(int, iXMin, getXMin)
    ADD_CLASS_FIELD_NOSETTER(int, iXMax, getXMax)
    ADD_CLASS_FIELD_NOSETTER(int, iXCordMin, getXCordMin)
    ADD_CLASS_FIELD_NOSETTER(int, iXCordMax, getXCordMax)
    ADD_CLASS_FIELD_NOSETTER(double, dXSpacing, getXSpacing)

    ADD_CLASS_FIELD_NOSETTER(int, iYMin, getYMin)
    ADD_CLASS_FIELD_NOSETTER(int, iYMax, getYMax)
    ADD_CLASS_FIELD_NOSETTER(int, iYCordMin, getYCordMin)
    ADD_CLASS_FIELD_NOSETTER(int, iYCordMax, getYCordMax)
    ADD_CLASS_FIELD_NOSETTER(double, dYSpacing, getYSpacing)

    ADD_CLASS_FIELD_NOSETTER(int, iZMin, getZMin)
    ADD_CLASS_FIELD_NOSETTER(int, iZMax, getZMax)
    ADD_CLASS_FIELD_NOSETTER(int, iZCordMin, getZCordMin)
    ADD_CLASS_FIELD_NOSETTER(int, iZCordMax, getZCordMax)
    ADD_CLASS_FIELD_NOSETTER(double, dZSpacing, getZSpacing)

    ADD_CLASS_FIELD(double, dDeadZone, getDeadZone, setDeadZone)

    /// lighting
    ADD_CLASS_FIELD(int, iHighlightLayer, getHightlightLayer, setHightlightLayer)
    ADD_CLASS_FIELD(double, dRhoAmb, getRhoAmb, setRhoAmb)
    ADD_CLASS_FIELD(double, dRhoDif, getRhoDif, setRhoDif)
    ADD_CLASS_FIELD(double, dRhoSpe, getRhoSpe, setRhoSpe)
    ADD_CLASS_FIELD(double, dF, getF, setF)
    ADD_CLASS_FIELD(double, dIntensityFactor, getIntensityFactor, setIntensityFactor)
    ADD_CLASS_FIELD(QVector4D, cLightColor, getLightColor, setLightColor)


    /// texture & network
    ADD_CLASS_FIELD_NOSETTER(QVector<QPointF>, acROICenters, getROICenters)

signals:

public slots:
};





#endif // RAYCASTVOLRENDERER_H
