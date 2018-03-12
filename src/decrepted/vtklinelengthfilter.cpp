#include "vtklinelengthfilter.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include "vtkSmartPointer.h"

vtkStandardNewMacro(vtkLineLengthFilter)

vtkLineLengthFilter::vtkLineLengthFilter()
{
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);
}

vtkLineLengthFilter::~vtkLineLengthFilter()
{

}

int vtkLineLengthFilter::RequestData(vtkInformation *vtkNotUsed(request),
                                    vtkInformationVector **inputVector,
                                    vtkInformationVector *outputVector)
{

    // get the input and output
    vtkPolyData* input = vtkPolyData::GetData(inputVector[0],0);
    vtkPolyData* output = vtkPolyData::GetData(outputVector,0);

    input->GetPoints()->InsertNextPoint(1.0, 1.0, 1.0);

    output->ShallowCopy(input);

    return 1;
}


void vtkLineLengthFilter::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);
}
