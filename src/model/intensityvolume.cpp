#include "intensityvolume.h"
#include <QDebug>
IntensityVolume::IntensityVolume(QObject *parent) : QObject(parent)
{
    init();
}

IntensityVolume::~IntensityVolume()
{

}

void IntensityVolume::init()
{
    /// iso surface pipeline
        /// create
    this->setReader(vtkNIFTIImageReader::New());
    this->setVolume(vtkImageData::New());
    this->setGradient(vtkImageData::New());
    this->setFlipFilter(vtkImageFlip::New());
    this->setGradientFilter(vtkImageGradient::New());
    this->getFlipFilter()->SetFilteredAxis(2);
    this->getFlipFilter()->SetInputConnection(this->getReader()->GetOutputPort());
    this->getGradientFilter()->SetDimensionality(3);
    this->getGradientFilter()->SetInputConnection(this->getFlipFilter()->GetOutputPort());
}

bool IntensityVolume::openNifti(const QString &strFilename)
{
    this->setFilepath(strFilename);
    this->getReader()->SetFileName(strFilename.toStdString().c_str());
    this->getReader()->Modified();
    this->getGradientFilter()->Update();

    this->getVolume()->ShallowCopy(this->getFlipFilter()->GetOutput());

    this->getGradient()->ShallowCopy(this->getGradientFilter()->GetOutput());

    double* adSpacing = this->getVolume()->GetSpacing();
    double* adOrigin = this->getVolume()->GetOrigin();
    double* adCenter = this->getVolume()->GetCenter();

    qDebug() << "Voxel Size is" << adSpacing[0] << adSpacing[1] << adSpacing[2];
    qDebug() << "Origin is" << adOrigin[0] << adOrigin[1] << adOrigin[2];
    qDebug() << "Center is" << adCenter[0] << adCenter[1] << adCenter[2];

    emit updated();

    return true;
}

void IntensityVolume::setFilpImage(bool bFlip)
{
    if(bFlip)
        this->getFlipFilter()->SetFilteredAxes(2);
    else
        this->getFlipFilter()->SetFilteredAxes(-1);

    if(this->getFilepath().isEmpty() == false)
        openNifti(this->getFilepath());
}

