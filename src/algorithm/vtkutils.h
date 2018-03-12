#ifndef VTKUTILS_H
#define VTKUTILS_H


class VTKUtils
{
public:
    VTKUtils();
    ~VTKUtils();
    static bool isEmptyDimension(int *aiDims);
    static bool compareIntVector3(int *aiDims1, int *aiDims2);
    static bool compareDoubleVector3(double *adDims1, double* adDims2);
    static void readLabelImg(int x, int y, int z, int* iLabel);
    static void readIntensityImg(int x, int y, int z, double* dIntensity);
};

#endif // VTKUTILS_H
