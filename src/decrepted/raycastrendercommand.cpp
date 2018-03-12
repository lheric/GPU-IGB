#include "raycastrendercommand.h"
#include "model/modellocator.h"

RayCastRenderCommand::RayCastRenderCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool RayCastRenderCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    pcModel->getRayCastRenderer().render();
    return true;
}

