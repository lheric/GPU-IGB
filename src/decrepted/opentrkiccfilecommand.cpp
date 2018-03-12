#include "opentrkiccfilecommand.h"
#include "model/modellocator.h"

OpenTrkICCFileCommand::OpenTrkICCFileCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool OpenTrkICCFileCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();
    FiberClassification *pcROIConnection = &pcSubject->getFiberClassification();


    QVariant vValue;
    vValue = rcInputArg.getParameter("filename");
    QString strFilename = vValue.toString();

    pcROIConnection->openICCFile(strFilename);
//    pcSubject->getFiber().update();

    rcOutputArg.setParameter("trk_icc_file", strFilename);
    rcOutputArg.setParameter("trk_icc_min", pcROIConnection->getMinICC());
    rcOutputArg.setParameter("trk_icc_max", pcROIConnection->getMaxICC());

    return true;
}
