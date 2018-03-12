#include "labelvolume.h"

LabelVolume::LabelVolume(QObject *parent) : QObject(parent)
{
    init();
}

LabelVolume::~LabelVolume()
{

}

void LabelVolume::init()
{
    /// label pipeline
        /// create
    this->setReader(vtkNIFTIImageReader::New());

    this->setFlipFilter(vtkImageFlip::New());
    this->getFlipFilter()->SetFilteredAxis(2);

    this->setClipper(vtkClipVolume::New());
    this->getClipper()->SetValue(0.9);    //remove zero value voxels
    this->getClipper()->GenerateClippedOutputOff();
    this->getClipper()->GenerateClipScalarsOff();

    this->setUnclippedVolume(vtkImageData::New());
    this->setVolume(vtkUnstructuredGrid::New());
    this->setLocator(vtkCellLocator::New());

        /// connect
    this->getFlipFilter()->SetInputData(vtkImageData::New());
    this->getClipper()->SetInputConnection(this->getFlipFilter()->GetOutputPort());

}

void LabelVolume::setFlipImage(bool bFlip)
{
    if(bFlip)
        this->getFlipFilter()->SetFilteredAxes(2);
    else
        this->getFlipFilter()->SetFilteredAxes(-1);

    if(this->getFilepath().isEmpty() == false)
        openNifti(this->getFilepath());
}

bool LabelVolume::openNifti(const QString &strFilename)
{
    /// read
    this->setFilepath(strFilename);
    this->getReader()->SetFileName(strFilename.toStdString().c_str());
    this->getFlipFilter()->SetInputConnection(this->getReader()->GetOutputPort());
    this->getReader()->Update();

    this->getFlipFilter()->Update();
    this->getUnclippedVolume()->ShallowCopy(this->getFlipFilter()->GetOutput());

    /// get the clipped result (as a side effect, the label in the clipped volume is lost)
    this->getClipper()->Update();
    this->getVolume()->ShallowCopy(this->getClipper()->GetOutput());

    /// recover the label for clipped volume (use locator to find the correspondence with the unclipped volume)
        /// build cell locator
    vtkSmartPointer<vtkCellLocator> pcUnclippedVolLocator = vtkCellLocator::New();
    pcUnclippedVolLocator->SetDataSet(this->getUnclippedVolume());
    pcUnclippedVolLocator->BuildLocator();
    pcUnclippedVolLocator->Update();
        /// find corresponding point and copy label
    vtkIdType iTotalPts = this->getVolume()->GetPointData()->GetScalars()->GetSize();
    double adClosest[3]; vtkIdType iCellIdx; int iSubID; double dDist;
    for(int iPtIdx = 0; iPtIdx < iTotalPts; iPtIdx++)
    {
        double* adPt = this->getVolume()->GetPoints()->GetPoint(iPtIdx);
        pcUnclippedVolLocator->FindClosestPoint(adPt, adClosest, iCellIdx, iSubID, dDist);
        vtkIdList* ptIDs = this->getUnclippedVolume()->GetCell(iCellIdx)->GetPointIds();
        vtkIdType iVoxPtNum = ptIDs->GetNumberOfIds();
        double dLabel = 0;
        for(int iVoxPtIdx = 0; iVoxPtIdx < iVoxPtNum; iVoxPtIdx++)  /// avoid 0 labels on the border
        {
            dLabel = getUnclippedVolume()->GetPointData()->GetScalars()->GetTuple1(ptIDs->GetId(iVoxPtIdx));
            if(round(dLabel) != 0)
                break;
        }
        this->getVolume()->GetPointData()->GetScalars()->SetTuple1(iPtIdx, dLabel);
    }
    this->getVolume()->Modified();

    /// build up cell locator for label query
    this->getLocator()->SetDataSet(this->getVolume());
    this->getLocator()->BuildLocator();
    this->getLocator()->Update();

    /// get label range
    double* adScalarRange = this->getVolume()->GetPointData()->GetScalars()->GetRange();
    this->setLabelMin(round(adScalarRange[0])); this->setLabelMax(round(adScalarRange[1]));
    qDebug() << QString("Label Range: [%1, %2]").arg(this->getLabelMin()).arg(this->getLabelMax());

    emit updated();

    return true;

}

