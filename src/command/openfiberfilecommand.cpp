#include "openfiberfilecommand.h"
#include "model/modellocator.h"
#include "gitlupdateuievt.h"
#include "gitlivkcmdevt.h"


OpenFiberFileCommand::OpenFiberFileCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool OpenFiberFileCommand::execute( GitlCommandParameter& rcInputArg, GitlCommandParameter& rcOutputArg )
{
    /// busy dialog
    GitlUpdateUIEvt evt;
    evt.setParameter("busy_dialog_visible", true);
    evt.dispatch();
    SCOPE_EXIT( GitlUpdateUIEvt evt;
        evt.setParameter("busy_dialog_visible", false);
        evt.dispatch(); );

    ModelLocator* pcModel = ModelLocator::getInstance();
    Subject *pcSubject = &pcModel->getSubject();

    QVariant vValue;
    vValue = rcInputArg.getParameter("filename");
    QString strFilename = vValue.toString();

    pcSubject->getFiber().openTrkFile(strFilename);
    pcModel->getPluginLoader().redraw();

    rcOutputArg.setParameter("trk_file", strFilename);

    return true;
}
