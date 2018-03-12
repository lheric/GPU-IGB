#ifndef VTKLINECOLORFILTER_H
#define VTKLINECOLORFILTER_H

#include "vtkPolyDataAlgorithm.h"

class vtkLineColorFilter : public vtkPolyDataAlgorithm
{
public:
    vtkTypeMacro(vtkLineColorFilter,vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    static vtkLineColorFilter *New();

protected:
    vtkLineColorFilter();
    ~vtkLineColorFilter();

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);


private:
    void convertToColor(double *adStartPos, double *adEndPos, unsigned char *auhColor);


private:
    vtkLineColorFilter(const vtkLineColorFilter&);  // Not implemented.
    void operator=(const vtkLineColorFilter&);  // Not implemented.

};


#endif // VTKLINECOLORFILTER_H
