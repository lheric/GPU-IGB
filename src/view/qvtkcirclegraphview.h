#ifndef QVTKCIRCLEGRAPHVIEW_H
#define QVTKCIRCLEGRAPHVIEW_H

#include "gitldef.h"
#include <QVTKWidget.h>
#include <vtkSplineGraphEdges.h>
#include <vtkGraphHierarchicalBundleEdges.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkGraphLayoutView.h>
#include <vtkTree.h>
#include <vtkPolyData.h>
#include <vtkTexturedActor2D.h>
#include <vtkPointSetToLabelHierarchy.h>
#include <vtkQtLabelRenderStrategy.h>
#include <vtkLabelPlacementMapper.h>
#include <vtkInteractorStyle.h>
#include <QDebug>

/// this class is a trick to fix the bug that we can not switch to Qt Rendering for graph label
class MyGraphLayoutView: public vtkGraphLayoutView
{
public:
    vtkTypeMacro(MyGraphLayoutView, vtkGraphLayoutView);

    static MyGraphLayoutView *New()
    {
        return new MyGraphLayoutView();
    }

    vtkRenderer* GetLabelRenderer()
    {
        return this->LabelRenderer;
    }


protected:
  MyGraphLayoutView()
  {
      vtkQtLabelRenderStrategy* pcRenderStrategy = vtkQtLabelRenderStrategy::New();
      //pcRenderStrategy->GetRenderer()->GetRenderWindow()->SetMultiSamples(true);
      this->LabelPlacementMapper->SetRenderStrategy(pcRenderStrategy);
      this->LabelRenderer->InteractiveOn();
      this->LabelActor->SetPickable(true);

  }
};



class QVTKCircleGraphView : public QVTKWidget
{
    Q_OBJECT
public:
    QVTKCircleGraphView(QWidget *parent);
    ~QVTKCircleGraphView();

    void init();
    void setGraph(vtkMutableDirectedGraph *pcGraph);

    /// circle graph
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<vtkMutableDirectedGraph>, pcGraph, getGraph)
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<vtkTree>, pcTree, getTree)
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<vtkGraphHierarchicalBundleEdges>, pcBundlingAlg, getBundlingAlg)
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<vtkSplineGraphEdges>, pcSoomther, getSoomther)

    /// circle graph label
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<vtkPolyData>, pcLabel, getLabel)
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<vtkPointSetToLabelHierarchy>, pcLabelFilter, getLabelFilter)

    /// view
    ADD_CLASS_FIELD_NOSETTER(vtkSmartPointer<MyGraphLayoutView>, pcGraphView, getGraphView)


private slots:
    void onQVTKWidgetMouseEvt(QMouseEvent *pcEvt);
};

#endif // QVTKCIRCLEGRAPHVIEW_H
