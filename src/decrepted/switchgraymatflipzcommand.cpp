#include "switchgraymatflipzcommand.h"
#include "model/modellocator.h"

SwitchGrayMatFlipZCommand::SwitchGrayMatFlipZCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool SwitchGrayMatFlipZCommand::execute( GitlCommandParameter& rcInputArg, GitlCommandParameter& rcOutputArg )
{
    bool bSwitch = rcInputArg.getParameter("status").toBool();

    ModelLocator* pcModel = ModelLocator::getInstance();
    pcModel->getSubject().getGrayMat().setFilp(bSwitch);
//    pcModel->getSubject().getGrayMat().addToRenderer(pcModel->getRenderer());

    pcModel->getRenderWindow()->Render();
    return true;
}
