#ifndef BRAINEXPVOLRENDERWIDGET_H
#define BRAINEXPVOLRENDERWIDGET_H
#include <QVTKWidget.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolume.h>
#include "gitldef.h"
#include "calculation_backend/Common/CArray2DTCP.h"
#include "calculation_backend/Common/CArray1DTCP.h"

class BrainExpVolRenderWidget : public QVTKWidget
{
    Q_OBJECT
public:
    BrainExpVolRenderWidget(QWidget *parent);
    ~BrainExpVolRenderWidget();
    void init();
    void setBGImage(vtkImageData *pcImageData);
    void setVoxelData(void*);
    void setTransferFunc(bool bOpaqueMode, double dOpacity = 1.0);

protected:
    void keyPressEvent ( QKeyEvent * event );
    void xRenderFGData();

signals:
    void exitFullscreen();

    ADD_CLASS_FIELD(bool, bBGImageLoaded, getBGImageLoaded, setBGImageLoaded)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkImageData>, pcBGImage, getBGImage, setBGImage)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkImageData>, pcFusedImage, getFusedImage, setFusedImage)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkRenderer>, pcRenderer, getRenderer, setRenderer)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkSmartVolumeMapper>, pcMapper, getMapper, setMapper)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkPiecewiseFunction>, pcOpacityFunc, getOpacityFunc, setOpacityFunc)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkColorTransferFunction>, pcColorFunc, getColorFunc, setColorFunc)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkVolumeProperty>, pcProperty, getProperty, setProperty)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkVolume>, pcVolume, getVolume, setVolume)

    /// for getting the voxels values for all the brain
    /// request a number_of_voxel X 1 "window" into the big matrix
    ADD_CLASS_FIELD(CArray2D_TCP<float>*,   pcBrainExpVoxelSNP,     getBrainExpVoxelSNP,        setBrainExpVoxelSNP)    /// Voxel-SNP result
    ADD_CLASS_FIELD(CArray1D_TCP<int>*,     pcBrainExpVoxelList,    getBrainExpVoxelList,       setBrainExpVoxelList)   /// Voxel filter
    ADD_CLASS_FIELD(CArray1D_TCP<int>*,     pcBrainExpSNPList,      getBrainExpSNPList,         setBrainExpSNPList)     /// SNP filter
};

#endif // BRAINEXPVOLRENDERWIDGET_H
