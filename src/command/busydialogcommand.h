#ifndef BUSYDIALOGCOMMAND_H
#define BUSYDIALOGCOMMAND_H
#include "gitlabstractcommand.h"
#include "gitlmodule.h"

class BusyDialogCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit BusyDialogCommand(QObject *parent = 0);
    Q_INVOKABLE bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg);

signals:

public slots:

};

#endif // BUSYDIALOGCOMMAND_H
