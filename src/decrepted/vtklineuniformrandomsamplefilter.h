#ifndef VTKLINEUNIFORMRANDOMSAMPLEFILTER_H
#define VTKLINEUNIFORMRANDOMSAMPLEFILTER_H

#include "vtkPolyDataAlgorithm.h"

class vtkLineUniformRandomSampleFilter : public vtkPolyDataAlgorithm
{

public:
    vtkTypeMacro(vtkLineUniformRandomSampleFilter,vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    vtkSetMacro(SkipProbability, double)
    vtkGetMacro(SkipProbability, double)


    static vtkLineUniformRandomSampleFilter *New();

protected:
    vtkLineUniformRandomSampleFilter();
    ~vtkLineUniformRandomSampleFilter();

    double SkipProbability;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);


private:
    vtkLineUniformRandomSampleFilter(const vtkLineUniformRandomSampleFilter&);  // Not implemented.
    void operator=(const vtkLineUniformRandomSampleFilter&);  // Not implemented.
};

#endif // VTKLINEUNIFORMRANDOMSAMPLEFILTER_H
