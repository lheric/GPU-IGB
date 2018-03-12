#include "raycastopenclvolrenderer.h"
#include "model/intensityvolume.h"
#include <vtkCamera.h>
#include <vtkViewport.h>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QPoint>
#include <qmath.h>
#include <QVector4D>
#include "algorithm/vtkutils.h"
#include "model/subject.h"
#include "algorithm/imageutils.h"
#include "model/rom.h"
#include <vtkLookupTable.h>
#include <vtkImageGradient.h>
#include "perlintexturegenerator.h"
#include "colorconverter.h"
#include <vtkCommand.h>
#include <QPainter>
#include "gitlivkcmdevt.h"

#define Floats2Vector3D(ad3fArray) QVector3D(ad3fArray[0], ad3fArray[1], ad3fArray[2])
#define Floats2Vector4D(ad3fArray) QVector4D(ad3fArray[0], ad3fArray[1], ad3fArray[2], ad3fArray[3])
#define EPSILON 0.01
#define RAYCASTDEBUG 0

static bool xCheckOpenCLError(cl_int iCode, QString strMessage)
{
    if(iCode != CL_SUCCESS)
    {
        qCritical() << "Error!" << strMessage << "\nCode:" << iCode;
        return false;
    }
    return true;
}

RayCastOpenCLVolRenderer::RayCastOpenCLVolRenderer(QObject *parent) : QObject(parent)
{
    init();
}

RayCastOpenCLVolRenderer::~RayCastOpenCLVolRenderer()
{

}

void RayCastOpenCLVolRenderer::init()
{
    m_pcPointWidget = vtkSmartPointer<vtkPointWidget>::New();
    m_pcPointWidget->AddObserver(vtkCommand::InteractionEvent, this, &RayCastOpenCLVolRenderer::onCameraPosMove);
    m_pcPointWidget->AllOff();

    m_cSize = QSize(-1,-1);
    m_cImage = QImage(100, 100, QImage::Format_ARGB32);
    m_cRays.clear();
    m_pcSubject = NULL;
    m_pcRenderer = NULL;
    m_iXMin = m_iXMax = -1; m_dXSpacing = 1; m_iXCordMin = m_iXCordMax = -1;
    m_iYMin = m_iYMax = -1; m_dYSpacing = 1; m_iYCordMin = m_iYCordMax = -1;
    m_iZMin = m_iZMax = -1; m_dZSpacing = 1; m_iZCordMin = m_iZCordMax = -1;
    m_cBackgroundColor = QColor(0,0,0,255);
    m_dDeadZone = 1.5;

    /// lighting
    m_dTStep = 1;
    m_dRhoAmb = 0.5;   ///0.25
    m_dRhoDif = 0.4;    ///0.4
    m_dRhoSpe = 0.77;   ///0.77
    m_dF = 76;          ///76
    m_iHighlightLayer = -1;
    m_dIntensityFactor = 1;
    m_cLightColor = QVector4D(1,1,1,1);

    /// init opencl
    initOpenCL();
}

void RayCastOpenCLVolRenderer::setSubject(Subject *pcSubject)
{
    m_pcSubject = pcSubject;
    connect(&m_pcSubject->getGrayMat().getIntensityVol(),  SIGNAL(updated()), this, SLOT(copyIntensityVolumeToDevice()));
    connect(&m_pcSubject->getGrayMat().getIntensityVol(),  SIGNAL(updated()), this, SLOT(copyGradientVolumeToDevice()));
    connect(&m_pcSubject->getGrayMat().getIntensityVol(),  SIGNAL(updated()), this, SLOT(resetCameraPointWidget()));
    connect(&m_pcSubject->getGrayMat().getLabelVol(), SIGNAL(updated()), this, SLOT(copyLabelVolumeToDevice()));
}

void RayCastOpenCLVolRenderer::setInteractor(vtkRenderWindowInteractor *pcInteractor)
{
    m_pcInteractor = pcInteractor;
    m_pcPointWidget->SetInteractor(pcInteractor);
}

bool RayCastOpenCLVolRenderer::initOpenCL()
{
    OpenCLContext& rcOpenCLContext = this->getOpenCLContext();
    cl::Platform::get(&rcOpenCLContext.platforms);

    if(rcOpenCLContext.platforms.size() == 0) {
        std::cerr << "No platforms available" <<std::endl;
        return false;
    }

    //list all available platforms
    for(auto& platform : rcOpenCLContext.platforms) {
        std::cout << "Available platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
    }

    //choose the first available platform (I'll assume we only have one installed)
    rcOpenCLContext.chosen_platform = rcOpenCLContext.platforms[0];    /// TODO BUG auto GPU choose needed
    std::cout << "Using platform: " << rcOpenCLContext.chosen_platform.getInfo<CL_PLATFORM_NAME>() << std::endl;


    //retrieve all devices usable by the selected platform
    rcOpenCLContext.chosen_platform.getDevices(CL_DEVICE_TYPE_ALL, &rcOpenCLContext.devices);

    if(rcOpenCLContext.devices.size() == 0) {
        std::cerr << "No device found" << std::endl;
        return false;
    }

    //list all available devices
    for(cl::Device& device : rcOpenCLContext.devices) {
        std::cout << "Available device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        std::cout << "Vendor: " << device.getInfo<CL_DEVICE_VENDOR>() << std::endl;
        std::cout << "Device Version: " << device.getInfo<CL_DEVICE_VERSION>() << std::endl;
        std::cout << "Driver Version: " << device.getInfo<CL_DRIVER_VERSION>() << std::endl;
        std::cout << "Device Size:" << device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()[0] << " "
                                    << device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()[1] << " "
                                    << device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()[2] << " "
                                    << std::endl;
    }

    //choose the first available devices
    rcOpenCLContext.chosen_device = rcOpenCLContext.devices[0];
    std::cout << "Using device: " << std::string(rcOpenCLContext.chosen_device.getInfo<CL_DEVICE_NAME>()) << std::endl;


    rcOpenCLContext.context = cl::Context({rcOpenCLContext.chosen_device});


    QString strSourcePath = "E:\\Learning\\Github\\BrainExplorerForConnectomicAnalysis\\src\\assets\\opencl_kernel\\kernel.cl";
    QFile cSourceFile(strSourcePath);

    if(!cSourceFile.open(QIODevice::ReadOnly))
    {
        qCritical() << "Fail to read" << strSourcePath;
        return false;
    }
    QString strKernelCode = QTextStream(&cSourceFile).readAll();
//    qDebug() << strKernelCode;
    cSourceFile.close();

    rcOpenCLContext.sources.push_back({strKernelCode.toUtf8().data(), strKernelCode.toUtf8().size()+1});

    rcOpenCLContext.program = cl::Program(rcOpenCLContext.context,rcOpenCLContext.sources);
    if(rcOpenCLContext.program.build({rcOpenCLContext.chosen_device})!=CL_SUCCESS){
        std::cout<<" Error building: "<<rcOpenCLContext.program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(rcOpenCLContext.chosen_device)<<"\n";
        return false;
    }

    //create queue to which we will push commands for the device.
    m_cOpenCLContext.queue = cl::CommandQueue(m_cOpenCLContext.context,
                                              m_cOpenCLContext.chosen_device);


    // copy color table to deivce
    this->copyColorTableToDevice();

    return true;
}

void RayCastOpenCLVolRenderer::copyIntensityVolumeToDevice()
{

    /// check
    vtkImageData* pcIntensityVolume = this->getSubject()->getGrayMat().getIntensityVol().getVolume();
    if(VTKUtils::isEmptyDimension(pcIntensityVolume->GetDimensions()))
    {
        qWarning() << "Invalid Intensity Volume";
        return;
    }

    /// create buffers on the device
    int* iDim = pcIntensityVolume->GetDimensions();
    delete [] m_cOpenCLContext.intensityVolBuf;
    m_cOpenCLContext.intensityVolBuf = new float[iDim[0]*iDim[1]*iDim[2]];

    /// copy
    for(int x = 0; x < iDim[0]; x++)
    {
        for(int y = 0; y < iDim[1]; y++)
        {
            for(int z = 0; z < iDim[2]; z++)
            {
                int iOffset = z*iDim[0]*iDim[1] + y*iDim[0] + x;
                m_cOpenCLContext.intensityVolBuf[iOffset] = pcIntensityVolume->GetScalarComponentAsFloat(x,y,z,0);
            }
        }
    }

    ///
    cl::ImageFormat format;
    format.image_channel_data_type = CL_FLOAT;
    format.image_channel_order = CL_LUMINANCE;
    cl_int iResult;
    m_cOpenCLContext.intensityVol = cl::Image3D( m_cOpenCLContext.context,
                                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR ,
                                                 format, iDim[0], iDim[1], iDim[2], 0, 0,
                                                 m_cOpenCLContext.intensityVolBuf, &iResult);
    xCheckOpenCLError(iResult, "Copying Intensity Volume to Divece Fail");
}


void RayCastOpenCLVolRenderer::copyGradientVolumeToDevice()
{

    /// check
    vtkImageData* pcGradientVolume = this->getSubject()->getGrayMat().getIntensityVol().getGradient();
    if(VTKUtils::isEmptyDimension(pcGradientVolume->GetDimensions()))
    {
        qWarning() << "Invalid Gradient Volume";
        return;
    }

    /// create buffers on the device
    int* iDim = pcGradientVolume->GetDimensions();
    delete [] m_cOpenCLContext.gradientVolBuf;
    m_cOpenCLContext.gradientVolBuf = new cl_float[iDim[0]*iDim[1]*iDim[2]*4];


    /// copy
    for(int x = 0; x < iDim[0]; x++)
    {
        for(int y = 0; y < iDim[1]; y++)
        {
            for(int z = 0; z < iDim[2]; z++)
            {
                for(int d = 0; d < 3; d++)
                {
                    int iOffset = z*iDim[1]*iDim[0]*4 + y*iDim[0]*4 + x*4 + d;
                    m_cOpenCLContext.gradientVolBuf[iOffset] = pcGradientVolume->GetScalarComponentAsFloat(x,y,z,d);
                }

            }
        }
    }

    ///
    cl::ImageFormat format;
    format.image_channel_data_type = CL_FLOAT;
    format.image_channel_order = CL_RGBA;
    cl_int iResult;
    m_cOpenCLContext.gradientVol =  cl::Image3D( m_cOpenCLContext.context,
                                                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR ,
                                                format, iDim[0], iDim[1], iDim[2], 0, 0,
                                                m_cOpenCLContext.gradientVolBuf, &iResult);
    xCheckOpenCLError(iResult, "Copying Gradient Volume to Divece Fail");

}

void RayCastOpenCLVolRenderer::copyLabelVolumeToDevice()
{
    /// check
    vtkImageData* pcLabelVolume = this->getSubject()->getGrayMat().getLabelVol().getUnclippedVolume();
    if(VTKUtils::isEmptyDimension(pcLabelVolume->GetDimensions()))
    {
        qWarning() << "Invalid Label Volume";
        return;
    }


    /// colored label volume
    /// create buffers on the device
    int* iDim = pcLabelVolume->GetDimensions();
    delete [] m_cOpenCLContext.labelColorVolBuf;
    m_cOpenCLContext.labelColorVolBuf = new cl_float[iDim[0]*iDim[1]*iDim[2]*4];

    /// copy
    for(int x = 0; x < iDim[0]; x++)
    {
        for(int y = 0; y < iDim[1]; y++)
        {
            for(int z = 0; z < iDim[2]; z++)
            {
                int iLabel = round(pcLabelVolume->GetScalarComponentAsFloat(x,y,z,0));
                QVector4D& rcColor = g_acColorTableForRaycastHighlight[iLabel%COLOR_TABLE_SIZE];
                for(int channel = 0; channel < 4; channel++)
                {
                    int iOffset = z*iDim[0]*iDim[1]*4 + y*iDim[0]*4 + x*4 + channel;
                    m_cOpenCLContext.labelColorVolBuf[iOffset] = rcColor[channel];

                }

            }
        }
    }

    ///
    cl::ImageFormat format;
    format.image_channel_data_type = CL_FLOAT;
    format.image_channel_order = CL_RGBA;
    int iResult;
    m_cOpenCLContext.labelColorVol = cl::Image3D( m_cOpenCLContext.context,
                                             CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR ,
                                             format, iDim[0], iDim[1], iDim[2], 0, 0,
                                             m_cOpenCLContext.labelColorVolBuf, &iResult);
    xCheckOpenCLError(iResult, "Copying Label Color Volume to Divece Fail");


    /// label volume
    /// create buffers on the device
    delete [] m_cOpenCLContext.labelVolBuf;
    m_cOpenCLContext.labelVolBuf = new cl_short[iDim[0]*iDim[1]*iDim[2]];

    /// copy
    for(int x = 0; x < iDim[0]; x++)
    {
        for(int y = 0; y < iDim[1]; y++)
        {
            for(int z = 0; z < iDim[2]; z++)
            {
                int iOffset = z*iDim[0]*iDim[1] + y*iDim[0] + x;
                m_cOpenCLContext.labelVolBuf[iOffset] = round(pcLabelVolume->GetScalarComponentAsFloat(x,y,z,0));
            }
        }
    }

    ///
    format.image_channel_data_type = CL_SIGNED_INT16;
    format.image_channel_order = CL_LUMINANCE;
    m_cOpenCLContext.labelVol = cl::Image3D( m_cOpenCLContext.context,
                                             CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR ,
                                             format, iDim[0], iDim[1], iDim[2], 0, 0,
                                             m_cOpenCLContext.labelVolBuf, &iResult);
    xCheckOpenCLError(iResult, "Copying Label Volume to Divece Fail");


    ///
    delete [] m_cOpenCLContext.occlusionMatrixBuf;
    int iROINumPlus1 = this->getSubject()->getGrayMat().getLabelSurface().getROIs().size()+1;
    m_cOpenCLContext.occMatrixSize = iROINumPlus1;
    m_cOpenCLContext.occlusionMatrixBuf = new cl_int[iROINumPlus1*iROINumPlus1];
    memset(m_cOpenCLContext.occlusionMatrixBuf, 0, sizeof(cl_int)*iROINumPlus1*iROINumPlus1);
    m_cOpenCLContext.occlusionMatrix = cl::Buffer(  m_cOpenCLContext.context,
                                                    cl_mem_flags(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR),
                                                    size_t(iROINumPlus1*iROINumPlus1*sizeof(cl_int)),
                                                    m_cOpenCLContext.occlusionMatrixBuf,
                                                    &iResult);
    xCheckOpenCLError(iResult, "Copying Occlusion Matrix to Divece Fail");

}

void RayCastOpenCLVolRenderer::copyColorTableToDevice()
{

    /// create buffers on the device
    delete [] m_cOpenCLContext.colorTableBuf;
    m_cOpenCLContext.colorTableBuf = new cl_float4[COLOR_TABLE_SIZE];

    /// copy
    for(int i = 0; i < COLOR_TABLE_SIZE; i++)
    {

        m_cOpenCLContext.colorTableBuf[i].x = g_acColorTableForRaycastHighlight[i].x();
        m_cOpenCLContext.colorTableBuf[i].y = g_acColorTableForRaycastHighlight[i].y();
        m_cOpenCLContext.colorTableBuf[i].z = g_acColorTableForRaycastHighlight[i].z();
        m_cOpenCLContext.colorTableBuf[i].w = g_acColorTableForRaycastHighlight[i].w();
    }

    ///
    cl::ImageFormat format;
    format.image_channel_data_type = CL_SNORM_INT16;
    format.image_channel_order = CL_LUMINANCE;
    cl_int iResult;
    m_cOpenCLContext.colorTable = cl::Buffer( m_cOpenCLContext.context,
                                              (cl_mem_flags)(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR) ,
                                              sizeof(cl_float4)*COLOR_TABLE_SIZE,
                                              m_cOpenCLContext.colorTableBuf, &iResult);
    xCheckOpenCLError(iResult, "Copying Color Table to Divece Fail");
}

void RayCastOpenCLVolRenderer::resetCameraPointWidget()
{
    auto pcPointWidget = this->getPointWidget();

    pcPointWidget->SetPosition(0,0,0);
    pcPointWidget->SetInteractor(this->getInteractor());
    pcPointWidget->PlaceWidget(this->getSubject()->getGrayMat().getIntensityVol().getVolume()->GetBounds());
}

void RayCastOpenCLVolRenderer::onCameraPosMove()
{
    qDebug() << "Moving Center";
    double* adPos = this->getPointWidget()->GetPosition();
    vtkImageData *pcProbVol = this->getSubject()->getGrayMat().getIntensityVol().getVolume().GetPointer();

    QVector3D cCenter;
    for(int i = 0; i < 3; i++)
        cCenter[i] = adPos[i]/pcProbVol->GetSpacing()[i];

    GitlIvkCmdEvt cEvt("change_raycast_parameters");
    cEvt.setParameter("change_center", true);
    cEvt.setParameter("new_center", cCenter);
    cEvt.dispatch();
}

void RayCastOpenCLVolRenderer::initPerspectiveRays()
{
    vtkCamera* pcCamera = this->getRenderer()->GetActiveCamera();
    QVector3D cCamPos = Floats2Vector3D(pcCamera->GetPosition());
    QVector3D cCamUp  = Floats2Vector3D(pcCamera->GetViewUp());
    QVector3D cCamFo  = Floats2Vector3D(pcCamera->GetFocalPoint());
    double dAngle = pcCamera->GetViewAngle();   /// *** in degree, typically 30 ***
    double dAngleRadian = dAngle/360*vtkMath::Pi();
    double* adAspect = this->getRenderer()->GetAspect();
    double dAspectRatio = adAspect[0] / adAspect[1];
    int iWidth = this->getRenderer()->GetSize()[0];
    int iHeight = this->getRenderer()->GetSize()[1];
    m_cSize = QSize(iWidth, iHeight);

#if RAYCASTDEBUG
    qDebug() << "Resolution is" << iWidth << "X" << iHeight;
    qDebug() << "Camera" << cCamPos << cCamUp << cCamFo;
    qDebug() << "Aspect Ratio" << adAspect[0] << adAspect[1] << dAspectRatio;
    qDebug() << "Vertical Angle is" << dAngle;
#endif
    /// init rays
    this->getRays().clear();

    QVector3D cCenterToEye = (cCamFo - cCamPos).normalized();
    QVector3D cUpUnit = cCamUp.normalized()*(cCenterToEye.length()*tan(dAngleRadian)/(iHeight/2.0));
    QVector3D cRightUnit = QVector3D::crossProduct(cCenterToEye, cUpUnit).normalized();
    cRightUnit = cRightUnit * cUpUnit.length();

#if RAYCASTDEBUG
    QFile cRayFile;
    cRayFile.setFileName("ray_direction.txt");
    cRayFile.open(QIODevice::WriteOnly);
    QTextStream cRayStream(&cRayFile);

    QFile cHitFile;
    cHitFile.setFileName("hit_points.txt");
    cHitFile.open(QIODevice::WriteOnly);
    QTextStream cHitStream(&cHitFile);
#endif


    vtkImageData* pcVolume = this->getSubject()->getGrayMat().getIntensityVol().getVolume().GetPointer();
    this->computeBound(pcVolume);

    double* adSpaces = pcVolume->GetSpacing();
    int* iExtends = pcVolume->GetExtent();

    double dVolumeRadius =  (iExtends[1]*adSpaces[0])*(iExtends[1]*adSpaces[0]) +
                            (iExtends[3]*adSpaces[1])*(iExtends[3]*adSpaces[1]) +
                            (iExtends[5]*adSpaces[2])*(iExtends[5]*adSpaces[2]);
    QVector3D cVolumeCenter =   Floats2Vector3D(pcVolume->GetOrigin()) +
                                QVector3D(iExtends[1]*adSpaces[0]/2, iExtends[3]*adSpaces[1]/2, iExtends[5]*adSpaces[2]/2);
    dVolumeRadius = qSqrt(dVolumeRadius)/2;

    double adBox[6];
    for(int iDim = 0; iDim < 3; iDim++)
    {
        adBox[iDim*2]   = iExtends[iDim*2]  *adSpaces[iDim];
        adBox[iDim*2+1] = iExtends[iDim*2+1]*adSpaces[iDim];
    }
//    qDebug() << iExtends[1] << iExtends[3] << iExtends[5];

#if RAYCASTDEBUG
    qDebug() << "Vol Center & Radius" << cVolumeCenter << dVolumeRadius;
#endif

    Ray cRay;
    for(int i = -iHeight/2; i < iHeight/2; i++)
    {
        for(int j = -iWidth/2; j < iWidth/2; j++)
        {
            QVector3D cRayDir = cCenterToEye + i*cUpUnit + j*cRightUnit;
            cRayDir.normalize();
            cRay.setPoint(cCamPos);
            cRay.setDir(cRayDir);
            cRay.setViewportPos(QPoint(j+iWidth/2,(iHeight-1)-(i+iHeight/2)));  /// (iHeight-1)-(i+iHeight/2) for fliping Y axes beacuse Y axes is downwards in image

            /// find intersection
            this->intersectSpheres(cVolumeCenter, dVolumeRadius, cRay);
#if RAYCASTDEBUG
            cRayStream   << (cRay.getPoint()+cRayDir).x() << ","
                         << (cRay.getPoint()+cRayDir).y() << ","
                         << (cRay.getPoint()+cRayDir).z() << endl;

            if(cRay.getHit())
            {
                QVector3D cHit1 = cRay.getPoint() + cRayDir*cRay.getTStart();
                cHitStream   << cHit1.x() << ","
                             << cHit1.y() << ","
                             << cHit1.z() << endl;

                QVector3D cHit2 = cRay.getPoint() + cRayDir*cRay.getTEnd();
                cHitStream << cHit2.x() << ","
                             << cHit2.y() << ","
                             << cHit2.z() << endl;
            }
#endif
            this->getRays().push_back(cRay);
        }
    }

#if RAYCASTDEBUG
    cHitFile.close();
    cRayFile.close();
#endif
}

void RayCastOpenCLVolRenderer::initSphericalRays()
{
    vtkImageData* pcVolume = this->getSubject()->getGrayMat().getIntensityVol().getVolume().GetPointer();
    double* adSpaces = pcVolume->GetSpacing();
    int* iExtends = pcVolume->GetExtent();
    QVector3D cCamPos = Floats2Vector3D(pcVolume->GetCenter());
    cCamPos += QVector3D(+4,0,0);
    int iWidth = this->getRenderer()->GetSize()[0];
    int iHeight = this->getRenderer()->GetSize()[1];
    m_cSize = QSize(iWidth, iHeight);

#if RAYCASTDEBUG
    qDebug() << "Resolution is" << iWidth << "X" << iHeight;
    qDebug() << "Camera" << cCamPos;
#endif
    /// init rays
    this->getRays().clear();



#if RAYCASTDEBUG
    QFile cRayFile;
    cRayFile.setFileName("ray_direction.txt");
    cRayFile.open(QIODevice::WriteOnly);
    QTextStream cRayStream(&cRayFile);

    QFile cHitFile;
    cHitFile.setFileName("hit_points.txt");
    cHitFile.open(QIODevice::WriteOnly);
    QTextStream cHitStream(&cHitFile);
#endif


    this->computeBound(pcVolume);


    double adBox[6];
    for(int iDim = 0; iDim < 3; iDim++)
    {
        adBox[iDim*2]   = iExtends[iDim*2]  *adSpaces[iDim];
        adBox[iDim*2+1] = iExtends[iDim*2+1]*adSpaces[iDim];
    }

#if RAYCASTDEBUG
    qDebug() << "Box" << adBox[0] << adBox[1] << adBox[2] << adBox[3] << adBox[4] << adBox[5];
#endif

    Ray cRay;
    for(int i = 0; i < iHeight; i++)
    {
        double dAngleHeight = (double(i)/iHeight-0.5)*M_PI;
        for(int j = 0; j < iWidth; j++)
        {
            double dAngleWidth = (double(j)/iWidth-0.25)*2*M_PI;
            QVector3D cRayDir = QVector3D(qCos(dAngleWidth)*qCos(dAngleHeight),
                                          qSin(dAngleWidth)*qCos(dAngleHeight),
                                          qSin(dAngleHeight));
            cRayDir.normalize();
            cRay.setPoint(cCamPos);
            cRay.setDir(cRayDir);
            cRay.setViewportPos(QPoint(j,(iHeight-1)-i));  /// fliping Y axes beacuse Y axes is downwards in image

            /// find intersection
            this->intersectBox(adBox, cRay);

#if RAYCASTDEBUG
            cRayStream   << (cRay.getPoint()+cRayDir).x() << ","
                         << (cRay.getPoint()+cRayDir).y() << ","
                         << (cRay.getPoint()+cRayDir).z() << endl;

            if(cRay.getHit())
            {
                QVector3D cHit1 = cRay.getPoint() + cRayDir*cRay.getTStart();
                cHitStream   << cHit1.x() << ","
                             << cHit1.y() << ","
                             << cHit1.z() << endl;

                QVector3D cHit2 = cRay.getPoint() + cRayDir*cRay.getTEnd();
                cHitStream << cHit2.x() << ","
                             << cHit2.y() << ","
                             << cHit2.z() << endl;
            }
#endif
            this->getRays().push_back(cRay);

            /// fill the ray for OpenCL (coord in voxel space)
//            CLRay* pcRay = &(m_cOpenCLContext.rayBuf[i*iWidth+j]);
//            pcRay->direction.x = cRayDir.x()/m_dXSpacing;
//            pcRay->direction.y = cRayDir.y()/m_dYSpacing;
//            pcRay->direction.z = cRayDir.z()/m_dZSpacing;
//            pcRay->point.x = (cCamPos.x()-m_iXMin)/m_dXSpacing;
//            pcRay->point.y = (cCamPos.y()-m_iYMin)/m_dYSpacing;
//            pcRay->point.z = (cCamPos.z()-m_iZMin)/m_dZSpacing;
//            pcRay->tEnd = cRay.getTEnd();
//            pcRay->tStart = cRay.getTStart();

        }
    }

#if RAYCASTDEBUG
    cHitFile.close();
    cRayFile.close();
#endif
}

void RayCastOpenCLVolRenderer::overlayTextures(QString strFilePattern)
{
    auto cLayerLabels = this->getLayerTabel().getLayerToLabel();
    for(int iLayer = 0; iLayer < cLayerLabels.size(); iLayer++)
    {
        QSet<int>& rcLabels = cLayerLabels[iLayer];
        qDebug() << "For layer:" << iLayer << rcLabels;

        QImage cBaseImg;
        QImage cOverlayImg;
        for(auto cROIIter = rcLabels.begin(); cROIIter != rcLabels.end(); cROIIter++)
        {
            int iROI = *cROIIter;
            if(cROIIter == rcLabels.begin())
            {
                cBaseImg.load(strFilePattern.arg(iROI));
            }
            else
            {
                cOverlayImg.load(strFilePattern.arg(iROI));
                ImageUtils::overlayImage(cBaseImg, cOverlayImg);
            }

        }
        cBaseImg.save(QString("Layer_#%1.png").arg(iLayer));
    }
}

bool RayCastOpenCLVolRenderer::blendingTextureAndLighting(QImage &rcTexture, QImage &rcLighting, QImage& rcResult)
{
    if(rcTexture.size().isEmpty() || rcTexture.size() != rcLighting.size())
    {
        qWarning() << "Invalid Texture and Lighting Map.";
        qWarning() << "Texture:" << rcTexture.size() << "Lighting:" << rcLighting.size();
        return false;
    }
    rcResult = QImage(rcTexture.size(), QImage::Format_ARGB32);

    QRgb cLightPix; QRgb cTexturePix;
    int iR, iG, iB, iA;

#define CLIP_255(component) \
    ((component) > 255 ? 255 : component);

    for(int i = 0; i < rcTexture.width(); i++)
    {
        for(int j = 0; j < rcTexture.height(); j++)
        {
            cLightPix = rcLighting.pixel(i, j);
            cTexturePix = rcTexture.pixel(i, j);
            iR = (qRed(cTexturePix)*qRed(cLightPix))/255;
            iG = (qGreen(cTexturePix)*qGreen(cLightPix))/255;
            iB = (qBlue(cTexturePix)*qBlue(cLightPix))/255;
            iA = (qAlpha(cTexturePix)*qAlpha(cLightPix))/255;

            iR = CLIP_255(iR);
            iG = CLIP_255(iG);
            iB = CLIP_255(iB);
            iA = CLIP_255(iA);

            rcResult.setPixel(i, j, qRgba(iR, iG, iB, iA));
        }
    }

#undef CLIP_255

    return true;
}



void RayCastOpenCLVolRenderer::render()
{
    if(this->getSubject() == NULL || this->getRenderer() == NULL)
        return;

    vtkImageData *pcProbVol = this->getSubject()->getGrayMat().getIntensityVol().getVolume().GetPointer();
    vtkImageData *pcLabelVol = this->getSubject()->getGrayMat().getLabelVol().getUnclippedVolume().GetPointer();
    if(VTKUtils::isEmptyDimension(pcProbVol->GetDimensions()) ||
            VTKUtils::isEmptyDimension(pcLabelVol->GetDimensions()))
    {
        qWarning() << "Volume Data Incomplete... Quit Raycasting...";
        return;
    }

    qDebug() << "Initilizaing Rays..";
    /// allocate opencl

    int iWidth = this->getRenderer()->GetSize()[0];
    int iHeight = this->getRenderer()->GetSize()[1];

    if(iWidth < iHeight*M_PI)
    {
        iHeight = iWidth/M_PI;
    }
    else
    {
        iWidth = iHeight*M_PI;
    }

    qDebug() << iWidth << iHeight;

    iHeight = 768;
    iWidth = 1024;

    static int iLastWidth = -1;
    static int iLastHeight = -1;

    if(iLastWidth != iWidth || iLastHeight != iHeight)
    {
        cl::ImageFormat format;
        format.image_channel_data_type = CL_FLOAT;
        format.image_channel_order = CL_RGBA;

        delete m_cOpenCLContext.renderedImgBuf;
        m_cOpenCLContext.renderedImgBuf = new cl_float4[iHeight*iWidth];
        m_cOpenCLContext.renderedImg = cl::Image2D( m_cOpenCLContext.context,
                                                    CL_MEM_WRITE_ONLY,
                                                    format, iWidth, iHeight);
        iLastWidth = iWidth;
        iLastHeight = iHeight;
        qDebug() << "New Buffer Allocated";
    }


    /// run

//    QVector3D cCamPos = Floats2Vector3D(pcProbVol->GetCenter());
//    cCamPos += QVector3D(+4,0,0);
//    m_cOpenCLContext.parameters.center.x = cCamPos.x()/pcProbVol->GetSpacing()[0];
//    m_cOpenCLContext.parameters.center.y = cCamPos.y()/pcProbVol->GetSpacing()[1];
//    m_cOpenCLContext.parameters.center.z = cCamPos.z()/pcProbVol->GetSpacing()[2];

    m_cOpenCLContext.kernel=cl::Kernel(m_cOpenCLContext.program, m_cOpenCLContext.kernal_name.toStdString().c_str());
    m_cOpenCLContext.kernel.setArg(0, m_cOpenCLContext.intensityVol);
    m_cOpenCLContext.kernel.setArg(1, m_cOpenCLContext.labelVol);
    m_cOpenCLContext.kernel.setArg(2, m_cOpenCLContext.labelColorVol);
    m_cOpenCLContext.kernel.setArg(3, m_cOpenCLContext.gradientVol);
    m_cOpenCLContext.kernel.setArg(4, m_cOpenCLContext.colorTable);
    m_cOpenCLContext.kernel.setArg(5, m_cOpenCLContext.parameters);
    m_cOpenCLContext.kernel.setArg(6, m_cOpenCLContext.renderedImg);
    m_cOpenCLContext.kernel.setArg(7, m_cOpenCLContext.occlusionMatrix);
    m_cOpenCLContext.kernel.setArg(8, m_cOpenCLContext.occMatrixSize);

    int datasize;
    std::cout << "CL_KERNEL_WORK_GROUP_SIZE" << m_cOpenCLContext.kernel.getWorkGroupInfo(m_cOpenCLContext.chosen_device,CL_KERNEL_WORK_GROUP_SIZE ,&datasize) << " " << datasize << endl;

    cl_int iResult = m_cOpenCLContext.queue.enqueueNDRangeKernel(   m_cOpenCLContext.kernel,
                                                                    cl::NullRange,
                                                                    cl::NDRange(iWidth, iHeight),
                                                                    cl::NullRange);
    xCheckOpenCLError(iResult, "Kernel Invoke Error");
    m_cOpenCLContext.queue.finish();


    cl::size_t<3> cOrigin; cOrigin[0] = cOrigin[1] = cOrigin[2] = 0;
    cl::size_t<3> cRegion; cRegion[0] = iWidth; cRegion[1] = iHeight; cRegion[2] = 1;
    iResult = m_cOpenCLContext.queue.enqueueReadImage(  m_cOpenCLContext.renderedImg,
                                                        CL_TRUE,
                                                        cOrigin,
                                                        cRegion,
                                                        0, 0, m_cOpenCLContext.renderedImgBuf);

    if( !xCheckOpenCLError(iResult, "Read Rendered Image from Device Failed!") )
        return;


    iResult = m_cOpenCLContext.queue.enqueueReadBuffer(m_cOpenCLContext.occlusionMatrix,
                                                       CL_TRUE,
                                                       0,
                                                       m_cOpenCLContext.occMatrixSize*m_cOpenCLContext.occMatrixSize*sizeof(cl_int),
                                                       m_cOpenCLContext.occlusionMatrixBuf);
    if( !xCheckOpenCLError(iResult, "Read Occlusion Matrix from Device Failed!") )
        return;

    const QSize& rcSize = m_cRenderedImage.size();
    if(rcSize.height() != iHeight || rcSize.width() != iWidth)
    {
        m_cRenderedImage = QImage(iWidth, iHeight, QImage::Format_RGBA8888);
    }

    QRgb pixel;
    for(int x = 0; x < iWidth; x++)
    {
        for(int y = 0; y < iHeight; y++)
        {
            pixel = qRgba( m_cOpenCLContext.renderedImgBuf[y*iWidth+x].x*255,
                           m_cOpenCLContext.renderedImgBuf[y*iWidth+x].y*255,
                           m_cOpenCLContext.renderedImgBuf[y*iWidth+x].z*255,
                           m_cOpenCLContext.renderedImgBuf[y*iWidth+x].w*255 );
            m_cRenderedImage.setPixel(x, y, pixel);
        }
    }
    m_cRenderedImage = ImageUtils::hammerAitoffProjection(m_cRenderedImage, Qt::black);

    Matrix<int> cOccMatrix;
    int iOccMatrixSize = m_cOpenCLContext.occMatrixSize;
    cOccMatrix.allocate(iOccMatrixSize, iOccMatrixSize);
    for(int row = 0; row < iOccMatrixSize; row++)
    {
        for(int col = 0; col < iOccMatrixSize; col++)
        {
            cOccMatrix[row][col] = m_cOpenCLContext.occlusionMatrixBuf[row*iOccMatrixSize + col];
        }
    }
//    cOccMatrix.writeToCSV("occ.csv");
//    qDebug() << "Done";

    return;

}

void RayCastOpenCLVolRenderer::renderFMRITexture(FMRITimeSeriesData *pcFMRITimeSeriesData)
{
    int iWidth = this->getRenderer()->GetSize()[0];
    int iHeight = this->getRenderer()->GetSize()[1];
    QImage cResult = QImage(iWidth, iHeight, QImage::Format_ARGB32);

    Matrix<double> cMatrix = pcFMRITimeSeriesData->getMatrix();
    double dMin = cMatrix.getMin();
    double dMax = cMatrix.getMax();

    QVector<ROI*>& apcROI = getSubject()->getGrayMat().getLabelSurface().getROIs();
    if(apcROI.size() != cMatrix.getNumRow())
    {
        qWarning() << "Number of ROI does not match the fMRI data.";
        qWarning() << "Num of ROI:" << apcROI.size() << " Num of rows in fMRI data:" << cMatrix.getNumRow();
        return;
    }

    ContourTextureGenerator cTextureGen;
    m_acROICenters.resize(apcROI.size());

    QVector<QSet<int>> cLayerLabels = this->getLayerTabel().getLayerToLabel();
    for(int iLayer = 0; iLayer < cLayerLabels.size(); iLayer++)
    {
        QSet<int>& rcLabels = cLayerLabels[iLayer];
        qDebug() << "For layer:" << iLayer << rcLabels;
        for(auto cROIIter = rcLabels.begin(); cROIIter != rcLabels.end(); cROIIter++)
        {
            int iROI = *cROIIter;
            QVector4D& rcColor = g_acColorTableForRaycastHighlight[iROI%COLOR_TABLE_SIZE];
            QColor cColor(rcColor.x()*255, rcColor.y()*255, rcColor.z()*255);
            qDebug() << "Doing ROI" << iROI;
            for(int i = 0; i < this->getSize().width(); i++)
            {
                for(int j = 0; j < this->getSize().height(); j++)
                {
                    int iLabels = m_cLabelImg[iLayer][i][j];
                    if(iLabels == iROI)
                    {
                        cResult.setPixel(i,j,cColor.rgb());
                    }
                    else
                    {
                        cResult.setPixel(i,j,qRgba(0,0,0,0));
                    }
                }
            }
//            qDebug() << "Closing Img";
//            QImage cClosedResult = cResult;
//            ImageUtils::close(cClosedResult, cResult, 7, cColor.rgb());
            cResult.save(QString("label_map_%1.png").arg(iROI));
            cResult = ImageUtils::hammerAitoffProjection(cResult,Qt::transparent);
            cResult.save(QString("label_map_hammer_%1.png").arg(iROI));

            qDebug() << "Generating";
            ROI* pcROI = this->getSubject()->getGrayMat().getLabelSurface().getROIs()[iROI-1];
            QVector<double>& adTimeSeriesData = cMatrix[iROI-1];
            cTextureGen.generateTexture(cResult, adTimeSeriesData, pcROI, dMin, dMax, &(m_acROICenters[iROI-1]));
        }
    }
    this->overlayTextures("raycast_contour_#%1.png");

}

void RayCastOpenCLVolRenderer::renderMDTexture(MultiDiseaseMeasurement *pcMDMeasurement)
{
    int iWidth = this->getRenderer()->GetSize()[0];
    int iHeight = this->getRenderer()->GetSize()[1];
    QImage cResult = QImage(iWidth, iHeight, QImage::Format_ARGB32);

    Matrix<double> cMatrix = pcMDMeasurement->getMatrix();
    double dMin = cMatrix.getMin();
    double dMax = cMatrix.getMax();

    QVector<ROI*>& apcROI = getSubject()->getGrayMat().getLabelSurface().getROIs();
    if(apcROI.size() != cMatrix.getNumRow())
    {
        qWarning() << "Number of ROI does not match the fMRI data.";
        qWarning() << "Num of ROI:" << apcROI.size() << " Num of rows in fMRI data:" << cMatrix.getNumRow();
        return;
    }

    PerlinTextureGenerator cTextureGen;

    QVector<QSet<int>> cLayerLabels = this->getLayerTabel().getLayerToLabel();
    for(int iLayer = 0; iLayer < cLayerLabels.size(); iLayer++)
    {
        QSet<int>& rcLabels = cLayerLabels[iLayer];
        qDebug() << "For layer:" << iLayer << rcLabels;
        for(auto cROIIter = rcLabels.begin(); cROIIter != rcLabels.end(); cROIIter++)
        {
            int iROI = *cROIIter;
            QVector4D& rcColor = g_acColorTableForRaycastHighlight[iROI%COLOR_TABLE_SIZE];
            QColor cColor(rcColor.x()*255, rcColor.y()*255, rcColor.z()*255);
            qDebug() << "Doing ROI" << iROI;
            for(int i = 0; i < this->getSize().width(); i++)
            {
                for(int j = 0; j < this->getSize().height(); j++)
                {
                    int iLabels = m_cLabelImg[iLayer][i][j];
                    if(iLabels == iROI)
                    {
                        cResult.setPixel(i,j,cColor.rgb());
                    }
                    else
                    {
                        cResult.setPixel(i,j,qRgba(0,0,0,0));
                    }
                }
            }
//            qDebug() << "Closing Img";
//            QImage cClosedResult = cResult;
//            ImageUtils::close(cClosedResult, cResult, 7, cColor.rgb());
            cResult.save(QString("label_map_%1.png").arg(iROI));
            cResult = ImageUtils::hammerAitoffProjection(cResult,Qt::transparent);
            cResult.save(QString("label_map_hammer_%1.png").arg(iROI));

            qDebug() << "Generating";
//            ROI* pcROI = this->getSubject()->getGrayMat().getLabelSurface().getROIs()[iROI-1];
            QVector<double>& adMDMeasurement = cMatrix[iROI-1];
            QString strTexturePath;
            QImage cPerlinTexture = cTextureGen.generatePerlineTexture(adMDMeasurement, 3, iROI, strTexturePath, iWidth, iHeight);
            QImage cPerlinROITexture = ImageUtils::maskImage(cPerlinTexture, cResult);
            cPerlinROITexture.save(QString("Perlin_%1.png").arg(iROI));
        }
    }
    this->overlayTextures("Perlin_%1.png");
}

void RayCastOpenCLVolRenderer::renderNetwork(FunctionalConnectivityData *pcConnectivity, bool bDebugMode)
{
    QVector<QSet<int>>& rcLayerToLabel = this->getLayerTabel().getLayerToLabel();
    QMap<int,int>& rcLabelToLayer = this->getLayerTabel().getLabelToLayer();
    /// load image for each layer
    QVector<QImage> acLayerImgs(rcLayerToLabel.size());
    for(int i = 0; i < acLayerImgs.size(); i++)
    {
        QImage& rcLayerImg = acLayerImgs[i];
        rcLayerImg.load(QString("%1.png").arg(i));
    }
    /// determine the centroid for each ROI
    QVector<ROI*>& apcROI = getSubject()->getGrayMat().getLabelSurface().getROIs();
    int iNumROI = apcROI.size();
    QVector<QPointF>& acCentroid = this->getROICenters();

    if(acCentroid.empty())
    {
        qWarning() << "Haven't computed ROI texture centers yet.";
        return;
    }

    /// draw image
    QPainter cPainter;

    QPen cPen;
    cPen.setColor(Qt::gray);
    cPen.setWidth(5.0);
    QBrush cBrush(Qt::SolidPattern);
    cBrush.setColor(Qt::black);

    QRadialGradient cGradient;
    cGradient.setRadius(15);
    cGradient.setColorAt(0, Qt::black);
    cGradient.setColorAt(1, Qt::black);


    QVector<QLineF> cLines;
    QVector<double> cWeights;
    QVector<QVector<QPointF>> cLayerHolesPos(rcLayerToLabel.size());

    double dThreshold = 0.85;

    for(int iStartMinus1 = 0; iStartMinus1 < iNumROI-1; iStartMinus1++)
    {
        for(int iEndMinus1 = iStartMinus1+1; iEndMinus1 < iNumROI; iEndMinus1++)
        {
            double dWeight = pcConnectivity->getMatrix()[iStartMinus1][iEndMinus1];
            if(qAbs(dWeight) < dThreshold)
                continue;
            int iStartLayer = rcLabelToLayer[iStartMinus1+1];
            int iEndLayer = rcLabelToLayer[iEndMinus1+1];
            QPointF& rcStartPos = acCentroid[iStartMinus1];
            QPointF& rcEndPos = acCentroid[iEndMinus1];

            /// skip the ROI without area (too small)
            if(rcStartPos.manhattanLength() < 0.1 || rcEndPos.manhattanLength() < 0.1)
                continue;


            for(int i = iStartLayer-1; i >= 0; i--)
            {
                cLayerHolesPos[i].push_back(rcStartPos);
            }

            for(int i = iEndLayer-1; i >= 0; i--)
            {
                cLayerHolesPos[i].push_back(rcEndPos);
            }

            cLines.push_back(QLineF(rcStartPos, rcEndPos));
            cWeights.push_back(dWeight);
        }
    }

    /// punch holes on layer in front of the start/end centroid
    for(int i = 0; i < cLayerHolesPos.size(); i++)
    {
        QImage cAlphaChannel = acLayerImgs[i].alphaChannel().convertToFormat(QImage::Format_RGB32);
        double dRadius = 5*(cLayerHolesPos.size()-i);

        cPainter.begin(&cAlphaChannel);
        cPainter.setPen(Qt::NoPen);



//        cPainter.setBrush(cBrush);

        for(QPointF& rcPos: cLayerHolesPos[i])
        {
            cGradient.setCenter(rcPos);
            cGradient.setFocalPoint(rcPos);
            cPainter.setBrush(cGradient);
            cPainter.drawEllipse(rcPos, dRadius, dRadius);
        }

        cPainter.end();
        acLayerImgs[i].setAlphaChannel(cAlphaChannel);
        acLayerImgs[i].save(QString("%1_holes.png").arg(i));
        cAlphaChannel.save(QString("%1_alpha.png").arg(i));
    }

    /// overlay layer images
    QImage cOverlay;
    double dLayerOpacity = 0.5;
    for(int i = acLayerImgs.size()-2; i >= 0 ; i--)
    {
        if(cOverlay.isNull())
        {
            cOverlay = acLayerImgs[i];
            cOverlay.fill(Qt::black);
        }
        ImageUtils::overlayImage(cOverlay,acLayerImgs[i],dLayerOpacity);
    }
    cOverlay.save("overlay.png");


    /// draw lines
    vtkSmartPointer<vtkLookupTable> pcPosLUT = vtkSmartPointer<vtkLookupTable>::New();
    pcPosLUT->SetHueRange(0.0,0.0);    ///red
    pcPosLUT->SetSaturationRange(0.35, 1.0);
    pcPosLUT->SetValueRange(1.0,1.0);
    pcPosLUT->SetTableRange(dThreshold,1.0);
    pcPosLUT->Build();

    vtkSmartPointer<vtkLookupTable> pcNegLUT = vtkSmartPointer<vtkLookupTable>::New();
    pcNegLUT->SetHueRange(0.67, 0.67);    ///blue
    pcNegLUT->SetSaturationRange(1.0,0.35);
    pcNegLUT->SetValueRange(1.0,1.0);
    pcNegLUT->SetTableRange(-1, -dThreshold);
    pcNegLUT->Build();

    QPen cPenNetwork;
    double adLineColor[3];
    cPenNetwork.setWidth(4.0);

    for(int i = 0; i< cLines.size(); i++)
    {
        QLineF& rcLine = cLines[i];
        double dWeight = cWeights[i];
        if(dWeight > 0)
            pcPosLUT->GetColor(dWeight, adLineColor);
        else
            pcNegLUT->GetColor(dWeight, adLineColor);
        cPenNetwork.setColor(QColor::fromRgbF(adLineColor[0],adLineColor[1],adLineColor[2],0.6));

        cPainter.begin(&cOverlay);
        cPainter.setRenderHint(QPainter::Antialiasing);
        cPainter.setPen(cPenNetwork);

        cPainter.drawLine(rcLine);
        cPainter.end();

    }
    cOverlay.save("overlay_network.png");

}

void RayCastOpenCLVolRenderer::renderNetworkTile(FunctionalConnectivityData *pcConnectivity, bool bDebugMode)
{
    QVector<QSet<int>>& rcLayerToLabel = this->getLayerTabel().getLayerToLabel();
    QMap<int,int>& rcLabelToLayer = this->getLayerTabel().getLabelToLayer();

    /// load image for each layer
    QVector<QImage> acLayerImgs(rcLayerToLabel.size());
    for(int i = 0; i < acLayerImgs.size(); i++)
    {
        QImage& rcLayerImg = acLayerImgs[i];
        rcLayerImg.load(QString("%1.png").arg(i));
    }

    int iWidth = acLayerImgs[0].width();
    int iHeight = acLayerImgs[0].height();

    /// determine the centroid for each ROI
    QVector<ROI*>& apcROI = getSubject()->getGrayMat().getLabelSurface().getROIs();
    int iNumROI = apcROI.size();
    QVector<QPointF>& acCentroid = this->getROICenters();

    if(acCentroid.empty())
    {
        qWarning() << "Haven't computed ROI texture centers yet.";
        return;
    }

    /// offset and scale for each layer
    double scale[] =  {1, 0.75,  0.5,   0.25};
    double transx[] = {0, 1.0/8, 2.0/8, 3.0/8};
    double transy[] = {0, 1.0,   1.75,  2.25};


    /// draw image
    QPainter cPainter;

    QPen cPen;
    cPen.setColor(Qt::gray);
    cPen.setWidth(5.0);
    QBrush cBrush(Qt::SolidPattern);
    cBrush.setColor(Qt::black);

    QRadialGradient cGradient;
    cGradient.setRadius(15);
    cGradient.setColorAt(0, Qt::black);
    cGradient.setColorAt(1, Qt::black);


    QVector<QLineF> cLines;
    QVector<double> cWeights;

    double dThreshold = 0.85;

    for(int iStartMinus1 = 0; iStartMinus1 < iNumROI-1; iStartMinus1++)
    {
        for(int iEndMinus1 = iStartMinus1+1; iEndMinus1 < iNumROI; iEndMinus1++)
        {
            double dWeight = pcConnectivity->getMatrix()[iStartMinus1][iEndMinus1];
            if(qAbs(dWeight) < dThreshold)
                continue;
            int iStartLayer = rcLabelToLayer[iStartMinus1+1];
            int iEndLayer = rcLabelToLayer[iEndMinus1+1];
            QPointF& rcStartPos = acCentroid[iStartMinus1];
            QPointF& rcEndPos = acCentroid[iEndMinus1];

            /// skip the ROI without area (too small)
            if(rcStartPos.manhattanLength() < 0.1 || rcEndPos.manhattanLength() < 0.1)
                continue;

            cLines.push_back(QLineF(rcStartPos*scale[iStartLayer] + QPointF(transx[iStartLayer]*iWidth, transy[iStartLayer]*iHeight),
                                    rcEndPos*scale[iEndLayer] + QPointF(transx[iEndLayer]*iWidth, transy[iEndLayer]*iHeight)) );
            cWeights.push_back(dWeight);
        }
    }


    /// overlay layer images
    QImage cOverlay;

    for(int i = 0; i < acLayerImgs.size() ; i++)
    {
        if(cOverlay.isNull())
        {
            cOverlay = QImage(acLayerImgs[i].width(), (acLayerImgs[i].height()*5)/2, QImage::Format_RGBA8888);
            cOverlay.fill(Qt::black);
        }
        QPainter painter(&cOverlay);
        painter.drawImage(transx[i]*acLayerImgs[i].width(),
                          transy[i]*acLayerImgs[i].height(),
                          acLayerImgs[i].scaled(scale[i]*acLayerImgs[i].size()) );
        painter.end();
    }
    cOverlay.save("overlay_scaled.png");


    /// draw lines
    vtkSmartPointer<vtkLookupTable> pcPosLUT = vtkSmartPointer<vtkLookupTable>::New();
    pcPosLUT->SetHueRange(0.0,0.0);    ///red
    pcPosLUT->SetSaturationRange(0.35, 1.0);
    pcPosLUT->SetValueRange(1.0,1.0);
    pcPosLUT->SetTableRange(dThreshold,1.0);
    pcPosLUT->Build();

    vtkSmartPointer<vtkLookupTable> pcNegLUT = vtkSmartPointer<vtkLookupTable>::New();
    pcNegLUT->SetHueRange(0.67, 0.67);    ///blue
    pcNegLUT->SetSaturationRange(1.0,0.35);
    pcNegLUT->SetValueRange(1.0,1.0);
    pcNegLUT->SetTableRange(-1, -dThreshold);
    pcNegLUT->Build();

    QPen cPenNetwork;
    double adLineColor[3];
    cPenNetwork.setWidth(4.0);

    for(int i = 0; i< cLines.size(); i++)
    {
        QLineF& rcLine = cLines[i];
        double dWeight = cWeights[i];
        if(dWeight > 0)
            pcPosLUT->GetColor(dWeight, adLineColor);
        else
            pcNegLUT->GetColor(dWeight, adLineColor);
        cPenNetwork.setColor(QColor::fromRgbF(adLineColor[0],adLineColor[1],adLineColor[2],0.6));

        cPainter.begin(&cOverlay);
        cPainter.setRenderHint(QPainter::Antialiasing);
        cPainter.setPen(cPenNetwork);

        cPainter.drawLine(rcLine);
        cPainter.end();

    }
    cOverlay.save("overlay_network_scaled.png");
}


void RayCastOpenCLVolRenderer::renderNetwork(MultiDiseaseConnectivity *pcConnectivity, bool bDebugMode)
{
    QVector<QSet<int>>& rcLayerToLabel = this->getLayerTabel().getLayerToLabel();
    QMap<int,int>& rcLabelToLayer = this->getLayerTabel().getLabelToLayer();
    /// load image for each layer
    QVector<QImage> acLayerImgs(rcLayerToLabel.size());
    for(int i = 0; i < acLayerImgs.size(); i++)
    {
        QImage& rcLayerImg = acLayerImgs[i];
        rcLayerImg.load(QString("%1.png").arg(i));
    }
    /// determine the centroid for each ROI
    QVector<ROI*>& apcROI = getSubject()->getGrayMat().getLabelSurface().getROIs();
    int iNumROI = apcROI.size();
    QVector<QPointF>& acCentroid = this->getROICenters();

    if(acCentroid.empty())
    {
        qWarning() << "Haven't computed ROI texture centers yet.";
        return;
    }

    /// draw image
    QPainter cPainter;

    QPen cPen;
    cPen.setColor(Qt::gray);
    cPen.setWidth(5.0);
    QBrush cBrush(Qt::SolidPattern);
    cBrush.setColor(Qt::black);

    QRadialGradient cGradient;
    cGradient.setRadius(15);
    cGradient.setColorAt(0, Qt::black);
    cGradient.setColorAt(1, Qt::black);


    QVector<QLineF> cLines;
    QVector<QVector<double>> cWeights;
    QVector<QVector<QPointF>> cLayerHolesPos(rcLayerToLabel.size());

    double dThreshold = 0.85;

    for(int iStartMinus1 = 0; iStartMinus1 < iNumROI-1; iStartMinus1++)
    {
        for(int iEndMinus1 = iStartMinus1+1; iEndMinus1 < iNumROI; iEndMinus1++)
        {
            QVector<double> adElement = pcConnectivity->getMatrix()[iStartMinus1][iEndMinus1];
            if(!xBiasCheck(adElement, 0.85))
            {
                //qDebug() << "#" << i << j << "Skipped" << dCorrelation;
                continue;
            }
            int iStartLayer = rcLabelToLayer[iStartMinus1+1];
            int iEndLayer = rcLabelToLayer[iEndMinus1+1];
            QPointF& rcStartPos = acCentroid[iStartMinus1];
            QPointF& rcEndPos = acCentroid[iEndMinus1];

            /// skip the ROI without area (too small)
            if(rcStartPos.manhattanLength() < 0.1 || rcEndPos.manhattanLength() < 0.1)
                continue;


            for(int i = iStartLayer-1; i >= 0; i--)
            {
                cLayerHolesPos[i].push_back(rcStartPos);
            }

            for(int i = iEndLayer-1; i >= 0; i--)
            {
                cLayerHolesPos[i].push_back(rcEndPos);
            }

            cLines.push_back(QLineF(rcStartPos, rcEndPos));
            cWeights.push_back(adElement);
        }
    }

    /// punch holes on layer in front of the start/end centroid
    for(int i = 0; i < cLayerHolesPos.size(); i++)
    {
        QImage cAlphaChannel = acLayerImgs[i].alphaChannel().convertToFormat(QImage::Format_RGB32);
        double dRadius = 5*(cLayerHolesPos.size()-i);

        cPainter.begin(&cAlphaChannel);
        cPainter.setPen(Qt::NoPen);



//        cPainter.setBrush(cBrush);

        for(QPointF& rcPos: cLayerHolesPos[i])
        {
            cGradient.setCenter(rcPos);
            cGradient.setFocalPoint(rcPos);
            cPainter.setBrush(cGradient);
            cPainter.drawEllipse(rcPos, dRadius, dRadius);
        }

        cPainter.end();
        acLayerImgs[i].setAlphaChannel(cAlphaChannel);
        acLayerImgs[i].save(QString("%1_holes.png").arg(i));
        cAlphaChannel.save(QString("%1_alpha.png").arg(i));
    }

    /// overlay layer images
    QImage cOverlay;
    double dLayerOpacity = 0.5;
    for(int i = acLayerImgs.size()-2; i >= 0 ; i--)
    {
        if(cOverlay.isNull())
        {
            cOverlay = acLayerImgs[i];
            cOverlay.fill(Qt::black);
        }
        ImageUtils::overlayImage(cOverlay,acLayerImgs[i],dLayerOpacity);
    }
    cOverlay.save("overlay.png");


    QPen cPenNetwork;
    static unsigned char auhCurveColor[3];

    cPenNetwork.setWidth(4.0);

    for(int i = 0; i< cLines.size(); i++)
    {
        QLineF& rcLine = cLines[i];
        QVector<double>& adElement = cWeights[i];

        qDebug() << auhCurveColor[0] << auhCurveColor[1] << auhCurveColor[2];
        xGetConnectionColor(adElement, auhCurveColor);
        cPenNetwork.setColor(QColor::fromRgb(auhCurveColor[0],auhCurveColor[1],auhCurveColor[2],0.6*255));

        cPainter.begin(&cOverlay);
        cPainter.setRenderHint(QPainter::Antialiasing);
        cPainter.setPen(cPenNetwork);

        cPainter.drawLine(rcLine);
        cPainter.end();

    }
    cOverlay.save("overlay_network.png");

}

void RayCastOpenCLVolRenderer::renderNetworkTile(MultiDiseaseConnectivity *pcConnectivity, bool bDebugMode)
{
    QVector<QSet<int>>& rcLayerToLabel = this->getLayerTabel().getLayerToLabel();
    QMap<int,int>& rcLabelToLayer = this->getLayerTabel().getLabelToLayer();

    /// load image for each layer
    QVector<QImage> acLayerImgs(rcLayerToLabel.size());
    for(int i = 0; i < acLayerImgs.size(); i++)
    {
        QImage& rcLayerImg = acLayerImgs[i];
        rcLayerImg.load(QString("%1.png").arg(i));
    }

    int iWidth = acLayerImgs[0].width();
    int iHeight = acLayerImgs[0].height();

    /// determine the centroid for each ROI
    QVector<ROI*>& apcROI = getSubject()->getGrayMat().getLabelSurface().getROIs();
    int iNumROI = apcROI.size();
    QVector<QPointF>& acCentroid = this->getROICenters();

    if(acCentroid.empty())
    {
        qWarning() << "Haven't computed ROI texture centers yet.";
        return;
    }

    /// offset and scale for each layer
    double scale[] =  {1, 0.75,  0.5,   0.25};
    double transx[] = {0, 1.0/8, 2.0/8, 3.0/8};
    double transy[] = {0, 1.0,   1.75,  2.25};


    /// draw image
    QPainter cPainter;

    QPen cPen;
    cPen.setColor(Qt::gray);
    cPen.setWidth(5.0);
    QBrush cBrush(Qt::SolidPattern);
    cBrush.setColor(Qt::black);

    QRadialGradient cGradient;
    cGradient.setRadius(15);
    cGradient.setColorAt(0, Qt::black);
    cGradient.setColorAt(1, Qt::black);


    QVector<QLineF> cLines;
    QVector<QVector<double>> cWeights;

    double dThreshold = 0.85;

    for(int iStartMinus1 = 0; iStartMinus1 < iNumROI-1; iStartMinus1++)
    {
        for(int iEndMinus1 = iStartMinus1+1; iEndMinus1 < iNumROI; iEndMinus1++)
        {
            QVector<double> adElement = pcConnectivity->getMatrix()[iStartMinus1][iEndMinus1];
            if(!xBiasCheck(adElement, 0.85))
            {
                //qDebug() << "#" << i << j << "Skipped" << dCorrelation;
                continue;
            }
            int iStartLayer = rcLabelToLayer[iStartMinus1+1];
            int iEndLayer = rcLabelToLayer[iEndMinus1+1];
            QPointF& rcStartPos = acCentroid[iStartMinus1];
            QPointF& rcEndPos = acCentroid[iEndMinus1];

            /// skip the ROI without area (too small)
            if(rcStartPos.manhattanLength() < 0.1 || rcEndPos.manhattanLength() < 0.1)
                continue;

            cLines.push_back(QLineF(rcStartPos*scale[iStartLayer] + QPointF(transx[iStartLayer]*iWidth, transy[iStartLayer]*iHeight),
                                    rcEndPos*scale[iEndLayer] + QPointF(transx[iEndLayer]*iWidth, transy[iEndLayer]*iHeight)) );
            cWeights.push_back(adElement);
        }
    }


    /// overlay layer images
    QImage cOverlay;

    for(int i = 0; i < acLayerImgs.size() ; i++)
    {
        if(cOverlay.isNull())
        {
            cOverlay = QImage(acLayerImgs[i].width(), (acLayerImgs[i].height()*5)/2, QImage::Format_RGBA8888);
            cOverlay.fill(Qt::black);
        }
        QPainter painter(&cOverlay);
        painter.drawImage(transx[i]*acLayerImgs[i].width(),
                          transy[i]*acLayerImgs[i].height(),
                          acLayerImgs[i].scaled(scale[i]*acLayerImgs[i].size()) );
        painter.end();
    }
    cOverlay.save("overlay_scaled.png");


    /// draw lines
    vtkSmartPointer<vtkLookupTable> pcPosLUT = vtkSmartPointer<vtkLookupTable>::New();
    pcPosLUT->SetHueRange(0.0,0.0);    ///red
    pcPosLUT->SetSaturationRange(0.35, 1.0);
    pcPosLUT->SetValueRange(1.0,1.0);
    pcPosLUT->SetTableRange(dThreshold,1.0);
    pcPosLUT->Build();

    vtkSmartPointer<vtkLookupTable> pcNegLUT = vtkSmartPointer<vtkLookupTable>::New();
    pcNegLUT->SetHueRange(0.67, 0.67);    ///blue
    pcNegLUT->SetSaturationRange(1.0,0.35);
    pcNegLUT->SetValueRange(1.0,1.0);
    pcNegLUT->SetTableRange(-1, -dThreshold);
    pcNegLUT->Build();

    QPen cPenNetwork;
    unsigned char auhCurveColor[3];
    cPenNetwork.setWidth(4.0);

    for(int i = 0; i< cLines.size(); i++)
    {
        QLineF& rcLine = cLines[i];
        QVector<double>& adElement = cWeights[i];

        xGetConnectionColor(adElement, auhCurveColor);
        cPenNetwork.setColor(QColor::fromRgb(auhCurveColor[0],auhCurveColor[1],auhCurveColor[2],0.6*255));

        cPainter.begin(&cOverlay);
        cPainter.setRenderHint(QPainter::Antialiasing);
        cPainter.setPen(cPenNetwork);

        cPainter.drawLine(rcLine);
        cPainter.end();

    }
    cOverlay.save("overlay_network_scaled.png");
}

bool RayCastOpenCLVolRenderer::xBiasCheck(QVector<double>& adValues, double dThreshold)
{
    static const QVector3D cZero(1.0/3, 1.0/3, 1.0/3);
    static const double dNormalizeFactor = 0.8165;  /// sqrt(6)/3

    if(adValues.size() != 3)
    {
        qWarning() << "Can only handle 3 disease for now";
        return false;
    }
    QVector3D cValue( adValues[0], adValues[1], adValues[2] );
    if(cValue.length() < 0.01)
        return false;
    double dSum = cValue[0] + cValue[1] + cValue[2];
    cValue /= dSum;

    if( cValue.distanceToPoint(cZero)/dNormalizeFactor < dThreshold )
    {
        return false;
    }
    return true;
}


void RayCastOpenCLVolRenderer::xGetConnectionColor(QVector<double>& adValues, unsigned char *auhColor)
{
    if(adValues.size() != 3)
    {
        auhColor[0] = 255;
        auhColor[1] = 255;
        auhColor[2] = 255;
        auhColor[3] = 255;
        qWarning() << "Can only handle 3 disease for now";
        return;
    }

    double dSum = adValues[0] + adValues[1] + adValues[2];

    if(dSum < 0.01)
    {
        auhColor[0] = 255;
        auhColor[1] = 255;
        auhColor[2] = 255;
        auhColor[3] = 255;
    }
    else
    {


        QColor cRgb = ColorConverter::ryb2rgb(adValues[0]/dSum, adValues[1]/dSum, adValues[2]/dSum);

        auhColor[0] = cRgb.red();
        auhColor[1] = cRgb.green();
        auhColor[2] = cRgb.blue();
        auhColor[3] = 255;
    }
}


void RayCastOpenCLVolRenderer::composeRays()
{
    /// init label map
    int iNumLayer = this->getLayerTabel().getLayerToLabel().size();
    m_cLabelImg.resize(iNumLayer);
    for(int iLayer = 0; iLayer < iNumLayer; iLayer++)
    {
        m_cLabelImg[iLayer].allocate(this->getSize().width(), this->getSize().height());
        for(int iWidth = 0; iWidth < this->getSize().width(); iWidth++)
        {
            for(int iHeight = 0; iHeight < this->getSize().height(); iHeight++)
            {
                 m_cLabelImg[iLayer][iWidth][iHeight] = 0;
            }
        }
    }


    for(int i = 0; i < this->getRays().size(); i++)
    {
        QVector4D rcLabelColor(0,0,0,0);
        QVector4D rcShadeColor(0,0,0,0);
        Ray& rcRay = this->getRays()[i];
        QVector<RaySamplePoint>& racSampledPnts = this->getRays()[i].getSamplePoints();

        ///
        QVector<int> iFirstLayerLabels(iNumLayer, 0);


        /// for each sampled point
        double adAccLabelGain = 1.0;
        double adAccShadeGain = 1.0;
        for(int j = 0; j < racSampledPnts.size(); j++)
        {
            RaySamplePoint& rcPoint = racSampledPnts[j];
            int iLabel = rcPoint.getLabel();

            /// blending sample point label colors
            rcLabelColor += adAccLabelGain*rcPoint.getColor();
            adAccLabelGain *= (1-rcPoint.getColor().w());

            /// blending sample point shade colors
            rcShadeColor += adAccShadeGain*rcPoint.getShade();
            adAccShadeGain *= (1-rcPoint.getShade().w());

//            if(rcColor.w() > 1-EPSILON*0.1)
//                break;

            /// set label map
            if( iLabel != 0 && rcPoint.getShade().w() > 0.15 )
            {
                for(int iLayer = 0; iLayer < iNumLayer; iLayer++)
                {
                    if(iFirstLayerLabels[iLayer] == 0 && this->getLayerTabel().getLayerToLabel()[iLayer].contains(iLabel))
                    {
                        iFirstLayerLabels[iLayer] = iLabel;
                        this->getLabelImg()[iLayer][rcRay.getViewportPos().x()][rcRay.getViewportPos().y()] = iLabel;
                    }
                }
            }

        }

        /// add background color
        rcLabelColor += (1-rcLabelColor.w())*QVector4D(m_cBackgroundColor.redF(),
                                             m_cBackgroundColor.greenF(),
                                             m_cBackgroundColor.blueF(),
                                             m_cBackgroundColor.alphaF());

        rcShadeColor += (1-rcShadeColor.w())*QVector4D(m_cBackgroundColor.redF(),
                                             m_cBackgroundColor.greenF(),
                                             m_cBackgroundColor.blueF(),
                                             m_cBackgroundColor.alphaF());

        /// set result
        QColor& rRayColor = this->getRays()[i].getColor();
        rRayColor.setRedF(rcLabelColor.x());
        rRayColor.setGreenF(rcLabelColor.y());
        rRayColor.setBlueF(rcLabelColor.z());
        rRayColor.setAlphaF(rcLabelColor.w());

        QColor& rcShade = this->getRays()[i].getShade();
        rcShade.setRedF(rcShadeColor.x());
        rcShade.setGreenF(rcShadeColor.y());
        rcShade.setBlueF(rcShadeColor.z());
        rcShade.setAlphaF(rcShadeColor.w());

    }


//    for(int iLayer = 0; iLayer < iNumLayer; iLayer++)
//    {
//        qDebug() << iLayer;
//        m_cLabelImg[iLayer].writeToCSV(QString("%1.csv").arg(iLayer));
//    }
}

void RayCastOpenCLVolRenderer::intersectSpheres(QVector3D &rcCenter, double dRadius, Ray& rcRay)
{
    double dDistance = rcCenter.distanceToLine(rcRay.getPoint(), rcRay.getDir());
    if(dRadius - dDistance < EPSILON)
    {
        rcRay.setHit(false);
    }
    else
    {
        rcRay.setHit(true);
        double dDelta = qSqrt(dRadius*dRadius-dDistance*dDistance);
        double dTMiddle = QVector3D::dotProduct( rcCenter-rcRay.getPoint(), rcRay.getDir() )/rcRay.getDir().length();
        rcRay.setTStart((dTMiddle-dDelta)/rcRay.getDir().length());
        rcRay.setTEnd((dTMiddle+dDelta)/rcRay.getDir().length());

        /// if the ray start point is inside the sphere
        if(rcRay.getTStart() < 0)
            rcRay.setTStart(0);
    }
}

void RayCastOpenCLVolRenderer::intersectBox(double *adDim, Ray &rcRay)
{
    double tNear = -std::numeric_limits<double>::max();
    double tFar = std::numeric_limits<double>::max();
    const double epslon = 10e-6;
    QVector3D& rcPoint = rcRay.getPoint();
    QVector3D& rcDir = rcRay.getDir();
    for(size_t dim = 0; dim < 3; ++dim)
    {
        double t0, t1;
        if(std::abs(rcDir[dim]) < epslon)
        {
            /// parallel
            if(rcPoint[dim] < adDim[2*dim] || rcPoint[dim] > adDim[2*dim+1])
            {
                rcRay.setHit(false);
                return;
            }
        }

        t0 = (adDim[2*dim]   - rcPoint[dim])/rcDir[dim];
        t1 = (adDim[2*dim+1] - rcPoint[dim])/rcDir[dim];
        if(t0 > t1)
            std::swap(t0, t1);
        tNear = std::max(tNear, t0);
        tFar  = std::min(tFar, t1);
        if(tNear > tFar)
        {
            rcRay.setHit(false);
            return;
        }
        if(tFar < 0.0)
        {
            rcRay.setHit(false);
            return;
        }
    }
    rcRay.setTStart((tNear<0)?0:tNear);
    rcRay.setTEnd(tFar);
    rcRay.setHit(true);
    return;
}

void RayCastOpenCLVolRenderer::computeBound(vtkImageData *pcVolume)
{
    double* adSpaces = pcVolume->GetSpacing();
    int* iExtends = pcVolume->GetExtent();

    m_iXMin = iExtends[0]*adSpaces[0]; m_iXMax = iExtends[1]*adSpaces[0]; m_dXSpacing = adSpaces[0];
    m_iXCordMin = iExtends[0]; m_iXCordMax = iExtends[1];

    m_iYMin = iExtends[2]*adSpaces[1]; m_iYMax = iExtends[3]*adSpaces[1]; m_dYSpacing = adSpaces[1];
    m_iYCordMin = iExtends[2]; m_iYCordMax = iExtends[3];

    m_iZMin = iExtends[4]*adSpaces[2]; m_iZMax = iExtends[5]*adSpaces[2]; m_dZSpacing = adSpaces[2];
    m_iZCordMin = iExtends[4]; m_iZCordMax = iExtends[5];
}

bool RayCastOpenCLVolRenderer::saveRenderedImage(QString strImgPath)
{
    /// save rendered image
    int iWidth = this->getRenderer()->GetSize()[0];
    int iHeight = this->getRenderer()->GetSize()[1];
    QImage cResult = QImage(iWidth, iHeight, QImage::Format_ARGB32);
    QImage cLightMap = QImage(iWidth, iHeight, QImage::Format_ARGB32);
    for(int i = 0; i < this->getRays().size(); i++)
    {

        Ray& rcRay = this->getRays()[i];
        cResult.setPixel(rcRay.getViewportPos(),
                         rcRay.getColor().rgb());
        cLightMap.setPixel(rcRay.getViewportPos(),
                         rcRay.getShade().rgb());
    }
    ImageUtils::hammerAitoffProjection(cResult, Qt::transparent).save("hammer.png");
    cResult.save(strImgPath);
    ImageUtils::hammerAitoffProjection(cLightMap, Qt::transparent).save("hammer_lightmap.png");
    cLightMap.save("rendered_lightmap.png");


    /// save label map
    QImage cLabelMap = QImage(iWidth, iHeight, QImage::Format_ARGB32);
    int iNumLayer = this->getLayerTabel().getLayerToLabel().size();
    int iLabel;
    QVector4D rcColor;
    for(int iLayer = 0; iLayer < iNumLayer; iLayer++)
    {
        Matrix<int>& cMap = this->getLabelImg()[iLayer];
        for(int iW = 0; iW < iWidth; iW++ )
        {
            for(int iH = 0; iH < iHeight; iH++ )
            {
                iLabel = cMap[iW][iH];
                rcColor = g_acColorTableForRaycast[iLabel%COLOR_TABLE_SIZE]*255;
                cLabelMap.setPixel(iW, iH, qRgba(rcColor[0], rcColor[1], rcColor[2], rcColor[3]));
            }
        }
        cLabelMap.save(QString("label_map_layer_%1.png").arg(iLayer));
    }


    return true;
}


void RayCastOpenCLVolRenderer::xFillSamplePointInfo(RaySamplePoint &rcSamplePnt, QVector3D &rcLocation, vtkImageData *pcLabelImg, vtkImageData *pcIntensityImg, vtkImageData *pcGradientImg)
{
    static double c000, c001, c010, c011, c100, c101, c110, c111;   ///intensities

    static int    L000, L001, L010, L011, L100, L101, L110, L111;   ///labels

    static QVector3D g000, g001, g010, g011, g100, g101, g110, g111; ///gradient
    static int ijk[3];
    static double adGradient[3];
    vtkDataArray* adGradientArray = pcGradientImg->GetPointData()->GetScalars();

    double world_x = rcLocation.x();
    double world_y = rcLocation.y();
    double world_z = rcLocation.z();

    rcSamplePnt.getComponents().clear();
    static InterpComponent cComp;

    if( (world_x - m_iXMin > EPSILON) &&
        (m_iXMax - world_x > EPSILON) &&
        (world_y - m_iYMin > EPSILON) &&
        (m_iYMax - world_y > EPSILON) &&
        (world_z - m_iZMin > EPSILON) &&
        (m_iZMax - world_z > EPSILON) )
    {

        double x = (world_x - m_iXMin)/m_dXSpacing;
        double y = (world_y - m_iYMin)/m_dYSpacing;
        double z = (world_z - m_iZMin)/m_dZSpacing;

        int x0 = floor(x); int x1 = min(x0+1, m_iXCordMax);
        int y0 = floor(y); int y1 = min(y0+1, m_iYCordMax);
        int z0 = floor(z); int z1 = min(z0+1, m_iZCordMax);

        double x_d = x - x0;
        double y_d = y - y0;
        double z_d = z - z0;

        double dMaxWeight = 0.0;
        int iMaxLabel = 0;

#define SET_COMPONENT(subX, subY, subZ) \
    c##subX##subY##subZ = pcIntensityImg->GetScalarComponentAsDouble(x##subX, y##subY, z##subZ, 0); \
    cComp.setIntensity(c##subX##subY##subZ); \
    L##subX##subY##subZ = qRound(pcLabelImg->GetScalarComponentAsDouble(x##subX, y##subY, z##subZ, 0)); \
    cComp.setLabel(L##subX##subY##subZ); \
    ijk[0]=x##subX; ijk[1]=y##subY; ijk[2]=z##subZ; \
    adGradientArray->GetTuple(pcGradientImg->ComputePointId(ijk), adGradient); \
    cComp.setGradient(QVector3D(adGradient[0], adGradient[1], adGradient[2]));

        SET_COMPONENT(0,0,0)
        cComp.setWeight((1-x_d)*(1-y_d)*(1-z_d));
        rcSamplePnt.getComponents().insert(cComp);
        if(L000 != 0 && cComp.getWeight() > dMaxWeight)
        {
            dMaxWeight = cComp.getWeight();
            iMaxLabel = L000;
        }

        SET_COMPONENT(0,1,0)
        cComp.setWeight((1-x_d)*(y_d)*(1-z_d));
        rcSamplePnt.getComponents().insert(cComp);
        if(L010 != 0 && cComp.getWeight() > dMaxWeight)
        {
            dMaxWeight = cComp.getWeight();
            iMaxLabel = L010;
        }


        SET_COMPONENT(0,0,1)
        cComp.setWeight((1-x_d)*(1-y_d)*(z_d));
        rcSamplePnt.getComponents().insert(cComp);
        if(L001 != 0 && cComp.getWeight() > dMaxWeight)
        {
            dMaxWeight = cComp.getWeight();
            iMaxLabel = L001;
        }


        SET_COMPONENT(0,1,1)
        cComp.setWeight((1-x_d)*(y_d)*(z_d));
        rcSamplePnt.getComponents().insert(cComp);
        if(L011 != 0 && cComp.getWeight() > dMaxWeight)
        {
            dMaxWeight = cComp.getWeight();
            iMaxLabel = L011;
        }


        SET_COMPONENT(1,0,0)
        cComp.setWeight((x_d)*(1-y_d)*(1-z_d));
        rcSamplePnt.getComponents().insert(cComp);
        if(L100 != 0 && cComp.getWeight() > dMaxWeight)
        {
            dMaxWeight = cComp.getWeight();
            iMaxLabel = L100;
        }


        SET_COMPONENT(1,1,0)
        cComp.setWeight((x_d)*(y_d)*(1-z_d));
        rcSamplePnt.getComponents().insert(cComp);
        if(L110 != 0 && cComp.getWeight() > dMaxWeight)
        {
            dMaxWeight = cComp.getWeight();
            iMaxLabel = L110;
        }


        SET_COMPONENT(1,0,1)
        cComp.setWeight((x_d)*(1-y_d)*(z_d));
        rcSamplePnt.getComponents().insert(cComp);
        if(L101 != 0 && cComp.getWeight() > dMaxWeight)
        {
            dMaxWeight = cComp.getWeight();
            iMaxLabel = L101;
        }


        SET_COMPONENT(1,1,1)
        cComp.setWeight((x_d)*(y_d)*(z_d));
        rcSamplePnt.getComponents().insert(cComp);
        if(L111 != 0 && cComp.getWeight() > dMaxWeight)
        {
            dMaxWeight = cComp.getWeight();
            iMaxLabel = L111;
        }
#undef SET_COMPONENT

        rcSamplePnt.setLabel(iMaxLabel);
        rcSamplePnt.setNumLabel(rcSamplePnt.getLabelSet().size());

    }
}

void RayCastOpenCLVolRenderer::xRayMarch(Ray *pcRay)
{
    pcRay->getSamplePoints().clear();
    if(pcRay->getHit() == false)
    {
        pcRay->setColor(this->getBGColor());
        pcRay->setShade(this->getBGColor());
        return;
    }

    double dTCur = pcRay->getTEnd();
    double dTEnd = pcRay->getTStart();
#if RAYCASTDEBUG
    qDebug() << dTCur << dTEnd;
#endif
    vtkImageData *pcProbVol = this->getSubject()->getGrayMat().getIntensityVol().getVolume().GetPointer();
    vtkImageData *pcLabelVol = this->getSubject()->getGrayMat().getLabelVol().getUnclippedVolume().GetPointer();
    vtkImageData *pcGradient = this->getSubject()->getGrayMat().getIntensityVol().getGradient().GetPointer();
//    pcProbVol->Print(cout);
//    double data = *((float*)(pcProbVol->GetScalarPointer(62,48,40)));
//    qDebug() << data;

//    return;

    QVector3D cCurPos;
    QColor& rcColor = pcRay->getColor();
    int iLastLabel = -1;
    int iLabel = 0;
    QVector<RaySamplePoint>& acSamplePoint = pcRay->getSamplePoints();
    RaySamplePoint cTempPnt; QVector4D cTempLabelColor; QVector4D cTempLightColor;
    double dTempIntensity; QVector3D cTempGradient; double dAlpha;
    /// set the light direction to be the ray direction
    static QVector3D cLightDir;
    static QVector3D cHalfVec;
    cLightDir = -pcRay->getDir();
    while(dTCur > dTEnd + m_dDeadZone )
    {

        cCurPos = pcRay->getPoint() + dTCur*pcRay->getDir();

        xFillSamplePointInfo(cTempPnt, cCurPos, pcLabelVol, pcProbVol, pcGradient);
        cTempPnt.getInterpColor(cTempLabelColor);
        cTempPnt.getInterpLightColor(cTempLightColor);
        dTempIntensity = cTempPnt.getInterpIntensity();
        cTempPnt.getInterpGradient(cTempGradient);
        iLabel = cTempPnt.getLabel();
        double dGradientLen = cTempGradient.length();
        double dLambert = 0; double dPhong = 0;
        if(dGradientLen > EPSILON*0.01)
        {
            double dDotProd = QVector3D::dotProduct(cTempGradient,cLightDir);
            double dRayLen = cLightDir.length();
            dLambert = qMax(0.0, dDotProd/(dRayLen*dGradientLen));

            cHalfVec = cLightDir.normalized()+(-pcRay->getDir());
            double dHalfLen = cHalfVec.length();
            dDotProd = QVector3D::dotProduct(cTempGradient,cHalfVec);
            dPhong = qMax(0.0, dDotProd/(dHalfLen*dGradientLen));
        }
//        if(dTempIntensity > EPSILON)
//            dTempIntensity = 1;
        /// TODO debug (override color)
//        cTempColor = QVector4D(1,1,1,1);

        /// light map color
        dAlpha = cTempLightColor.w();
        cTempLightColor *= dAlpha;
        cTempLightColor = cTempLightColor*m_dRhoAmb + cTempLightColor*m_dRhoDif*dLambert
                        + cTempLightColor*m_dRhoSpe*qPow(dPhong, m_dF);
        ImageUtils::clipVector4D(cTempLightColor, dAlpha);
        cTempLightColor.setW(dAlpha);
        cTempLightColor = cTempLightColor*(dTempIntensity*m_dIntensityFactor);


        /// mutiple lambert except the alpha channel
        dAlpha = cTempLabelColor.w();
        cTempLabelColor *= dAlpha;
        cTempLabelColor = cTempLabelColor*m_dRhoAmb + cTempLabelColor*m_dRhoDif*dLambert
                        + cTempLabelColor*m_dRhoSpe*qPow(dPhong, m_dF);
        ImageUtils::clipVector4D(cTempLabelColor, dAlpha);
        cTempLabelColor.setW(dAlpha);
        cTempLabelColor = cTempLabelColor*(dTempIntensity*m_dIntensityFactor);

//        QSet<int>& cLabels = cTempPnt.getLabelSet();
//        if(cLabels.size() > 1)
//            iLabel = 0;
//        else
//            iLabel = *(cLabels.begin());// TODO
        dTCur -= m_dTStep;


        /// add a sample point
        if(cTempLabelColor.length() > EPSILON*0.0001 || cTempLightColor.length() > EPSILON*0.0001)
        {
            if(true /*iLastLabel != iLabel || acSamplePoint.empty()*/)
            {
//                cTempPnt.setLabel(iLabel);
                cTempPnt.setColor(cTempLabelColor);
                cTempPnt.setShade(cTempLightColor);
                //qDebug() << cTempPnt.getComponents().size() << " ";
                cTempPnt.getComponents().clear();
                acSamplePoint.push_back(cTempPnt);

            }
            else
            {
                RaySamplePoint& rcSamplePnt = acSamplePoint[acSamplePoint.size()-1];
                cTempPnt.setColor(cTempLabelColor);
                cTempPnt.setShade(cTempLightColor);
                cTempPnt.getComponents().clear();
                rcSamplePnt.merge(cTempPnt);
            }

            iLastLabel = iLabel;
        }


    }
//    qDebug() << endl;

}

