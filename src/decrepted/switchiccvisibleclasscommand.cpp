#include "switchiccvisibleclasscommand.h"
#include "model/modellocator.h"
#include "model/fiberclassification.h"

SwitchICCVisibleClassCommand::SwitchICCVisibleClassCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool SwitchICCVisibleClassCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    ModelLocator* pcModel = ModelLocator::getInstance();
    FiberClassification* pcFiberClassification = &pcModel->getSubject().getFiberClassification();


    if(rcInputArg.hasParameter("show_unknown_enabled"))
    {
        bool bEnabled = rcInputArg.getParameter("show_unknown_enabled").toBool();
        pcFiberClassification->setShowUnknown(bEnabled);    ///TODO should be in classification class?
    }
    if(rcInputArg.hasParameter("show_reproducible_enabled"))
    {
        bool bEnabled = rcInputArg.getParameter("show_reproducible_enabled").toBool();
        pcFiberClassification->setShowReproducible(bEnabled);
    }
    if(rcInputArg.hasParameter("show_backbone_enabled"))
    {
        bool bEnabled = rcInputArg.getParameter("show_backbone_enabled").toBool();
        pcFiberClassification->setShowBackbone(bEnabled);
    }
    if(rcInputArg.hasParameter("show_noise_enabled"))
    {
        bool bEnabled = rcInputArg.getParameter("show_noise_enabled").toBool();
        pcFiberClassification->setShowNoise(bEnabled);
    }
    pcFiberClassification->update();
    pcModel->getRenderWindow()->Render();
    return true;
}


