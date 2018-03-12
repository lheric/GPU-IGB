#include "labelsurface.h"
#include "algorithm/imageutils.h"

LabelSurface::LabelSurface(QObject *parent) : QObject(parent)
{
    init();
}

LabelSurface::~LabelSurface()
{

}

void LabelSurface::init()
{
    m_pcIntensityVol = NULL;
    m_pcLabelVol = NULL;
    m_pcHighlightedROI = NULL;
    m_dISO = 0.3;
    m_dOpacity = 0.15;
    this->setContourFilter(vtkContourFilter::New());
    this->setSurface(vtkPolyData::New());
    this->setSurfaceLabels(vtkIntArray::New());
    this->setPolyCleaner(vtkCleanPolyData::New());
    this->setLabelMapper(vtkPointDataToCellData::New());
    xDeleteAllROIs();
    this->setROIClipUpBound(vtkClipPolyData::New());
    this->setROIClipLowBound(vtkClipPolyData::New());
    this->setROIExtractor(vtkThreshold::New());
    this->setGeoFilter(vtkGeometryFilter::New());
}

//bool LabelSurface::buildLabelSurace()
//{
//    /// extract iso surface
//    if(m_pcIntensityVol == NULL || m_pcIntensityVol->getFilepath().isEmpty())
//        return false;
//    this->getContourFilter()->SetInputData(m_pcIntensityVol->getVolume());
//    this->getContourFilter()->GenerateValues(1, m_dISO, m_dISO+0.1); // (numContours, rangeStart, rangeEnd)
//    this->getContourFilter()->Modified();
//    this->getContourFilter()->Update();
//    this->getPolyCleaner()->SetInputData(this->getContourFilter()->GetOutput());
//    this->getPolyCleaner()->Update();
//    this->getSurface()->ShallowCopy(this->getPolyCleaner()->GetOutput());

//    qDebug() << this->getSurface()->GetNumberOfPolys() << this->getSurface()->GetNumberOfPoints();

//    /// label it with label volume
//    int iTotalCells = this->getSurface()->GetPolys()->GetNumberOfCells();
//    vtkSmartPointer<vtkIntArray> auhCellLabels = this->getSurfaceLabels();
//    auhCellLabels->SetNumberOfComponents(1);
//    auhCellLabels->SetNumberOfTuples(iTotalCells);
//    this->getSurface()->GetCellData()->SetScalars(auhCellLabels);
//    if(m_pcLabelVol == NULL || m_pcLabelVol->getFilepath().isEmpty())
//    {
//        /// no label volume, fill ones for all cells
//        for(int i = 0; i < iTotalCells; ++i)
//        {
//            this->getSurface()->GetCellData()->GetScalars()->SetTuple1(i, 0);
//        }
//    }
//    else
//    {
//        vtkCellLocator* pcLocator = m_pcLabelVol->getLocator();
//        vtkUnstructuredGrid* pcGMLabel = m_pcLabelVol->getVolume();
//        double closest[3]; vtkIdType idx; int subID; double dist;
//        vtkDataArray* pLabel = pcGMLabel->GetPointData()->GetScalars();
//        double * pt = NULL;
//        qDebug() << "Start Loop";
//        for(int i = 0; i < iTotalCells; ++i)
//        {

//            vtkCell* pcCell = this->getSurface()->GetCell(i);
//            Q_ASSERT(pcCell->GetNumberOfPoints() != 0);
//            pt = this->getSurface()->GetPoint(pcCell->GetPointIds()->GetId(0));
//            pcLocator->FindClosestPoint(pt, closest, idx, subID, dist);
//            int label = pLabel->GetTuple1(pcGMLabel->GetCell(idx)->GetPointId(0));
//            this->getSurface()->GetCellData()->GetScalars()->SetTuple1(i, label);
//        }
//        qDebug() << "End Loop";
//        this->getSurface()->Modified();
//    }

//    /// extracting ROIs
//    xSplitIntoROIByLabel();

//    /// calculate ROI-based gradient
//    ImageUtils::calculateROIGradient(m_pcIntensityVol->getVolume().GetPointer(),
//                                     m_pcLabelVol->getUnclippedVolume().GetPointer(),
//                                     m_pcIntensityVol->getGradient().GetPointer());

//    emit updated();

//    return true;
//}

//bool LabelSurface::addActorsToRenderer(vtkRenderer *pcRenderer)
//{
//    /// remove first
//    removeActorsFromRenderer(pcRenderer);
//    foreach(ROI* pcROI, this->getROIs())
//        pcROI->addToRenderer(pcRenderer);
//    return true;
//}

//bool LabelSurface::removeActorsFromRenderer(vtkRenderer *pcRenderer)
//{
//    foreach(ROI* pcROI, this->getROIs())
//        pcROI->removeFromRenderer(pcRenderer);
//    return true;
//}


ROI *LabelSurface::getROIByActor(vtkObject* pcObj)
{
    foreach(ROI* pcROI, m_cROIs)
        if(pcObj == pcROI->getActor().GetPointer())
            return pcROI;
    return NULL;
}

ROI *LabelSurface::getROIByLabel(int iLabel)
{
    ///
    if(iLabel <= 0)
        return NULL;


    /// try corresponding index first
    if(iLabel >0 && iLabel <= m_cROIs.size())
        if(iLabel == m_cROIs[iLabel-1]->getLabel())
            return m_cROIs[iLabel-1];

    /// exhausted search
    foreach(ROI* pcROI, m_cROIs)
        if(iLabel == pcROI->getLabel())
            return pcROI;
    return NULL;
}

void LabelSurface::setHighlightedROI(ROI *pcHighlightROI)
{
    ROI* pcPrevHighlightedROI = this->getHighlightedROI();
    /// de-highlight the previous highlighted one
    if(pcPrevHighlightedROI != NULL)
         pcPrevHighlightedROI->setIsHighlighted(false);
    /// highlight the new one
    if(pcHighlightROI != NULL)
    {
        pcHighlightROI->setIsHighlighted(true);
        this->m_pcHighlightedROI = pcHighlightROI;
    }
}


void LabelSurface::xSplitIntoROIByLabel()
{
    /// clear
    xDeleteAllROIs();

    if(m_pcLabelVol == NULL || m_pcLabelVol->getFilepath().isEmpty())
    {
        /// no label volume. take the whole gray matter as one ROI
        ROI* pcROI = new ROI();
        pcROI->setLabel(0);
        pcROI->setOpacity(m_dOpacity);
        pcROI->getSurface()->DeepCopy(this->getSurface());
        QVector3D cNormal;
        pcROI->setTextureNormal(cNormal);
        pcROI->getSurface()->Modified();
        this->getROIs().push_back(pcROI);
    }
    else
    {
        this->getROIClipLowBound()->SetInputData(this->getSurface());
        this->getROIClipLowBound()->SetInsideOut(false);
        this->getROIClipUpBound()->SetInputConnection(this->getROIClipLowBound()->GetOutputPort());
        this->getROIClipUpBound()->SetInsideOut(true);


        this->getROIExtractor()->SetInputData(this->getSurface());
        this->getROIExtractor()->SetInputArrayToProcess(0, 0, 0,
                                                        vtkDataObject::FIELD_ASSOCIATION_CELLS,
                                                        vtkDataSetAttributes::SCALARS);
        this->getGeoFilter()->SetInputConnection(getROIExtractor()->GetOutputPort());

        int iLabelMin = this->m_pcLabelVol->getLabelMin();
        int iLabelMax = this->m_pcLabelVol->getLabelMax();

        double* adBrainCenter = this->getSurface()->GetCenter();
        double* adROICenter = NULL;

        for(int iLabel = iLabelMin; iLabel < iLabelMax+1; iLabel++)
        {

            this->getROIExtractor()->ThresholdBetween(iLabel-0.1, iLabel+0.1);
            this->getROIExtractor()->Modified();
            this->getROIExtractor()->Update();

            this->getGeoFilter()->Update();

            ROI* pcROI = new ROI();
            pcROI->setLabel(iLabel);
            pcROI->setOpacity(m_dOpacity);
            pcROI->getSurface()->DeepCopy( this->getGeoFilter()->GetOutput());
            adROICenter = pcROI->getSurface()->GetCenter();
            QVector3D cNormal(  adROICenter[0]-adBrainCenter[0],
                                adROICenter[1]-adBrainCenter[1],
                                adROICenter[2]-adBrainCenter[2]);
            pcROI->setTextureNormal(cNormal);
            pcROI->getSurface()->Modified();
            this->getROIs().push_back(pcROI);
            qDebug() << "ROI #" <<  iLabel << "Parcellation Complete";
        }
    }
}

void LabelSurface::xDeleteAllROIs()
{
    this->m_pcHighlightedROI = NULL;
    foreach(ROI*pcROI, m_cROIs)
    {
        pcROI->removeFromRenderer(NULL);
        delete pcROI;
    }
    this->m_cROIs.clear();
}

