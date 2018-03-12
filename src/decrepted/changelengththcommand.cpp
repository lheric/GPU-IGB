#include "changelengththcommand.h"
#include <QTime>
#include <vector>
#include <QThread>
#include "vtkPolyLine.h"
#include "model/modellocator.h"
#include "algorithm/directionalcolor.h"
#include "algorithm/trklengthfilter.h"
#include "algorithm/startendposfilter.h"
#include "gitlivkcmdevt.h"
#include "gitlupdateuievt.h"

ChangeLengthThCommand::ChangeLengthThCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool ChangeLengthThCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    Q_UNUSED(rcInputArg)
    Q_UNUSED(rcOutputArg)

    //TODO
//    /// busy dialog
//    GitlUpdateUIEvt evt;
//    evt.setParameter("busy_dialog_visible", true);
//    evt.dispatch();
//    SCOPE_EXIT( GitlUpdateUIEvt evt;
//        evt.setParameter("busy_dialog_visible", false);
//        evt.dispatch(); );


//    if(!rcInputArg.hasParameter("threshold"))
//        return false;

//    double dShortTrkTh = rcInputArg.getParameter("threshold").toDouble();

//    ModelLocator* pcModel = ModelLocator::getInstance();
//    Subject *pcSubject = &pcModel->getSubject();
//    pcSubject->getFiber().changeLengthThreshold(dShortTrkTh);
//    pcSubject->getFiber().update();

    // schedule rerender
//    pcModel->getRenderWindow()->Render();
    return true;
}
