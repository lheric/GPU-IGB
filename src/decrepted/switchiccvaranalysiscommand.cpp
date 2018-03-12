#include "switchiccvaranalysiscommand.h"
#include "model/modellocator.h"

SwitchICCVarAnalysisCommand::SwitchICCVarAnalysisCommand(QObject *parent) : GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool SwitchICCVarAnalysisCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ///TODO
//    ModelLocator* pcModel = ModelLocator::getInstance();
//    bool enabled = rcInputArg.getParameter("enabled").toBool();
//    pcModel->getSubject().getFiber().setEnableMask(enabled);

//    pcModel->getRenderWindow()->Render();
    return true;
}

