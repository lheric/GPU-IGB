#include "busydialogcommand.h"
#include "model/modellocator.h"

BusyDialogCommand::BusyDialogCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

bool BusyDialogCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{
    bool bShowBusyDialog = rcInputArg.getParameter("busy_dialog_visible").toBool();
    rcOutputArg.setParameter("busy_dialog_visible", bShowBusyDialog);
    return true;
}
