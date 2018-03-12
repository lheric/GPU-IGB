#include "pickcoordinatecommand.h"
#include "model/modellocator.h"
#include <QPointF>
#include <QDebug>
#include <vtkPointData.h>
#include <vtkProp3DCollection.h>
#include <vtkPoints.h>
PickCoordinateCommand::PickCoordinateCommand(QObject *parent)
{
    Q_UNUSED(parent)
    setInWorkerThread(false);
}

bool PickCoordinateCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{

    QPointF cPt = rcInputArg.getParameter("coord").toPointF();
    vtkRenderer* pcRenderer = (vtkRenderer*)(rcInputArg.getParameter("renderer").value<void*>());
    auto pcPicker = ModelLocator::getInstance()->getCellPicker();
    bool bHit = pcPicker->Pick(cPt.x(), cPt.y(), 0, pcRenderer);
    vtkActor* pcPickedActor = pcPicker->GetActor();
    rcOutputArg.setParameter("picker_hit", bHit);
    rcOutputArg.setParameter("picked_actor", QVariant::fromValue<void*>((void*)pcPickedActor));
    return true;
}
