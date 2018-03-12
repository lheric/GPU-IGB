#include "vtklinecolorfilter.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include <vtkNew.h>
#include <vtkIdList.h>
#include <vtkCellData.h>
#include "vtkSmartPointer.h"
#include <QDebug>

vtkStandardNewMacro(vtkLineColorFilter)

vtkLineColorFilter::vtkLineColorFilter()
{
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);
}

vtkLineColorFilter::~vtkLineColorFilter()
{

}

int vtkLineColorFilter::RequestData(vtkInformation *vtkNotUsed(request),
                                    vtkInformationVector **inputVector,
                                    vtkInformationVector *outputVector)
{

    // get the input and output
    vtkPolyData* input = vtkPolyData::GetData(inputVector[0],0);
    vtkPolyData* output = vtkPolyData::GetData(outputVector,0);

    vtkSmartPointer<vtkCellArray> pcLines = input->GetLines();
    vtkSmartPointer<vtkPoints> pcPoints = input->GetPoints();
    vtkSmartPointer<vtkUnsignedCharArray> pcLineColors= vtkSmartPointer<vtkUnsignedCharArray>::New();
    pcLineColors->SetNumberOfComponents(4);

    vtkIdType iNumofLines = pcLines->GetNumberOfCells();
    vtkSmartPointer<vtkIdList> pcList = vtkSmartPointer<vtkIdList>::New();
    double adFirstPnt[3];
    double adLastPnt[3];
    unsigned char auhTrkColor[4] = {255, 255, 255, 100};
    /// for each line
    pcLines->InitTraversal();
    while(pcLines->GetNextCell(pcList))
    {

        vtkIdType iNumofPnts = pcList->GetNumberOfIds();
        vtkIdType iFirst = pcList->GetId(0);
        vtkIdType iLast = pcList->GetId(iNumofPnts-1);
        pcPoints->GetPoint(iFirst, adFirstPnt);
        pcPoints->GetPoint(iLast, adLastPnt);

        this->convertToColor(adFirstPnt, adLastPnt, auhTrkColor);

        pcLineColors->InsertNextTupleValue(auhTrkColor);

    }

    input->GetCellData()->SetScalars(pcLineColors);

    output->ShallowCopy(input);

    return 1;
}


void vtkLineColorFilter::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);
}

void vtkLineColorFilter::convertToColor(double *adStartPos, double *adEndPos, unsigned char *auhColor)
{

    double dSum = 0.0;
    static double adColor[3];
    for(int i = 0; i < 3; i++)
    {
        adColor[i] = qAbs(adStartPos[i]-adEndPos[i]);
        dSum += adColor[i];
    }

    for(int i = 0; i < 3; i++)
    {
        auhColor[i] = qRound((adColor[i]/dSum)*255.0);
    }
}

