#include "openmultidiseaseconnectivitycommand.h"
#include "model/modellocator.h"

OpenMultiDiseaseConnectivityCommand::OpenMultiDiseaseConnectivityCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool OpenMultiDiseaseConnectivityCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();
    MultiDiseaseConnectivity* pcMDConnectivity = &pcSubject->getMDConnectivity();

    QVariant vValue;
    vValue = rcInputArg.getParameter("filename");
    QString strFilename = vValue.toString();

    pcMDConnectivity->readFromCSV(strFilename);
    pcSubject->getMDNetwork().addActorsToRenderer(pcModel->getRenderer());

    rcOutputArg.setParameter("MD_network_file", strFilename);
    rcOutputArg.setParameter("MD_network_min", 0.0);
    rcOutputArg.setParameter("MD_network_max", 1.0);

    pcModel->getRenderWindow()->Render();
    return true;
}
