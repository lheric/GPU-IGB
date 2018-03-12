#include "vtklineuniformrandomsamplefilter.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include <vtkSmartPointer.h>
#include <vtkCleanPolyData.h>
#include <QTime>

vtkStandardNewMacro(vtkLineUniformRandomSampleFilter)

vtkLineUniformRandomSampleFilter::vtkLineUniformRandomSampleFilter()
{
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);

    this->SkipProbability = 0.0;
}

vtkLineUniformRandomSampleFilter::~vtkLineUniformRandomSampleFilter()
{

}

int vtkLineUniformRandomSampleFilter::RequestData(vtkInformation *vtkNotUsed(request),
                                                  vtkInformationVector **inputVector,
                                                  vtkInformationVector *outputVector)
{

    // get the input and output
    vtkPolyData* input = vtkPolyData::GetData(inputVector[0],0);
    vtkPolyData* output = vtkPolyData::GetData(outputVector,0);
    output->ShallowCopy(input);


    qsrand(QTime::currentTime().msec());


    vtkSmartPointer<vtkCellArray> pcSrcLines = input->GetLines();
    vtkSmartPointer<vtkCellArray> pcDstLines = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkIdList> pcList = vtkSmartPointer<vtkIdList>::New();
    /// for each line
    pcSrcLines->InitTraversal();

    while(pcSrcLines->GetNextCell(pcList))
    {
        if( (qrand() % 10000)/10000.0 > this->SkipProbability )
        {
            // keep it - copy it into dst
            pcDstLines->InsertNextCell(pcList);
        }

    }
    vtkSmartPointer<vtkCleanPolyData> pcCleaner = vtkSmartPointer<vtkCleanPolyData>::New();

    output->SetLines(pcDstLines);
    pcCleaner->SetInputData(output);
    pcCleaner->Update();
    output->ShallowCopy(pcCleaner->GetOutput());



    return 1;
}


void vtkLineUniformRandomSampleFilter::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);
}
