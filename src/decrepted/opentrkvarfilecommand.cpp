#include "opentrkvarfilecommand.h"
#include "model/modellocator.h"

OpenTrkVarFileCommand::OpenTrkVarFileCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool OpenTrkVarFileCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();
    FiberClassification *pcROIConnection = &pcSubject->getFiberClassification();

    QVariant vValue;
    vValue = rcInputArg.getParameter("filename");
    QString strFilename = vValue.toString();

    pcROIConnection->openVarFile(strFilename);
//    pcSubject->getFiber().update();

    rcOutputArg.setParameter("trk_var_file", strFilename);
    rcOutputArg.setParameter("trk_var_min", pcROIConnection->getMinVar());
    rcOutputArg.setParameter("trk_var_max", pcROIConnection->getMaxVar());
    return true;
}


