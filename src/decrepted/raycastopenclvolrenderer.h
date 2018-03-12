#ifndef RAYCASTOPENCLVOLRENDERER_H
#define RAYCASTOPENCLVOLRENDERER_H

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
#include "cl.hpp"
#include <vtkPointWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindowInteractor.h>

class Subject;
class FMRITimeSeriesData;
class FunctionalConnectivityData;
class MultiDiseaseMeasurement;
class MultiDiseaseConnectivity;

struct RaycastParameters
{
    cl_float3 center;
//    cl_float3 cam_up;
//    cl_float3 cam_view;
    cl_float ambient = 0.5;
    cl_float diffuse = 0.4;
    cl_float specular = 0.77;
    cl_float transparency = 0.5;
    cl_float f = 76.0;
    cl_float padding0;
    cl_float padding1;
    cl_float padding2;
};


struct OpenCLContext
{    
    std::vector<cl::Platform> platforms;
    cl::Platform chosen_platform;
    std::vector<cl::Device> devices;
    cl::Device chosen_device;
    cl::Context context;
    cl::Program::Sources sources;
    cl::Program program;
    cl::CommandQueue queue;
    cl::Kernel kernel;
    QString kernal_name = "far_to_near";

    cl_float* intensityVolBuf = NULL;
    cl::Image3D intensityVol;

    cl_float* labelColorVolBuf = NULL;
    cl::Image3D labelColorVol;

    cl_short* labelVolBuf = NULL;
    cl::Image3D labelVol;

    cl_float* gradientVolBuf = NULL;
    cl::Image3D gradientVol;


    cl_float4* colorTableBuf = NULL;
    cl::Buffer colorTable;
    cl_int tableSize = 0;


    cl_float4* renderedImgBuf = NULL;
    cl::Image2D renderedImg;

    cl_int* occlusionMatrixBuf = NULL;
    cl::Buffer occlusionMatrix;
    cl_int occMatrixSize;


    RaycastParameters parameters;
};



class RayCastOpenCLVolRenderer : public QObject
{
    Q_OBJECT
public:
    explicit RayCastOpenCLVolRenderer(QObject *parent = 0);
    ~RayCastOpenCLVolRenderer();
    void init();
    void setSubject(Subject* pcSubject);
    void setInteractor(vtkRenderWindowInteractor* pcInteractor);
    bool initOpenCL();

    bool getResultFromDevice();
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

public slots:
    void copyIntensityVolumeToDevice();
    void copyGradientVolumeToDevice();
    void copyLabelVolumeToDevice();
    void copyColorTableToDevice();
    void resetCameraPointWidget();
    void onCameraPosMove();

protected:
    //    QColor interpolateLabelColor(QVector3D& rcLocation, vtkImageData *pcVol, int* piLabel = NULL);
    //    double interpolateIntensity(QVector3D& rcLocation, vtkImageData *pcVol);
    void xFillSamplePointInfo(RaySamplePoint& rcSamplePnts, QVector3D& rcLocation, vtkImageData* pcLabelImg, vtkImageData* pcIntensityImg, vtkImageData* pcGradientImg);
    void xRayMarch(Ray* pcRay);



    ADD_CLASS_FIELD_NOSETTER(LayerTable, cLayerTabel, getLayerTabel)

    ADD_CLASS_FIELD_NOSETTER(QVector<Matrix<int>>, cLabelImg, getLabelImg)


    ADD_CLASS_FIELD_NOSETTER(QVector<Ray>, cRays, getRays)

    ADD_CLASS_FIELD_NOSETTER(Subject*, pcSubject, getSubject)
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

    ADD_CLASS_FIELD_NOSETTER(double, iYMin, getYMin)
    ADD_CLASS_FIELD_NOSETTER(double, iYMax, getYMax)
    ADD_CLASS_FIELD_NOSETTER(int, iYCordMin, getYCordMin)
    ADD_CLASS_FIELD_NOSETTER(int, iYCordMax, getYCordMax)
    ADD_CLASS_FIELD_NOSETTER(double, dYSpacing, getYSpacing)

    ADD_CLASS_FIELD_NOSETTER(double, iZMin, getZMin)
    ADD_CLASS_FIELD_NOSETTER(double, iZMax, getZMax)
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

    /// opencl context
    ADD_CLASS_FIELD_NOSETTER(OpenCLContext, cOpenCLContext, getOpenCLContext)

    ///
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<vtkPointWidget>, pcPointWidget, getPointWidget)

    ///
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<vtkRenderWindowInteractor>, pcInteractor, getInteractor)

    ///
    ADD_CLASS_FIELD_NOSETTER(QImage, cRenderedImage, getRenderedImage)




};





#endif // RAYCASTOPENCLVOLRENDERER_H
