#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)

#include "brainexpvolrenderwidget.h"
#include <QDebug>
#include <QKeyEvent>
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include <vtkRendererCollection.h>
#include <vtkSampleFunction.h>
#include <vtkSphere.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkImageShiftScale.h>
#include <QtMath>
#include "genomeexplorermodel.h"


#define FREE_IF_NOT_NULL(ptr) \
    if(ptr != nullptr) \
    { \
    delete ptr; \
    ptr = nullptr; \
    }

BrainExpVolRenderWidget::BrainExpVolRenderWidget(QWidget *parent) :
    QVTKWidget(parent)
{
    init();
}

BrainExpVolRenderWidget::~BrainExpVolRenderWidget()
{

}

void BrainExpVolRenderWidget::init()
{
    ///
    this->setBGImageLoaded(false);
    this->setBrainExpSNPList(nullptr);
    this->setBrainExpVoxelList(nullptr);
    this->setBrainExpVoxelSNP(nullptr);

    /// create a fake data so the pipeline won't complain
    vtkSmartPointer<vtkImageData> pcBGImage = vtkSmartPointer<vtkImageData>::New();
    pcBGImage->SetDimensions(10, 10, 10);
    pcBGImage->AllocateScalars(VTK_DOUBLE, 1.0f);
    this->setBGImage(pcBGImage);
    this->setFusedImage(vtkSmartPointer<vtkImageData>::New());
    this->getFusedImage()->DeepCopy(pcBGImage);

    this->setRenderer(vtkSmartPointer<vtkRenderer>::New());
    this->GetRenderWindow()->AddRenderer(this->getRenderer());

    this->setMapper(vtkSmartPointer<vtkSmartVolumeMapper>::New());
//    this->getMapper()->SetBlendModeToComposite(); // composite first
    this->getMapper()->SetInputData(this->getFusedImage());
//    this->getMapper()->SetSampleDistance(0.1);

    this->setOpacityFunc(vtkSmartPointer<vtkPiecewiseFunction>::New());

    /// for background data opacity
    this->setTransferFunc(false, 0.02);

    this->setColorFunc(vtkSmartPointer<vtkColorTransferFunction>::New());

    // for background data color
    this->getColorFunc()->AddRGBPoint(0.0  ,0.0,0.0,0.0);
    this->getColorFunc()->AddRGBPoint(0.09  ,0.0,0.0,0.0);
    this->getColorFunc()->AddRGBPoint(0.1  ,1.0,1.0,1.0);
    this->getColorFunc()->AddRGBPoint(1.0  ,1.0,1.0,1.0);

    // for foreground data color
    this->getColorFunc()->AddHSVPoint(1.01, 0.667,1.0,1.0);
    this->getColorFunc()->AddHSVPoint(1.33, 0.444,1.0,1.0);
    this->getColorFunc()->AddHSVPoint(2.0 , 0.0,  1.0,1.0);
    this->getColorFunc()->AddHSVPoint(255.0,0.0,  1.0,1.0);

    this->setProperty(vtkSmartPointer<vtkVolumeProperty>::New());
    this->getProperty()->ShadeOn();
    this->getProperty()->SetInterpolationType(VTK_NEAREST_INTERPOLATION);
    this->getProperty()->SetScalarOpacity(this->getOpacityFunc()); // composite first.
    this->getProperty()->SetColor(this->getColorFunc());

    this->setVolume(vtkSmartPointer<vtkVolume>::New());
    this->getVolume()->SetMapper(this->getMapper());
    this->getVolume()->SetProperty(this->getProperty());

    this->getRenderer()->AddViewProp(this->getVolume());
    this->getRenderer()->ResetCamera();


    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();
    connect(pcModel, &GenomeExplorerModel::onHoveringPointChanged,
            this, &BrainExpVolRenderWidget::xRenderFGData);
    connect(pcModel, &GenomeExplorerModel::onSetOpaqueMode,
            this, [&](bool bOpaqueMode) {
                this->setTransferFunc(bOpaqueMode);
                this->GetRenderWindow()->Render();
            });
    connect(pcModel, &GenomeExplorerModel::onOpacityChanged,
            this, [&](double bOpacity) {
                this->setTransferFunc(false, bOpacity);
                this->GetRenderWindow()->Render();
            });
}

void BrainExpVolRenderWidget::setBGImage(vtkImageData *pcImageData)
{
    this->getBGImage()->DeepCopy(pcImageData);
    this->getFusedImage()->DeepCopy(pcImageData);
    this->setBGImageLoaded(true);

    this->GetRenderWindow()->Render();
    this->getRenderer()->ResetCamera();


}

void BrainExpVolRenderWidget::setTransferFunc(bool bOpaqueMode, double dOpacity)
{
    this->getOpacityFunc()->RemoveAllPoints();
    if( bOpaqueMode == false )
    {
        /// for background data opacity
        this->getOpacityFunc()->AddPoint(0.0,   0.0);
        this->getOpacityFunc()->AddPoint(1.0,   1.0*dOpacity);

        /// for foreground data opacity
        this->getOpacityFunc()->AddPoint(1.01,  1.0*dOpacity);
        this->getOpacityFunc()->AddPoint(2.0,   1.0*dOpacity);
        this->getOpacityFunc()->AddPoint(255.0, 1.0*dOpacity);
    }
    else
    {
        /// for background data opacity
        this->getOpacityFunc()->AddPoint(0.0,   1.0);
        this->getOpacityFunc()->AddPoint(1.0,   1.0);

        /// for foreground data opacity
        this->getOpacityFunc()->AddPoint(1.01,  0.0);
        this->getOpacityFunc()->AddPoint(2.0,   1.0);
        this->getOpacityFunc()->AddPoint(255.0, 1.0);
    }
}


void BrainExpVolRenderWidget::keyPressEvent(QKeyEvent *event)
{

    if(event->type() == QEvent::KeyPress)
    {
        if( event->key() == Qt::Key_Escape )
        {
            emit exitFullscreen();
        }
    }
    QVTKWidget::keyPressEvent(event);
}

void BrainExpVolRenderWidget::xRenderFGData()
{
    static int iLastSNPDataIdxX = -1;
    GenomeExplorerModel* pcModel = GenomeExplorerModel::getInstance();

    /// sanity check
    if(pcModel->getDataUploaded() == false || this->getBGImageLoaded() == false)
        return;

    CSocket* pcSocket = &pcModel->getClientSocket();
    QPoint cHoveringPoint = pcModel->getHoveringPoint();
    int iNumberOfVoxels = pcModel->getNumberOfVoxels();
    int iNumberOfSubjects= pcModel->getNumberOfSubjects();
    int iNumberOfSNPs = pcModel->getNumberOfSNPs();
    int iSNPDataIdxX = cHoveringPoint.x();

    /// avoid duplicate ANOVA call for the same input data (when user move the mouse in the same column)
    if(iLastSNPDataIdxX == iSNPDataIdxX)
        return;

    /// allocate matrix
    if(this->getBrainExpSNPList() == nullptr)
    {
        this->setBrainExpSNPList(new CArray1D_TCP<int>(pcSocket, 1, true, 0));
    }
    (*this->getBrainExpSNPList())(0) = iSNPDataIdxX;
    this->getBrainExpSNPList()->CopyToServer(); /// upload


    if(this->getBrainExpVoxelList() == nullptr || this->getBrainExpVoxelList()->GetNumElements() != iNumberOfVoxels)
    {
        delete this->getBrainExpVoxelList();
        this->setBrainExpVoxelList(new CArray1D_TCP<int>(pcSocket, iNumberOfVoxels, false));
        for(int i = 0; i < iNumberOfVoxels; ++i)
            (*this->getBrainExpVoxelList())(i) = i; /// select all voxels
        this->getBrainExpVoxelList()->CopyToServer();   /// upload

    }

    if(this->getBrainExpVoxelSNP() == nullptr ||
            this->getBrainExpVoxelSNP()->GetWidth()  != 1 ||
            this->getBrainExpVoxelSNP()->GetHeight() != iNumberOfVoxels)
    {
        this->setBrainExpVoxelSNP(new CArray2D_TCP<float>(&pcModel->getClientSocket(),
                                                          iNumberOfVoxels,
                                                          1,
                                                          false));
    }

    /// do ANOVA
    bool bResult = DoFunctionANova(
                pcModel->getVoxelSubject()->GetServerDataPtr(),
                pcModel->getSNPSubject()->GetServerDataPtr(),
                this->getBrainExpVoxelSNP()->GetServerDataPtr(),
                iNumberOfVoxels,
                iNumberOfSubjects,
                iNumberOfSNPs,
                this->getBrainExpVoxelList()->GetServerDataPtr(),
                this->getBrainExpVoxelList()->GetNumElements(),
                this->getBrainExpSNPList()->GetServerDataPtr(),
                this->getBrainExpSNPList()->GetNumElements(),
                &pcModel->getClientSocket());

    /// get result
    if(bResult == false)
    {
        qCritical() << "ANOVA in Brain Explorer failed";
    }
    else
    {
        qDebug() << "ANOVA in Brain Explorer succeed";
        this->getBrainExpVoxelSNP()->CopyFromServer();  /// copy results from server
    }

    /// fusion of background MRI image and foreground data
    vtkSmartPointer<vtkImageData> pcFusedImage = this->getFusedImage();
    pcFusedImage->DeepCopy(this->getBGImage());


    if(this->getFusedImage()->GetPointData()->GetNumberOfArrays() == 0)
    {
        qDebug() << "BrainExpVolRenderWidget no scalar array found in image data";
        return;
    }

    vtkDataArray* pcScalar = pcFusedImage->GetPointData()->GetArray(0);
    int iImageMaxIdx = pcScalar->GetNumberOfTuples();
    CArray2D_TCP<float>* pcData = this->getBrainExpVoxelSNP();
    for(int i = 0; i < iNumberOfVoxels; ++i)
    {
        int iVoxelIdx = i;
        if( iVoxelIdx >= iImageMaxIdx )
        {
            qDebug() << "BrainExpVolRenderWidget" << "image scalar index out of range" << iVoxelIdx << ">=" << iImageMaxIdx;
            break;
        }
        double dBGValue = *pcScalar->GetTuple(iVoxelIdx);
        double dFGData = (*pcData)(i, 0);

        if(dBGValue > 0.01 && dFGData > 0.7)
        {
            double fusedData = 1.0+dFGData;
            pcScalar->SetTuple(iVoxelIdx, &fusedData);
        }
    }

    iLastSNPDataIdxX = iSNPDataIdxX;    /// update the last SNP index

    this->GetRenderWindow()->Render();
}

#undef FREE_IF_NOT_NULL
