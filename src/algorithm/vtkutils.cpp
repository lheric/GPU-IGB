#include "vtkutils.h"
#include <vtkMath.h>


VTKUtils::VTKUtils()
{

}

VTKUtils::~VTKUtils()
{

}

bool VTKUtils::isEmptyDimension(int *aiDims)
{
    if(aiDims[0] == 0 || aiDims[1] == 0 || aiDims[2] == 0)
        return true;
    return false;
}

bool VTKUtils::compareIntVector3(int *aiDims1, int* aiDims2)
{
    if(aiDims1[0] == aiDims2[0] && aiDims1[1] == aiDims2[1] && aiDims1[2] == aiDims2[2])
        return true;
    return false;
}

bool VTKUtils::compareDoubleVector3(double *adDims1, double* adDims2)
{
    const double dEpslon = 0.0001;
    if( abs(adDims1[0]-adDims2[0]) < dEpslon &&
            abs(adDims1[1]-adDims2[1]) < dEpslon &&
            abs(adDims1[2]-adDims2[2]) < dEpslon)
    {
        return true;
    }
    return false;
}

