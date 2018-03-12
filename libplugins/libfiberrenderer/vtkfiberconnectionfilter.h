#ifndef VTKFIBERCONNECTIONFILTER_H
#define VTKFIBERCONNECTIONFILTER_H

#include <vtkPolyDataAlgorithm.h>
#include "fiberroiconnection.h"

class vtkFiberConnectionFilter : public vtkPolyDataAlgorithm
{
public:
    vtkTypeMacro(vtkFiberConnectionFilter,vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    static vtkFiberConnectionFilter *New();

    vtkSetMacro(StartROIIdx, int)
    vtkGetMacro(StartROIIdx, int)

    vtkSetMacro(EndROIIdx, int)
    vtkGetMacro(EndROIIdx, int)

    vtkSetMacro(FiberConnections, FiberROIConnection*)
    vtkGetMacro(FiberConnections, FiberROIConnection*)


protected:
    vtkFiberConnectionFilter();
    ~vtkFiberConnectionFilter();

    int StartROIIdx;
    int EndROIIdx;

    FiberROIConnection* FiberConnections;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
    vtkFiberConnectionFilter(const vtkFiberConnectionFilter&);  // Not implemented.
    void operator=(const vtkFiberConnectionFilter&);  // Not implemented.
};

#endif // VTKFIBERCONNECTIONFILTER_H
