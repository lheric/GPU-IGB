#include "openfunctionalconnectivitycommand.h"
#include "model/modellocator.h"

OpenFunctionalConnectivityCommand::OpenFunctionalConnectivityCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool OpenFunctionalConnectivityCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();
    FunctionalConnectivityData* pcFCData = &pcSubject->getFCData();

    QVariant vValue;
    vValue = rcInputArg.getParameter("filename");
    QString strFilename = vValue.toString();

    pcFCData->readFromCSV(strFilename);
    pcSubject->getFCNetwork().addActorsToRenderer(pcModel->getRenderer());


    rcOutputArg.setParameter("GM_network_file", strFilename);
    rcOutputArg.setParameter("GM_network_min", pcFCData->getMatrix().getMin());
    rcOutputArg.setParameter("GM_network_max", pcFCData->getMatrix().getMax());

    pcModel->getRenderWindow()->Render();

    return true;
}
