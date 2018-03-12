#include "changemdnetworkthresholdcommand.h"
#include "model/modellocator.h"

ChangeMDNetworkThresholdCommand::ChangeMDNetworkThresholdCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool ChangeMDNetworkThresholdCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();
    MultiDiseaseNetwork* pcNetwork = &pcSubject->getMDNetwork();

    if(!rcInputArg.hasParameter("threshold"))
        return false;

    double dThreshold = rcInputArg.getParameter("threshold").toDouble();

    pcNetwork->setThreshold(dThreshold);

    // schedule rerender
    pcModel->getRenderWindow()->Render();

    return true;
}

