#include "changerenderingprefcommand.h"
#include "model/modellocator.h"

ChangeRenderingPrefCommand::ChangeRenderingPrefCommand(QObject *parent) : GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool ChangeRenderingPrefCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    Q_UNUSED(rcOutputArg)

    ModelLocator* pcModel = ModelLocator::getInstance();
    if(rcInputArg.hasParameter("two_side_lighting"))
    {
        bool bSwitch = rcInputArg.getParameter("two_side_lighting").toBool();
        pcModel->getRenderer()->SetTwoSidedLighting(bSwitch);
        qDebug() << "Two-side Lighting Is Now" << (bSwitch?"ON":"OFF");
    }
    if(rcInputArg.hasParameter("deep_peeling"))
    {
        bool bSwitch = rcInputArg.getParameter("deep_peeling").toBool();
        pcModel->getRenderer()->SetUseDepthPeeling(bSwitch);
        qDebug() << "Depth Peeling Is Now" << (bSwitch?"ON":"OFF");
    }

    // schedule update [call rerender command will cause deadlock]
    pcModel->getRenderWindow()->Render();
    return true;
}
