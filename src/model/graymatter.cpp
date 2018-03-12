#include "graymatter.h"
#include "vtkPointData.h"
#include <QDebug>
#include "model/rom.h"

GrayMatter::GrayMatter(QObject *parent) : QObject(parent)
{
}

GrayMatter::~GrayMatter()
{

}

void GrayMatter::setFilp(bool bStatus)
{
    this->getIntensityVol().setFilpImage(bStatus);
    this->getLabelVol().setFlipImage(bStatus);
}

bool GrayMatter::openIntensityVolume(const QString &strFilepath)
{
    this->getIntensityVol().openNifti(strFilepath);
    return true;
}

bool GrayMatter::openLabelVolume(const QString &strFilepath, double& rdLabelMin, double& rdLabelMax)
{
    this->getLabelVol().openNifti(strFilepath);
    rdLabelMax = this->getLabelVol().getLabelMax();
    rdLabelMin = this->getLabelVol().getLabelMin();
    return true;
}
