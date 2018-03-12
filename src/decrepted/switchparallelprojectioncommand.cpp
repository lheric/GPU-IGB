#include "switchparallelprojectioncommand.h"
#include "model/modellocator.h"
#include <vtkCamera.h>

SwitchParallelProjectionCommand::SwitchParallelProjectionCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool SwitchParallelProjectionCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    bool bEnabled = rcInputArg.getParameter("enabled").toBool();

    pcModel->getRenderer()->GetActiveCamera()->SetParallelProjection(bEnabled);
    pcModel->getRenderWindow()->Render();

    return true;
}
