#include "raycasttexturemappingcommand.h"
#include "model/modellocator.h"

RaycastTextureMappingCommand::RaycastTextureMappingCommand(QObject *parent) : GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool RaycastTextureMappingCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();
    QString strClass = rcInputArg.getParameter("class").toString();

    if(strClass == "fMRI")
    {
        FMRITimeSeriesData* pcFMRITimeData = &pcSubject->getFMRITSData();
        if(pcFMRITimeData->getMatrix().isEmpty())
        {
            qWarning() << "fMRI Data Not Loaded..";
            return false;
        }
        pcModel->getRayCastRenderer().renderFMRITexture(pcFMRITimeData);
    }
    else
    {
        MultiDiseaseMeasurement* pcMDMeasures = &pcSubject->getMDMeasurement();
        if(pcMDMeasures->getMatrix().isEmpty())
        {
            qWarning() << "Multi-disease Data Not Loaded..";
            return false;
        }
        pcModel->getRayCastRenderer().renderMDTexture(pcMDMeasures);
    }
    return true;
}
