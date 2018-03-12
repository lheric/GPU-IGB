#include "changeinteractorstylecommand.h"
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include "model/modellocator.h"

ChangeInteractorStyleCommand::ChangeInteractorStyleCommand(QObject *parent)
{
    Q_UNUSED(parent)
    setInWorkerThread(false);
}

bool ChangeInteractorStyleCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    Q_UNUSED(rcOutputArg)

    ModelLocator* pcModel = ModelLocator::getInstance();
    QString strStyle = rcInputArg.getParameter("interactor_style").toString();
    vtkRenderWindow* pcRenderWin = pcModel->getRenderWindow();
    if(strStyle == "actor_style")
    {
        pcRenderWin->GetInteractor()->SetInteractorStyle(pcModel->getActorStyle());
    }
    else if(strStyle == "camera_style")
    {
        pcRenderWin->GetInteractor()->SetInteractorStyle(pcModel->getCameraStyle());
    }
    else
    {
        return false;
    }
    return true;
}
