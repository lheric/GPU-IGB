#include "changesampleratiocommand.h"
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

ChangeSampleRatioCommand::ChangeSampleRatioCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool ChangeSampleRatioCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    /// TODO
//    /// busy dialog
//    GitlUpdateUIEvt evt;
//    evt.setParameter("busy_dialog_visible", true);
//    evt.dispatch();
//    SCOPE_EXIT( GitlUpdateUIEvt evt;
//        evt.setParameter("busy_dialog_visible", false);
//        evt.dispatch(); );

//    ModelLocator* pcModel = ModelLocator::getInstance();

//    if(!rcInputArg.hasParameter("sample_ratio"))
//        return false;

//    double dRatio = rcInputArg.getParameter("sample_ratio").toDouble();

//    /// random sampling
//    Subject *pcSubject = &pcModel->getSubject();
//    pcSubject->getFiber().changeSampleRatio(dRatio);
//    pcSubject->getFiber().update();


//    // schedule rerender
//    pcModel->getRenderWindow()->Render();



    return true;
}
