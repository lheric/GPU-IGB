#include "openmultidiseasedatacommand.h"
#include "model/modellocator.h"

OpenMultiDiseaseMeasurementCommand::OpenMultiDiseaseMeasurementCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool OpenMultiDiseaseMeasurementCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    QString strFilename = rcInputArg.getParameter("filename").toString();
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();
    MultiDiseaseMeasurement* pcMultiDiseaseData = &pcSubject->getMDMeasurement();

    pcMultiDiseaseData->openMutiDiseasMeasurement(strFilename);

    int iDiseaseNum = pcMultiDiseaseData->getNumDisease();
    pcModel->getRenderWindow()->Render();

    rcOutputArg.setParameter("muti_disease_file", strFilename);
    rcOutputArg.setParameter("disease_num", iDiseaseNum);
    return true;
}


