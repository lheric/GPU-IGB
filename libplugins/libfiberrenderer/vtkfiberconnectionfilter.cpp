#include "vtkfiberconnectionfilter.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include "vtkSmartPointer.h"
#include "vtkCellData.h"
#include <vtkCellArray.h>
#include <vtkCleanPolyData.h>
#include <QDebug>

vtkStandardNewMacro(vtkFiberConnectionFilter)

vtkFiberConnectionFilter::vtkFiberConnectionFilter()
{
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);

    this->StartROIIdx = -1;
    this->EndROIIdx = -1;

    this->SetFiberConnections(new FiberROIConnection());

}

vtkFiberConnectionFilter::~vtkFiberConnectionFilter()
{
    delete this->FiberConnections;
}

int vtkFiberConnectionFilter::RequestData(vtkInformation *vtkNotUsed(request),
                                          vtkInformationVector **inputVector,
                                          vtkInformationVector *outputVector)
{

    // get the input and output
    vtkPolyData* input = vtkPolyData::GetData(inputVector[0],0);
    vtkPolyData* output = vtkPolyData::GetData(outputVector,0);
    output->ShallowCopy(input);


    if( (this->StartROIIdx != -1 || this->EndROIIdx != -1) && this->FiberConnections != nullptr )
    {
        vtkSmartPointer<vtkCellArray> pcSrcLines = input->GetLines();
        vtkSmartPointer<vtkCellArray> pcDstLines = vtkSmartPointer<vtkCellArray>::New();
        vtkSmartPointer<vtkIdList> pcList = vtkSmartPointer<vtkIdList>::New();
        /// for each line
        pcSrcLines->InitTraversal();
        int iCellIdx = 0;

        int sizef = this->FiberConnections->getFiberConnectionInfo().size();

        while(pcSrcLines->GetNextCell(pcList))
        {
            ConnectedROIs& rcConn = this->FiberConnections->getFiberConnectionInfo()[iCellIdx];
            if( (this->StartROIIdx == -1 || rcConn.isConnecting(this->StartROIIdx)) &&
                    (this->EndROIIdx == -1 || rcConn.isConnecting(this->EndROIIdx)) )
            {
                // keep it - copy it into dst
                pcDstLines->InsertNextCell(pcList);
            }
            iCellIdx++;

        }
        vtkSmartPointer<vtkCleanPolyData> pcCleaner = vtkSmartPointer<vtkCleanPolyData>::New();

        output->SetLines(pcDstLines);
        pcCleaner->SetInputData(output);
        pcCleaner->Update();
        output->ShallowCopy(pcCleaner->GetOutput());

    }

    return 1;
}


void vtkFiberConnectionFilter::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);
}
