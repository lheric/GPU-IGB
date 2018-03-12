#ifndef VTKLINELENGTHFILTER_H
#define VTKLINELENGTHFILTER_H
#include <vtkPolyDataAlgorithm.h>

class vtkLineLengthFilter : public vtkPolyDataAlgorithm
{
public:
    vtkTypeMacro(vtkLineLengthFilter,vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    static vtkLineLengthFilter *New();

protected:
    vtkLineLengthFilter();
    ~vtkLineLengthFilter();

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
    vtkLineLengthFilter(const vtkLineLengthFilter&);  // Not implemented.
    void operator=(const vtkLineLengthFilter&);  // Not implemented.
};

#endif // VTKLINELENGTHFILTER_H
