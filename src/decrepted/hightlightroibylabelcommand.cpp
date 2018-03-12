#include "hightlightroibylabelcommand.h"
#include "model/modellocator.h"
HightLightROIByLabelCommand::HightLightROIByLabelCommand(QObject *parent) : GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool HightLightROIByLabelCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    Q_UNUSED(rcOutputArg)

    int iROILabel = rcInputArg.getParameter("roi_label").toInt();
    ModelLocator* pcModel = ModelLocator::getInstance();
    pcModel->getSubject().getGrayMat().hightlightROIByLabel(iROILabel);

    pcModel->getRenderWindow()->Render();
    return true;
}


