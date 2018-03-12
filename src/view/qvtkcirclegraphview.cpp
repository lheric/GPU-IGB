#include "qvtkcirclegraphview.h"
#include <vtkDataSetAttributes.h>
#include <vtkIntArray.h>
#include <qmath.h>
#include <vtkSpline.h>
#include <vtkNew.h>
#include <vtkRandomGraphSource.h>
#include <vtkIdTypeArray.h>
#include <vtkStringArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <QEvent>
#include <QMouseEvent>
#include <vtkRendererCollection.h>
#include "gitlivkcmdevt.h"
#include "model/rom.h"


QVTKCircleGraphView::QVTKCircleGraphView(QWidget *parent) :
    QVTKWidget(parent)
{
    init();
    connect(this, SIGNAL(mouseEvent(QMouseEvent*)), this, SLOT(onQVTKWidgetMouseEvt(QMouseEvent*)));
}

QVTKCircleGraphView::~QVTKCircleGraphView()
{

}

void QVTKCircleGraphView::init()
{
    m_pcGraph = vtkSmartPointer<vtkMutableDirectedGraph>::New();
    m_pcTree = vtkSmartPointer<vtkTree>::New();
    m_pcBundlingAlg = vtkSmartPointer<vtkGraphHierarchicalBundleEdges>::New();
    m_pcSoomther = vtkSmartPointer<vtkSplineGraphEdges>::New();
    m_pcGraphView = vtkSmartPointer<MyGraphLayoutView>::New();
    m_pcLabel = vtkSmartPointer<vtkPolyData>::New();
    m_pcLabelFilter = vtkSmartPointer<vtkPointSetToLabelHierarchy>::New();

    m_pcBundlingAlg->SetInputDataObject(0, m_pcGraph);
    m_pcBundlingAlg->SetInputDataObject(1, m_pcTree);
    m_pcBundlingAlg->SetDirectMapping(true);
    m_pcBundlingAlg->SetBundlingStrength(0.3);
    m_pcBundlingAlg->Update();
    m_pcLabelFilter->SetInputData(vtkSmartPointer<vtkPolyData>::New());

    m_pcSoomther->SetInputConnection(m_pcBundlingAlg->GetOutputPort());
    m_pcSoomther->SetNumberOfSubdivisions(35);
    m_pcSoomther->Update();

    m_pcGraphView->AddRepresentationFromInputConnection(m_pcSoomther->GetOutputPort());
    m_pcGraphView->AddLabels(this->m_pcLabelFilter->GetOutputPort());
    m_pcGraphView->SetLayoutStrategyToPassThrough();
    m_pcGraphView->SetEdgeLayoutStrategyToPassThrough();

    m_pcGraphView->SetInteractor(this->GetInteractor());

    this->SetRenderWindow(m_pcGraphView->GetRenderWindow());
    m_pcGraphView->ResetCamera();

//    m_pcGraphView->AddRepresentation(vtkBalloonRepresentation);

    setGraph(nullptr);
}

void QVTKCircleGraphView::setGraph(vtkMutableDirectedGraph *pcGraph)
{
    Q_UNUSED(pcGraph)

    auto source = vtkSmartPointer<vtkRandomGraphSource>::New();
    source->SetNumberOfVertices(83);
    source->SetStartWithTree(true);
    source->SetDirected(true);
    source->SetEdgeProbability(0.8);
    source->Update();

    vtkGraph *randomGraph = source->GetOutput();






    double centerX = 0;
    double centerY = 0;
    double radius = 50;

//    m_pcGraph->DeepCopy(pcGraph);
    m_pcGraph->CheckedDeepCopy(randomGraph);
    m_pcTree->Initialize();

    auto pcGraph4Tree = vtkSmartPointer<vtkMutableDirectedGraph>::New();
    auto pcIDArray = vtkSmartPointer<vtkIntArray>::New();

    auto pcPnts = vtkSmartPointer<vtkPoints>::New();
    auto iNumofVertices = m_pcGraph->GetNumberOfVertices();
    auto iCurLayer = m_pcGraph->GetNumberOfVertices();
    auto iGroupSize = 4;
//    while( iCurLayer > 0 )
//    {
//        iNumofVertices += iCurLayer;
//    }
    QVector<vtkIdType> nodes;
    auto pcLabelPoints = vtkSmartPointer<vtkPoints>::New();
    auto labels = vtkSmartPointer<vtkStringArray>::New(); labels->SetName("label");
    auto orientation = vtkSmartPointer<vtkDoubleArray>::New(); orientation->SetName("Orientation");
    auto fontsize = vtkSmartPointer<vtkIntArray>::New(); fontsize->SetName("size");
    for(vtkIdType i = 0; i < iNumofVertices; i++)
    {
        double angle = (2*M_PI*i)/iNumofVertices;
        nodes.push_back(pcGraph4Tree->AddVertex());
        pcIDArray->InsertNextValue(i+1);
        pcPnts->InsertNextPoint(centerX+radius*qCos(angle),
                                centerY+radius*qSin(angle),
                                0.0);
        pcLabelPoints->InsertNextPoint(centerX+radius*1.1*qCos(angle),
                                       centerY+radius*1.1*qSin(angle),
                                       0.0);
        //labels->InsertNextValue(QString("1sdfasdfasf%1").arg(double(360.0*i)/iNumofVertices).toStdString().c_str()); //label name
        //labels->InsertNextValue(QString("#1111%1_2").arg(g_astrROINameLUT125[i]).toStdString().c_str()); //label name
        labels->InsertNextValue(QString("#%1 %2").arg(i).arg(g_astrROINameLUT33[i]).toStdString().c_str()); //label name

        //labels->InsertNextValue("asdfasdfasdf"); //label name
        orientation->InsertNextValue((360.0*i)/iNumofVertices);

        fontsize->InsertNextValue(20);


    }
    m_pcLabel->SetPoints(pcLabelPoints);
    m_pcLabel->GetPointData()->AddArray(labels);
    m_pcLabel->GetPointData()->AddArray(orientation);
    m_pcLabel->GetPointData()->AddArray(fontsize);

    m_pcLabelFilter->SetInputData(m_pcLabel);
    m_pcLabelFilter->SetOrientationArrayName("Orientation");
    m_pcLabelFilter->SetLabelArrayName("label");
    m_pcLabelFilter->GetTextProperty()->SetFontSize(16);
    m_pcLabelFilter->GetTextProperty()->SetBold(false);


    vtkIdType rootid = pcGraph4Tree->AddVertex(); //root
    pcIDArray->InsertNextValue(0);
    for(int i = 0; i < nodes.size(); i++)
    {
        pcGraph4Tree->AddEdge(rootid,nodes[i]);
    }

    m_pcTree->CheckedShallowCopy(pcGraph4Tree);
    m_pcTree->GetVertexData()->SetPedigreeIds(pcIDArray);
    m_pcTree->SetPoints(pcPnts);


    m_pcGraphView->ResetCamera();
}

void QVTKCircleGraphView::onQVTKWidgetMouseEvt(QMouseEvent *pcEvt)
{
    static QPointF cPt;
    static bool bMouseDown = false;
    QEvent::Type eType = pcEvt->type();
    if(eType == QEvent::MouseButtonPress )
    {
        bMouseDown = true;
        cPt = pcEvt->windowPos();

        int iWinHeight = this->height();
        cPt.setY(iWinHeight - cPt.y());
        vtkRenderer* pcRenderer = this->m_pcGraphView->GetLabelRenderer();

        qDebug() << "clicked" << cPt;
        GitlIvkCmdEvt cEvt("pick_coord");
        cEvt.setParameter("coord", cPt);
        cEvt.setParameter("renderer", QVariant::fromValue<void*>(pcRenderer));
        cEvt.dispatch();
        qDebug() << pcRenderer;
    }
    else if(eType == QEvent::MouseButtonRelease)
    {
        bMouseDown = false;
    }
    else if(eType == QEvent::MouseMove)
    {


        if( (pcEvt->modifiers()&Qt::AltModifier) && bMouseDown)
        {
            static GitlIvkCmdEvt cEvt("rotate_projection_plane");
            cEvt.dispatch();
        }
    }
}
