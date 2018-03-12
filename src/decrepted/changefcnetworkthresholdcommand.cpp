#include "changefcnetworkthresholdcommand.h"
#include "model/modellocator.h"

ChangeFCNetworkThresholdCommand::ChangeFCNetworkThresholdCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool ChangeFCNetworkThresholdCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();
    FunctionalConnectivityNetwork* pcNetwork = &pcSubject->getFCNetwork();

    if(!rcInputArg.hasParameter("threshold"))
        return false;

    double dThreshold = rcInputArg.getParameter("threshold").toDouble();

    pcNetwork->setThreshold(dThreshold);

    // schedule rerender
    pcModel->getRenderWindow()->Render();

    return true;
}
