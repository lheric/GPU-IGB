#include "GMISORenderer.h"
#include "model/subject.h"
#include "algorithm/imageutils.h"
#include "model/roi.h"
#include "model/subject.h"
#include "model/parcellationcolorscheme.h"
#include <vtkDiscreteMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <QDebug>

GMISORenderer::GMISORenderer(QObject *parent)
{
    Q_UNUSED(parent);

    setName("Gray Matter");
    m_pcConfigPanel = new GMISOConfigPanel(this);
}

GMISORenderer::~GMISORenderer()
{
    delete m_pcConfigPanel;
}

bool GMISORenderer::init()
{
    /// create
    m_pcIntensityVol = NULL;
    m_pcLabelVol = NULL;
    m_pcHighlightedROI = NULL;
    m_bMapParcellation2Surface = false;
    m_bShowIntensity = true;
    m_bShowParcellation = true;
    m_dISO = 0.3;
    m_dIntensityOpacity = 0.15;
    m_dParcellationOpacity = 0.45;
    this->setContourFilter(vtkContourFilter::New());
    this->setSurface(vtkPolyData::New());
    this->setSurfaceLabels(vtkIntArray::New());
    this->setPolyCleaner(vtkCleanPolyData::New());
    this->setLabelMapper(vtkPointDataToCellData::New());

    this->setROIClipUpBound(vtkClipPolyData::New());
    this->setROIClipLowBound(vtkClipPolyData::New());
    this->setROIExtractor(vtkThreshold::New());
    this->setGeoFilter(vtkGeometryFilter::New());

    /// add property panel to mainwin
    m_pcConfigPanel->attach(getContext()->pcEventBus);
    getContext()->pcPluginPropTabs->addTab(m_pcConfigPanel, this->getName());

    /// set color map
    m_pcConfigPanel->getColorSchemeModel().setColorMap(&getContext()->pcSubject->getGrayMat().getColorScheme().getColorMap());

    GrayMatter& rcGM = getContext()->pcSubject->getGrayMat();
    m_pcIntensityVol = &rcGM.getIntensityVol();
    m_pcLabelVol = &rcGM.getLabelVol();
    m_pcLabelNames = &rcGM.getLabelNames();
    this->connect(m_pcIntensityVol, SIGNAL(updated()), this, SLOT(buildIntensitySurface()));
    this->connect(m_pcLabelVol, SIGNAL(updated()), this, SLOT(buildLabelSurface()));
    this->connect(&rcGM.getColorScheme(), SIGNAL(updated(ParcellationColorScheme*)), this, SLOT(onNewColorScheme(ParcellationColorScheme*)));

    /// enable this plugin by default
    setEnable(true);
    return true;
}

bool GMISORenderer::uninit()
{
    xDeleteIntensityROIs();

    return true;
}


bool GMISORenderer::redraw()
{
    return true;
}


bool GMISORenderer::buildIntensitySurface()
{
    /// extract iso surface
    if(m_pcIntensityVol == NULL || m_pcIntensityVol->getFilepath().isEmpty())
        return false;
    this->getContourFilter()->SetInputData(m_pcIntensityVol->getVolume());
    this->getContourFilter()->GenerateValues(1, m_dISO, m_dISO+0.1); // (numContours, rangeStart, rangeEnd)
    this->getContourFilter()->Modified();
    this->getContourFilter()->Update();
    this->getPolyCleaner()->SetInputData(this->getContourFilter()->GetOutput());
    this->getPolyCleaner()->Update();
    this->getSurface()->ShallowCopy(this->getPolyCleaner()->GetOutput());

    qDebug() << this->getSurface()->GetNumberOfPolys() << this->getSurface()->GetNumberOfPoints();

    /// label it with label volume
    int iTotalCells = this->getSurface()->GetPolys()->GetNumberOfCells();
    vtkSmartPointer<vtkIntArray> auhCellLabels = this->getSurfaceLabels();
    auhCellLabels->SetNumberOfComponents(1);
    auhCellLabels->SetNumberOfTuples(iTotalCells);
    this->getSurface()->GetCellData()->SetScalars(auhCellLabels);
    if(this->getMapParcellation2Surface() == false || m_pcLabelVol == NULL || m_pcLabelVol->getFilepath().isEmpty())
    {
        /// no label volume, fill ones for all cells
        for(int i = 0; i < iTotalCells; ++i)
        {
            this->getSurface()->GetCellData()->GetScalars()->SetTuple1(i, 0);
        }
    }
    else
    {
        vtkCellLocator* pcLocator = m_pcLabelVol->getLocator();
        vtkUnstructuredGrid* pcGMLabel = m_pcLabelVol->getVolume();
        double closest[3]; vtkIdType idx; int subID; double dist;
        vtkDataArray* pLabel = pcGMLabel->GetPointData()->GetScalars();
        double * pt = NULL;
        qDebug() << "Start Loop";
        for(int i = 0; i < iTotalCells; ++i)
        {

            vtkCell* pcCell = this->getSurface()->GetCell(i);
            Q_ASSERT(pcCell->GetNumberOfPoints() != 0);
            pt = this->getSurface()->GetPoint(pcCell->GetPointIds()->GetId(0));
            pcLocator->FindClosestPoint(pt, closest, idx, subID, dist);
            int label = pLabel->GetTuple1(pcGMLabel->GetCell(idx)->GetPointId(0));
            this->getSurface()->GetCellData()->GetScalars()->SetTuple1(i, label);
        }
        qDebug() << "End Loop";
        this->getSurface()->Modified();
    }

    /// extracting ROIs
    xSplitIntoROIByLabel();

    /// calculate ROI-based gradient
//    ImageUtils::calculateROIGradient(m_pcIntensityVol->getVolume().GetPointer(),
//                                     m_pcLabelVol->getUnclippedVolume().GetPointer(),
//                                     m_pcIntensityVol->getGradient().GetPointer());

    this->update();

    return true;
}

bool GMISORenderer::buildLabelSurface()
{
    if(m_pcLabelVol == NULL || m_pcLabelVol->getFilepath().isEmpty())
        return false;

    int iLabelMin = this->m_pcLabelVol->getLabelMin();
    int iLabelMax = this->m_pcLabelVol->getLabelMax();

    double* adBrainCenter = m_pcLabelVol->getVolume()->GetCenter();

    /// label contours
    xDeleteLabelROIs();

    vtkSmartPointer<vtkDiscreteMarchingCubes> pcContourFilter = vtkSmartPointer<vtkDiscreteMarchingCubes>::New();
    vtkSmartPointer<vtkWindowedSincPolyDataFilter> pcSoomther = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
    pcContourFilter->SetInputData(m_pcLabelVol->getUnclippedVolume());
    pcSoomther->SetInputConnection(pcContourFilter->GetOutputPort());
    for(int iLabel = iLabelMin; iLabel < iLabelMax+1; iLabel++)
    {
        pcContourFilter->GenerateValues(1, iLabel, iLabel); // (numContours, rangeStart, rangeEnd)
        pcSoomther->Update();

        ROI* pcROI = new ROI();
        pcROI->setLabel(iLabel);
        pcROI->setOpacity(m_dParcellationOpacity);
        pcROI->getSurface()->DeepCopy( pcSoomther->GetOutput());
        double* adROICenter = pcROI->getSurface()->GetCenter();
        QVector3D cNormal(  adROICenter[0]-adBrainCenter[0],
                            adROICenter[1]-adBrainCenter[1],
                            adROICenter[2]-adBrainCenter[2]);
        pcROI->setTextureNormal(cNormal);
        pcROI->getSurface()->Modified();
        this->getParcROIs().push_back(pcROI);
        qDebug() << "ROI #" <<  iLabel << "Parcellation Complete";
    }


    this->update();

    return true;
}

void GMISORenderer::onNewColorScheme(ParcellationColorScheme* pcNewColorScheme)
{
    QColor cColor;
    foreach(ROI* pcROI, this->getParcROIs())
    {
        if( pcNewColorScheme->query(pcROI->getLabel(), &cColor) == false)
        {
            qDebug() << "cannot find color for label" << pcROI->getLabel();
            continue;
        }
        pcROI->setColor(cColor);
    }

}


bool GMISORenderer::onEnabled()
{
    this->addActorsToRenderer();
    /// intensity surface
    foreach(ROI*pcROI, this->getIntensityROIs())
        pcROI->getActor()->SetVisibility(this->getShowIntensity());

    /// label surface
    foreach(ROI* pcROI, this->getParcROIs())
        pcROI->getActor()->SetVisibility(this->getShowParcellatio());
    return true;
}

bool GMISORenderer::onDisabled()
{
    this->removeActorsFromRenderer();
    foreach(ROI*pcROI, this->getIntensityROIs())
        pcROI->getActor()->VisibilityOff();
    foreach(ROI* pcROI, this->getParcROIs())
        pcROI->getActor()->VisibilityOff();
    return true;
}

void GMISORenderer::setISO(double dISO)
{
    this->m_dISO = dISO;
    this->buildIntensitySurface();
}

void GMISORenderer::setHightlightedROI(ROI *pcROI)
{
    clearHightlight();
    if(pcROI != nullptr)
    {
        pcROI->setIsHighlighted(true);
        pcROI->applyOpacity();
        m_pcHighlightedROI = pcROI;
    }
}

void GMISORenderer::clearHightlight()
{
    if(getHighlightedROI() != nullptr)
    {
        this->getHighlightedROI()->setIsHighlighted(false);
        this->getHighlightedROI()->applyOpacity();
        this->getHighlightedROI() = nullptr;
    }
}

bool GMISORenderer::setIntensityOpacity(double dOpacity, int iROILabel)
{
    if(iROILabel < 0)
    {
        qWarning() << "Negative ROI Label Is Not Accepted.." << iROILabel;
        return false;
    }
    if(iROILabel > getIntensityROIs().size())
    {
        qWarning() << "ROI Label Overflow.." << iROILabel;
        return false;
    }

    if(iROILabel == 0)
    {
        foreach(ROI* pcROI, this->getIntensityROIs())
            pcROI->setOpacity(dOpacity);
        m_dIntensityOpacity = dOpacity;
    }
    else
    {
        this->getIntensityROIs()[iROILabel-1]->setOpacity(dOpacity);
    }

    return true;
}

bool GMISORenderer::setParcellationOpacity(double dOpacity, int iROILabel)
{
    if(iROILabel < 0)
    {
        qWarning() << "Negative ROI Label Is Not Accepted.." << iROILabel;
        return false;
    }
    if(iROILabel > getParcROIs().size())
    {
        qWarning() << "ROI Label Overflow.." << iROILabel;
        return false;
    }

    if(iROILabel == 0)
    {
        foreach(ROI* pcROI, this->getParcROIs())
            pcROI->setOpacity(dOpacity);
        m_dParcellationOpacity = dOpacity;
    }
    else
    {
        this->getParcROIs()[iROILabel-1]->setOpacity(dOpacity);
    }

    return true;
}

void GMISORenderer::xSplitIntoROIByLabel()
{
    /// clear
    xDeleteIntensityROIs();


    int iLabelMin = this->m_pcLabelVol->getLabelMin();
    int iLabelMax = this->m_pcLabelVol->getLabelMax();

    double* adBrainCenter = this->getSurface()->GetCenter();
    double* adROICenter = NULL;

    if( this->getMapParcellation2Surface() == false || m_pcLabelVol == NULL || m_pcLabelVol->getFilepath().isEmpty())
    {
        /// no label volume. take the whole gray matter as one ROI
        ROI* pcROI = new ROI();
        pcROI->setLabel(0);
        pcROI->setOpacity(m_dIntensityOpacity);
        pcROI->getSurface()->DeepCopy(this->getSurface());
        QVector3D cNormal;
        pcROI->setTextureNormal(cNormal);
        pcROI->getSurface()->Modified();
        this->getIntensityROIs().push_back(pcROI);
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




        for(int iLabel = iLabelMin; iLabel < iLabelMax+1; iLabel++)
        {

            this->getROIExtractor()->ThresholdBetween(iLabel-0.1, iLabel+0.1);
            this->getROIExtractor()->Modified();
            this->getROIExtractor()->Update();

            this->getGeoFilter()->Update();

            ROI* pcROI = new ROI();
            pcROI->setLabel(iLabel);
            pcROI->setOpacity(m_dIntensityOpacity);
            pcROI->getSurface()->DeepCopy( this->getGeoFilter()->GetOutput());
            adROICenter = pcROI->getSurface()->GetCenter();
            QVector3D cNormal(  adROICenter[0]-adBrainCenter[0],
                                adROICenter[1]-adBrainCenter[1],
                                adROICenter[2]-adBrainCenter[2]);
            pcROI->setTextureNormal(cNormal);
            pcROI->getSurface()->Modified();
            this->getIntensityROIs().push_back(pcROI);
            qDebug() << "ROI #" <<  iLabel << "Parcellation Complete";
        }       
    }


}


void GMISORenderer::xDeleteIntensityROIs()
{
    this->m_pcHighlightedROI = NULL;
    foreach(ROI*pcROI, this->getIntensityROIs())
    {
        pcROI->removeFromRenderer(NULL);
        delete pcROI;
    }
    this->getIntensityROIs().clear();
}

void GMISORenderer::xDeleteLabelROIs()
{
    foreach(ROI*pcROI, this->getParcROIs())
    {
        pcROI->removeFromRenderer(NULL);
        delete pcROI;
    }
    this->getParcROIs().clear();

}


bool GMISORenderer::addActorsToRenderer()
{
    /// remove first
    removeActorsFromRenderer();
    vtkRenderer *pcRenderer = getContext()->pcRenderer;
    foreach(ROI* pcROI, this->getIntensityROIs())
        pcROI->addToRenderer(pcRenderer);
    foreach(ROI* pcROI, this->getParcROIs())
        pcROI->addToRenderer(pcRenderer);
    return true;
}

bool GMISORenderer::removeActorsFromRenderer()
{
    vtkRenderer *pcRenderer = getContext()->pcRenderer;
    foreach(ROI* pcROI, this->getIntensityROIs())
        pcROI->removeFromRenderer(pcRenderer);
    foreach(ROI* pcROI, this->getParcROIs())
        pcROI->removeFromRenderer(pcRenderer);
    return true;
}
