#include "fiberrenderer.h"
#include "../src/model/subject.h"
#include <QDebug>
#include <QTableWidget>
#include <QtMath>
#include "vtkFloatArray.h"

FiberRenderer::FiberRenderer(QObject *parent)
{
    Q_UNUSED(parent)
    setName("Fiber Renderer");
    m_pcConfigPanel = new FiberConfigPanel(this);

}

FiberRenderer::~FiberRenderer()
{
    delete m_pcConfigPanel;
}

bool FiberRenderer::init()
{
    /// create
    this->setFiberPolyData(vtkPolyData::New());
    this->setFiberConnFilter(vtkFiberConnectionFilter::New());
    this->setColorFilter(vtkLineColorFilter::New());
    this->setFiberMapper(vtkPolyDataMapper::New());
    this->setFiberActors(vtkActor::New());

    /// connect
    this->getFiberConnFilter()->SetInputData(this->getFiberPolyData());
    this->getColorFilter()->SetInputConnection(this->getFiberConnFilter()->GetOutputPort());
    this->getFiberMapper()->SetInputConnection(this->getColorFilter()->GetOutputPort());
    this->getFiberActors()->SetMapper(this->getFiberMapper());

    /// set parameters
    this->getFiberActors()->GetProperty()->SetLineWidth(1);

    /// add fiber actor to renderer
    getContext()->pcRenderer->AddActor(this->getFiberActors());

    /// add property panel to mainwin
    m_pcConfigPanel->attach(getContext()->pcEventBus);
    getContext()->pcPluginPropTabs->addTab(m_pcConfigPanel, this->getName());

    /// listen to changes about fiber and parcellation
    /// rebuild the fiber connection table when necessary
    connect( &getContext()->pcSubject->getFiber(), SIGNAL(updated()), this, SLOT(xRebuildFiberConnTable()) );
    connect( &getContext()->pcSubject->getGrayMat().getLabelVol(), SIGNAL(updated()), this, SLOT(xRebuildFiberConnTable()));


    /// enable this filter
    setEnable(true);


    return true;
}

bool FiberRenderer::uninit()
{
    /// remove fiber actor from renderer
    getContext()->pcRenderer->RemoveActor(this->getFiberActors());

    /// remove property panel from mainwin
    getContext()->pcPluginPropTabs->removeTab(getContext()->pcPluginPropTabs->indexOf(m_pcConfigPanel));

    return true;
}

bool FiberRenderer::redraw()
{
    qDebug() << "redrawing";
    if(getContext()->pcSubject == NULL)
    {
        qWarning() << "no subject to draw with";
        return false;
    }

    vtkSmartPointer<vtkPolyData> pcFiber = getContext()->pcSubject->getFiber().getFiberPolyData();
    this->getFiberConnFilter()->SetInputData(pcFiber);


    return true;
}

bool FiberRenderer::onEnabled()
{
    this->getFiberActors()->VisibilityOn();
    this->getContext()->pcRenderer->GetRenderWindow()->Render();
    return true;
}

bool FiberRenderer::onDisabled()
{
    this->getFiberActors()->VisibilityOff();
    this->getContext()->pcRenderer->GetRenderWindow()->Render();
    return true;
}

void FiberRenderer::xRebuildFiberConnTable()
{
    /// parcellation
    vtkSmartPointer<vtkImageData> pcPar = getContext()->pcSubject->getGrayMat().getLabelVol().getUnclippedVolume();
    int* aiDim = pcPar->GetDimensions();

    /// fiber
    vtkSmartPointer<vtkPolyData> pcFiber = getContext()->pcSubject->getFiber().getFiberPolyData();

    /// voxel resolution
    vtkFloatArray* pcVoxelSize = (vtkFloatArray*)(pcFiber->GetFieldData()->GetArray("voxel_size"));
    double* voxel_size = NULL;
    if(pcVoxelSize != NULL)
        voxel_size = pcVoxelSize->GetTuple3(0);

    ///
    vtkSmartPointer<vtkPoints> pcPoints = pcFiber->GetPoints();
    vtkSmartPointer<vtkCellArray> pcLines = pcFiber->GetLines();
    int iTrkTotal = pcLines->GetNumberOfCells();
    qDebug() << "iTrkTotal" << iTrkTotal;
    pcLines->InitTraversal();
    this->getFiberConnFilter()->GetFiberConnections()->init();
    ConnectedROIs cConnection;
    for(int i = 0; i < iTrkTotal; ++i)
    {
        /// for each line
        vtkSmartPointer<vtkIdList> pcLine = vtkSmartPointer<vtkIdList>::New();
        pcLines->GetNextCell(pcLine);
        int iTotalPnts = pcLine->GetNumberOfIds();
        if( iTotalPnts < 2 )
            continue;
        double* pdStartPoint; int piStartPoint[3];
        double* pdEndPoint; int piEndPoint[3];
        /// get start and end coords
        pdStartPoint = pcPoints->GetPoint(pcLine->GetId(0));
        pdEndPoint = pcPoints->GetPoint(pcLine->GetId(iTotalPnts-1));

        /// convert into voxel domain
        pdStartPoint[0] = pdStartPoint[0]/voxel_size[0]; piStartPoint[0] = qFloor(pdStartPoint[0]);
        pdStartPoint[1] = pdStartPoint[1]/voxel_size[1]; piStartPoint[1] = qFloor(pdStartPoint[1]);
        pdStartPoint[2] = pdStartPoint[2]/voxel_size[2]; piStartPoint[2] = qFloor(pdStartPoint[2]);
        pdEndPoint[0] = pdEndPoint[0]/voxel_size[0]; piEndPoint[0] = qFloor(pdEndPoint[0]);
        pdEndPoint[1] = pdEndPoint[1]/voxel_size[1]; piEndPoint[1] = qFloor(pdEndPoint[1]);
        pdEndPoint[2] = pdEndPoint[2]/voxel_size[2]; piEndPoint[2] = qFloor(pdEndPoint[2]);

        double dStartLabel = 0;
        double dEndLabel = 0;

        if( piStartPoint[0] >= 0 && piStartPoint[1] >= 0 && piStartPoint[2] >= 0 &&
            piStartPoint[0] < aiDim[0] && piStartPoint[1] < aiDim[1] && piStartPoint[2] < aiDim[2])
        {
            dStartLabel = pcPar->GetScalarComponentAsDouble( piStartPoint[0],
                                                             piStartPoint[1],
                                                             piStartPoint[2],
                                                             0 );
        }

        if( piEndPoint[0] >= 0 && piEndPoint[1] >= 0 && piEndPoint[2] >= 0 &&
            piEndPoint[0] < aiDim[0] && piEndPoint[1] < aiDim[1] && piEndPoint[2] < aiDim[2])
        {
            dEndLabel = pcPar->GetScalarComponentAsDouble( piEndPoint[0],
                                                           piEndPoint[1],
                                                           piEndPoint[2],
                                                           0 );
        }

        cConnection.setStartROI(dStartLabel);
        cConnection.setEndROI(dEndLabel);
        this->getFiberConnFilter()->GetFiberConnections()
                ->getFiberConnectionInfo().push_back(cConnection);

    }
    /// update pipeline
    this->getFiberConnFilter()->Modified();
    this->getFiberConnFilter()->Update();
}
