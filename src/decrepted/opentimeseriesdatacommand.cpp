#include "opentimeseriesdatacommand.h"
#include "model/modellocator.h"


OpenTimeSeriesDataCommand::OpenTimeSeriesDataCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}


bool OpenTimeSeriesDataCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    QString strFilename = rcInputArg.getParameter("filename").toString();
    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();
    FMRITimeSeriesData* pcFMRITimeData = &pcSubject->getFMRITSData();


    pcFMRITimeData->readFromCSV(strFilename);

    int iTimePointNum = pcFMRITimeData->getMatrix().getNumCol();
    rcOutputArg.setParameter("time_series_file", strFilename);
    rcOutputArg.setParameter("time_point_num", iTimePointNum);
    return true;
}
