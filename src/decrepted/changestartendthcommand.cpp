#include "changestartendthcommand.h"
#include "model/modellocator.h"
#include "algorithm/startendposfilter.h"
#include "gitlivkcmdevt.h"
#include "gitlupdateuievt.h"
#include <vector>
ChangeStartEndThCommand::ChangeStartEndThCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool ChangeStartEndThCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
//    /// busy dialog
//    GitlUpdateUIEvt evt;
//    evt.setParameter("busy_dialog_visible", true);
//    evt.dispatch();
//    SCOPE_EXIT( GitlUpdateUIEvt evt;
//        evt.setParameter("busy_dialog_visible", false);
//        evt.dispatch(); );


//    if(!rcInputArg.hasParameter("threshold"))
//        return false;

//    double dStEnTh = rcInputArg.getParameter("threshold").toDouble();

//    ModelLocator* pcModel = ModelLocator::getInstance();
//    Subject *pcSubject = &pcModel->getSubject();
//    pcSubject->getFiber().changeStrEndThreshold(dStEnTh);
//    pcSubject->getFiber().update();

//    // schedule rerender
//    pcModel->getRenderWindow()->Render();

    return true;
}
