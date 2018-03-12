#include "changeiccvarthresholdcommand.h"
#include "model/modellocator.h"

ChangeICCVarThresholdCommand::ChangeICCVarThresholdCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool ChangeICCVarThresholdCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    /// TODO
//    ModelLocator* pcModel = ModelLocator::getInstance();
//    Subject *pcSubject = &pcModel->getSubject();

//    if(rcInputArg.hasParameter("icc_threshold"))
//    {
//        double dICCThres = rcInputArg.getParameter("icc_threshold").toDouble();
//        qDebug() << "new icc threshold" << dICCThres;
//        pcSubject->getFiberClassification().setICCThreshold(dICCThres);
//    }

//    if(rcInputArg.hasParameter("var_threshold"))
//    {
//        double dVarThres = rcInputArg.getParameter("var_threshold").toDouble();
//        qDebug() << "new variance threshold" << dVarThres;
//        pcSubject->getFiberClassification().setVarThreshold(dVarThres);
//    }

//    if(rcInputArg.hasParameter("count_threshold"))
//    {
//        int dCountThres = rcInputArg.getParameter("count_threshold").toInt();
//        qDebug() << "new variance threshold" << dCountThres;
//        pcSubject->getFiberClassification().setCountThreshold(dCountThres);
//    }

//    pcSubject->getFiber().update();

//    // schedule rerender
//    pcModel->getRenderWindow()->Render();

    return true;
}

