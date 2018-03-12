#include "opengraymatterimagecommand.h"
#include "vtkNIFTIImageReader.h"
#include "model/modellocator.h"
#include "gitlivkcmdevt.h"

OpenGrayMatterImageCommand::OpenGrayMatterImageCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool OpenGrayMatterImageCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    QString strFilename = rcInputArg.getParameter("filename").toString();
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();
    pcSubject->getGrayMat().openIntensityVolume(strFilename);
    pcModel->getPluginLoader().redraw();
    rcOutputArg.setParameter("GM_intensity_file", strFilename);
    return true;
}
