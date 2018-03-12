#include "opentrkcountfilecommand.h"
#include "model/modellocator.h"

OpenTrkCountFileCommand::OpenTrkCountFileCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool OpenTrkCountFileCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();
    FiberClassification *pcROIConnection = &pcSubject->getFiberClassification();

    QVariant vValue;
    vValue = rcInputArg.getParameter("filename");
    QString strFilename = vValue.toString();

    pcROIConnection->openCountFile(strFilename);
//    pcSubject->getFiber().update();

    rcOutputArg.setParameter("trk_count_file", strFilename);
    rcOutputArg.setParameter("trk_count_min", pcROIConnection->getMinCount());
    rcOutputArg.setParameter("trk_count_max", pcROIConnection->getMaxCount());


    return true;
}
