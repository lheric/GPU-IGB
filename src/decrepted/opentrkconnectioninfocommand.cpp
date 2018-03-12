#include "opentrkconnectioninfocommand.h"
#include "model/modellocator.h"
#include "model/fiberroiconnection.h"

OpenTrkConnectionInfoCommand::OpenTrkConnectionInfoCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool OpenTrkConnectionInfoCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    Q_UNUSED(rcInputArg)
    Q_UNUSED(rcOutputArg)

    ///TODO
//    ModelLocator* pcModel = ModelLocator::getInstance();
//    Subject *pcSubject = &pcModel->getSubject();
//    FiberROIConnection *pcROIConnection = &pcSubject->getFiberConnection();

//    QVariant vValue;
//    vValue = rcInputArg.getParameter("filename");
//    QString strFilename = vValue.toString();

//    pcROIConnection->openConnectionInfo(strFilename);
//    pcSubject->getFiber().getFiberConnFilter()->SetFiberConnections(pcROIConnection);
//    pcSubject->getFiber().getFiberConnFilter()->Modified();

//    rcOutputArg.setParameter("trk_connection_file", strFilename);

    return true;
}


