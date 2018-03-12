#include "vtktrackvistrkreader.h"
#include "vtkCellArray.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include <vtkPolyLine.h>
#include "vtkSmartPointer.h"
#include <vtkNew.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <QString>
#include <QDebug>

vtkStandardNewMacro(vtkTrackVisTrkReader);


//----------------------------------------------------------------------------
vtkTrackVisTrkReader::vtkTrackVisTrkReader()
{
    this->FileName = 0;
    this->SetNumberOfInputPorts(0);
}

//----------------------------------------------------------------------------
vtkTrackVisTrkReader::~vtkTrackVisTrkReader()
{
    this->SetFileName(0);
}

//----------------------------------------------------------------------------
void vtkTrackVisTrkReader::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);
    os << indent << "FileName: "
       << (this->FileName ? this->FileName : "(none)") << "\n";

}

//----------------------------------------------------------------------------
int vtkTrackVisTrkReader::RequestData(vtkInformation*,
                                      vtkInformationVector**,
                                      vtkInformationVector* outputVector)
{
    // Make sure we have a file to read.
    if(!this->FileName)
    {
        vtkErrorMacro("A FileName must be specified.");
        return 0;
    }

    // Open the input file.
    static TrkFileReader cTrkReader;
    cTrkReader.close();
    QString strFilepath(this->FileName);
    cTrkReader.setFilepath(strFilepath.toStdString());
    if( !cTrkReader.open() )
    {
        vtkErrorMacro("File to Open " << strFilepath.toStdString().c_str());
        return false;
    }
    size_t iTrkTotal = cTrkReader.getTotalTrkNum();

    ///
    auto pcPoints = vtkSmartPointer<vtkPoints>::New();
    auto pcLines= vtkSmartPointer<vtkCellArray>::New();;

    /// cleanup previous result
    pcPoints->Initialize();
    pcLines->Initialize();

    /// random sampling
    vector<float> cTrk;
    cTrk.reserve(100*3);

    vtkSmartPointer<vtkPolyLine> pcPolyLine = vtkSmartPointer<vtkPolyLine>::New();
    for(int iTrkIdx = 0; iTrkIdx < iTrkTotal; iTrkIdx++)
    {
        cTrk.clear();
        cTrkReader.readTrack(iTrkIdx, cTrk); ///< read one track from input

        size_t iTotalPnts = cTrk.size()/3;

        pcPolyLine->Initialize();
        pcPolyLine->GetPointIds()->SetNumberOfIds(iTotalPnts);
        for(int j = 0; j < iTotalPnts; j++)
        {
            vtkIdType iPtIdx = pcPoints->InsertNextPoint(cTrk[j*3], cTrk[j*3+1], cTrk[j*3+2]);
            pcPolyLine->GetPointIds()->SetId(j,iPtIdx);
        }
        pcLines->InsertNextCell(pcPolyLine);
    }

    // Store the points and cells in the output data object.
    vtkPolyData* pcPolyData = vtkPolyData::GetData(outputVector);

    // Add the points to the dataset
    pcPolyData->SetPoints(pcPoints.GetPointer());
    // Add the lines to the dataset
    pcPolyData->SetLines(pcLines.GetPointer());

    // Add voxel size info (for calcuating the connection table)
    vtkSmartPointer<vtkFloatArray> pcVoxelSize = vtkSmartPointer<vtkFloatArray>::New();
    pcVoxelSize->SetNumberOfComponents(3);
    pcVoxelSize->SetName("voxel_size");
    pcVoxelSize->InsertNextTuple(cTrkReader.getHeader().voxel_size);
    pcPolyData->GetFieldData()->AddArray(pcVoxelSize);

    return 1;
}
