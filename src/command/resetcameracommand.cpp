#include "resetcameracommand.h"
#include "model/modellocator.h"

ResetCameraCommand::ResetCameraCommand(QObject *parent) : GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool ResetCameraCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    Q_UNUSED(rcInputArg)
    Q_UNUSED(rcOutputArg)

    ModelLocator::getInstance()->getRenderer()->ResetCamera();
    return true;
}


