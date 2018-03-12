#include "raycastopenclrendercommand.h"
#include "model/modellocator.h"

RayCastOpenCLRenderCommand::RayCastOpenCLRenderCommand(QObject *parent) : GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool RayCastOpenCLRenderCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    pcModel->getRayCastOpenCLRenderer().render();
    QImage cRenderedImg = pcModel->getRayCastOpenCLRenderer().getRenderedImage();

    rcOutputArg.setParameter("raycasting_opencl_rendered", cRenderedImg);
    return true;

}


